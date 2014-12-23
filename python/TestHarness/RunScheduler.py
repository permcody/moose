from subprocess import *
from time import sleep
from timeit import default_timer as clock
from MooseObject import MooseObject

from tempfile import TemporaryFile
#from Queue import Queue
from collections import deque
from Tester import Tester
from signal import SIGTERM


import os, sys

## This class provides an interface to run commands in parallel
#
# To use this class, call the .run() method with the command and the test
# options. When the test is finished running it will call harness.testOutputAndFinish
# to complete the test. Be sure to call join() to make sure all the tests are finished.
#
class RunScheduler(MooseObject):

  @staticmethod
  def validParams():
    params = MooseObject.validParams()
    params.addRequiredParam('test_harness', "TestHarness instance")
    params.addParam('max_processes',     1, "The maximum number of jobs to run concurrently")
    params.addParam('average_load',     64, "The load average not to exceed")
    params.addParam('mode',       "NORMAL", "The scheduler mode: NORMAL, PBS, PBS_EMULATOR")
    params.addParam('batch_number',      0, "The batch number to use for queue statusing")
    return params

  ## Return this return code if the process must be killed because of timeout
  TIMEOUT = -999999

  def __init__(self, name, params):
    MooseObject.__init__(self, name, params)

    ## The test harness to run callbacks on
    self.harness = self._pars['test_harness']

    # The mode of operation (determines how jobs are launched and statused)
    self.mode = self._pars['mode']

    if self.mode == 'PBS':
      template_script = open(os.path.join(self.harness.getMooseDir(), 'python', 'TestHarness', 'pbs_template.i'), 'r')
      self.cluster_launcher_template = template_script.read()
      template_script.close()
      self.test_serial_number = 0
      self.batch_number = self._pars['batch_number']

    # Retrieve and store the TestHarness options for use in this object
    self.options = self.harness.getOptions()

    ## List of currently running jobs as (Popen instance, command, test, time when expires) tuples
    # None means no job is running in this slot
    self.jobs = [None] * self._pars['max_processes']

    # Requested average load level to stay below
    self.average_load = self._pars['average_load']

    # queue for jobs needing a prereq
    self.queue = deque()

    # Jobs that have been finished
    self.finished_jobs = set()

    # List of skipped jobs to resolve prereq issues for tests that never run
    self.skipped_jobs = set()

    # Jobs we are reporting as taking longer then 10% of MAX_TIME
    self.reported_jobs = set()

    # Reporting timer which resets when ever data is printed to the screen.
    self.reported_timer = clock()

  ## run the command asynchronously and call testharness.testOutputAndFinish when complete
  def run(self, tester, command, recurse=True):
    if self.mode == 'PBS':
      # Override the command to run
      command = self.createClusterLauncherFile(tester)

    # First see if any of the queued jobs can be run but only if recursion is allowed on this run
    if recurse:
      self.startReadyJobs()

    # Now make sure that this job doesn't have an unsatisfied prereq
    if tester.specs['prereq'] != None and len(set(tester.specs['prereq']) - self.finished_jobs) and self.options.pbs is None:
      self.queue.append([tester, command, os.getcwd()])
      return

    # Make sure we are complying with the requested load average
    self.satisfyLoad()

    # Wait for a job to finish if the jobs queue is full
    while self.jobs.count(None) == 0:
      self.spinwait()

    # Pre-run preperation
    tester.prepare()

    job_index = self.jobs.index(None) # find an empty slot
    log( 'Command %d started: %s' % (job_index, command) )

    # It seems that using PIPE doesn't work very well when launching multiple jobs.
    # It deadlocks rather easy.  Instead we will use temporary files
    # to hold the output as it is produced
    try:
      if self.options.dry_run:
        tmp_command = command
        command = "echo"

      f = TemporaryFile()

      # On Windows, there is an issue with path translation when the command is passed in
      # as a list.
      p = Popen(command,stdout=f,stderr=f,close_fds=False, shell=True)

      if self.options.dry_run:
        command = tmp_command
    except:
      print "Error in launching a new task"
      raise

    self.jobs[job_index] = (p, command, tester, clock(), f)

  def startReadyJobs(self):
    queue_items = len(self.queue)
    for i in range(0, queue_items):
      (tester, command, dirpath) = self.queue.popleft()
      saved_dir = os.getcwd()
      sys.path.append(os.path.abspath(dirpath))
      os.chdir(dirpath)
      # We want to avoid "dual" recursion so pass a False flag here
      self.run(tester, command, False)
      os.chdir(saved_dir)
      sys.path.pop()

  ## Return control the the test harness by finalizing the test output and calling the callback
  def returnToTestHarness(self, job_index):
    (p, command, tester, time, f) = self.jobs[job_index]

    log( 'Command %d done:    %s' % (job_index, command) )
    did_pass = True

    if p.poll() == None: # process has not completed, it timed out
      output = self.readOutput(f)
      output += '\n' + "#"*80 + '\nProcess terminated by test harness. Max time exceeded (' + str(tester.specs['max_time']) + ' seconds)\n' + "#"*80 + '\n'
      f.close()
      os.kill(p.pid, SIGTERM)        # Python 2.4 compatibility
      #p.terminate()                 # Python 2.6+

      if not self.harness.testOutputAndFinish(tester, RunScheduler.TIMEOUT, output, time, clock()):
        did_pass = False
    else:
      output = 'Working Directory: ' + tester.specs['test_dir'] + '\nRunning command: ' + command + '\n'
      output += self.readOutput(f)
      f.close()

      if tester in self.reported_jobs:
        tester.specs.addParam('caveats', ['FINISHED'], "")

      if not self.harness.testOutputAndFinish(tester, p.returncode, output, time, clock()):
        did_pass = False

    if did_pass:
      self.finished_jobs.add(tester.specs['test_name'])
    else:
      self.skipped_jobs.add(tester.specs['test_name'])

    self.jobs[job_index] = None

  ## Don't return until one of the running processes exits.
  #
  # When a process exits (or times out) call returnToTestHarness and return from
  # this function.
  def spinwait(self, time_to_wait=0.05):
    now = clock()
    job_index = 0
    slot_freed = False
    for tuple in self.jobs:
      if tuple != None:
        (p, command, tester, start_time, f) = tuple
        if p.poll() != None or now > (start_time + float(tester.specs['max_time'])):
          # finish up as many jobs as possible, don't sleep until
          # we've cleared all of the finished jobs
          self.returnToTestHarness(job_index)
          # We just output to the screen so reset the test harness "activity" timer
          self.reported_timer = now

          slot_freed = True
          # We just reset the timer so no need to check if we've been waiting for awhile in
          # this iteration

        # Has the TestHarness done nothing for awhile
        elif now > (self.reported_timer + 10.0):
          # Has the current test been previously reported?
          if tester not in self.reported_jobs:
            if tester.specs.isValid('min_reported_time'):
              start_min_threshold = start_time + float(tester.specs['min_reported_time'])
            else:
              start_min_threshold = start_time + (0.1 * float(tester.specs['max_time']))

            threshold = max(start_min_threshold, (0.1 * float(tester.specs['max_time'])))

            if now >= threshold:
              self.harness.handleTestResult(tester.specs, '', 'RUNNING...', start_time, now, False)

              self.reported_jobs.add(tester)
              self.reported_timer = now

      job_index += 1

    if not slot_freed:
      sleep(time_to_wait)

  def satisfyLoad(self):
    # Get the current load average, or zero if it isn't available for some reason (such as being
    #   run on a non-posix operating system)
    loadAverage = 0.0
    try:
      loadAverage = os.getloadavg()[0]
    except AttributeError:
      pass      # getloadavg() not available in this implementation of os

    # We'll always run at least one job regardless of load or we'll starve!
    while self.jobs.count(None) < len(self.jobs) and loadAverage >= self.average_load:

#      print "DEBUG: Sleeping... ", len(self.jobs) - self.jobs.count(None), " jobs running (load average: ", os.getloadavg()[0], ")\n"
      self.spinwait(0.5) # If the load average is high we'll sleep longer here to let things clear out
#      print "DEBUG: Ready to run (load average: ", os.getloadavg()[0], ")\n"

  ## Wait until all processes are done, then return
  def join(self):
    while self.jobs.count(None) != len(self.jobs):
      self.spinwait()
      self.startReadyJobs()

    if len(self.queue) != 0:
      # See if there are any tests left in the queue simply because their dependencies where skipped
      keep_going = True
      while keep_going:
        keep_going = False
        queue_items = len(self.queue)
        for i in range(0, queue_items):
          (tester, command, dirpath) = self.queue.popleft()
          if len(set(tester.specs['prereq']) & self.skipped_jobs):
            self.harness.handleTestResult(tester.specs, '', 'skipped (skipped dependency)')
            self.skipped_jobs.add(tester.specs['test_name'])
            keep_going = True
          else:
            self.queue.append([tester, command, dirpath])
      # Anything left is a cyclic dependency
      if len(self.queue) != 0:
        print "Cyclic or Invalid Dependency Detected!"
        for (tester, command, dirpath) in self.queue:
          print tester.specs['test_name']
        sys.exit(1)

  # This function reads output from the file (i.e. the test output)
  # but trims it down to the specified size.  It'll save the first two thirds
  # of the requested size and the last third trimming from the middle
  def readOutput(self, f, max_size=100000):
    first_part = int(max_size*(2.0/3.0))
    second_part = int(max_size*(1.0/3.0))
    output = ''

    f.seek(0)
    if self.harness.options.sep_files != True:
      output = f.read(first_part)     # Limit the output to 1MB
      if len(output) == first_part:   # This means we didn't read the whole file yet
        output += "\n" + "#"*80 + "\n\nOutput trimmed\n\n" + "#"*80 + "\n"
        f.seek(-second_part, 2)       # Skip the middle part of the file

        if (f.tell() <= first_part):  # Don't re-read some of what you've already read
          f.seek(first_part+1, 0)

    output += f.read()              # Now read the rest
    return output


  # Add a skipped job to the list
  def jobSkipped(self, name):
    self.skipped_jobs.add(name)

  def createClusterLauncherFile(self, tester):
    self.test_serial_number += 1
#    command = None
#    tester = None
#    # Create the tests.cluster input file
#    (should_run, reason) = tester.checkRunnableBase(self.options, self.checks)
#    if should_run:

    content = self.cluster_launcher_template
    params = tester.getParameters()

    # Convert MAX_TIME to hours:minutes for walltime use
    hours = int(int(params['max_time']) / 3600)
    minutes = int(int(params['max_time']) / 60) % 60
    params['walltime'] = '{0:02d}'.format(hours) + ':' + '{0:02d}'.format(minutes) + ':00'

    # Truncate JOB_NAME. PBS can only accept 13 character (6 characters from test name + _## (test serial number) + _### (serialized number generated by cluster_launcher) = the 13 character limit)
    params['job_name'] = params['input'][:6] + '_' + str(self.test_serial_number).zfill(2)
    params['job_name'] = params['job_name'].replace('.', '')
    params['job_name'] = params['job_name'].replace('-', '')

    # Convert TEST_NAME to input tests file name (normally just 'tests')
    params['no_copy'] = self.harness.getOptions().input_file_name

    # Do all of the replacements for the valid parameters
    for param in params.valid_keys():
      if param in params.substitute:
        params[param] = params.substitute[param].replace(param.upper(), params[param])
      content = content.replace('<' + param.upper() + '>', str(params[param]))

    # Make sure we strip out any string substitution parameters that were not supplied
    for param in params.substitute_keys():
      if not params.isValid(param):
        content = content.replace('<' + param.upper() + '>', '')

    # Write the cluster_launcher input file
    file_name = os.getcwd() + '/' + self.batch_number + '.cluster'
    if os.path.exists(file_name):
      file_handle = open(os.getcwd() + '/' + self.batch_number + '.cluster', 'r+')
      file_handle.seek(-3, 2)
      file_handle.write('\n')
    else:
      file_handle = open(os.getcwd() + '/' + self.batch_number + '.cluster', 'w')
      file_handle.write('[Jobs]\n')
    
    file_handle.write(content)
    file_handle.write('[]\n')
    file_handle.close()
  
    return os.path.join(params['moose_dir'], 'framework', 'scripts', 'cluster_launcher.py') + ' ' + self.batch_number + '.cluster'


#    file_handle.write('[Jobs]\n')
# 
# 
#      if self.options.cluster_handle == None:
#        self.options.cluster_handle = open(os.getcwd() + '/' + self.options.pbs + '.cluster', 'w')
#      self.options.cluster_handle.write('[Jobs]\n')
#      # This returns the command to run as well as builds the parameters of the test
#      # The resulting command once this loop has completed is sufficient to launch
#      # all previous jobs
#      command = tester.getCommand(self.options)
#      self.options.cluster_handle.write('[]\n')
#    else: # This job is skipped - notify the runner
#      if (reason != ''):
#        self.handleTestResult(tester.parameters(), '', reason)
#        self.runner.jobSkipped(tester.parameters()['test_name'])
# 
#    # Close the tests.cluster file
#    if self.options.cluster_handle is not None:
#      self.options.cluster_handle.close()
#      self.options.cluster_handle = None
# 
#    # Return the final tester/command (sufficient to run all tests)
#    return (tester, command)


## Static logging string for debugging
LOG = []
LOG_ON = False
def log(msg):
  if LOG_ON:
    LOG.append(msg)
    print msg

#!/usr/bin/env python
import os, sys, re, shutil, inspect
from optparse import OptionParser, OptionGroup, Values

# Get the real path of cluster_launcher
if(os.path.islink(sys.argv[0])):
  pathname = os.path.dirname(os.path.realpath(sys.argv[0]))
else:
  pathname = os.path.dirname(sys.argv[0])
  pathname = os.path.abspath(pathname)

# Add the utilities/python_getpot directory
MOOSE_DIR = os.path.abspath(os.path.join(pathname, '..', '..'))
FRAMEWORK_DIR = os.path.abspath(os.path.join(pathname, '..', '..', 'framework'))
#### See if MOOSE_DIR is already in the environment instead
if os.environ.has_key("MOOSE_DIR"):
  MOOSE_DIR = os.environ['MOOSE_DIR']
  FRAMEWORK_DIR = os.path.join(MOOSE_DIR, 'framework')
if os.environ.has_key("FRAMEWORK_DIR"):
  FRAMEWORK_DIR = os.environ['FRAMEWORK_DIR']

# Import the TestHarness and Helper functions from the MOOSE toolkit
sys.path.append(os.path.join(MOOSE_DIR, 'python'))
import path_tool
path_tool.activate_module('TestHarness')
path_tool.activate_module('FactorySystem')
path_tool.activate_module('ClusterLauncher')

import ParseGetPot
from InputParameters import InputParameters
from Factory import Factory
from Job import Job

# Default file to read if only a directory is supplied
job_list = 'job_list'

def getNextDirName(file_name, files):
  largest_serial_num = 0

  for name in files:
    m = re.search(file_name + '_(\d{3})', name)
    if m != None and int(m.group(1)) > largest_serial_num:
      largest_serial_num = int(m.group(1))
  return file_name + "_" +  str(largest_serial_num+1).zfill(3)

class ClusterLauncher:
  def __init__(self):
    self.factory = Factory()
    self.factory.loadPlugins([os.path.dirname(os.path.abspath(inspect.getfile(inspect.currentframe())))], 'QueueSystemHelpers', Job)

  def parseJobsFile(self, template_dir, job_file):
    jobs = []
    # We expect the job list to be named "job_list"
    filename = os.path.join(template_dir, job_file)

    try:
      data = ParseGetPot.readInputFile(filename)
    except:        # ParseGetPot class
      print "Parse Error: " + filename
      return jobs

    # We expect our root node to be called "Jobs"
    if 'Jobs' in data.children:
      jobs_node = data.children['Jobs']

      # Get the active line
      active_jobs = None
      if 'active' in jobs_node.params:
        active_jobs = jobs_node.params['active'].split(' ')

      for jobname, job_node in jobs_node.children.iteritems():
        # Make sure this job is active
        if active_jobs != None and not jobname in active_jobs:
          continue

        # First retrieve the type so we can get the valid params
        if 'type' not in job_node.params:
          print "Type missing in " + filename
          sys.exit(1)

        params = self.factory.validParams(job_node.params['type'])

        params['job_name'] = jobname

        # Now update all the base level keys
        params_parsed = set()
        params_ignored = set()
        for key, value in job_node.params.iteritems():
          params_parsed.add(key)
          if key in params:
            if params.type(key) == list:
              params[key] = value.split(' ')
            else:
              if re.match('".*"', value):  # Strip quotes
                params[key] = value[1:-1]
              else:
                params[key] = value
          else:
            params_ignored.add(key)

        # Append the name of the job specification file to the no copy list
        params['no_copy'].append(job_file)

        # Make sure that all required parameters are supplied
        required_params_missing = params.required_keys() - params_parsed
        if len(required_params_missing):
          print 'Required Missing Parameter(s): ', required_params_missing
          sys.exit(1)
        if len(params_ignored):
          print 'Ignored Parameter(s): ', params_ignored

        jobs.append(params)
    return jobs

  def createJob(self, template_dir, params, create_separate_dir, log_message):
    next_dir = '.'
    next_name = getNextDirName(params['job_name'], os.listdir('.'))
    saved_cwd = os.getcwd()
    if create_separate_dir:
      os.mkdir(os.path.join(template_dir, next_name))

      # Log it
      if len(log_message):
        f = open(os.path.join(template_dir, 'jobs.log'), 'a')
        f.write(next_name.ljust(20) + ': ' + log_message + '\n')
        f.close()

      os.chdir(os.path.join(template_dir, next_name))
      next_dir = next_name

    # Turn the remaining work over to the Job instance
    # To keep everything consistent we'll also append our serial number to our job name
    params['job_name'] = next_name
    params['MOOSE_DIR'] = MOOSE_DIR
    job_instance = self.factory.create(params['type'], params['job_name'], params)

    # Copy files
    job_instance.copyFiles()

    # Prepare the Job Script
    script_name = job_instance.prepareJobScript(create_separate_dir)

#    # Launch it!
#    job_instance.launch(script_name)

    os.chdir(saved_cwd)

    return job_instance

  def registerJobType(self, type, name):
    self.factory.register(type, name)

  ### Parameter Dump ###
  def printDump(self):
    self.factory.printDump("Jobs")
    sys.exit(0)

  def run(self, template_dir, job_file, options):
    jobs = self.parseJobsFile(template_dir, job_file)

    for job in jobs:
      job_instance = self.createJob(template_dir, job, options.create_separate_dir, options.message)
      job_instance.launch()


########################################################
def main():
  parser = OptionParser(usage='Usage: %prog [options] <template directory>')
  parser.add_option("--dump", action="store_true", dest="dump", default=False, help="Dump the parameters for the testers in GetPot Format")
  parser.add_option("-m", action="store", dest="message", help="A message that will be stored in a local log file that describes the job")
  parser.add_option("-x", action="store_false", dest="create_separate_dir", default=True, help="Skip creating separate directories for each job launch")
  (options, location) = parser.parse_args()

  cluster_launcher = ClusterLauncher()

  if options.dump:
    cluster_launcher.printDump()

  if not location:
    parser.print_help()
    sys.exit(1)

  # See if the user passed a file or a directory
  abs_location = os.path.abspath(location[0])

  if os.path.isdir(abs_location):
    dir = abs_location
    file = job_list
  elif os.path.isfile(abs_location):
    (dir, file) = os.path.split(abs_location)
#  dir = dir + '/'

  # Launch it
  cluster_launcher.run(dir, file, options)

if __name__ == '__main__':
  main()

from InputParameters import InputParameters
from Job import Job

import os, sys, subprocess, shutil

class PBSJob(Job):

  @staticmethod
  def validParams():
    params = Job.validParams()

    params.addRequiredParam('chunks', "The number of PBS chunks.")
    # Only one of either of the next two paramteres can be specified
    params.addParam('mpi_procs', "The number of MPI processes per chunk.")
    params.addParam('total_mpi_procs', "The total number of MPI processes to use divided evenly among chunks.")

    params.addParam('template_script', os.path.join('QueueSystemHelpers', 'pbs_cluster_submit.sh'), "The template job script to use.")
    params.addParam('place', 'scatter:excl', "The PBS job placement scheme to use.")
    params.addParam('walltime', '4:00:00', "The requested walltime for this job.")
    params.addParam('copy_files', "A list of files specifically to copy")
    params.addParam('use_emulator', True, "Use the PBS Emulator")

    params.addStringSubParam('combine_streams', '#PBS -j oe', "Combine stdout and stderror into one file (needed for NO EXPECTED ERR)")
    params.addStringSubParam('threads', '--n-threads=THREADS', "The number of threads to run per MPI process.")
    params.addStringSubParam('queue', '#PBS -q QUEUE', "Which queue to submit this job to.")
    params.addStringSubParam('module', 'module load MODULE', 'moose-dev-gcc', "The module to load.")
    params.addStringSubParam('cli_args', 'CLI_ARGS', "Any extra command line arguments to tack on.")
    params.addStringSubParam('notifications', '#PBS -m NOTIFICATIONS', "The PBS notifications to enable: 'b' for begin, 'e' for end, 'a' for abort.")
    params.addStringSubParam('notify_address', '#PBS -M NOTIFY_ADDRESS', "The email address to use for PBS notifications")
    params.addStringSubParam('command', 'COMMAND', "The completely assembled command if one is supplied, otherwise it can be assembled for you.")

    # Soft linked output during run
    params.addParam('soft_link_output', False, "Create links to your STDOUT and STDERR files in your working directory during the run.")

    params.addRequiredParam('moose_application', "The full path to the application to run.")
    params.addRequiredParam('input_file', "The input file name.")

    return params

  def __init__(self, name, params):
    Job.__init__(self, name, params)

    # Prepend the MOOSE directory and python stub to the front of the template script path
    self.params['template_script'] = os.path.join(params['MOOSE_DIR'], 'python', 'ClusterLauncher', params['template_script'])
    self.final_template_script = ''
    self.working_dir = os.getcwd()

  # Called from the current directory to copy files (usually from the parent)
  def copyFiles(self):
    super(PBSJob, self).copyFiles()

    # Copy directories
    if self.params.isValid('copy_files'):
      for file in self.params['copy_files'].split():
        print file
        if os.path.isfile(os.path.join('..', file)):
          shutil.copy(os.path.join('..', file), '.')
        elif os.path.isdir(os.path.join('..', file)):
          shutil.copytree(os.path.join('..', file), file)

  def prepareJobScript(self, create_separate_dir):
    f = open(self.params['template_script'], 'r')
    content = f.read()
    f.close()

    if create_separate_dir:
      final_template_script = os.path.split(self.params['template_script'])[1]
    else:
      final_template_script = self.params['job_name'] + '.sh'

    params = self.params
    # Error check
    if params.isValid('mpi_procs') and params.isValid('total_mpi_procs'):
      print "ERROR: 'mpi_procs' and 'total_mpi_procs' are exclusive.  Only specify one!"
      sys.exit(1)

    # Do a few PBS job size calculations
    if params.isValid('mpi_procs'):
      params['mpi_procs_per_chunk'] = params['mpi_procs']
    elif params.isValid('total_mpi_procs'):
      params['mpi_procs_per_chunk'] = str(int(params['total_mpi_procs']) / int(params['chunks']))  # Need some more error checking here
    else:
      print "ERROR: You must specify either 'mpi_procs' or 'total_mpi_procs'"
      sys.exit(1)
    if params.isValid('threads'):
      threads = int(params['threads'])
    else:
      threads = 1
    params['ncpus_per_chunk'] = str(int(params['mpi_procs_per_chunk']) * threads)

    # Soft Link output requires several substitutions in the template file
    soft_link1 = ''
    soft_link2 = ''
    soft_link3 = ''
    if params['soft_link_output'] == 'True':
      soft_link1 = '#PBS -koe'
      soft_link2 = 'ln -s $HOME/$PBS_JOBNAME.o$JOB_NUM $PBS_JOBNAME.o$JOB_NUM\nln -s $HOME/$PBS_JOBNAME.e$JOB_NUM $PBS_JOBNAME.e$JOB_NUM'
      soft_link3 = 'rm $PBS_JOBNAME.o$JOB_NUM\nmv $HOME/$PBS_JOBNAME.o$JOB_NUM $PBS_JOBNAME.o$JOB_NUM\nmv $HOME/$PBS_JOBNAME.e$JOB_NUM $PBS_JOBNAME.e$JOB_NUM'
    # Add substitutions on the fly
    params.addStringSubParam('soft_link1', 'SOFT_LINK1', soft_link1, 'private')
    params.addStringSubParam('soft_link2', 'SOFT_LINK2', soft_link2, 'private')
    params.addStringSubParam('soft_link3', 'SOFT_LINK3', soft_link3, 'private')

    f = open(final_template_script, 'w')

    # Do all of the replacements for the valid parameters
    content = params.substituteParamsInContent(content)
#    for param in params.valid_keys():
#      if param in params.substitute:
#        params[param] = params.substitute[param].replace(param.upper(), params[param])
#      content = content.replace('<' + param.upper() + '>', str(params[param]))
#
#    # Make sure we strip out any string substitution parameters that were not supplied
#    for param in params.substitute_keys():
#      if not params.isValid(param):
#        content = content.replace('<' + param.upper() + '>', '')

    f.write(content)
    f.close()

    # Set the template script instance variable
    self.final_template_script = final_template_script

  def launch(self):
    saved_dir = os.getcwd()
    os.chdir(self.working_dir)

    # Finally launch the job
    if self.params['use_emulator']:
      my_process = subprocess.Popen('bash ' + self.final_template_script, stdout=subprocess.PIPE, shell=True)
      my_process.wait()
    else:
      my_process = subprocess.Popen('qsub ' + self.final_template_script, stdout=subprocess.PIPE, shell=True)
      print 'JOB_NAME:', self.params['job_name'], 'JOB_ID:', my_process.communicate()[0].split('.')[0], 'TEST_NAME:', self.params['test_name']

    os.chdir(saved_dir)

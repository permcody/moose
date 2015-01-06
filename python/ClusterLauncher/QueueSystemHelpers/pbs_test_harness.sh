#!/bin/bash

#Job name <JOB_NAME>

cd <JOB_DIR>

mpiexec -n <MPI_PROCS_PER_CHUNK> <MOOSE_APPLICATION> -i <INPUT_FILE> <THREADS> <CLI_ARGS>

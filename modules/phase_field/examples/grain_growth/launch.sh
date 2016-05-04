#!/bin/bash

echo_exec() { echo "\$ $@" ; "$@" ; }

for procs in 2 12
do
  # OP == Grains
  for grains in 6 8
  do
      printf "****************************************************************\nRunning FauxGrainTracker procs = $procs, grains = $grains, OP = $grains\n"
      echo_exec mpiexec -n $procs ../../phase_field-oprof -i 2D_timing.i GlobalParams/op_num=$grains ICs/PolycrystalICs/PolycrystalVoronoiIC/grain_num=$grains Postprocessors/grain_tracker/type=FauxGrainTracker
      printf "****************************************************************\nRunning GrainTracker grains = $grains, OP = $grains\n"
      echo_exec mpiexec -n $procs ../../phase_field-oprof -i 2D_timing.i GlobalParams/op_num=$grains ICs/PolycrystalICs/PolycrystalVoronoiIC/grain_num=$grains Postprocessors/grain_tracker/type=GrainTracker Postprocessors/grain_tracker/remap_grains=false
  done

  # OP < Grains
  for grains in 10 12 14 16 18 20 25 30 35 40 45 50 55 60 65 70 75 80 85 90 95 100
  do
      printf "****************************************************************\nRunning FauxGrainTracker procs = $procs, grains = $grains, OP = $grains\n"
      echo_exec mpiexec -n $procs ../../phase_field-oprof -i 2D_timing.i GlobalParams/op_num=$grains ICs/PolycrystalICs/PolycrystalVoronoiIC/grain_num=$grains Postprocessors/grain_tracker/type=FauxGrainTracker

      printf "****************************************************************\nRunning GrainTracker grains = $grains, OP = 8\n"
      echo_exec mpiexec -n $procs ../../phase_field-oprof -i 2D_timing.i GlobalParams/op_num=8 ICs/PolycrystalICs/PolycrystalVoronoiIC/grain_num=$grains Postprocessors/grain_tracker/type=GrainTracker
  done
done

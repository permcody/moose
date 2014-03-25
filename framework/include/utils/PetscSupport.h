/****************************************************************/
/*               DO NOT MODIFY THIS HEADER                      */
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*           (c) 2010 Battelle Energy Alliance, LLC             */
/*                   ALL RIGHTS RESERVED                        */
/*                                                              */
/*          Prepared by Battelle Energy Alliance, LLC           */
/*            Under Contract No. DE-AC07-05ID14517              */
/*            With the U. S. Department of Energy               */
/*                                                              */
/*            See COPYRIGHT for full restrictions               */
/****************************************************************/

#ifndef PETSCSUPPORT_H
#define PETSCSUPPORT_H

#include "libmesh/libmesh.h"

#ifdef LIBMESH_HAVE_PETSC

// Moose includes
#include "Problem.h"
#include "NonlinearSystem.h"
#include "CommandLine.h"
#include "Console.h"

// libMesh
#include "libmesh/petsc_nonlinear_solver.h"

class FEProblem;

namespace Moose
{
namespace PetscSupport
{
void petscSetOptions(FEProblem & problem);

void petscSetDefaults(FEProblem & problem);

void petscSetupDampers(NonlinearImplicitSystem& sys);

void petscSetupDM(NonlinearSystem & nl);

PetscErrorCode petscSetupOutput(CommandLine * cmd_line);

/**
 * Helper function for outputing the norm values with/without color
 */
void outputNorm(Real old_norm, Real norm, bool use_color = false);

/**
 * Helper function for displaying the linear residual during PETSC solve
 */
PetscErrorCode petscLinearMonitor(KSP /*ksp*/, PetscInt its, PetscReal rnorm, void *void_ptr);

}
}

#endif //LIBMESH_HAVE_PETSC

#endif //PETSCSUPPORT_H

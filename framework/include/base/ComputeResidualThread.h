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

#ifndef COMPUTERESIDUALTHREAD_H
#define COMPUTERESIDUALTHREAD_H

#include "ThreadedElementLoop.h"

// libMesh includes
#include "libmesh/elem_range.h"

// Forward declarations
class FEProblem;
class NonlinearSystem;
class IntegratedBC;
class DGKernel;
class InterfaceKernel;
class TimeKernel;
class KernelBase;
class KernelWarehouse;

class ComputeResidualThread : public ThreadedElementLoop<ConstElemRange>
{
public:
  ComputeResidualThread(FEProblem & fe_problem, NonlinearSystem & sys, Moose::KernelType type);
  // Splitting Constructor
  ComputeResidualThread(ComputeResidualThread & x, Threads::split split);

protected:
  virtual void subdomainChanged() override;
  virtual void onElement(const Elem *elem ) override;
  virtual void onBoundary(const Elem *elem, unsigned int side, BoundaryID bnd_id) override;
  virtual void onInterface(const Elem *elem, unsigned int side, BoundaryID bnd_id) override;
  virtual void onInternalSide(const Elem *elem, unsigned int side) override;
  virtual void postElement(const Elem * /*elem*/) override;
  virtual void post() override;

  void join(const ComputeResidualThread & /*y*/);

  NonlinearSystem & _sys;
  Moose::KernelType _kernel_type;
  unsigned int _num_cached;

  /// Reference to BC storage structures
  const MooseObjectWarehouse<IntegratedBC> & _integrated_bcs;

  /// Reference to DGKernel storage structure
  const MooseObjectWarehouse<DGKernel> & _dg_kernels;

  /// Reference to interface kernel storage structure
  const MooseObjectWarehouse<InterfaceKernel> & _interface_kernels;

  ///@{
  /// Reference to Kernel storage structures
  const KernelWarehouse & _kernels;
  const MooseObjectWarehouse<KernelBase> & _time_kernels;
  const MooseObjectWarehouse<KernelBase> & _non_time_kernels;
  ///@}
};

#endif //COMPUTERESIDUALTHREAD_H

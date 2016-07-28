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

#ifndef COMPUTEMARKERTHREAD_H
#define COMPUTEMARKERTHREAD_H

#include "ThreadedElementLoop.h"

// libMesh includes
#include "libmesh/elem_range.h"

class AuxiliarySystem;

class ComputeMarkerThread : public ThreadedElementLoop<ConstElemRange>
{
public:
  ComputeMarkerThread(FEProblem & fe_problem, AuxiliarySystem & sys);

  // Splitting Constructor
  ComputeMarkerThread(ComputeMarkerThread & x, Threads::split split);

protected:
  virtual void subdomainChanged() override;
  virtual void onElement(const Elem *elem) override;
  virtual void onBoundary(const Elem *elem, unsigned int side, BoundaryID bnd_id) override;
  virtual void onInternalSide(const Elem *elem, unsigned int side) override;
  virtual void postElement(const Elem * /*elem*/) override;
  virtual void post() override;

  void join(const ComputeMarkerThread & /*y*/);

  FEProblem & _fe_problem;
  AuxiliarySystem & _aux_sys;

  /// Reference to the Marker warhouse in FEProblem
  const MooseObjectWarehouse<Marker> & _marker_whs;
};

#endif //COMPUTEMARKERTHREAD_H

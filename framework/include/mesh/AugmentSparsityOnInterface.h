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

#ifndef AUGMENT_SPARSITY_ON_INTERFACE_H
#define AUGMENT_SPARSITY_ON_INTERFACE_H

// App includes
#include "AutomaticMortarGeneration.h"

// libMesh includes
#include "libmesh/ghosting_functor.h"
#include "libmesh/mesh_base.h"

/**
 * Class derived from GhostingFunctor based on the code in misc_ex9 in libmesh.
 */
class AugmentSparsityOnInterface : public GhostingFunctor
{
public:
  AugmentSparsityOnInterface(AutomaticMortarGeneration & amg);

  /**
   * This function must be overriden by application codes to add
   * required elements from (range_begin, range_end) to the
   * coupled_elements map.
   */
  virtual void operator()(const MeshBase::const_element_iterator & range_begin,
                          const MeshBase::const_element_iterator & range_end,
                          processor_id_type p,
                          map_type & coupled_elements) libmesh_override;

  /**
   * According to the base class docs, "We call mesh_reinit() whenever
   * the relevant Mesh has changed, but before remote elements on a
   * distributed mesh are deleted."
   */
  virtual void mesh_reinit() libmesh_override;

  /**
   * Update the cached _lower_to_upper map whenever our Mesh has been
   * redistributed.  We'll be lazy and just recalculate from scratch.
   */
  virtual void redistribute() libmesh_override { this->mesh_reinit(); }

private:
  /**
   * The Mesh we're calculating on
   */
  AutomaticMortarGeneration & _amg;
};

#endif

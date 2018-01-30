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
#include "GeometricRelationshipManager.h"
#include "libmesh/mesh_base.h"

/**
 * Class derived from GhostingFunctor based on the code in misc_ex9 in libmesh.
 */
class AugmentSparsityOnInterface : public GeometricRelationshipManager
{
public:
  AugmentSparsityOnInterface(const InputParameters & parameters);

  void attachRelationshipManagersInternal(Moose::RelationshipManagerType rm_type) override;
  virtual void operator()(const MeshBase::const_element_iterator & range_begin,
                          const MeshBase::const_element_iterator & range_end,
                          processor_id_type p,
                          map_type & coupled_elements) override;

private:
  AutomaticMortarGeneration & _amg;
};

#endif

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

// App includes
#include "AugmentSparsityOnInterface.h"

// libMesh includes
#include "libmesh/elem.h"

AugmentSparsityOnInterface::AugmentSparsityOnInterface(const InputParameters & parameters)
  : GeometricRelationshipManager(parameters),
    _amg(*getCheckedPointerParam<AutomaticMortarGeneration *>("amg", "AMG is nullptr"))
{
}

void AugmentSparsityOnInterface::attachRelationshipManagersInternal(
    Moose::RelationshipManagerType /*rm_type*/)
{
  if (_mesh.isDistributedMesh())
  {
    attachGeometricFunctorHelper(*this);
  }
}

void
AugmentSparsityOnInterface::operator()(const MeshBase::const_element_iterator & range_begin,
                                       const MeshBase::const_element_iterator & range_end,
                                       processor_id_type p,
                                       map_type & coupled_elements)
{
  const CouplingMatrix * const null_mat = libmesh_nullptr;

  for (MeshBase::const_element_iterator elem_it = range_begin; elem_it != range_end; ++elem_it)
  {
    const Elem * const elem = *elem_it;

    // This code is present in both the PointNeighborCoupling class that lives
    // in libmesh and the misc_ex9 code, so I think it's required but I don't really
    // understand it.
    if (elem->processor_id() != p)
      coupled_elements.insert(std::make_pair(elem, null_mat));

    // If this Elem has an interior_parent, add it to the list.
    const Elem * elem_ip = elem->interior_parent();
    if (elem_ip && elem_ip->processor_id() != p)
    {
      // libMesh::out << "Adding Elem " << elem_ip->id()
      //              << " as coupled Elem for " << elem->id()
      //              << "." << std::endl;
      coupled_elements.insert(std::make_pair(elem_ip, null_mat));
    }

    // Look up elem in the mortar_interface_coupling data structure.
    auto bounds = _amg.mortar_interface_coupling.equal_range(elem);
    for (const auto & pr : as_range(bounds))
    {
      const Elem * cross_interface_neighbor = pr.second;

      if (cross_interface_neighbor->processor_id() != p)
      {
        // libMesh::out << "Adding Elem " << cross_interface_neighbor->id()
        //              << " as coupled Elem for " << pr.first->id()
        //              << "." << std::endl;
        coupled_elements.insert(std::make_pair(cross_interface_neighbor, null_mat));
      }

      // If the cross_interface_neighbor is a lower-dimensional element with
      // an interior parent, add the interior parent to the
      // list of Elems coupled to us.
      const Elem * cross_interface_neighbor_ip = cross_interface_neighbor->interior_parent();
      if (cross_interface_neighbor_ip && cross_interface_neighbor_ip->processor_id() != p)
      {
        // libMesh::out << "Adding Elem " << cross_interface_neighbor_ip->id()
        //              << " as coupled Elem for " << pr.first->id()
        //              << "." << std::endl;
        coupled_elements.insert(std::make_pair(cross_interface_neighbor_ip, null_mat));
      }
    }
  } // end loop over range
}

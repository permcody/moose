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

#include "MortarMeshGenerator.h"
#include "MooseApp.h"
#include "MooseMesh.h"
#include "FEProblem.h"
#include "ActionWarehouse.h"
#include "AutomaticMortarGeneration.h"

template <>
InputParameters
validParams<MortarMeshGenerator>()
{
  InputParameters params = validParams<MeshModifier>();
  params.addRequiredParam<std::vector<BoundaryName>>("master", "Master side ID(s)");
  params.addRequiredParam<std::vector<BoundaryName>>("slave", "Slave side ID(s)");
  return params;
}

MortarMeshGenerator::MortarMeshGenerator(const InputParameters & params)
  : MeshModifier(params),
    _master(getParam<std::vector<BoundaryName>>("master")),
    _slave(getParam<std::vector<BoundaryName>>("slave")),
    _h_max(0.)
{
}

void
MortarMeshGenerator::modify()
{
  _mesh_ptr->errorIfDistributedMesh("MortarMeshGenerator");

  addLowerDimensionalElements();
}

void
MortarMeshGenerator::addLowerDimensionalElements()
{
  MeshBase & mesh = _mesh_ptr->getMesh();

  // Temporary storage to catch ids handed back by the BoundaryInfo object.
  std::vector<boundary_id_type> side_ids;

  // A reference to the Mesh's BoundaryInfo object
  BoundaryInfo & bi = mesh.get_boundary_info();

  // Data structures to store results of calling BoundaryInfo::build_side_list().
  std::vector<dof_id_type> element_id_list;
  std::vector<unsigned short int> side_list;
  std::vector<boundary_id_type> bc_id_list;

  // Get lists of (elem, side, id) triplets.
  bi.build_side_list(element_id_list, side_list, bc_id_list);

  // We'll build up a list of lower-dimensional (Elem,side) pairs
  // to be added in the loop, then add them afterword. We don't
  // want to add Elems while iterating because that may invalidate
  // iterators due to reallocation.
  std::vector<ElemSideBCTriple> slave_sides_to_add;
  std::vector<ElemSideBCTriple> master_sides_to_add;

  const auto slave_ids = _mesh_ptr->getBoundaryIDs(_slave);
  const auto master_ids = _mesh_ptr->getBoundaryIDs(_master);

  for (auto i = beginIndex(element_id_list); i < element_id_list.size(); ++i)
  {
    if (std::find(slave_ids.begin(), slave_ids.end(), bc_id_list[i]) != slave_ids.end())
      slave_sides_to_add.emplace_back(
          mesh.elem_ptr(element_id_list[i]), side_list[i], bc_id_list[i]);

    if (std::find(master_ids.begin(), master_ids.end(), bc_id_list[i]) != master_ids.end())
      master_sides_to_add.emplace_back(
          mesh.elem_ptr(element_id_list[i]), side_list[i], bc_id_list[i]);
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Now add lower-dimensional elements on the slave side.
  ////////////////////////////////////////////////////////////////////////////////
  {
    std::set<subdomain_id_type> added_slave_subdomain_ids;

    for (const auto & triple : slave_sides_to_add)
    {
      const Elem * added_side = loopBody(triple, added_slave_subdomain_ids);

      // Keep track of max lower-dimensional element size on the slave side.
      _h_max = std::max(_h_max, added_side->volume());
    }

    // Associate added subdomain ids with unique names.
    for (const auto & sid : added_slave_subdomain_ids)
    {
      std::ostringstream oss;
      oss << "lower_dimensional_subdomain_" << sid;
      mesh.subdomain_name(sid) = oss.str();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  // Now add lower-dimensional elements on the master side.
  ////////////////////////////////////////////////////////////////////////////////
  {
    std::set<subdomain_id_type> added_master_subdomain_ids;

    for (const auto & triple : master_sides_to_add)
    {
      loopBody(triple, added_master_subdomain_ids);
    }

    // Associate added subdomain ids with unique names.
    for (const auto & sid : added_master_subdomain_ids)
    {
      std::ostringstream oss;
      oss << "lower_dimensional_subdomain_" << sid;
      mesh.subdomain_name(sid) = oss.str();
    }
  }

  // Now that we have added lower-dimensional elements to the Mesh,
  // we need to call find_neighbors() to set up their neighbor
  // pointers.
  // mesh.find_neighbors();
}

const Elem *
MortarMeshGenerator::loopBody(const ElemSideBCTriple & triple,
                              std::set<subdomain_id_type> & added_ids)
{
  Elem * elem = triple._elem;
  unsigned int side = triple._side;
  boundary_id_type bc_id = triple._bc_id;

  // Build a non-proxy element from this side.
  std::unique_ptr<Elem> side_elem(elem->build_side_ptr(side, /*proxy=*/false));

  // The side will be added with the same processor id as the parent.
  side_elem->processor_id() = elem->processor_id();

  // The side should have a different subdomain id than the
  // parent, so that we can distinguish them and define
  // subdomain-restricted variables on them. Currently we just
  // offset the value by a constant amount.
  subdomain_id_type added_elem_subdomain_id =
      cast_int<subdomain_id_type>(AutomaticMortarGeneration::boundary_subdomain_id_offset + bc_id);
  side_elem->subdomain_id() = added_elem_subdomain_id;

  // We'll want to associate a name with this id later.
  added_ids.insert(added_elem_subdomain_id);

  // Also assign the side's interior parent, so it is always
  // easy to figure out the Elem we came from.
  side_elem->set_interior_parent(elem);

  // Finally, add the lower-dimensional element to the Mesh.
  const Elem * added_side = _mesh_ptr->getMesh().add_elem(side_elem.release());

  // Also keep track of the mapping in the other direction:
  // given a lower-dimensional element, we should be able to
  // figure out what side of the parent we were.
  _lower_elem_to_side_id[added_side] = side;

  return added_side;
}

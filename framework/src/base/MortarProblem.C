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

// MOOSE includes
#include "MortarProblem.h"

#include "Assembly.h"
#include "AuxiliarySystem.h"
#include "FEProblem.h"
#include "MooseApp.h"
#include "MooseMesh.h"
#include "NonlinearSystem.h"
#include "Problem.h"
#include "ResetDisplacedMeshThread.h"
#include "SubProblem.h"
#include "UpdateDisplacedMeshThread.h"

#include "libmesh/numeric_vector.h"

template <>
InputParameters
validParams<MortarProblem>()
{
  InputParameters params = validParams<SubProblem>();

  params.registerRelationshipManagers("AugmentSparsityOnInterface");

  params.addPrivateParam<MooseMesh *>("mesh");
  return params;
}

MortarProblem::MortarProblem(const InputParameters & parameters)
  : SubProblem(parameters), _mesh(*getCheckedPointerParam<MooseMesh *>("mesh")), _amg(_mesh)
{
  addLowerDimensionalElements();
}

bool
MortarProblem::isTransient() const
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->isTransient();
}

Moose::CoordinateSystemType
MortarProblem::getCoordSystem(SubdomainID sid)
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->getCoordSystem(sid);
}

std::set<dof_id_type> &
MortarProblem::ghostedElems()
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->ghostedElems();
}

void
MortarProblem::useFECache(bool /*fe_cache*/)
{
  // Nothing to do, but must override
}

void
MortarProblem::init()
{
  Moose::perf_log.push("MortarProblem::init::meshChanged()", "Setup");
  _mesh.meshChanged();
  Moose::perf_log.pop("MortarProblem::init::meshChanged()", "Setup");
}

void
MortarProblem::initAdaptivity()
{
}

void
MortarProblem::updateMesh()
{
}

void
MortarProblem::updateMesh(const NumericVector<Number> & /*soln*/,
                          const NumericVector<Number> & /*aux_soln*/)
{
}

bool
MortarProblem::hasVariable(const std::string & var_name)
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->hasVariable(var_name);
}

MooseVariable &
MortarProblem::getVariable(THREAD_ID tid, const std::string & var_name)
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->getVariable(tid, var_name);
}

bool
MortarProblem::hasScalarVariable(const std::string & var_name)
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->hasScalarVariable(var_name);
}

MooseVariableScalar &
MortarProblem::getScalarVariable(THREAD_ID tid, const std::string & var_name)
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->getScalarVariable(tid, var_name);
}

System &
MortarProblem::getSystem(const std::string & var_name)
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->getSystem(var_name);
}

void
MortarProblem::addVariable(const std::string & /*var_name*/,
                           const FEType & /*type*/,
                           Real /*scale_factor*/,
                           const std::set<SubdomainID> * const /*active_subdomains*/)
{
  // Nothing to do
}

void
MortarProblem::addAuxVariable(const std::string & /*var_name*/,
                              const FEType & /*type*/,
                              const std::set<SubdomainID> * const /*active_subdomains*/)
{
  // Nothing to do
}

void
MortarProblem::addScalarVariable(const std::string & /*var_name*/,
                                 Order /*order*/,
                                 Real /*scale_factor*/,
                                 const std::set<SubdomainID> * const /*active_subdomains*/)
{
  // Nothing to do
}

void
MortarProblem::addAuxScalarVariable(const std::string & /*var_name*/,
                                    Order /*order*/,
                                    Real /*scale_factor*/,
                                    const std::set<SubdomainID> * const /*active_subdomains*/)
{
  // Nothing to do
}

void
MortarProblem::prepare(const Elem * /*elem*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::prepareNonlocal(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::prepareFace(const Elem * /*elem*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::prepare(const Elem * /*elem*/,
                       unsigned int /*ivar*/,
                       unsigned int /*jvar*/,
                       const std::vector<dof_id_type> & /*dof_indices*/,
                       THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::setCurrentSubdomainID(const Elem * /*elem*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::setNeighborSubdomainID(const Elem * /*elem*/,
                                      unsigned int /*side*/,
                                      THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::prepareBlockNonlocal(unsigned int /*ivar*/,
                                    unsigned int /*jvar*/,
                                    const std::vector<dof_id_type> & /*idof_indices*/,
                                    const std::vector<dof_id_type> & /*jdof_indices*/,
                                    THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::prepareAssembly(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::prepareAssemblyNeighbor(THREAD_ID /*tid*/)
{
  // Nothing to do
}

bool
MortarProblem::reinitDirac(const Elem * /*elem*/, THREAD_ID /*tid*/)
{
  // Nothing to do
  return false;
}

void
MortarProblem::reinitElem(const Elem * /*elem*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitElemPhys(const Elem * /*elem*/,
                              const std::vector<Point> & /*phys_points_in_elem*/,
                              THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitElemFace(const Elem * /*elem*/,
                              unsigned int /*side*/,
                              BoundaryID /*bnd_id*/,
                              THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitNode(const Node * /*node*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitNodeFace(const Node * /*node*/, BoundaryID /*bnd_id*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitNodes(const std::vector<dof_id_type> & /*nodes*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitNodesNeighbor(const std::vector<dof_id_type> & /*nodes*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitNeighbor(const Elem * /*elem*/, unsigned int /*side*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitNeighborPhys(const Elem * /*neighbor*/,
                                  unsigned int /*neighbor_side*/,
                                  const std::vector<Point> & /*physical_points*/,
                                  THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitNeighborPhys(const Elem * /*neighbor*/,
                                  const std::vector<Point> & /*physical_points*/,
                                  THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::reinitNodeNeighbor(const Node * /*node*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::reinitScalars(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::reinitOffDiagScalars(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::getDiracElements(std::set<const Elem *> & /*elems*/)
{
  // Nothing to do
}

void
MortarProblem::clearDiracInfo()
{
  // Nothing to do
}

void MortarProblem::addResidual(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::addResidualNeighbor(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::cacheResidual(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::cacheResidualNeighbor(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::addCachedResidual(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::addCachedResidualDirectly(NumericVector<Number> & /*residual*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::setResidual(NumericVector<Number> & /*residual*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::setResidualNeighbor(NumericVector<Number> & /*residual*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::addJacobian(SparseMatrix<Number> & /*jacobian*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::addJacobianNonlocal(SparseMatrix<Number> & /*jacobian*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::addJacobianNeighbor(SparseMatrix<Number> & /*jacobian*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::cacheJacobian(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::cacheJacobianNonlocal(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void MortarProblem::cacheJacobianNeighbor(THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::addCachedJacobian(SparseMatrix<Number> & /*jacobian*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::addJacobianBlock(SparseMatrix<Number> & /*jacobian*/,
                                unsigned int /*ivar*/,
                                unsigned int /*jvar*/,
                                const DofMap & /*dof_map*/,
                                std::vector<dof_id_type> & /*dof_indices*/,
                                THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::addJacobianBlockNonlocal(SparseMatrix<Number> & /*jacobian*/,
                                        unsigned int /*ivar*/,
                                        unsigned int /*jvar*/,
                                        const DofMap & /*dof_map*/,
                                        const std::vector<dof_id_type> & /*idof_indices*/,
                                        const std::vector<dof_id_type> & /*jdof_indices*/,
                                        THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::addJacobianNeighbor(SparseMatrix<Number> & /*jacobian*/,
                                   unsigned int /*ivar*/,
                                   unsigned int /*jvar*/,
                                   const DofMap & /*dof_map*/,
                                   std::vector<dof_id_type> & /*dof_indices*/,
                                   std::vector<dof_id_type> & /*neighbor_dof_indices*/,
                                   THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::prepareShapes(unsigned int /*var*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::prepareFaceShapes(unsigned int /*var*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

void
MortarProblem::prepareNeighborShapes(unsigned int /*var*/, THREAD_ID /*tid*/)
{
  // Nothing to do
}

GeometricSearchData &
MortarProblem::geomSearchData()
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->geomSearchData();
}

Assembly &
MortarProblem::assembly(THREAD_ID tid)
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->assembly(tid);
}

void
MortarProblem::updateGeomSearch(GeometricSearchData::GeometricSearchType type)
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  _fe_problem->geomSearchData().update(type);
}

void
MortarProblem::meshChanged()
{
  // mesh changed
  _mesh.meshChanged();
}

void MortarProblem::addGhostedElem(dof_id_type /*elem_id*/)
{
  // Nothing to do
}

void MortarProblem::addGhostedBoundary(BoundaryID /*boundary_id*/)
{
  // Nothing to do
}

void
MortarProblem::ghostGhostedBoundaries()
{
  // Nothing to do
}

void
MortarProblem::solve()
{
  // Nothing to do
}

bool
MortarProblem::converged()
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->converged();
}

bool
MortarProblem::computingInitialResidual()
{
  mooseAssert("_fe_problem", "FEProblem pointer is nullptr");
  return _fe_problem->computingInitialResidual();
}

void
MortarProblem::onTimestepBegin()
{
  // Nothing to do
}

void
MortarProblem::onTimestepEnd()
{
  // Nothing to do
}

Void
MortarProblem::addLowerDimensionalElements()
{
  MeshBase & mesh = _mesh.getMesh();

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

  const auto slave_ids = _mesh.getBoundaryIDs(_slave);
  const auto master_ids = _mesh.getBoundaryIDs(_master);

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
MortarProblem::loopBody(const ElemSideBCTriple & triple, std::set<subdomain_id_type> & added_ids)
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

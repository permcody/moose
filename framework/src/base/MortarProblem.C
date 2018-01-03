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
  params.addPrivateParam<std::vector<std::string>>("displacements");
  return params;
}

MortarProblem::MortarProblem(const InputParameters & parameters)
  : SubProblem(parameters),
    _fe_problem(parameters.have_parameter<FEProblemBase *>("_fe_problem_base")
                    ? *getParam<FEProblemBase *>("_fe_problem_base")
                    : *getParam<FEProblem *>("_fe_problem")),
    _mesh(*getParam<MooseMesh *>("mesh")),
    _eq(_fe_problem.es())
{
}

bool
MortarProblem::isTransient() const
{
  return _fe_problem.isTransient();
}

Moose::CoordinateSystemType
MortarProblem::getCoordSystem(SubdomainID sid)
{
  return _fe_problem.getCoordSystem(sid);
}

std::set<dof_id_type> &
MortarProblem::ghostedElems()
{
  return _fe_problem.ghostedElems();
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
  return _fe_problem.hasVariable(var_name);
}

MooseVariable &
MortarProblem::getVariable(THREAD_ID tid, const std::string & var_name)
{
  return _fe_problem.getVariable(tid, var_name);
}

bool
MortarProblem::hasScalarVariable(const std::string & var_name)
{
  return _fe_problem.hasScalarVariable(var_name);
}

MooseVariableScalar &
MortarProblem::getScalarVariable(THREAD_ID tid, const std::string & var_name)
{
  return _fe_problem.getScalarVariable(tid, var_name);
}

System &
MortarProblem::getSystem(const std::string & var_name)
{
  return _fe_problem.getSystem(var_name);
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
  return _fe_problem.geomSearchData();
}

Assembly &
MortarProblem::assembly(THREAD_ID tid)
{
  return _fe_problem.assembly(tid);
}

void
MortarProblem::updateGeomSearch(GeometricSearchData::GeometricSearchType type)
{
  _fe_problem.geomSearchData().update(type);
}

void
MortarProblem::meshChanged()
{
  // mesh changed
  _eq.reinit();
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
  return _fe_problem.converged();
}

bool
MortarProblem::computingInitialResidual()
{
  return _fe_problem.computingInitialResidual();
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

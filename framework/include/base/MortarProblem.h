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

#ifndef MORTARPROBLEM_H
#define MORTARPROBLEM_H

#include "SubProblem.h"
#include "DisplacedSystem.h"
#include "GeometricSearchData.h"
#include "AutomaticMortarGeneration.h"

// libMesh
#include "libmesh/equation_systems.h"
#include "libmesh/enum_quadrature_type.h"

// Forward declarations
class MooseVariable;
class AssemblyData;
class MortarProblem;
class MooseMesh;
class Assembly;
class FEProblemBase;

// libMesh forward declarations
namespace libMesh
{
template <typename T>
class NumericVector;
}

template <>
InputParameters validParams<MortarProblem>();

class MortarProblem : public SubProblem
{
public:
  MortarProblem(const InputParameters & parameters);

  // Used to temporarily store information about which lower-dimensional
  // sides to add and what subdomain id to use for the added sides.
  struct ElemSideBCTriple
  {
    ElemSideBCTriple(Elem * elem_in, unsigned short int side_in, boundary_id_type bc_id_in)
      : elem(elem_in), side(side_in), bc_id(bc_id_in)
    {
    }

    Elem * elem;
    unsigned short int side;
    boundary_id_type bc_id;
  };

  virtual EquationSystems & es() override
  {
    mooseAssert(_eq, "EquationSystems is nullptr");
    return *_eq;
  }
  virtual MooseMesh & mesh() override { return _mesh; }
  MooseMesh & refMesh();

  /**
   * Whether or not this problem should utilize FE shape function caching.
   *
   * @param fe_cache True for using the cache false for not.
   */
  virtual void useFECache(bool fe_cache) override;

  virtual void init() override;
  virtual void solve() override;
  virtual bool converged() override;

  /**
   * Copy the solutions on the undisplaced systems to the displaced systems and
   * reinitialize the geometry search data and Dirac kernel information due to mesh displacement.
   */
  virtual void updateMesh();

  /**
   * Synchronize the solutions on the displaced systems to the given solutions and
   * reinitialize the geometry search data and Dirac kernel information due to mesh displacement.
   */
  virtual void updateMesh(const NumericVector<Number> & soln,
                          const NumericVector<Number> & aux_soln);

  virtual bool isTransient() const override;
  virtual Moose::CoordinateSystemType getCoordSystem(SubdomainID sid) override;

  // Variables /////
  virtual bool hasVariable(const std::string & var_name) override;
  virtual MooseVariable & getVariable(THREAD_ID tid, const std::string & var_name) override;
  virtual bool hasScalarVariable(const std::string & var_name) override;
  virtual MooseVariableScalar & getScalarVariable(THREAD_ID tid,
                                                  const std::string & var_name) override;
  virtual System & getSystem(const std::string & var_name) override;

  virtual void addVariable(const std::string & var_name,
                           const FEType & type,
                           Real scale_factor,
                           const std::set<SubdomainID> * const active_subdomains = NULL);
  virtual void addAuxVariable(const std::string & var_name,
                              const FEType & type,
                              const std::set<SubdomainID> * const active_subdomains = NULL);
  virtual void addScalarVariable(const std::string & var_name,
                                 Order order,
                                 Real scale_factor = 1.,
                                 const std::set<SubdomainID> * const active_subdomains = NULL);
  virtual void addAuxScalarVariable(const std::string & var_name,
                                    Order order,
                                    Real scale_factor = 1.,
                                    const std::set<SubdomainID> * const active_subdomains = NULL);

  // Adaptivity /////
  virtual void initAdaptivity();
  virtual void meshChanged() override;

  // reinit /////
  virtual void prepare(const Elem * elem, THREAD_ID tid) override;
  virtual void prepareNonlocal(THREAD_ID tid);
  virtual void prepareFace(const Elem * elem, THREAD_ID tid) override;
  virtual void prepare(const Elem * elem,
                       unsigned int ivar,
                       unsigned int jvar,
                       const std::vector<dof_id_type> & dof_indices,
                       THREAD_ID tid) override;
  virtual void setCurrentSubdomainID(const Elem * elem, THREAD_ID tid) override;
  virtual void setNeighborSubdomainID(const Elem * elem, unsigned int side, THREAD_ID tid) override;
  virtual void prepareBlockNonlocal(unsigned int ivar,
                                    unsigned int jvar,
                                    const std::vector<dof_id_type> & idof_indices,
                                    const std::vector<dof_id_type> & jdof_indices,
                                    THREAD_ID tid);
  virtual void prepareAssembly(THREAD_ID tid) override;
  virtual void prepareAssemblyNeighbor(THREAD_ID tid);

  virtual bool reinitDirac(const Elem * elem, THREAD_ID tid) override;
  virtual void reinitElem(const Elem * elem, THREAD_ID tid) override;
  virtual void reinitElemPhys(const Elem * elem,
                              const std::vector<Point> & phys_points_in_elem,
                              THREAD_ID tid) override;
  virtual void
  reinitElemFace(const Elem * elem, unsigned int side, BoundaryID bnd_id, THREAD_ID tid) override;
  virtual void reinitNode(const Node * node, THREAD_ID tid) override;
  virtual void reinitNodeFace(const Node * node, BoundaryID bnd_id, THREAD_ID tid) override;
  virtual void reinitNodes(const std::vector<dof_id_type> & nodes, THREAD_ID tid) override;
  virtual void reinitNodesNeighbor(const std::vector<dof_id_type> & nodes, THREAD_ID tid) override;
  virtual void reinitNeighbor(const Elem * elem, unsigned int side, THREAD_ID tid) override;
  virtual void reinitNeighborPhys(const Elem * neighbor,
                                  unsigned int neighbor_side,
                                  const std::vector<Point> & physical_points,
                                  THREAD_ID tid) override;
  virtual void reinitNeighborPhys(const Elem * neighbor,
                                  const std::vector<Point> & physical_points,
                                  THREAD_ID tid) override;
  virtual void reinitNodeNeighbor(const Node * node, THREAD_ID tid) override;
  virtual void reinitScalars(THREAD_ID tid) override;
  virtual void reinitOffDiagScalars(THREAD_ID tid) override;

  /// Fills "elems" with the elements that should be looped over for Dirac Kernels
  virtual void getDiracElements(std::set<const Elem *> & elems) override;
  virtual void clearDiracInfo() override;

  virtual void addResidual(THREAD_ID tid) override;
  virtual void addResidualNeighbor(THREAD_ID tid) override;

  virtual void cacheResidual(THREAD_ID tid) override;
  virtual void cacheResidualNeighbor(THREAD_ID tid) override;
  virtual void addCachedResidual(THREAD_ID tid) override;

  virtual void addCachedResidualDirectly(NumericVector<Number> & residual, THREAD_ID tid);

  virtual void setResidual(NumericVector<Number> & residual, THREAD_ID tid) override;
  virtual void setResidualNeighbor(NumericVector<Number> & residual, THREAD_ID tid) override;

  virtual void addJacobian(SparseMatrix<Number> & jacobian, THREAD_ID tid) override;
  virtual void addJacobianNonlocal(SparseMatrix<Number> & jacobian, THREAD_ID tid);
  virtual void addJacobianNeighbor(SparseMatrix<Number> & jacobian, THREAD_ID tid) override;
  virtual void addJacobianBlock(SparseMatrix<Number> & jacobian,
                                unsigned int ivar,
                                unsigned int jvar,
                                const DofMap & dof_map,
                                std::vector<dof_id_type> & dof_indices,
                                THREAD_ID tid) override;
  virtual void addJacobianBlockNonlocal(SparseMatrix<Number> & jacobian,
                                        unsigned int ivar,
                                        unsigned int jvar,
                                        const DofMap & dof_map,
                                        const std::vector<dof_id_type> & idof_indices,
                                        const std::vector<dof_id_type> & jdof_indices,
                                        THREAD_ID tid);
  virtual void addJacobianNeighbor(SparseMatrix<Number> & jacobian,
                                   unsigned int ivar,
                                   unsigned int jvar,
                                   const DofMap & dof_map,
                                   std::vector<dof_id_type> & dof_indices,
                                   std::vector<dof_id_type> & neighbor_dof_indices,
                                   THREAD_ID tid) override;

  virtual void cacheJacobian(THREAD_ID tid) override;
  virtual void cacheJacobianNonlocal(THREAD_ID tid);
  virtual void cacheJacobianNeighbor(THREAD_ID tid) override;
  virtual void addCachedJacobian(SparseMatrix<Number> & jacobian, THREAD_ID tid) override;

  virtual Assembly & assembly(THREAD_ID tid) override;
  virtual void prepareShapes(unsigned int var, THREAD_ID tid) override;
  virtual void prepareFaceShapes(unsigned int var, THREAD_ID tid) override;
  virtual void prepareNeighborShapes(unsigned int var, THREAD_ID tid) override;

  // Geom Search /////
  virtual void updateGeomSearch(
      GeometricSearchData::GeometricSearchType type = GeometricSearchData::ALL) override;

  virtual GeometricSearchData & geomSearchData() override;

  virtual bool computingInitialResidual() override;

  virtual void onTimestepBegin() override;
  virtual void onTimestepEnd() override;

  /**
   * Return the list of elements that should have their DoFs ghosted to this processor.
   * @return The list
   */
  virtual std::set<dof_id_type> & ghostedElems() override;

  /**
   * Will make sure that all dofs connected to elem_id are ghosted to this processor
   */
  virtual void addGhostedElem(dof_id_type elem_id) override;

  /**
   * Will make sure that all necessary elements from boundary_id are ghosted to this processor
   * @param boundary_id Boundary ID
   */
  virtual void addGhostedBoundary(BoundaryID boundary_id) override;

  /**
   * Causes the boundaries added using addGhostedBoundary to actually be ghosted.
   */
  virtual void ghostGhostedBoundaries() override;

  const AutomaticMortarGeneration & getAMG() const { return _amg; }

protected:
  void addLowerDimensionalElements();

  const Elem * loopBody(const ElemSideBCTriple & triple, std::set<subdomain_id_type> & added_ids);

  MooseMesh & _mesh;
  FEProblemBase * _fe_problem;
  EquationSystems * _eq;

  AutomaticMortarGeneration _amg;
};

#endif /* MORTARPROBLEM_H */

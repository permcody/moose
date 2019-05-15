//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "DGKernelBase.h"

#include "MooseVariableScalar.h"

class ArrayDGKernel;

template <>
InputParameters validParams<ArrayDGKernel>();

/**
 * The DGKernel class is responsible for calculating the residuals for various
 * physics on internal sides (edges/faces).
 */
class ArrayDGKernel : public DGKernelBase, public NeighborMooseVariableInterface<RealEigenVector>
{
public:
  /**
   * Factory constructor initializes all internal references needed for residual computation.
   *
   *
   * @param parameters The parameters object for holding additional parameters for kernels and
   * derived kernels
   */
  ArrayDGKernel(const InputParameters & parameters);

  /**
   * The variable that this kernel operates on.
   */
  virtual MooseVariableFEBase & variable() override { return _var; }

  /**
   * Computes the residual for this element or the neighbor
   */
  virtual void computeElemNeighResidual(Moose::DGResidualType type) override;

  /**
   * Computes the element/neighbor-element/neighbor Jacobian
   */
  virtual void computeElemNeighJacobian(Moose::DGJacobianType type) override;

  /**
   * Computes d-residual / d-jvar...
   */
  virtual void computeOffDiagJacobian(unsigned int jvar) override;

  /**
   * Computes the element-element off-diagonal Jacobian
   */
  virtual void computeOffDiagElemNeighJacobian(Moose::DGJacobianType type,
                                               unsigned int jvar) override;

protected:
  /**
   * This is the virtual that derived classes should override for computing the residual on
   * neighboring element.
   */
  virtual RealEigenVector computeQpResidual(Moose::DGResidualType type) = 0;

  /**
   * This is the virtual that derived classes should override for computing the Jacobian on
   * neighboring element.
   */
  virtual RealEigenVector computeQpJacobian(Moose::DGJacobianType type) = 0;

  /**
   * This is the virtual that derived classes should override for computing the off-diag Jacobian.
   */
  virtual RealArray computeQpOffDiagJacobian(Moose::DGJacobianType type, MooseVariableFEBase & jvar)
  {
    if (jvar.number() == _var.number())
    {
      RealEigenVector v = computeQpJacobian(type);
      RealArray t = RealArray::Zero(_var.count(), _var.count());
      t.diagonal() = v;
      return t;
    }
    else
      return RealArray::Zero(_var.count(), jvar.count());
  }

  /// for array kernel
  void saveLocalArrayResidual(DenseVector<Number> & re,
                              unsigned int i,
                              unsigned int ntest,
                              const RealEigenVector & v)
  {
    for (unsigned int j = 0; j < v.size(); ++j, i += ntest)
      re(i) += v(j);
  }
  void saveDiagLocalArrayJacobian(DenseMatrix<Number> & ke,
                                  unsigned int i,
                                  unsigned int ntest,
                                  unsigned int j,
                                  unsigned int nphi,
                                  const RealEigenVector & v)
  {
    for (unsigned int k = 0; k < v.size(); ++k, i += ntest, j += nphi)
      ke(i, j) += v(k);
  }
  void saveFullLocalArrayJacobian(DenseMatrix<Number> & ke,
                                  unsigned int i,
                                  unsigned int ntest,
                                  unsigned int j,
                                  unsigned int nphi,
                                  const RealArray & v)
  {
    unsigned int saved_j = j;
    for (unsigned int k = 0; k < v.rows(); ++k, i += ntest)
    {
      j = saved_j;
      for (unsigned int l = 0; l < v.cols(); ++l, j += nphi)
        ke(i, j) += v(k, l);
    }
  }

  /// Variable this kernel operates on
  ArrayMooseVariable & _var;
  /// Holds the current solution at the current quadrature point on the face.
  const ArrayVariableValue & _u;
  /// Holds the current solution gradient at the current quadrature point on the face.
  const ArrayVariableGradient & _grad_u;
  /// Shape functions
  const ArrayVariablePhiValue & _phi;
  /// Gradient of shape function
  const ArrayVariablePhiGradient & _grad_phi;
  const MappedArrayVariablePhiGradient & _array_grad_phi;
  /// test functions
  const ArrayVariableTestValue & _test;
  /// Gradient of side shape function
  const ArrayVariableTestGradient & _grad_test;
  const MappedArrayVariablePhiGradient & _array_grad_test;
  /// Side shape function
  const ArrayVariablePhiValue & _phi_neighbor;
  /// Gradient of side shape function
  const ArrayVariablePhiGradient & _grad_phi_neighbor;
  const MappedArrayVariablePhiGradient & _array_grad_phi_neighbor;
  /// Side test function
  const ArrayVariableTestValue & _test_neighbor;
  /// Gradient of side shape function
  const ArrayVariableTestGradient & _grad_test_neighbor;
  const MappedArrayVariablePhiGradient & _array_grad_test_neighbor;
  /// Holds the current solution at the current quadrature point
  const ArrayVariableValue & _u_neighbor;
  /// Holds the current solution gradient at the current quadrature point
  const ArrayVariableGradient & _grad_u_neighbor;
  /// Normals in type of Eigen map
  const std::vector<Eigen::Map<RealDIMValue>> & _array_normals;
  /// Number of components of the array variable
  const unsigned int _count;
};

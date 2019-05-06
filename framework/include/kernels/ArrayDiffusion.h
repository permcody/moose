//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "ArrayKernel.h"

class ArrayDiffusion;

template <>
InputParameters validParams<ArrayDiffusion>();

class ArrayDiffusion : public ArrayKernel
{
public:
  ArrayDiffusion(const InputParameters & parameters);

protected:
  virtual RealArrayValue computeQpResidual() override;
  virtual RealArrayValue computeQpJacobian() override;
  virtual RealArray computeQpOffDiagJacobian(MooseVariableFEBase & jvar) override;

  /// diffusion coefficient type
  unsigned int _dc_type;
  /// scalar diffusion coefficient
  const MaterialProperty<Real> * _d;
  /// array diffusion coefficient
  const MaterialProperty<RealArrayValue> * _d_array;
  /// matrix diffusion coefficient
  const MaterialProperty<RealArray> * _d_2d_array;
};

//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef INTEGRATEDINDEXAUX_H
#define INTEGRATEDINDEXAUX_H

#include "AuxKernel.h"

// Forward Declarations
class IntegratedIndexAux;
class FeatureFloodCount;

template <>
InputParameters validParams<IntegratedIndexAux>();

/**
 * Output euler angles from user object to an AuxVariable based
 * on the larger value of the integrated order parameter and the
 * void concentration.
 */
class IntegratedIndexAux : public AuxKernel

{
public:
  IntegratedIndexAux(const InputParameters & parameters);

  virtual void precalculateValue();
  virtual Real computeValue();

  /**
   * Returns the integrated index for the given element ID.
   */
  //int getIntegratedIndex(unsigned int elem_id) const;

protected:
  /// A Boolean indicating if void phase is integrated
  const bool _consider_voids;

  /// A reference to the feature flood count object
  const FeatureFloodCount & _feature_counter;

  /// Grain tracker object
  //const GrainTracker & _grain_tracker;

  /// Concentration variable
  const VariableValue & _c;

  /// Value to return for points belonging to a void
  const int _void_index;

  /// Grain Index to output
  Real _grain_id;
  //AuxVariableValue & _grain_id
  //std::vector<AuxVariableName> _grain_id;

private:
  Real computeVarIntegral(std::size_t var_index) const;
  Real computeVoidIntegral() const;

  const std::vector<MooseVariableFEBase *> & _vars;
  std::vector<const VariableValue *> _coupled_sln;

  MooseMesh & _mesh;
  Assembly & _assembly;
  const MooseArray<Point> & _q_point;
  QBase *& _qrule;
  const MooseArray<Real> & _JxW;
  const MooseArray<Real> & _coord;
};

#endif // INTEGRATEDINDEXAUX_H

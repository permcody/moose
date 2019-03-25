//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#include "IntegratedIndexAux.h"
#include "Assembly.h"
#include "FeatureFloodCount.h"
#include "GrainTrackerInterface.h"
#include "MooseMesh.h"
#include "libmesh/quadrature.h"

registerMooseObject("PhaseFieldApp", IntegratedIndexAux);

template <>
InputParameters
validParams<IntegratedIndexAux>()
{
  InputParameters params = validParams<AuxKernel>();
  params.addClassDescription(
      "Output integrated feature ID from GrainTracker user object to an AuxVariable."
      "The grain index of the order parameter with the highest integrated value on the"
      "element is output. For the porosity case, the order parameter with the highest"
      "integrated value is compared to the integrated value of the void concentration"
      "to determine the predominant feature to be output.");
  params.addParam<bool>("consider_voids", false, "Set to true to consider datsets with voids");
  params.addRequiredParam<UserObjectName>("flood_counter",
                                          "The FeatureFloodCount UserObject to get values from.");
  params.addCoupledVar("c", 0.0, "Concentration representing voids");
  params.addParam<int>(
      "void_index", -1.0, "Feature index assigned to void regions from input file.");
  return params;
}

IntegratedIndexAux::IntegratedIndexAux(const InputParameters & parameters)
  : AuxKernel(parameters),
    _consider_voids(getParam<bool>("consider_voids")),
    _feature_counter(getUserObject<FeatureFloodCount>("flood_counter")),
    _c(coupledValue("c")),
    _void_index(getParam<int>("void_index")),
    _vars(_feature_counter.getFECoupledVars()),
    _mesh(_subproblem.mesh()),
    _assembly(_subproblem.assembly(_tid)),
    _q_point(_assembly.qPoints()),
    _qrule(_assembly.qRule()),
    _JxW(_assembly.JxW()),
    _coord(_assembly.coordTransformation())
{
  _coupled_sln.reserve(_vars.size());
  for (auto & var : _feature_counter.getCoupledVars())
    _coupled_sln.push_back(&var->sln());
}

void
IntegratedIndexAux::precalculateValue()
{
  // Retrieve the var_to_features vector on the current element
  auto elem_id = _current_elem->id();
  const auto & var_to_features = _feature_counter.getVarToFeatureVector(elem_id);
  unsigned int dominant_feature_id = FeatureFloodCount::invalid_id;
  Real max_var_value = std::numeric_limits<Real>::lowest();

  for (auto var_index = beginIndex(var_to_features); var_index < var_to_features.size();
       ++var_index)
  {
    // Only sample "active" variables
    if (var_to_features[var_index] != FeatureFloodCount::invalid_id)
    {
      auto feature_id = var_to_features[var_index];
      auto var_integral_value = computeVarIntegral(var_index);

      // Find the dominant feature index and associated value for the current element
      if (var_integral_value > max_var_value)
      {
        max_var_value = var_integral_value;
        dominant_feature_id = feature_id;
      }
    }
  }
  if (_consider_voids)
  {
    // Integrate void concentration on the current element
    auto void_integral_value = computeVoidIntegral();

    // Output void_index if integrated void cencentration is greater than the order parameter
    if (void_integral_value > max_var_value)
      _grain_id = _void_index;
    else
      _grain_id = dominant_feature_id;
    // Moose::out << "current_elem: " << _current_elem->id() << " ,void_integral_value: " <<
    // void_integral_value <<  " ,max_var_value: " << max_var_value << " ,grain_id_out: " <<
    // _grain_id << std::endl;
  }
  else
  {
    _grain_id = dominant_feature_id;
  }
}

Real
IntegratedIndexAux::computeValue()
{
  Moose::out << "Elem: " << _current_elem->id() << " , Integrated Index should print first!"
             << std::endl;
  return _grain_id;
}
/*
int
IntegratedIndexAux::getIntegratedIndex(unsigned int elem_id) const
{
  mooseAssert(elem_id < _grain_id.size(), "feature_id is out of range");
  return _grain_id[elem_id];
}
*/

Real
IntegratedIndexAux::computeVarIntegral(std::size_t var_index) const
{
  Real sum = 0;

  for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
    sum += _JxW[qp] * _coord[qp] * (*_coupled_sln[var_index])[qp];

  return sum;
}

Real
IntegratedIndexAux::computeVoidIntegral() const
{
  Real sum = 0;

  for (unsigned int qp = 0; qp < _qrule->n_points(); ++qp)
    sum += _JxW[qp] * _coord[qp] * _c[qp];

  return sum;
}

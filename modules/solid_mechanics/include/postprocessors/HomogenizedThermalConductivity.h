// See
// Homogenization of Temperature-Dependent Thermal Conductivity in Composite
// Materials, Journal of Thermophysics and Heat Transfer, Vol. 15, No. 1,
// January-March 2001.


#ifndef HOMOGENIZEDTHERMALCONDUCTIVITY_H
#define HOMOGENIZEDTHERMALCONDUCTIVITY_H

#include "ElementAverageValue.h"

class HomogenizedThermalConductivity : public ElementAverageValue
{
public:
  HomogenizedThermalConductivity(const std::string & name, InputParameters parameters);

  virtual void initialize();
  virtual void execute();
  virtual Real getValue();
  virtual void threadJoin(const UserObject & y);

protected:
  virtual Real computeQpIntegral();

private:
  VariableGradient & _grad_temp_x;
  VariableGradient & _grad_temp_y;
  VariableGradient & _grad_temp_z;
  const unsigned int _component;
  MaterialProperty<Real> & _thermal_conductivity;
  Real _volume;
  Real _integral_value;
  const Real _scale;
};

template<>
InputParameters validParams<HomogenizedThermalConductivity>();

#endif //HOMOGENIZEDTHERMALCONDUCTIVITY_H

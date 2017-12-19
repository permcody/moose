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

#ifndef ADDNEWMORTARINTERFACEACTION_H
#define ADDNEWMORTARINTERFACEACTION_H

#include "Action.h"

class AddNewMortarInterfaceAction;

template <>
InputParameters validParams<AddNewMortarInterfaceAction>();

/**
 *
 */
class AddNewMortarInterfaceAction : public Action
{
public:
  AddNewMortarInterfaceAction(InputParameters parameters);

  virtual void act() override;

private:
  std::vector<BoundaryName> _master;
  std::vector<BoundaryName> _slave;
  std::vector<SubdomainName> _subdomain;
};

#endif /* ADDNEWMORTARINTERFACEACTION_H */

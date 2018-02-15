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
#include "AddNewMortarInterfaceAction.h"
#include "FEProblem.h"
#include "MooseMesh.h"
#include "MooseTypes.h"
#include "MortarProblem.h"

template <>
InputParameters
validParams<AddNewMortarInterfaceAction>()
{
  InputParameters params = validParams<Action>();
  params.addRequiredParam<std::vector<BoundaryName>>("master", "Master side ID(s)");
  params.addRequiredParam<std::vector<BoundaryName>>("slave", "Slave side ID(s)");
  params.addRequiredParam<std::vector<SubdomainName>>(
      "subdomain", "Subdomain name (ID) list for the mortar interface");

  return params;
}

AddNewMortarInterfaceAction::AddNewMortarInterfaceAction(InputParameters parameters)
  : Action(parameters),
    _master(getParam<std::vector<BoundaryName>>("master")),
    _slave(getParam<std::vector<BoundaryName>>("slave")),
    _subdomain(getParam<std::vector<SubdomainName>>("subdomain"))
{
  if (_master.size() != _slave.size())
    mooseError("The number of slave and master IDs must match");
  if (_master.size() != _subdomain.size())
    mooseError("The number of subdomain names must match the number of mortar surface pairs");
}

void
AddNewMortarInterfaceAction::act()
{
  if (_current_task == "add_mortar_problem")
  {
    InputParameters object_params = _factory.getValidParams("MortarProblem");
    object_params.set<MooseMesh *>("mesh") = _mesh.get();
    auto mortar_problem =
        _factory.create<MortarProblem>("MortarProblem", "MortarProblem", object_params);

    // TODO: Need to attach this somewhere.
    //_app.addMortarProblem(std::move(mortar_problem));
  }
  //  else if (_current_task = "")
  //
  //    if (_current_task == "add_mesh_modifier")
  //    {
  //      if (_app.isRecovering())
  //        return;
  //
  //      InputParameters params = _factory.getValidParams("MortarMeshGenerator");
  //      params.applyParameters(_pars);
  //
  //      _app.addMeshModifier("MortarMeshGenerator", "_mortar_mesh_generator", params);
  //    }
}

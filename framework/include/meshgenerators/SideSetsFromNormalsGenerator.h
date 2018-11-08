//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#ifndef SIDESETSFROMNORMALSGENERATOR_H
#define SIDESETSFROMNORMALSGENERATOR_H

#include "GenerateSideSetsBase.h"

// Forward declarations
class SideSetsFromNormalsGenerator;

template <>
InputParameters validParams<SideSetsFromNormalsGenerator>();

/**
 *
 */
class SideSetsFromNormalsGenerator : public GenerateSideSetsBase
{
public:
  SideSetsFromNormalsGenerator(const InputParameters & parameters);

  std::unique_ptr<MeshBase> generate();

protected:
  std::unique_ptr<MeshBase> & _input;

  std::vector<BoundaryName> _boundary_names;

  std::vector<Point> _normals;
};

#endif // SIDESETSFROMNORMALSGENERATOR_H

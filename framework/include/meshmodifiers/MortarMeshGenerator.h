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

#ifndef MORTARMESHGENERATOR_H
#define MORTARMESHGENERATOR_H

#include "MeshModifier.h"

// Forward Declaration
class MortarMeshGenerator;

template <>
InputParameters validParams<MortarMeshGenerator>();

class MortarMeshGenerator : public MeshModifier
{
public:
  MortarMeshGenerator(const InputParameters & params);

protected:
  virtual void modify() override;

private:
  void addLowerDimensionalElements();

  // Used to temporarily store information about which lower-dimensional
  // sides to add and what subdomain id to use for the added sides.
  struct ElemSideBCTriple
  {
    ElemSideBCTriple(Elem * elem, unsigned short int side, boundary_id_type bc_id)
      : _elem(elem), _side(side), _bc_id(bc_id)
    {
    }

    Elem * _elem;
    unsigned short int _side;
    boundary_id_type _bc_id;
  };

  const Elem * loopBody(const ElemSideBCTriple & triple, std::set<subdomain_id_type> & added_ids);

  std::vector<BoundaryName> _master;
  std::vector<BoundaryName> _slave;

  // Keeps track of the mapping between lower-dimensional elements and
  // the side_id of the interior_parent which they are.
  std::map<const Elem *, unsigned int> _lower_elem_to_side_id;

  /// Size of the largest slave side lower-dimensional element added to the mesh.
  /// Used in plotting convergence data.
  Real _h_max;
};

#endif // MORTARMESHGENERATOR_H

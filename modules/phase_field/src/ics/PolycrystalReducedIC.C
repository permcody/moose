/****************************************************************/
/* MOOSE - Multiphysics Object Oriented Simulation Environment  */
/*                                                              */
/*          All contents are licensed under LGPL V2.1           */
/*             See LICENSE for full restrictions                */
/****************************************************************/

#include "PolycrystalReducedIC.h"
#include "IndirectSort.h"
#include "MooseRandom.h"
#include "MooseMesh.h"

template<>
InputParameters validParams<PolycrystalReducedIC>()
{
  InputParameters params = validParams<InitialCondition>();
  params.addClassDescription("Random Voronoi tesselation polycrystal (used by PolycrystalVoronoiICAction)");
  params.addRequiredParam<unsigned int>("op_num", "Number of order parameters");
  params.addRequiredParam<unsigned int>("grain_num", "Number of grains being represented by the order parameters");
  params.addRequiredParam<unsigned int>("op_index", "The index for the current order parameter");
  params.addParam<unsigned int>("rand_seed", 12444, "The random seed");
  params.addParam<bool>("cody_test", false, "Use set grain center points for Cody's test. Grain num MUST equal 10");
  params.addParam<bool>("columnar_3D", false, "3D microstructure will be columnar in the z-direction?");
  return params;
}

PolycrystalReducedIC::PolycrystalReducedIC(const InputParameters & parameters) :
    InitialCondition(parameters),
    _mesh(_fe_problem.mesh()),
    _dim(_mesh.dimension()),
    _nl(_fe_problem.getNonlinearSystem()),
    _op_num(getParam<unsigned int>("op_num")),
    _grain_num(getParam<unsigned int>("grain_num")),
    _op_index(getParam<unsigned int>("op_index")),
    _rand_seed(getParam<unsigned int>("rand_seed")),
    _columnar_3D(getParam<bool>("columnar_3D"))
{
}

void
PolycrystalReducedIC::initialSetup()
{
 //Set up domain bounds with mesh tools
  for (unsigned int i = 0; i < LIBMESH_DIM; i++)
  {
    _bottom_left(i) = _mesh.getMinInDimension(i);
    _top_right(i) = _mesh.getMaxInDimension(i);
  }
  _range = _top_right - _bottom_left;

  if (_op_num > _grain_num)
     mooseError("ERROR in PolycrystalReducedIC: Number of order parameters (op_num) can't be larger than the number of grains (grain_num)");

  MooseRandom::seed(_rand_seed);

  //Randomly generate the centers of the individual grains represented by the Voronoi tesselation
  _centerpoints.resize(_grain_num);
  _assigned_op.resize(_grain_num);
  std::vector<Real> distances(_grain_num);

  //Assign actual center point positions
  for (unsigned int grain = 0; grain < _grain_num; grain++)
  {
    for (unsigned int i = 0; i < LIBMESH_DIM; i++)
      _centerpoints[grain](i) = _bottom_left(i) + _range(i) * MooseRandom::rand();
    if (_columnar_3D)
      _centerpoints[grain](2) = _bottom_left(2) + _range(2) * 0.5;
  }

  // Build node to elem map
  std::vector< std::vector< const Elem * > > nodes_to_elem_map;
  MeshTools::build_nodes_to_elem_map(_mesh.getMesh(), nodes_to_elem_map);

  // Build node to grain Map
  std::map<dof_id_type, unsigned int> node_to_grain;
  const MeshBase::node_iterator end = _mesh.getMesh().active_nodes_end();
  for (MeshBase::node_iterator nl = _mesh.getMesh().active_nodes_begin(); nl != end; ++nl)
  {
    unsigned int grain_index = PolycrystalICTools::assignPointToGrain(**nl, _centerpoints, _mesh, _var, _range.norm());

    node_to_grain.insert(std::pair<dof_id_type, unsigned int>((*nl)->id(), grain_index));
  }

//  for (std::map<dof_id_type, unsigned int>::const_iterator it = node_to_grain.begin(); it != node_to_grain.end(); ++it)
//    std::cout << it->first << ": " << it->second << '\n';

  // Build neighbor graph
  std::vector<std::vector<bool> > adjacency_matrix(_grain_num);
  for (unsigned int i = 0; i < _grain_num; ++i)
    adjacency_matrix[i].resize(_grain_num, false);

  for (MeshBase::node_iterator nl = _mesh.getMesh().active_nodes_begin(); nl != end; ++nl)
  {
    const Node * node = *nl;
    std::map<dof_id_type, unsigned int>::const_iterator grain_it = node_to_grain.find(node->id());
    mooseAssert(grain_it != node_to_grain.end(), "Node not found in map");
    unsigned int my_grain = grain_it->second;

    std::vector<const Node *> nodal_neighbors;
    MeshTools::find_nodal_neighbors(_mesh.getMesh(), *node, nodes_to_elem_map, nodal_neighbors);

    // Loop over all nodal neighbors
    for (unsigned int i = 0; i < nodal_neighbors.size(); ++i)
    {
      const Node * neighbor_node = nodal_neighbors[i];
      std::map<dof_id_type, unsigned int>::const_iterator grain_it2 = node_to_grain.find(neighbor_node->id());
      mooseAssert(grain_it2 != node_to_grain.end(), "Node not found in map");
      unsigned int their_grain = grain_it2->second;

      if (my_grain != their_grain)
        adjacency_matrix[my_grain][their_grain] = true;

    }
  }

  std::cout << "Output graph edges:\n";
  for (unsigned int i = 0; i < adjacency_matrix.size(); ++i)
  {
    for (unsigned int j = 0; j < adjacency_matrix.size(); ++j)
      std::cout << adjacency_matrix[i][j] << ' ';
    std::cout << '\n';
  }

  // Assign colors (op vars)
  std::vector<int> op_var_num(_grain_num, -1);
  short status = assignColors(adjacency_matrix, op_var_num);

  if (status == 0)
    for (unsigned int i = 0; i < _grain_num; ++i)
      std::cout << i << ": " << op_var_num[i] << '\n';

  //Assign grains to specific order parameters in a way that maximizes the distance
  _assigned_op = PolycrystalICTools::assignPointsToVariables(_centerpoints, _op_num, _mesh, _var);
}

short
PolycrystalReducedIC::assignColors(const std::vector<std::vector<bool> > & adjacency_matrix, std::vector<int> & colors)
{
  // Start by assigning grain zero the first color
  short status = -1;

  colors[0] = 0;
  for (unsigned int j = 0; j < _grain_num && status; ++j)
    if (adjacency_matrix[0][j])
      status = assignColorHelper(adjacency_matrix, colors, j, 1);

  return status;
}

short
PolycrystalReducedIC::assignColorHelper(const std::vector<std::vector<bool> > & adjacency_matrix, std::vector<int> & colors, unsigned int grain, unsigned int assigned)
{
  short status = -1;

  for (unsigned int my_color = 0; my_color < _op_num && status; ++my_color)
  {
    // Make sure no neighbors have this color
    bool conflicting_color = false;
    for (unsigned int neighbor_idx = 0; neighbor_idx < _grain_num; ++neighbor_idx)
      if (adjacency_matrix[grain][neighbor_idx] && colors[neighbor_idx] == my_color)
      {
        conflicting_color = true;
        break;
      }

    if (conflicting_color)
      continue;

    // Assign a color
    colors[grain] = my_color;

    // Are we done?
    if (assigned + 1 == _grain_num)
      return 0;

    // Find an uncolored neighbor and recurse
    for (unsigned int neighbor_idx = 0; neighbor_idx < _grain_num && status; ++neighbor_idx)
      if (adjacency_matrix[grain][neighbor_idx] && colors[neighbor_idx] == -1)
      {
        status = assignColorHelper(adjacency_matrix, colors, neighbor_idx, assigned+1);
        if (status == 0)
          return 0;
      }

    colors[grain] = -1;
  }

  return -1;
}

Real
PolycrystalReducedIC::value(const Point & p)
{
  Real val = 0.0;

  unsigned int min_index = PolycrystalICTools::assignPointToGrain(p, _centerpoints, _mesh, _var, _range.norm());

  //If the current order parameter index (_op_index) is equal to the min_index, set the value to 1.0
  if (_assigned_op[min_index] == _op_index) //Make sure that the _op_index goes from 0 to _op_num-1
    val = 1.0;

  if (val > 1.0)
    val = 1.0;

  if (val < 0.0)
    val = 0.0;

  return val;
}

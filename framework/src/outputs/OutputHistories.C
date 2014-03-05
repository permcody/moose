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

// Moose includes
#include "OutputHistories.h"
#include "LayeredBase.h"
#include "InfixIterator.h"

template<>
InputParameters validParams<OutputHistories>()
{
  // Get the parameters from the parent object
  InputParameters params = validParams<OutputBase>();
  params += validParams<FileOutputInterface>();

  params.addRequiredParam<std::vector<UserObjectName> >("layered_average_objects", "The LayeredAverage objects to get values from.");
  
  // Suppress unused parameters
  params.suppressParameter<unsigned int>("padding");

  return params;
}

OutputHistories::OutputHistories(const std::string & name, InputParameters & parameters) :
    OutputBase(name, parameters),
    FileOutputInterface(name, parameters),
    UserObjectInterface(parameters),
    _stream_open(false)
{
  _names = getParam<std::vector<UserObjectName> >("layered_average_objects");

  _line_average_objects.resize(_names.size());
  for (unsigned int i=0; i<_names.size(); ++i)
  {
    _line_average_objects[i] = dynamic_cast<const LayeredBase *>(&getUserObjectBaseByName(_names[i]));
    mooseAssert(_line_average_objects[i], "UserObject is NULL");
  }
}

OutputHistories::~OutputHistories()
{
}

std::string
OutputHistories::filename()
{
  return _file_base + ".csv";
}

void
OutputHistories::output()
{
  // We only want to do file I/O on processor zero
  if (libMesh::processor_id() != 0)
    return;
  
  if (!_stream_open)
  {
    _output_file.open(filename().c_str(), std::ios::trunc);
    _stream_open = true;
  }

  _output_file << "Time Step: " << _t_step << "\n";
  
  // Output the positions
  // ASSUMPTION: All coupled layered objects will use the same positions.  This can be changed later
  const std::vector<Point> & points = _line_average_objects[0]->getLayerCenters();

  // Join the point coordinates together with a different delimiter
  _output_file << "Positions:\n";
  std::vector<std::string> joined_points(points.size());
  for (unsigned int i=0; i<points.size(); ++i)
  {
    std::ostringstream oss;
    oss << points[i](0) << "~" << points[i](1) << "~" << points[i](2);
    joined_points[i] = oss.str();
  }

  std::copy(joined_points.begin(), joined_points.end(), infix_ostream_iterator<std::string>(_output_file, ", "));
  _output_file << "\n";

  // Output the coupled Layered Averages
  for (unsigned int i=0; i<_line_average_objects.size(); ++i)
  {
    _output_file << _names[i] << ":\n";  
    const std::vector<Real> & values = _line_average_objects[i]->getLayerValues();
    std::copy(values.begin(), values.end(), infix_ostream_iterator<Real>(_output_file, ", "));
    _output_file << "\n";
  }
  
  _output_file << "\n";
}

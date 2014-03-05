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

#ifndef OUTPUTHISTORIES_H
#define OUTPUTHISTORIES_H

// MOOSE includes
#include "OutputBase.h"
#include "FileOutputInterface.h"
#include "UserObjectInterface.h"

#include <fstream>

// Forward declerations
class OutputHistories;
class LayeredBase;

template<>
InputParameters validParams<OutputHistories>();

class OutputHistories :
  public OutputBase,
  public FileOutputInterface,
  public UserObjectInterface
{
public:

  /**
   * Class constructor
   */
  OutputHistories(const std::string & name, InputParameters & parameters);

  /**
   * Class destructor
   */
  virtual ~OutputHistories();

  /**
   * Output the histories table to a *.csv file
   */
  virtual void output();

  /**
   * The filename for the output file
   * @return A string of output file including the extension
   */
  virtual std::string filename();

protected:
  virtual void outputNodalVariables() {};
  virtual void outputElementalVariables() {};
  virtual void outputScalarVariables() {};
  virtual void outputPostprocessors() {};

  std::vector<const LayeredBase *> _line_average_objects;
  std::vector<UserObjectName> _names;

  std::ofstream _output_file;
  bool _stream_open;
};

#endif /* OUTPUTHISTORIES_H */

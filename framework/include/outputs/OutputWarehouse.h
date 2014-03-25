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

#ifndef OUTPUTWAREHOUSE_H
#define OUTPUTWAREHOUSE_H

// Standard includes
#include <vector>

// MOOSE includes
#include "InputParameters.h"

// Forward declarations
class OutputBase;
class Checkpoint;

/**
 * Class for storing and utilizing output objects
 */
class OutputWarehouse
{
public:

  /**
   * Class constructor
   */
  OutputWarehouse();

  /*
   * Class destructor
   * The OutputWarehouse deletes all output objects passed in via addOutput
   */
  virtual ~OutputWarehouse();

  /**
   * Adds an existing output object to the warehouse
   * @param output Pointer to the output object
   * It is the responsibility of the OutputWarehouse to delete the output objects
   * add using this method
   */
  void addOutput(OutputBase * output);

  /**
   * Returns true if the output object exists
   * @param name The name of the output object for which to test for existence within the warehouse
   */
  bool hasOutput(std::string name);

  /**
   * Calls the outputInitial method for each of the output objects
   */
  void outputInitial();

  /**
   * Calls the outputFailedStep method for each output object
   */
  void outputFailedStep();

  /**
   * Calls the outputStep method for each output object
   */
  void outputStep();

  /**
   * Calls the outputFinal method for each output object
   */
  void outputFinal();

  /**
   * Calls the meshChanged method for every output object
   */
  void meshChanged();

  /**
   * Calls the allowOutput method for every output object
   */
  void allowOutput(bool state);

  /**
   * Calls the forceOutput method for every output object
   */
  void forceOutput();

  /**
   * Calls the setFileNumber method for every FileOutputter output object
   */
  void setFileNumbers(std::map<std::string, unsigned int> input);

  /**
   * Extracts the file numbers from the outputters
   * @return Map of file numbers for the output objects
   */
  std::map<std::string, unsigned int> getFileNumbers();

  /**
   * Stores the common InputParameters object
   * @param params_ptr A pointer to the common parameters object to be stored
   *
   * @see CommonOutputAction
   */
  void setCommonParameters(InputParameters * params_ptr);

  /**
   * Get a reference to the common output parameters
   * @return Reference to the common InputParameters object
   */
  InputParameters & getCommonParameters();

  /**
   * Return the sync times for all objects
   */
  std::set<Real> & getSyncTimes();

protected:

  /// The list of all output objects
  std::vector<OutputBase *> _object_ptrs;

private:

  /**
   * Adds the file name to the list of filenames being output
   * The main function of this object is to test that the same output file
   * does not already exist to protect against output files overwriting each other
   * @param filename Name of an output file (extracted from filename() method of the objects)
   */
  void addOutputFilename(OutFileBase filename);

  /**
   * Calls the initialSetup function for each of the output objects
   * @see FEProblem::initialSetup()
   */
  void initialSetup();

  /**
   * Calls the timestepSetup function for each of the output objects
   * @see FEProblem::timestepSetup()
   */
  void timestepSetup();

  /// List of object names
  std::set<OutFileBase> _filenames;

  /// List of output names
  std::set<std::string> _output_names;

  /// Pointer to the common InputParameters (@see CommonOutputAction)
  InputParameters * _common_params_ptr;

  /// True if multiple Console output objects are added to the warehouse
  bool _has_screen_console;

  /// Sync times for all objects
  std::set<Real> _sync_times;

  /// Input file name for this output object
  std::string _input_file_name;

  // Allow complete access to
  friend class FEProblem;
  friend class TransientMultiApp;
};

#endif // OUTPUTWAREHOUSE_H

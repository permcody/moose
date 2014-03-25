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

#ifndef OUTPUTBASE_H
#define OUTPUTBASE_H

// MOOSE includes
#include "MooseObject.h"
#include "Restartable.h"
#include "MooseTypes.h"
#include "MooseMesh.h"

// libMesh
#include "libmesh/equation_systems.h"
#include "libmesh/numeric_vector.h"
#include "libmesh/mesh_function.h"

// Forward declarations
class Problem;
class OutputBase;

template<>
InputParameters validParams<OutputBase>();

/**
 * A structure for storing the various lists that contain
 * the names of the items to be exported. An instance of this
 * struct exists for each of the output types (non-linear variables,
 * scalar variables, and postprocessors)
 */
struct OutputData
{
  /// A list of all possible outputs
  std::vector<std::string> available;

  /// User-supplied list of outputs to display
  std::vector<std::string> show;

  /// User-supplied list of outputs to hide
  std::vector<std::string> hide;

  /// A list of the outputs to write
  std::vector<std::string> output;
};

/**
 * Based class for output objects
 *
 * Each output class (e.g., Exodus) should inherit from this base class. At a minimum, the pure
 * virtual methods for the various types of output must be defined in the child class.
 *
 * @see Exodus Console CSV
 */
class OutputBase :
  public MooseObject,
  public Restartable
{
public:

  /**
   * Class constructor
   *
   * The constructor performs all of the necessary initialization of the various
   * output lists required for the various output types.
   *
   * @see initAvailable init separate
   */
  OutputBase(const std::string & name, InputParameters & parameters);

  /**
   * Class destructor
   */
  virtual ~OutputBase();

  /**
   * Returns true if any of the other has methods return true.
   */
  virtual bool hasOutput();

  /**
   * Returns true if there exists nodal nonlinear variables for output
   * @return True if nonlinear variable output exists
   * @see getNodalVariableOutput
   */
  bool hasNodalVariableOutput();

  /**
   * The list of nodal nonlinear variables names that are set for output
   * @return A vector of strings containing the names of the nodal nonlinear variables for output
   *
   * Note: The list returned by this will contain the names of both elemental and nodal variable
   * names if 'elemental_as_nodal = true' in the input file. The libMesh output system
   * (EquationSystems::build_solution_vector) performs the correct action within the solution vector
   * by setting the nodal values as the average of the values for each of the element that the node
   * shares.
   *
   * @see hasNodalVariableOutput
   */
  const std::vector<std::string> & getNodalVariableOutput();

  /**
   * Returns true if there exists elemental nonlinear variables for output
   * @return True if nonlinear variable output exists
   * @see getElementalVariableOutput
   */
  bool hasElementalVariableOutput();

  /**
   * The list of elemental nonlinear variables names that are set for output
   * @return A vector of strings containing the names of the nonlinear variables for output
   * @see hasElementalVariableOutput
   */
  const std::vector<std::string> & getElementalVariableOutput();

  /**
   * Returns true if there exists scalar variables for output
   * @return True if scalar variable output exists
   * @see getScalarVariableOutput
   */
  bool hasScalarOutput();

  /**
   * The list of scalar variables names that are set for output
   * @return A vector of strings containing the names of the scalar variables for output
   * @see hasScalarVariableOutput
   */
  const std::vector<std::string> & getScalarOutput();

  /**
   * Returns true if there exists postprocessors for output
   * @return True if postprocessor output exists
   * @see getPostprocessorOutput
   */
  bool hasPostprocessorOutput();

  /**
   * The list of postprocessor names that are set for output
   * @return A vector of strings containing the names of the postprocessor variables for output
   * @see hasPostprocessorOutput
   */
  const std::vector<std::string> & getPostprocessorOutput();

  /**
   * This is called any time that the mesh changes
   * This class simply changes the _mesh_changed member variable to true, which is used by output()
   * to trigger a call of the outputSetup() method.
   */
  virtual void meshChanged();

  /**
   * Set the sequence state
   * When the sequence state is set to true then the outputSetup() method is called with every
   * call to output(). In the case of Exodus output, this creates a new file with each output.
   *
   * The sequence state is automatically set to true when 'use_displaced = true', otherwise it
   * is set to false initially
   */
  virtual void sequence(bool state);

  /**
   * A method for enable/disabing output.
   * @param state A true/false flag that enables or disables all output, if set to false
   * all subsequent calls to outputInitial(), outputStep(), and outputFinal() will not produce output.
   * However, forceOutput() will always produce output.
   */
  void allowOutput(bool state);

  /**
   * A method for forcing output
   * This will cause the next call to outputInitial, outputStep(), or outputFinal() to call output() method
   * regardless of intervals, allow states, etc. The rules for calling outputSetup() are preserved.
   */
  void forceOutput();

protected:

  /**
   * A single call to this function should output all the necessary data for a single timestep. By
   * default this function performs calls each of the four virtual output methods: outputScalarVariables(),
   * outputPostprocessors(), outputElementalVariables(), and outputNodalVariables(). But, only if output exists
   * for each type.
   *
   * @see outputNodalVariables outputElementalVariables outputScalarVariables outputPostprocessors
   */
  virtual void output();

  /**
   * Performs output of nodal nonlinear variables
   * The child class must define this method to output the nonlinear variables as desired
   * @see Exodus::outputNodalVariables
   */
  virtual void outputNodalVariables() = 0;

  /**
   * Performs output of elemental nonlinear variables
   * The child class must define this method to output the nonlinear variables as desired
   * @see Exodus::outputElementalVariables
   */
  virtual void outputElementalVariables() = 0;

  /**
   * Performs output of scalar variables
   * The child class must define this method to output the scalar variables as desired
   * @see Exodus::outputScalarVariables
   */
  virtual void outputScalarVariables() = 0;

  /**
   * Performs output of postprocessors
   * The child class must define this method to output the postprocessors as desired
   * @see Exodus::outputPostprocessors
   */
  virtual void outputPostprocessors() = 0;

  /**
   * Initial setup function that is called prior to any output
   * This method is called by FEProblem::initialSetup via the OutputWarehouse. It is the last initialSetup so
   * all objects are setup at the time of the execution.g
   *
   * For example, the Console object uses this function to write the system information which is desired to be before any
   * other output.
   *
   * @see FEProblem::initialSetup Console
   */
  virtual void initialSetup();

  /**
   * Timestep setup function that is called at the beginning of the timestep
   * This method is called by FEProblem::timestepSetup via the OutputWarhouse.
   *
   * @see FEProblem::timestepSetup()
   */
  virtual void timestepSetup();

  /**
   * This method is called initially by the output() method prior to any of the variable output methods.
   * Hence, the child class should use this method to prepare for outputing data. For example, the Exodus
   * output defines a pointer to the IO interface that is utilized for performing the data export.
   *
   * outputSetup() is also called  by the output() when the meshChanged() function is called; thus, the child class should
   * use the _mesh_changed flag to indicate if something is different when the mesh changes. After every call of
   * outputSetup() _mesh_changed flag is set to false.
   */
  virtual void outputSetup();

  /**
   * Performs the output of the input file
   * By default this method does nothing and it is called only once, on the first call to output()
   */
  virtual void outputInput();

  //virtual void outputLinearResidual();
  //virtual void outputNonlinearResidual();

  /**
   * \todo{Make this call automatic in similar fashion to outputInput}
   * Performs the output of system information
   * By default this method does nothing and is not called by output()
   */
  virtual void outputSystemInformation();

  /**
   * Returns true if the output interval is satisfied
   * \todo{Implement additional types of intervals (e.g., simulation time and real time)}
   */
  bool checkInterval();

  /// Pointer the the FEProblem object for output object (use this)
  FEProblem * _problem_ptr;

  /// Transient flag (true = transient)
  bool _transient;

  /// Flag for using displaced mesh
  bool _use_displaced;

  /// Reference the the libMesh::EquationSystems object that contains the data
  EquationSystems * _es_ptr;

  /// The current time for output purposes
  Real & _time;

  /// The old time
  Real & _time_old;

  /// The current time step
  int & _t_step;

  /// Time step delta
  Real & _dt;

  /// Old time step delta
  Real & _dt_old;

  /// Flag for outputing the initial solution
  bool _output_initial;

  /// Flag for outputing the final step
  bool _output_final;

  /// Flag for output the input file
  bool _output_input;

  /// Toggle for controlling the printing of nonlinear residuals
  bool _output_nonlinear;

  /// Toggle for controlling the printing of linear residuals
  bool _output_linear;

  /// Flag for outputting elemental variables as nodal
  bool _elemental_as_nodal;

  /// Flag for outputting scalar AuxVaraiables as nodal
  bool _scalar_as_nodal;

  /// System information output flag
  bool _system_information;

  /// True if the meshChanged() function has been called (restartable)
  bool & _mesh_changed;

  /// Flag for forcing call to outputSetup() with every call to output() (restartable)
  bool & _sequence;

private:

  /**
   * Performs initial output (if desired)
   *
   * If the output_initial input options is true calling this method will call the output() method, it
   * does nothing otherwise. This method is called by the Steady and Transient Executioners via
   * the FEProblem::outputInitial method.
   *
   * The main purpose is to have the output class perform the check on the output_initial option rather than
   * the Executioner.
   *
   * @see output Steady Transient FEProblem
   */
  virtual void outputInitial();

  /**
   * Performs the output call, if _output_failed = true
   * This method is what is called by the internal MOOSE framework, this should \b not be called by a user. This method
   * calls outputSetup() prior to output() anytime the meshChanged() method was called or the _sequence flag is true.
   *
   * @see Transient::takeStep()
   */
  virtual void outputFailedStep();

  /**
   * Performs the output call, handling interval and outputSetup calls.
   * This method is what is called by the internal MOOSE framework, this should \b not be called by a user. This method
   * calls outputSetup() prior to output() any time the meshChanged() method was called or the _sequence flag is true.
   */
  virtual void outputStep();

  /**
   * Force the output of the final timestep (if desired)
   *
   * This is called by the Transient executioner when the simulation is done, it will output the final solution regardless
   * of interval settings if 'output_final = true'.
   *
   * @see Transient::keepGoing
   */
  virtual void outputFinal();

  /**
   * Initializes the available lists for each of the output types
   */
  void initAvailableLists();

  /**
   * Timestep setup function that is called at the beginning of the timestep
   * just prior to timestepSetup(). This method is meant for internal use only.
   * This method is called by FEProblem::timestepSetup via the OutputWarhouse.
   *
   * @see PetscOutputter
   */
  virtual void timestepSetupInternal();

  /**
   * Parses the user-supplied input for hiding and showing variables and postprocessors into
   * a list for each type of output
   * @param show The vector of names that are to be output
   * @param hide The vector of names that are to be suppressed from the output
   */
  void initShowHideLists(const std::vector<VariableName> & show, const std::vector<VariableName> & hide);

  /**
   * Initializes the list of items to be output using the available, show, and hide lists
   * @param data The OutputData to operate on
   */
  void initOutputList(OutputData & data);

  /// Storage structure for the variable lists for elemental nonlinear variable output
  OutputData _nonlinear_elemental;

  /// Storage structure for the variable lists for nodal nonlinear variable output
  OutputData _nonlinear_nodal;

  /// Storage structure for the variable lists for postprocessor output
  OutputData _postprocessor;

  /// Storage structure for the variable lists for scalar output
  OutputData _scalar;

  /// The number of outputs written (restartable)
  unsigned int & _num;

  /// The output time step interval
  const unsigned int _interval;

  /// Sync times for this outputter
  std::set<Real> _sync_times;

  /// Start outputting time
  Real _start_time;

  /// End outputting time
  Real _end_time;

  /// Flag for only executing at sync times
  bool _sync_only;

  /// Flag for disabling/enabling outut
  bool _allow_output;

  /// Flag for forcing output
  bool _force_output;

  /// Flag for outputting faild time steps
  bool _output_failed;

  /// True if outputSetup was called
  /* Note, this is needed by recovery system to guarantee that outputStep was called. Normal runs rely on _num == 0, but when
     recovering _num will likely be non-zero */
  bool _output_setup_called;

  // Allow complete access
  friend class OutputWarehouse;
  friend class FileOutputter;
  friend class OversampleOutputter;
  friend class PetscOutputter;
};

#endif /* OUTPUTBASE_H */

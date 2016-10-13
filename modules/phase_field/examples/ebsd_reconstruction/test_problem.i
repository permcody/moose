#
# In this test we set the initial condition of a set of order parameters
# by pulling out the grain data from given EBSD data file ignoring the phase completely.
#

[Mesh]
  type = EBSDMesh
  filename = ebsd_40x40_2_phase.txt
[]

[GlobalParams]
  op_num = 8
  var_name_base = gr
[]

[UserObjects]
  [./ebsd]
    type = EBSDReader
  [../]
  [./tracker]
    type = GrainTracker
    ebsd_reader = ebsd
    phase = 2
    compute_var_to_feature_map = true
  [../]
[]

[ICs]
  [./PolycrystalICs]
    [./ReconVarIC]
      ebsd_reader = ebsd
      phase = 2
    [../]
  [../]
[]

[Variables]
  [./PolycrystalVariables]
  [../]
[]

[Kernels]
  [./PolycrystalKernel]
  [../]
[]

[AuxVariables]
  [./feature]
    family = MONOMIAL
    order = CONSTANT
  [../]
[]

[AuxKernels]
  [./feature]
    type = EBSDReaderAvgDataAux
    variable = feature
    ebsd_reader = ebsd
    grain_tracker = tracker
    data_name = feature_id
    execute_on = 'initial timestep_end'
  [../]
[]

[Materials]
  [./CuGrGr]
    # Material properties
    type = GBEvolution # Quantitative material properties for copper grain growth.  Dimensions are nm and ns
    block = 0 # Block ID (only one block in this problem)
    GBmob0 = 2.5e-6 #Mobility prefactor for Cu from Schonfelder1997
    GBenergy = 0.708 # GB energy in J/m^2
    Q = 0.23 #Activation energy for grain growth from Schonfelder 1997
    T = 450 # K   #Constant temperature of the simulation (for mobility calculation)
    wGB = 14 # nm    #Width of the diffuse GB
    outputs = exodus
  [../]
[]

[VectorPostprocessors]
  [./grain_volumes]
    type = FeatureVolumeVectorPostprocessor
    flood_counter = tracker
    ebsd_reader = ebsd
    phase = 2
    execute_on = 'initial'
  [../]
[]

[Executioner]
  type = Transient
  num_steps = 2
  dt = 0.05
[]

[Outputs]
  exodus = true
  print_linear_residuals = false
[]

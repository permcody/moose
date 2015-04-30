[Mesh]
  # uniform_refine = 4
  type = EBSDMesh
  filename = IN100_128x128.txt
[]

[GlobalParams]
  op_num = 20
  var_name_base = gr
[]

[UserObjects]
  [./ebsd]
    type = EBSDReader
  [../]
[]

[Variables]
  [./PolycrystalVariables]
  [../]
[]

[AuxVariables]
  [./bnds]
  [../]
  [./gt_indices]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./unique_grains]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./T]
    order = CONSTANT
    family = MONOMIAL
    initial_condition = 500
  [../]
  [./f_0]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_1]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_2]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_3]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_4]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_5]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_6]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_7]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_8]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_9]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_10]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_11]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_12]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_13]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_14]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_15]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_16]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_17]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_18]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./f_19]
    order = CONSTANT
    family = MONOMIAL
  [../]

[]

[ICs]
  [./PolycrystalICs]
    [./ReconVarIC]
      ebsd_reader = ebsd
      consider_phase = false
    [../]
  [../]
[]

[Kernels]
  [./PolycrystalKernel]
  [../]
[]

[AuxKernels]
  [./BndsCalc]
    type = BndsCalcAux
    variable = bnds
    execute_on = 'initial timestep_end'
  [../]
  [./gt_indices]
    type = FeatureFloodCountAux
    variable = gt_indices
    execute_on = 'initial timestep_end'
    bubble_object = grain_tracker
    field_display = VARIABLE_COLORING
  [../]
  [./unique_grains]
    type = FeatureFloodCountAux
    variable = unique_grains
    execute_on = 'initial timestep_end'
    bubble_object = grain_tracker
    field_display = UNIQUE_REGION
  [../]

  [./f_0]
    type = FeatureFloodCountAux
    variable = f_0
    map_index = 0
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_1]
    type = FeatureFloodCountAux
    variable = f_1
    map_index = 1
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_2]
    type = FeatureFloodCountAux
    variable = f_2
    map_index = 2
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_3]
    type = FeatureFloodCountAux
    variable = f_3
    map_index = 3
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_4]
    type = FeatureFloodCountAux
    variable = f_4
    map_index = 4
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_5]
    type = FeatureFloodCountAux
    variable = f_5
    map_index = 5
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_6]
    type = FeatureFloodCountAux
    variable = f_6
    map_index = 6
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_7]
    type = FeatureFloodCountAux
    variable = f_7
    map_index = 7
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_8]
    type = FeatureFloodCountAux
    variable = f_8
    map_index = 8
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_9]
    type = FeatureFloodCountAux
    variable = f_9
    map_index = 9
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_10]
    type = FeatureFloodCountAux
    variable = f_10
    map_index = 10
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_11]
    type = FeatureFloodCountAux
    variable = f_11
    map_index = 11
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_12]
    type = FeatureFloodCountAux
    variable = f_12
    map_index = 12
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_13]
    type = FeatureFloodCountAux
    variable = f_13
    map_index = 13
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_14]
    type = FeatureFloodCountAux
    variable = f_14
    map_index = 14
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_15]
    type = FeatureFloodCountAux
    variable = f_15
    map_index = 15
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_16]
    type = FeatureFloodCountAux
    variable = f_16
    map_index = 16
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_17]
    type = FeatureFloodCountAux
    variable = f_17
    map_index = 17
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_18]
    type = FeatureFloodCountAux
    variable = f_18
    map_index = 18
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
  [./f_19]
    type = FeatureFloodCountAux
    variable = f_19
    map_index = 19
    execute_on = 'initial timestep_end'
    bubble_object = feature_flood_count
    field_display = UNIQUE_REGION
  [../]
[]

[Materials]
  [./Copper]
    # T = 500 # K
    type = GBEvolution
    block = 0
    T = T
    wGB = 0.6 # um
    GBmob0 = 2.5e-6 # m^4/(Js) from Schoenfelder 1997
    Q = 0.23 # Migration energy in eV
    GBenergy = 0.708 # GB energy in J/m^2
    molar_volume = 7.11e-6; # Molar volume in m^3/mol
    length_scale = 1.0e-6
    time_scale = 1.0e-6
  [../]
[]

[Postprocessors]
  [./dt]
    type = TimestepSize
  [../]
  [./n_elements]
    type = NumElems
    execute_on = 'initial timestep_end'
  [../]
  [./n_nodes]
    type = NumNodes
    execute_on = 'initial timestep_end'
  [../]
  [./DOFs]
    type = NumDOFs
  [../]
  [./grain_tracker]
    type = GrainTracker
    threshold = 0.25
    convex_hull_buffer = 0.0
    use_single_map = false
    enable_var_coloring = true
    condense_map_info = true
    execute_on = 'initial timestep_end'
    tracking_step = 0
#    ebsd_reader = ebsd
    flood_entity_type = ELEMENTAL
  [../]


  [./feature_flood_count]
    type = GrainTracker
    threshold = 0.25
    use_single_map = false
    tracking_step = 0
    remap_grains = false
    execute_on = 'initial timestep_end'
    flood_entity_type = ELEMENTAL
  [../]


[]

[Executioner]
  # [./Adaptivity]
  # initial_adaptivity = 3
  # refine_fraction = 0.7
  # coarsen_fraction = 0.1
  # max_h_level = 4
  # print_changed_info = true
  # [../]
  type = Transient
  scheme = bdf2
  solve_type = PJFNK # Preconditioned JFNK (default)
  petsc_options_iname = '-pc_type -pc_hypre_type -pc_hypre_boomeramg_strong_threshold'
  petsc_options_value = '  hypre    boomeramg                   0.7'
  l_tol = 1.0e-4
  l_max_its = 20
  nl_max_its = 20
  nl_rel_tol = 1.0e-8
  start_time = 0.0
  num_steps = 30
  dt = 10
  [./TimeStepper]
    type = IterationAdaptiveDT
    cutback_factor = 0.9
    dt = 10.0
    growth_factor = 1.1
    optimal_iterations = 7
  [../]
[]

[Outputs]
  output_initial = true
  csv = true
  [./console]
    type = Console
    perf_log = true
    output_on = 'linear nonlinear'
  [../]
  [./exodus]
    type = Exodus
    interval = 1
    file_base = IN100-111grn
    output_on = 'initial timestep_end'
    output_postprocessors_on = 'initial timestep_end'
  [../]
[]

[Problem]
  type = FEProblem
  use_legacy_uo_initialization = false
[]

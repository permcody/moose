[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diff]
    type = CoefDiffusion
    variable = u
    coef = 0.1
  [../]
  [./time]
    type = TimeDerivative
    variable = u
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = u
    boundary = left
    value = 0
  [../]
  [./right]
    type = DirichletBC
    variable = u
    boundary = right
    value = 1
  [../]
[]

[Executioner]
  # Preconditioned JFNK (default)
  type = Transient
  num_steps = 10
  solve_type = PJFNK
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
  [./Steppers]
    [./simple]
      type = SimpleStepper
      dt = 0.4
    [../]
    [./pp]
      # Use the Postprocessor value to limit the dt
      type = PostprocessorStepper
      incoming_stepper = simple
      postprocessor = dt_pp
    [../]
  [../]
[]

[Postprocessors]
  [./dt_pp]
    type = FunctionValuePostprocessor
    function = 0.6-(t*0.1)
    execute_on = 'initial timestep_end'
  [../]
[]

[Outputs]
  exodus = true
[]

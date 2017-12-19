[Mesh]
  type = FileMesh
  file = nodal_normals_test_flat_nonmatching_nogap.e

  [./NewMortarInterfaces]
    [./interface]
      master = 2
      slave = 1
      subdomain = 1000
    [../]
  [../]
[]

[Variables]
  [./u]
  [../]
[]

[Kernels]
  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[BCs]
  [./left]
    type = DirichletBC
    variable = u
    boundary = outside_left
    value = 0
  [../]
  [./right]
    type = NeumannBC
    variable = u
    boundary = outside_right
    value = 1
  [../]
[]

[Executioner]
  type = Steady
  solve_type = 'PJFNK'
  petsc_options_iname = '-pc_type -pc_hypre_type'
  petsc_options_value = 'hypre boomeramg'
[]

[Outputs]
  exodus = true
[]

[Problem]
  solve = false
[]

[MeshGenerators]
  [./fmg]
    type = FileMeshGenerator
    file = square.e
  []

  [./extra_nodeset]
    type = GenerateExtraNodeset
    input = fmg
    new_boundary = 'middle_node'
    nodes = '2'
  []
[]

[Mesh]
  type = MeshGeneratorMesh
[]

[Variables]
  active = 'u'

  [./u]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[Kernels]
  active = 'diff'

  [./diff]
    type = Diffusion
    variable = u
  [../]
[]

[BCs]
  active = 'left right middle'

  [./left]
    type = DirichletBC
    variable = u
    boundary = 1
    value = 0
  [../]

  [./right]
    type = DirichletBC
    variable = u
    boundary = 2
    value = 1
  [../]

  [./middle]
    type = DirichletBC
    variable = u
    boundary = 'middle_node'
    value = -1
  [../]
[]

[Executioner]
  type = Steady

  solve_type = 'PJFNK'
[]

[Outputs]
  exodus = true
  file_base = out
[]

[MeshGenerators]
  [./gmg]
    type = GeneratedMeshGenerator
    dim = 3
    nx = 20
    ny = 20
    nz = 20
  []

  [./image]
    type = ImageSubdomainGenerator
    input = gmg
    file_base = stack/test
    file_suffix = png
    threshold = 2.7e4
  []
[]

[Mesh]
  type = MeshGeneratorMesh
[]

[Variables]
  [./u]
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
[../]

[Executioner]
  type = Steady
[]

[Outputs]
  exodus = true
  execute_on = 'timestep_end'
[]

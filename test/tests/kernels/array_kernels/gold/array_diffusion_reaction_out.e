CDF      
      
len_string     !   len_line   Q   four      	time_step          len_name   !   num_dim       	num_nodes         num_elem      
num_el_blk        num_node_sets         num_side_sets         num_el_in_blk1        num_nod_per_el1       num_side_ss1      num_side_ss2      num_side_ss3      num_side_ss4      num_nod_ns1       num_nod_ns2       num_nod_ns3       num_nod_ns4       num_nod_var       num_info  F         api_version       @�
=   version       @�
=   floating_point_word_size            	file_size               int64_status             title         array_diffusion_reaction_out.e     maximum_name_length                     
time_whole                            v(   	eb_status                             	    eb_prop1               name      ID              	   	ns_status         	                    	   ns_prop1      	         name      ID              	   	ss_status         
                    	(   ss_prop1      
         name      ID              	8   coordx                      �      	H   coordy                      �      
   eb_names                       $      
�   ns_names      	                 �      
�   ss_names      
                 �      �   
coor_names                         D         node_num_map                    d      H   connect1                  	elem_type         QUAD4               �   elem_num_map                    @      �   elem_ss1                          �   side_ss1                          �   elem_ss2                             side_ss2                             elem_ss3                          ,   side_ss3                          <   elem_ss4                          L   side_ss4                          \   node_ns1                          l   node_ns2                          �   node_ns3                          �   node_ns4                          �   vals_nod_var1                          �      v0   vals_nod_var2                          �      v�   name_nod_var                       D      �   info_records                      g(                                                                        ?�      ?�              ?�      ?�      ?�      ?�      ?�      ?�      ?�              ?�      ?�      ?�      ?�              ?�      ?�      ?�      ?�              ?�      ?�      ?�                      ?�      ?�              ?�              ?�              ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�      ?�                                          right                            left                             bottom                           top                              bottom                           right                            left                             top                                                                                                                             	   
                                                                                       	   
                                             
                                                                                                                                 	   
                                                                           	                                          	   
                                       	               u_0                              u_1                                ####################                                                             # Created by MOOSE #                                                             ####################                                                             ### Command Line Arguments ###                                                    ../../../moose_test-dbg -i array_diffusion_reaction.i### Version Info ###       Framework Information:                                                           MOOSE Version:           git commit bd344f7230 on 2019-05-07                     LibMesh Version:         c4c9fd5450489486fd5f7baf2ec9a8ac0d47fc99                PETSc Version:           3.10.1                                                  Current Time:            Tue May  7 12:30:44 2019                                Executable Timestamp:    Tue May  7 12:30:31 2019                                                                                                                                                                                                  ### Input File ###                                                                                                                                                []                                                                                 inactive                       = (no_default)                                    initial_from_file_timestep     = LATEST                                          initial_from_file_var          = INVALID                                         element_order                  = AUTO                                            order                          = AUTO                                            side_order                     = AUTO                                            type                           = GAUSS                                         []                                                                                                                                                                [BCs]                                                                                                                                                               [./left]                                                                           boundary                     = 1                                                 control_tags                 = INVALID                                           displacements                = INVALID                                           enable                       = 1                                                 extra_matrix_tags            = INVALID                                           extra_vector_tags            = INVALID                                           implicit                     = 1                                                 inactive                     = (no_default)                                      isObjectAction               = 1                                                 matrix_tags                  = system                                            type                         = ArrayDirichletBC                                  use_displaced_mesh           = 0                                                 variable                     = u                                                 vector_tags                  = nontime                                           diag_save_in                 = INVALID                                           save_in                      = INVALID                                           seed                         = 0                                                 values                       = 0                                             0                                                                                  [../]                                                                                                                                                             [./right]                                                                          boundary                     = 2                                                 control_tags                 = INVALID                                           displacements                = INVALID                                           enable                       = 1                                                 extra_matrix_tags            = INVALID                                           extra_vector_tags            = INVALID                                           implicit                     = 1                                                 inactive                     = (no_default)                                      isObjectAction               = 1                                                 matrix_tags                  = system                                            type                         = ArrayDirichletBC                                  use_displaced_mesh           = 0                                                 variable                     = u                                                 vector_tags                  = nontime                                           diag_save_in                 = INVALID                                           save_in                      = INVALID                                           seed                         = 0                                                 values                       = 1                                             2                                                                                  [../]                                                                          []                                                                                                                                                                [Executioner]                                                                      inactive                       = (no_default)                                    isObjectAction                 = 1                                               type                           = Steady                                          accept_on_max_picard_iteration = 0                                               compute_initial_residual_before_preset_bcs = 0                                   contact_line_search_allowed_lambda_cuts = 2                                      contact_line_search_ltol       = INVALID                                         control_tags                   = (no_default)                                    disable_picard_residual_norm_check = 0                                           enable                         = 1                                               l_abs_step_tol                 = -1                                              l_max_its                      = 10000                                           l_tol                          = 1e-05                                           line_search                    = default                                         line_search_package            = petsc                                           max_xfem_update                = 4294967295                                      mffd_type                      = wp                                              nl_abs_step_tol                = 1e-50                                           nl_abs_tol                     = 1e-50                                           nl_max_funcs                   = 10000                                           nl_max_its                     = 50                                              nl_rel_step_tol                = 1e-50                                           nl_rel_tol                     = 1e-08                                           petsc_options                  = INVALID                                         petsc_options_iname            = INVALID                                         petsc_options_value            = INVALID                                         picard_abs_tol                 = 1e-50                                           picard_force_norms             = 0                                               picard_max_its                 = 1                                               picard_rel_tol                 = 1e-08                                           relaxation_factor              = 1                                               relaxed_variables              = (no_default)                                    restart_file_base              = (no_default)                                    snesmf_reuse_base              = 1                                               solve_type                     = NEWTON                                          splitting                      = INVALID                                         time                           = 0                                               update_xfem_at_timestep_begin  = 0                                             []                                                                                                                                                                [Kernels]                                                                                                                                                           [./diff]                                                                           inactive                     = (no_default)                                      isObjectAction               = 1                                                 type                         = ArrayDiffusion                                    block                        = INVALID                                           control_tags                 = Kernels                                           diag_save_in                 = INVALID                                           diffusion_coefficient        = dc                                                diffusion_coefficient_type   = array                                             displacements                = INVALID                                           enable                       = 1                                                 extra_matrix_tags            = INVALID                                           extra_vector_tags            = INVALID                                           implicit                     = 1                                                 matrix_tags                  = system                                            save_in                      = INVALID                                           seed                         = 0                                                 use_displaced_mesh           = 0                                                 variable                     = u                                                 vector_tags                  = nontime                                         [../]                                                                                                                                                             [./reaction]                                                                       inactive                     = (no_default)                                      isObjectAction               = 1                                                 type                         = ArrayReaction                                     block                        = INVALID                                           control_tags                 = Kernels                                           diag_save_in                 = INVALID                                           displacements                = INVALID                                           enable                       = 1                                                 extra_matrix_tags            = INVALID                                           extra_vector_tags            = INVALID                                           implicit                     = 1                                                 matrix_tags                  = system                                            reaction_coefficient         = rc                                                reaction_coefficient_type    = full                                              save_in                      = INVALID                                           seed                         = 0                                                 use_displaced_mesh           = 0                                                 variable                     = u                                                 vector_tags                  = nontime                                         [../]                                                                          []                                                                                                                                                                [Materials]                                                                                                                                                         [./dc]                                                                             inactive                     = (no_default)                                      isObjectAction               = 1                                                 type                         = GenericConstantArray                              block                        = INVALID                                           boundary                     = INVALID                                           compute                      = 1                                                 constant_on                  = NONE                                              control_tags                 = Materials                                         enable                       = 1                                                 implicit                     = 1                                                 output_properties            = INVALID                                           outputs                      = none                                              prop_name                    = dc                                                prop_value                   = 1                                             1                                                                                    seed                         = 0                                                 use_displaced_mesh           = 0                                               [../]                                                                                                                                                             [./rc]                                                                             inactive                     = (no_default)                                      isObjectAction               = 1                                                 type                         = GenericConstant2DArray                            block                        = INVALID                                           boundary                     = INVALID                                           compute                      = 1                                                 constant_on                  = NONE                                              control_tags                 = Materials                                         enable                       = 1                                                 implicit                     = 1                                                 output_properties            = INVALID                                           outputs                      = none                                              prop_name                    = rc                                                prop_value                   = '1    0                                       -0.1    1'                                                                           seed                         = 0                                                 use_displaced_mesh           = 0                                               [../]                                                                          []                                                                                                                                                                [Mesh]                                                                             displacements                  = INVALID                                         inactive                       = (no_default)                                    use_displaced_mesh             = 1                                               include_local_in_ghosting      = 0                                               output_ghosting                = 0                                               block_id                       = INVALID                                         block_name                     = INVALID                                         boundary_id                    = INVALID                                         boundary_name                  = INVALID                                         construct_side_list_from_node_list = 0                                           ghosted_boundaries             = INVALID                                         ghosted_boundaries_inflation   = INVALID                                         isObjectAction                 = 1                                               second_order                   = 0                                               skip_partitioning              = 0                                               type                           = GeneratedMesh                                   uniform_refine                 = 0                                               allow_renumbering              = 1                                               bias_x                         = 1                                               bias_y                         = 1                                               bias_z                         = 1                                               centroid_partitioner_direction = INVALID                                         construct_node_list_from_side_list = 1                                           control_tags                   = (no_default)                                    dim                            = 2                                               elem_type                      = INVALID                                         enable                         = 1                                               gauss_lobatto_grid             = 0                                               ghosting_patch_size            = INVALID                                         max_leaf_size                  = 10                                              nemesis                        = 0                                               nx                             = 4                                               ny                             = 4                                               nz                             = 1                                               parallel_type                  = DEFAULT                                         partitioner                    = default                                         patch_size                     = 40                                              patch_update_strategy          = never                                           xmax                           = 1                                               xmin                           = 0                                               ymax                           = 1                                               ymin                           = 0                                               zmax                           = 1                                               zmin                           = 0                                             []                                                                                                                                                                [Mesh]                                                                           []                                                                                                                                                                [Mesh]                                                                           []                                                                                                                                                                [Outputs]                                                                          append_date                    = 0                                               append_date_format             = INVALID                                         checkpoint                     = 0                                               color                          = 1                                               console                        = 1                                               controls                       = 0                                               csv                            = 0                                               dofmap                         = 0                                               execute_on                     = 'INITIAL TIMESTEP_END'                          exodus                         = 1                                               file_base                      = INVALID                                         gmv                            = 0                                               gnuplot                        = 0                                               hide                           = INVALID                                         inactive                       = (no_default)                                    interval                       = 1                                               nemesis                        = 0                                               output_if_base_contains        = INVALID                                         perf_graph                     = 0                                               print_linear_residuals         = 1                                               print_mesh_changed_info        = 0                                               print_perf_log                 = 0                                               show                           = INVALID                                         solution_history               = 0                                               sync_times                     = (no_default)                                    tecplot                        = 0                                               vtk                            = 0                                               xda                            = 0                                               xdr                            = 0                                             []                                                                                                                                                                [Preconditioning]                                                                                                                                                   [./smp]                                                                            inactive                     = (no_default)                                      isObjectAction               = 1                                                 type                         = SMP                                               control_tags                 = Preconditioning                                   coupled_groups               = INVALID                                           enable                       = 1                                                 full                         = 1                                                 ksp_norm                     = unpreconditioned                                  mffd_type                    = wp                                                off_diag_column              = INVALID                                           off_diag_row                 = INVALID                                           pc_side                      = default                                           petsc_options                = INVALID                                           petsc_options_iname          = INVALID                                           petsc_options_value          = INVALID                                           solve_type                   = INVALID                                         [../]                                                                          []                                                                                                                                                                [Variables]                                                                                                                                                         [./u]                                                                              block                        = INVALID                                           component                    = 2                                                 eigen                        = 0                                                 family                       = LAGRANGE                                          inactive                     = (no_default)                                      initial_condition            = INVALID                                           order                        = FIRST                                             outputs                      = INVALID                                           scaling                      = INVALID                                           initial_from_file_timestep   = LATEST                                            initial_from_file_var        = INVALID                                         [../]                                                                          []                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         ?�      ?���7w4?�۬PC.?ًM=�l?�9����W?���\ޗ�?�<ɯ��Z?�ω��Ǒ?ƭ� ô�                ?��#9Y�?�V�����?�3*�3�?Ы��I^        ?�y�6i?�~ �??�b^�c�?����        ?�     ?�     ?�     ?�     ?�     ?��se<�?�Yw�x��?��\��?���ƛ?�4��da?�x���?���e_?��ʷ��T                ?���,z?�{�l�kA?�N��d�?��Y{�w        ?�)G���?��ME�x?�v�ew?�!u�h�B        @      @      @      @      @      
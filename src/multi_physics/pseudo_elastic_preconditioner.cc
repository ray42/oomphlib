//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.90. August 3, 2009.
//LIC// 
//LIC// Copyright (C) 2006-2009 Matthias Heil and Andrew Hazel
//LIC// 
//LIC// This library is free software; you can redistribute it and/or
//LIC// modify it under the terms of the GNU Lesser General Public
//LIC// License as published by the Free Software Foundation; either
//LIC// version 2.1 of the License, or (at your option) any later version.
//LIC// 
//LIC// This library is distributed in the hope that it will be useful,
//LIC// but WITHOUT ANY WARRANTY; without even the implied warranty of
//LIC// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//LIC// Lesser General Public License for more details.
//LIC// 
//LIC// You should have received a copy of the GNU Lesser General Public
//LIC// License along with this library; if not, write to the Free Software
//LIC// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//LIC// 02110-1301  USA.
//LIC// 
//LIC// The authors may be contacted at oomph-lib@maths.man.ac.uk.
//LIC// 
//LIC//====================================================================

#include "pseudo_elastic_preconditioner.h"

namespace oomph
{
 
 //=============================================================================
 /// \short Namespace to contain a flag to switch between new code and old.
 //=============================================================================
 namespace RayNamespace
 {
   bool UseRayCode;
 }

 //=============================================================================
 /// \short Functions to create instances of optimal subsidiary operators for
 /// the PseudoElasticPreconditioner
 //=============================================================================
 namespace Pseudo_Elastic_Preconditioner_Subsidiary_Operator_Helper
 {
#ifdef OOMPH_HAS_HYPRE

  /// \short AMG w/ GS smoothing for the augmented elastic subsidiary linear
  /// systems
  Preconditioner* get_elastic_preconditioner_hypre()
  {
   HyprePreconditioner* hypre_preconditioner_pt = 
    new HyprePreconditioner("Hypre for diagonal blocks in pseudo-solid");
   hypre_preconditioner_pt->set_amg_iterations(2);
   hypre_preconditioner_pt->amg_using_simple_smoothing();
   if (MPI_Helpers::communicator_pt()->nproc()>1)
    {
     // Jacobi in parallel
     hypre_preconditioner_pt->amg_simple_smoother() = 0;
    }
   else
    {
     // Gauss Seidel in serial (was 3 actually...)
     hypre_preconditioner_pt->amg_simple_smoother() =1;
    }
   hypre_preconditioner_pt->hypre_method() = HyprePreconditioner::BoomerAMG;
   hypre_preconditioner_pt->amg_damping() = 1.0;
   hypre_preconditioner_pt->amg_coarsening() = 6;
   return hypre_preconditioner_pt;
  }


  /// \short AMG w/ GS smoothing for the augmented elastic subsidiary linear
  /// systems -- calls Hypre version to stay consistent with previous default
  Preconditioner* get_elastic_preconditioner()
  {
   return get_elastic_preconditioner_hypre();
  }
  
#endif
  
#ifdef OOMPH_HAS_TRILINOS

  /// \short TrilinosML smoothing for the augmented elastic 
  /// subsidiary linear systems
  Preconditioner* get_elastic_preconditioner_trilinos_ml()
  {
   TrilinosMLPreconditioner* trilinos_prec_pt=
    new TrilinosMLPreconditioner;
   return trilinos_prec_pt;
  }

  /// \short CG with diagonal preconditioner for the lagrange multiplier
  /// subsidiary linear systems.
  Preconditioner* get_lagrange_multiplier_preconditioner()
  {
   InnerIterationPreconditioner
    <TrilinosAztecOOSolver,MatrixBasedDiagPreconditioner>* prec_pt = 
    new InnerIterationPreconditioner
    <TrilinosAztecOOSolver,MatrixBasedDiagPreconditioner>;
   // Note: This makes CG a proper "inner iteration" for
   // which GMRES (may) no longer converge. We should really
   // use FGMRES or GMRESR for this. However, here the solver
   // is so good that it'll converge very quickly anyway
   // so there isn't much to be gained by limiting the number
   // of iterations...
   // prec_pt->max_iter() = 4;
   prec_pt->solver_pt()->solver_type() = TrilinosAztecOOSolver::CG;
   prec_pt->solver_pt()->disable_doc_time();
   return prec_pt;
  }
#endif
 }


 
 ///////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////



 //=============================================================================
 // Setup method for the PseudoElasticPreconditioner.
 //=============================================================================
 void PseudoElasticPreconditioner::setup()
 {
  // clean
  this->clean_up_memory();
  
#ifdef PARANOID
  // paranoid check that meshes have been set
  if (Elastic_mesh_pt==0)
   {
    std::ostringstream error_message;
    error_message << "The elastic mesh must be set.";
    throw OomphLibError(error_message.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
    
   }
  if (Lagrange_multiplier_mesh_pt==0)
   {
    std::ostringstream error_message;
    error_message << "The Lagrange multiplier mesh must be set.";
    throw OomphLibError(error_message.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }
#endif
  
  // set the mesh
  unsigned n_solid_dof_types = 0;
  unsigned n_dof_types = 0;
  this->set_mesh(0,Elastic_mesh_pt);
  this->set_mesh(1,Lagrange_multiplier_mesh_pt);
  if (this->is_master_block_preconditioner())
   {
    
    // get the number of solid dof types from the first element
    n_solid_dof_types = this->ndof_types_in_mesh(0);
    
    // get the total number of dof types
    n_dof_types = n_solid_dof_types 
     + this->ndof_types_in_mesh(1);
   }
  else
   {
    n_dof_types = this->ndof_types();
    n_solid_dof_types = n_dof_types - (n_dof_types/3);
   }
#ifdef PARANOID
  if (n_dof_types%3 != 0)
   {
    std::ostringstream error_message;
    error_message << "This preconditioner requires DIM*3 types of DOF";
    throw OomphLibError(error_message.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }
#endif
  
  // determine the dimension
  Dim = n_dof_types/3;
  
  // Recast Jacobian matrix to CRDoubleMatrix
  CRDoubleMatrix* cr_matrix_pt = dynamic_cast<CRDoubleMatrix*>(matrix_pt());

#ifdef PARANOID
  if (cr_matrix_pt==0)
   {
    std::ostringstream error_message;
    error_message << "FSIPreconditioner only works with"
                  << " CRDoubleMatrix matrices" << std::endl;
    throw OomphLibError(error_message.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }
#endif

  RayNamespace::UseRayCode = true;

  // RAYRAY 
  // Setup the dof_list scheme for block_setup. 
  // The dof types are currently in the order (in 3D):
  // 0 1 2 3  4  5  6  7  8
  // x y z xc yc zc lx ly lz
  // 
  // We need to group the directional displacements together:
  // x xc y yc xz zc lx ly lz
  // The mapping required is:
  // 0 2 4 1 3 5 6 7 8
  // Think of this as 
  // "Where do I want to move the dof type in this position to?"
  Vector<unsigned> dof_list_for_block_setup(n_dof_types);
  for (unsigned dim_i = 0; dim_i < Dim; dim_i++) 
   {
    // bulk solid dof types
    dof_list_for_block_setup[dim_i] = 2*dim_i;

    // constrained solid dof types
    dof_list_for_block_setup[dim_i + Dim] = 2*dim_i + 1;

    // lagr dof types
    dof_list_for_block_setup[dim_i + 2*Dim] = 2*Dim + dim_i;
   }

  // Setup the block ordering. If UseRayCode, then we have the following:
  // Block types [0, 2*Dim) are the solid blocks.
  // Block types [2*Dim, 3*Dim) are the lagrange multiplier dof types.
  // 
  // For d = 0, 1, 2,
  // Bulk solid doftypes: 2*d
  // Constrained solid doftypes: 2*d+1
  // Lagr doftyoes: 2*Dim + d
  if(RayNamespace::UseRayCode)
  {
    this->block_setup(dof_list_for_block_setup);
  }
  else
  {
    this->block_setup();
  }

  // Create dof_list for subsidiary preconditioner. This will be used later
  // in turn_into_subsidiary_block_preconditioner(...)
  Vector<unsigned> dof_list_for_subsidiary_prec(n_solid_dof_types);
  if(RayNamespace::UseRayCode)
  {
    // RAYRAY 
    // Setup the dof_list scheme for block_setup. 
    // The dof types are currently in the order (in 3D):
    // 0 1 2 3  4  5  6  7  8
    // x y z xc yc zc lx ly lz
    // 
    // We need to group the directional displacements together:
    // x xc y yc xz zc lx ly lz
    // The mapping required is:
    // 0 3 1 4 2 5
    // Think of this as: 
    // "Which dof type do I want to move into this position?"
    for (unsigned dim_i = 0; dim_i < Dim; dim_i++) 
    {
      // bulk solid dof 
      dof_list_for_subsidiary_prec[2*dim_i] = dim_i;

      // constrained solid dof
      dof_list_for_subsidiary_prec[2*dim_i+1] = dim_i + Dim;
    }
  }
  else
  {
    // old code
    for (unsigned i = 0; i < n_solid_dof_types; i++)
    {
      dof_list_for_subsidiary_prec[i] = i;
    }
  } 

  // Get the solid blocks
  DenseMatrix<CRDoubleMatrix*> solid_matrix_pt(n_solid_dof_types,
      n_solid_dof_types,0);

  for (unsigned row_i = 0; row_i < n_solid_dof_types; row_i++) 
   {
    for (unsigned col_i = 0; col_i < n_solid_dof_types; col_i++) 
     {
      this->get_block(row_i,col_i,solid_matrix_pt(row_i,col_i));
     }
   }

  // compute the scaling (if required)
  if (Use_inf_norm_of_s_scaling)
   {

    if(RayNamespace::UseRayCode)
     {
      Vector<LinearAlgebraDistribution*> 
        solid_block_distribution_pt(n_solid_dof_types, 0);
      for (unsigned row_i = 0; row_i < n_solid_dof_types; row_i++) 
       {
        solid_block_distribution_pt[row_i] = Block_distribution_pt[row_i];
       }
 
      CRDoubleMatrix tmp_mat;
  
      CRDoubleMatrixHelpers::
        concatenate_without_communication(solid_block_distribution_pt,
                                          solid_matrix_pt,tmp_mat);
  
      Scaling = tmp_mat.inf_norm();
  
      tmp_mat.clear();

     } // UseRayCode
    else
     {
      Vector<unsigned> dof_list(n_solid_dof_types);
      for (unsigned i = 0; i < n_solid_dof_types; i++)
       {
        dof_list[i] = i;
       }

      PseudoElasticPreconditionerScalingHelper* helper_pt = 
       new PseudoElasticPreconditionerScalingHelper(this,
                                                    cr_matrix_pt,
                                                    dof_list,
                                                    Elastic_mesh_pt,
                                                    comm_pt());
      Scaling = helper_pt->s_inf_norm();
      delete helper_pt;
     } // !UseRayCode
   }
  else
   {
    Scaling = 1.0;
   }
  
  // Add the scaled identify matrix to the constrained solid blocks.
  for(unsigned d = 0; d < Dim; d++)
   {
    // Where is the constained block located?
    unsigned block_i = 2*d+1;

    // Data from the constrained block.
    double* s_values = solid_matrix_pt(block_i,block_i)->value();
    int* s_column_index = solid_matrix_pt(block_i,block_i)->column_index();
    int* s_row_start = solid_matrix_pt(block_i,block_i)->row_start();
    int s_nrow_local = solid_matrix_pt(block_i,block_i)->nrow_local();
    int s_first_row = solid_matrix_pt(block_i,block_i)->first_row();

    // Loop through the diagonal entries and add the scaling.
    for (int i = 0; i < s_nrow_local; i++)
     {
      bool found = false;
      for (int j = s_row_start[i]; 
           j < s_row_start[i+1] && !found; j++)
       {
        if (s_column_index[j] == i + s_first_row)
         {
          s_values[j] += Scaling;
          found = true;
         } // if
       } // for row start
      
      // Check if the diagonal entry is found.
      if(!found)
       {
        std::ostringstream error_message;
        error_message << "The diagonal entry for the constained block("
                      << block_i<<","<<block_i<<")\n"
                     << "on local row " << i << " does not exist."
                     << std::endl;
        throw OomphLibError(error_message.str(),
                            "PseudoElasticPreconditioner",
                            OOMPH_EXCEPTION_LOCATION);
       }
     } // for nrow_local
   } // for Dim 
  // setup the solid subsidiary preconditioner ////////////////////////////////
/* 
  // RAYRAY output the nrows.
  for (unsigned i = 0; i < n_solid_dof_types; i++) 
  {
    CRDoubleMatrix* tmpmat_pt = 0;
    this->get_block(i,0,tmpmat_pt);

    unsigned myrank = tmpmat_pt->distribution_pt()
                      ->communicator_pt()->my_rank();

    std::stringstream tmpmat_sstream;
    tmpmat_sstream << "nrowtest_block"<<i<<"r"<<myrank;
    std::ofstream tmpmat_fstream;
    tmpmat_fstream.open (tmpmat_sstream.str().c_str());
    tmpmat_fstream << tmpmat_pt->nrow_local()<<std::endl;
    tmpmat_fstream.close();
  }
  pause("done output"); 
  
// */
  // this preconditioner uses the full S matrix
  if (E_preconditioner_type == Exact_block_preconditioner)
   {
    if(RayNamespace::UseRayCode)
     {
      ExactBlockPreconditioner<CRDoubleMatrix>* s_prec_pt = 
        new ExactBlockPreconditioner<CRDoubleMatrix>;

      s_prec_pt->turn_into_subsidiary_block_preconditioner(
          this,dof_list_for_subsidiary_prec);

      if (Elastic_subsidiary_preconditioner_function_pt != 0)
       {
        s_prec_pt->
         set_subsidiary_preconditioner_function
         (Elastic_subsidiary_preconditioner_function_pt);
       }

      s_prec_pt->set_nmesh(1);
      s_prec_pt->set_mesh(0, Elastic_mesh_pt);
      s_prec_pt->set_precomputed_blocks(solid_matrix_pt);

      s_prec_pt->Preconditioner::setup(matrix_pt(),comm_pt());
      Elastic_preconditioner_pt = s_prec_pt;
     }
    else
     {
      PseudoElasticPreconditionerSubsidiaryPreconditioner* s_prec_pt = 
       new PseudoElasticPreconditionerSubsidiaryPreconditioner;
      Vector<unsigned> dof_list(n_solid_dof_types);
      for (unsigned i = 0; i < n_solid_dof_types; i++)
       {
        dof_list[i] = i;
       }
      s_prec_pt->turn_into_subsidiary_block_preconditioner(this,dof_list);
      if (Elastic_subsidiary_preconditioner_function_pt != 0)
       {
        s_prec_pt->
         set_subsidiary_preconditioner_function
         (Elastic_subsidiary_preconditioner_function_pt);
       }
      s_prec_pt->scaling() = Scaling;
      s_prec_pt->set_nmesh(1);
      s_prec_pt->set_mesh(0, Elastic_mesh_pt);
      s_prec_pt->Preconditioner::setup(matrix_pt(),comm_pt());
      Elastic_preconditioner_pt = s_prec_pt;
      }
   }
  
  // otherwise it is a block based preconditioner
  else 
   {
    if(RayNamespace::UseRayCode)
     {
    // create the preconditioner
    PseudoElasticPreconditionerSubsidiaryBlockPreconditioner* 
     s_prec_pt = 
     new PseudoElasticPreconditionerSubsidiaryBlockPreconditioner;
    
    if(RayNamespace::UseRayCode)
     {
      s_prec_pt->turn_into_subsidiary_block_preconditioner(
          this,dof_list_for_subsidiary_prec);
     }
    else
     {
      Vector<unsigned> dof_list(n_solid_dof_types);

      for (unsigned i = 0; i < n_solid_dof_types; i++)
       {
        dof_list[i] = i;
       }

      s_prec_pt->turn_into_subsidiary_block_preconditioner(this,dof_list);
     } // !UseRayCode
    
    // set the subsidiary solve method
    if (Elastic_subsidiary_preconditioner_function_pt != 0)
     {
      s_prec_pt->
       set_subsidiary_preconditioner_function
       (Elastic_subsidiary_preconditioner_function_pt);
     }
    
    // set the scaling
    s_prec_pt->scaling() = Scaling;
    
    // set the block preconditioning method
    switch (E_preconditioner_type)
     {
     case Block_diagonal_preconditioner:
      s_prec_pt->use_block_diagonal_approximation();
      break;
     case Block_upper_triangular_preconditioner:
      s_prec_pt->use_upper_triangular_approximation();
      break;
     case Block_lower_triangular_preconditioner:
      s_prec_pt->use_lower_triangular_approximation();
      break;
     default:
      break;
     }
    
    // setup
    s_prec_pt->set_nmesh(1);
    s_prec_pt->set_mesh(0, Elastic_mesh_pt);

    // The block to block map
    Vector<Vector<unsigned> > block_to_block_map(
        Dim,Vector<unsigned>(2,0));
    unsigned tmp_index = 0;
    for (unsigned d = 0; d < Dim; d++) 
     {
      block_to_block_map[d][0] = tmp_index++;
      block_to_block_map[d][1] = tmp_index++;
     }

/* 
    for (unsigned i = 0; i < block_to_block_map.size(); i++) 
    {
      for (unsigned j = 0; j < block_to_block_map[i].size(); j++) 
      {
        std::cout << block_to_block_map[i][j] << " ";
      }
      std::cout << std::endl; 
    }
    pause("outputted block to block map"); 
*/    
     
    s_prec_pt->set_precomputed_blocks(solid_matrix_pt,block_to_block_map);
    s_prec_pt->Preconditioner::setup(matrix_pt(),comm_pt());
    Elastic_preconditioner_pt = s_prec_pt;

     }
    else
     {
    // create the preconditioner
    PseudoElasticPreconditionerSubsidiaryBlockPreconditioner* 
     s_prec_pt = 
     new PseudoElasticPreconditionerSubsidiaryBlockPreconditioner;
    Vector<unsigned> dof_list(n_solid_dof_types);
    for (unsigned i = 0; i < n_solid_dof_types; i++)
     {
      dof_list[i] = i;
     }
    s_prec_pt->turn_into_subsidiary_block_preconditioner(this,dof_list);
    
    // set the subsidiary solve method
    if (Elastic_subsidiary_preconditioner_function_pt != 0)
     {
      s_prec_pt->
       set_subsidiary_preconditioner_function
       (Elastic_subsidiary_preconditioner_function_pt);
     }
    
    // set the scaling
    s_prec_pt->scaling() = Scaling;
    
    // set the block preconditioning method
    switch (E_preconditioner_type)
     {
     case Block_diagonal_preconditioner:
      s_prec_pt->use_block_diagonal_approximation();
      break;
     case Block_upper_triangular_preconditioner:
      s_prec_pt->use_upper_triangular_approximation();
      break;
     case Block_lower_triangular_preconditioner:
      s_prec_pt->use_lower_triangular_approximation();
      break;
     default:
      break;
     }
    
    // setup
    s_prec_pt->set_nmesh(1);
    s_prec_pt->set_mesh(0, Elastic_mesh_pt);
    s_prec_pt->Preconditioner::setup(matrix_pt(),comm_pt());
    Elastic_preconditioner_pt = s_prec_pt;

     }
   }

  // No longer require the solid blocks
  for (unsigned row_i = 0; row_i < n_solid_dof_types; row_i++) 
   {
    for (unsigned col_i = 0; col_i < n_solid_dof_types; col_i++) 
     {
      delete solid_matrix_pt(row_i,col_i);
     }
   }
 
  // next setup the lagrange multiplier preconditioners
  Lagrange_multiplier_preconditioner_pt.resize(Dim);
  for (unsigned d = 0; d < Dim; d++)
   {
    CRDoubleMatrix* b_pt = 0;
    if(RayNamespace::UseRayCode)
     {
      this->get_block(2*Dim+d,2*d+1,b_pt);
     }
    else
     {
      this->get_block(2*Dim+d,Dim+d,b_pt);
     }

    // if a non default preconditioner is specified create 
    // the preconditioners
    if (Lagrange_multiplier_subsidiary_preconditioner_function_pt != 0)
     {
      Lagrange_multiplier_preconditioner_pt[d] = 
       (*Lagrange_multiplier_subsidiary_preconditioner_function_pt)();
     }
    
    // else use default superlu preconditioner
    else
     {
      Lagrange_multiplier_preconditioner_pt[d] = new SuperLUPreconditioner;
     }
    
    // and setup
    Lagrange_multiplier_preconditioner_pt[d]->setup(b_pt,comm_pt());
    delete b_pt;
   }

 }
 
 //=============================================================================
 /// \short Apply the elastic subsidiary preconditioner.
 //=============================================================================
 void PseudoElasticPreconditioner::elastic_preconditioner_solve
 (const DoubleVector& r, DoubleVector& z)
 {
  // apply the solid preconditioner
  Elastic_preconditioner_pt->preconditioner_solve(r,z);
 }
 

 //=============================================================================
 /// \short Apply the lagrange multiplier subsidiary preconditioner.
 //=============================================================================
 void PseudoElasticPreconditioner::
 lagrange_multiplier_preconditioner_solve(const DoubleVector& r,
                                          DoubleVector& z)
 {
  // apply the lagrange multiplier preconditioner
  for (unsigned d = 0; d < Dim; d++)
   {
    DoubleVector x;
    this->get_block_vector(Dim*2+d,r,x);
    DoubleVector y;
    Lagrange_multiplier_preconditioner_pt[d]->preconditioner_solve(x,y);
    Lagrange_multiplier_preconditioner_pt[d]->preconditioner_solve(y,x);
    unsigned nrow_local = x.nrow_local();
    double* x_pt = x.values_pt();
    for (unsigned i = 0; i < nrow_local; i++)
     {
      x_pt[i] = x_pt[i] * Scaling;
     }     
    this->return_block_vector(Dim*2+d,x,z);
   }
 }

 //=============================================================================
 /// \short Clears the memory.
 //=============================================================================
 void PseudoElasticPreconditioner::clean_up_memory()
 {
  // clean the block preconditioner base class memory
  this->clear_block_preconditioner_base();
  
  // delete the solid preconditioner
  delete Elastic_preconditioner_pt;
  Elastic_preconditioner_pt = 0;
  
  // delete the lagrange multiplier preconditioner pt
  unsigned sz = Lagrange_multiplier_preconditioner_pt.size();
  for (unsigned i = 0; i < sz; i++)
   {
    delete Lagrange_multiplier_preconditioner_pt[i];
    Lagrange_multiplier_preconditioner_pt[i] = 0;
   }
 }



 ///////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////



 //=============================================================================
 /// \short Setup the preconditioner
 //=============================================================================
 void PseudoElasticPreconditionerSubsidiaryPreconditioner::
 setup()
 {
   // clean memory
   this->clean_up_memory();
     
#ifdef PARANOID
   // paranoid check that this preconditioner has an even number of DOF types
   if (this->ndof_types()%2 != 0)
    {
     std::ostringstream error_message;
     error_message
      << "This SUBSIDIARY preconditioner requires an even number of "
      << "types of DOF";
     throw OomphLibError(
      error_message.str(),
      OOMPH_CURRENT_FUNCTION,
      OOMPH_EXCEPTION_LOCATION);
    }
#endif
     
   if(RayNamespace::UseRayCode)
    {
     this->block_setup();

     unsigned ndof_types = this->ndof_types();
     unsigned n_constrained_doftypes = ndof_types/2;
// /* 
     // Get all blocks.
     DenseMatrix<CRDoubleMatrix*> s_pt(ndof_types,ndof_types,0);
     for (unsigned row_i = 0; row_i < ndof_types; row_i++) 
      {
       for (unsigned col_i = 0; col_i < ndof_types; col_i++) 
        {
         this->get_block(row_i,col_i,s_pt(row_i,col_i));
        }
      }
     

     

// */
/* 
     // add the scaled identity matrix to the constrained blocks
     for(unsigned constrained_i = 0; constrained_i < n_constrained_doftypes; 
         constrained_i++)
      {
       // Locate the constrained block.
       unsigned block_i = 2*constrained_i + 1;

       // Data for the constrained block.
       double* s_values = s_pt(block_i,block_i)->value();
       int* s_column_index = s_pt(block_i,block_i)->column_index();
       int* s_row_start = s_pt(block_i,block_i)->row_start();
       int s_nrow_local = s_pt(block_i,block_i)->nrow_local();
       int s_first_row = s_pt(block_i,block_i)->first_row();

       // Add the scaling to the diagonal entries.
       for (int i = 0; i < s_nrow_local; i++)
        {
         bool found = false;
         for (int j = s_row_start[i]; 
              j < s_row_start[i+1] && !found; j++)
          {
           if (s_column_index[j] == i + s_first_row)
            {
             s_values[j] += Scaling;
             found = true;
            }
          }
        }
      }
// */
     CRDoubleMatrix* s_prec_pt
       = new CRDoubleMatrix(this->preconditioner_matrix_distribution_pt());

     CRDoubleMatrixHelpers::concatenate_without_communication(
       Block_distribution_pt,s_pt,*s_prec_pt);

     for (unsigned row_i = 0; row_i < ndof_types; row_i++) 
      {
       for (unsigned col_i = 0; col_i < ndof_types; col_i++) 
        {
         delete s_pt(row_i,col_i);
        }
      }

     // setup the preconditioner
     if (Subsidiary_preconditioner_function_pt != 0)
      {
       Preconditioner_pt = (*Subsidiary_preconditioner_function_pt)();
      }
     else
      {
       Preconditioner_pt = new SuperLUPreconditioner;
      }

     Preconditioner_pt->setup(s_prec_pt,comm_pt());
     delete s_prec_pt;
    }
   else
    {
   // assemble dof_to_block_map
   unsigned ndof_types = this->ndof_types();
   Vector<unsigned> dof_to_block_map(ndof_types,0);
   for (unsigned i = ndof_types/2; i < ndof_types; i++)
    {
     dof_to_block_map[i] = 1;
    }

   this->block_setup(dof_to_block_map);

   // get block 11
   CRDoubleMatrix* s11_pt = 0;
   this->get_block(1,1,s11_pt);

   // add the scaled identity matrix to block 11
   double* s11_values = s11_pt->value();
   int* s11_column_index = s11_pt->column_index();
   int* s11_row_start = s11_pt->row_start();
   int s11_nrow_local = s11_pt->nrow_local();
   int s11_first_row = s11_pt->first_row();
   for (int i = 0; i < s11_nrow_local; i++)
    {
     bool found = false;
     for (int j = s11_row_start[i]; 
          j < s11_row_start[i+1] && !found; j++)
      {
       if (s11_column_index[j] == i + s11_first_row)
        {
         s11_values[j] += Scaling;
         found = true;
        }
      }
    }

   // get the remaining block and build the preconditioner
   DenseMatrix<CRDoubleMatrix* > s_pt(2,2,0);
   this->get_block(0,0,s_pt(0,0));
   this->get_block(0,1,s_pt(0,1));
   this->get_block(1,0,s_pt(1,0));
   s_pt(1,1) = s11_pt;
   
   CRDoubleMatrix* s_prec_pt
     = new CRDoubleMatrix(this->preconditioner_matrix_distribution_pt());

   CRDoubleMatrixHelpers::concatenate_without_communication(
     Block_distribution_pt,s_pt,*s_prec_pt);

   delete s_pt(0,0);
   delete s_pt(0,1);
   delete s_pt(1,0);
   delete s_pt(1,1);
   
   // setup the preconditioner
   if (Subsidiary_preconditioner_function_pt != 0)
    {
     Preconditioner_pt = (*Subsidiary_preconditioner_function_pt)();
    }
   else
    {
     Preconditioner_pt = new SuperLUPreconditioner;
    }
   Preconditioner_pt->setup(s_prec_pt,comm_pt());
   delete s_prec_pt;
    } // !UseRayCode

  }
   
 //=============================================================================
 /// \short Apply the preconditioner.
 //=============================================================================
 void PseudoElasticPreconditionerSubsidiaryPreconditioner::
 preconditioner_solve(const DoubleVector& r, DoubleVector& z)
 {
   DoubleVector x;
   this->get_block_ordered_preconditioner_vector(r,x);
   DoubleVector y;
   Preconditioner_pt->preconditioner_solve(x,y);
   this->return_block_ordered_preconditioner_vector(y,z);
  }



 ///////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////
 ///////////////////////////////////////////////////////////////////////////////


 
 //=============================================================================
 /// clean up the memory
 //=============================================================================
 void PseudoElasticPreconditionerSubsidiaryBlockPreconditioner::
 clean_up_memory()
 {
   //number of block types
  unsigned n_block = Diagonal_block_preconditioner_pt.size();
   
   //delete diagonal blocks
   for (unsigned i = 0 ; i < n_block; i++)
    {
     delete Diagonal_block_preconditioner_pt[i];
     Diagonal_block_preconditioner_pt[i] = 0;
     if (Method == 1)
      {
       for (unsigned j = i+1; j < n_block; j++)
        {
         delete Off_diagonal_matrix_vector_products(i,j);
         Off_diagonal_matrix_vector_products(i,j) = 0;
        }
      }
     else if (Method == 2)
      {
       for (unsigned j = 0; j < i; j++)
        {
         delete Off_diagonal_matrix_vector_products(i,j);
         Off_diagonal_matrix_vector_products(i,j) = 0;
        }
      }
    }
   
   // clean up the block preconditioner
   this->clear_block_preconditioner_base();
 }
 
 //=============================================================================
 /// \short Setup the preconditioner.
 //=============================================================================
 void PseudoElasticPreconditionerSubsidiaryBlockPreconditioner::
 setup()
 {
  // clean the memory
  this->clean_up_memory();
  
  // determine the number of DOF types
  unsigned n_dof_types =  this->ndof_types();
  
#ifdef PARANOID
  // must be Dim*2 dof types
  if (n_dof_types%2 != 0)
   {
    std::ostringstream error_message;
    error_message << "This preconditioner requires DIM*3 types of DOF";
    throw OomphLibError(error_message.str(),
                        OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
   }
#endif
  
  // store the dimension of the problem
  unsigned dim = n_dof_types/2;
  
  // assemble the dof to block lookup scheme
  Vector<unsigned> dof_to_block_map(n_dof_types,0);
  if(RayNamespace::UseRayCode)
  {
    for (unsigned d = 0; d < dim; d++) 
    {
      dof_to_block_map[d*2] = d;
      dof_to_block_map[d*2 + 1] = d;
    }
   
    //setup the blocks look up schemes
//    this->block_setup(dof_to_block_map);
    this->block_setup();

  }
  else
  {
    for (unsigned d = 0; d < dim; d++)
     {
      dof_to_block_map[d] = d;
      dof_to_block_map[d+dim] = d;
     }
    
    //setup the blocks look up schemes
    this->block_setup(dof_to_block_map);

  }
  

/* 
  // RAYRAY
  for (unsigned i = 0; i < dim; i++) 
  {
    CRDoubleMatrix* tmp_mat_pt = 0;
    this->get_block(i,0,tmp_mat_pt);
    unsigned tmp_mat_nrow = tmp_mat_pt->nrow();
    std::cout << "block " << i << ", nrow: " << tmp_mat_nrow << std::endl; 
  }
  //pause("From block preconditioner setup"); 
// */

  // Storage for the diagonal block preconditioners
  Diagonal_block_preconditioner_pt.resize(dim);
  
  // storage for the off diagonal matrix vector products
  Off_diagonal_matrix_vector_products.resize(dim,dim,0);

  std::cout << "About to setup block prec from exact" << std::endl; 

/*
  for (unsigned i = 0; i < this->Block_to_block_map.size(); i++) 
  {
    for (unsigned j = 0; j < this->Block_to_block_map[i].size(); j++) 
    {
      std::cout << this->Block_to_block_map[i][j] << " "; 
    }
    std::cout << std::endl; 
  }
  pause("done!!!"); 
*/
  
  
  // setup the subsidiary preconditioners
  for (unsigned d = 0; d < dim; d++)
   {
    Vector<unsigned> dof_list(2);
    if(RayNamespace::UseRayCode)
    {
      dof_list[0] = 2*d;
      dof_list[1] = 2*d+1;
    }
    else
    {
      dof_list[0]=d;
      dof_list[1]=d+dim;
    }

    Diagonal_block_preconditioner_pt[d] = new 
     PseudoElasticPreconditionerSubsidiaryPreconditioner; 
    Diagonal_block_preconditioner_pt[d]->
     turn_into_subsidiary_block_preconditioner(this,dof_list);
    if (Subsidiary_preconditioner_function_pt != 0)
     {
      Diagonal_block_preconditioner_pt[d]->
       set_subsidiary_preconditioner_function
       (Subsidiary_preconditioner_function_pt);
     }
    Diagonal_block_preconditioner_pt[d]->scaling() = Scaling;

    //??ds probably will work...
    Diagonal_block_preconditioner_pt[d]->set_nmesh(1);
    Diagonal_block_preconditioner_pt[d]->set_mesh(0,this->mesh_pt(0));
    if(RayNamespace::UseRayCode)
    {
      // Set precomputed blocks.
      DenseMatrix<CRDoubleMatrix*>prec_block_pt(2,2,0);
      for (unsigned i = 0; i < 2; i++) 
      {
        unsigned block_i = this->Block_to_block_map[d][i];

        for (unsigned j = 0; j < 2; j++) 
        {
          unsigned block_j = this->Block_to_block_map[d][j];

          prec_block_pt(i,j) 
            = this->Precomputed_block_pt(block_i,block_j);
        }
        
      }
      Diagonal_block_preconditioner_pt[d]->set_precomputed_blocks(
        prec_block_pt);
    }

    Diagonal_block_preconditioner_pt[d]->
     Preconditioner::setup(matrix_pt(),comm_pt());
    
    // the preconditioning method.\n
    // 0 - block diagonal\n
    // 1 - upper triangular\n
    // 2 - lower triangular\n 
    // next setup the off diagonal mat vec operators if required
    // RAYRAY need to look at this later
    if (Method == 1 || Method == 2)
     {
      unsigned l = d+1;
      unsigned u = dim;
      if (Method==2)
       {
        l = 0;
        u = d;
       }
      for (unsigned j = l; j < u; j++)
       {
        CRDoubleMatrix* block_matrix_pt = 0;
        this->get_block(d,j,block_matrix_pt);
        Off_diagonal_matrix_vector_products(d,j) 
         = new MatrixVectorProduct();
        if(this->Preconditioner_blocks_have_been_precomputed)
         {
          Off_diagonal_matrix_vector_products(d,j)
            ->setup(block_matrix_pt,
                    this->Precomputed_block_distribution_pt[j]);
         }
        else
         {
          Off_diagonal_matrix_vector_products(d,j)->setup(block_matrix_pt);
         }

        delete block_matrix_pt;
       }
     }
   } // setup the subsidiary preconditioner.
 } // preconditioner setup
 
 //=============================================================================
 /// Apply preconditioner to r
 //=============================================================================
 void PseudoElasticPreconditionerSubsidiaryBlockPreconditioner::
 preconditioner_solve(const DoubleVector &res, DoubleVector &z)
  {
   // copy r
   DoubleVector r(res);

   unsigned n_block;
   
   // Cache umber of block types (also the spatial DIM)
   if(RayNamespace::UseRayCode)
   {
     n_block = this->nblocks_precomputed();
   }
   else
   {
     n_block = this->nblock_types();
   }

   // loop parameters
   int start = n_block-1;
   int end = -1;
   int step = -1;
   if (Method != 1)
    {
     start = 0;
     end = n_block;
     step = 1;
    }

   // the preconditioning method.
   // 0 - block diagonal
   // 1 - upper triangular
   // 2 - lower triangular
   //
   // loop over the DIM
   // 
   // For Method = 0 or 2 (diagonal, lower)
   // start = 2, end = -1, step = -1
   // i = 2,1,0
   // 
   // For Method = 1 (upper)
   // start = 0, end = 3 step = 1
   // i = 0, 1, 2
   for (int i = start; i != end; i+=step)
    {
       
     // solve
     Diagonal_block_preconditioner_pt[i]->preconditioner_solve(r,z);
       
     // if upper or lower triangular
     if (Method != 0)
      {

       // substitute
       //
       for (int j = i + step; j !=end; j+=step)
        {
         DoubleVector x;
         this->get_block_vector(i,z,x);
         DoubleVector y;
         Off_diagonal_matrix_vector_products(j,i)->multiply(x,y);
         x.clear();
         this->get_block_vector(j,r,x);
         x -= y;
         this->return_block_vector(j,x,r);
        } // substitute
      } // if upper or lower
    } // for loop over DIM
  } // Block preconditioner solve
} // namespace oomph

//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//           Version 0.85. June 9, 2008.
//LIC// 
//LIC// Copyright (C) 2006-2008 Matthias Heil and Andrew Hazel
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
//This header defines a class for linear iterative solvers

//Include guards
#ifndef OOMPH_ITERATIVE_LINEAR_SOLVER_HEADER
#define OOMPH_ITERATIVE_LINEAR_SOLVER_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif


//oomph-lib headers
#include "matrices.h"
#include "problem.h"
#include "linear_solver.h"
#include "preconditioner.h"



namespace oomph
{


//=============================================================================
/// \short Base class for all linear iterative solvers.
/// This merely defines standard interfaces for linear iterative solvers,
/// so that different solvers can be used in a clean and transparent manner.
//=============================================================================
class IterativeLinearSolver : public LinearSolver
{
  
  public:
  
 /// \short Constructor: Set (default) trivial preconditioner and set
 /// defaults for tolerance and max. number of iterations
 IterativeLinearSolver()
  {      
   //Set pointer to default preconditioner
   Preconditioner_pt=&Default_preconditioner;
   
   //Set default convergence tolerance
   Tolerance=1.0e-8;
   
   //Set maximum number of iterations 
   Max_iter=1000;

   // set default for document convergence history
   Doc_convergence_history = false;

   // set default
   Setup_preconditioner_before_solve = true;
  }    
 
 /// Broken copy constructor
 IterativeLinearSolver(const IterativeLinearSolver&) 
  { 
   BrokenCopy::broken_copy("IterativeLinearSolver");
  } 
 
 /// Broken assignment operator
 void operator=(const IterativeLinearSolver&) 
  {
   BrokenCopy::broken_assign("IterativeLinearSolver");
  }

 /// Destructor (empty)
 virtual ~IterativeLinearSolver() {}
		
 /// Access function to preconditioner
 Preconditioner*& preconditioner_pt(){return Preconditioner_pt;}
 
 /// Access function to preconditioner (const version)
 Preconditioner* const &preconditioner_pt() const {return Preconditioner_pt;}
  
 /// Access to convergence tolerance
 double& tolerance() {return Tolerance;}
 
 /// Access to max. number of iterations
 unsigned& max_iter() {return Max_iter;}
 
 /// Number of iterations taken 
 virtual unsigned iterations()=0;
 
 /// \short Access to flag indicating if the convergence history is to be
 /// documented. By default history is doced on oomph_info stream.
 bool& doc_convergence_history() 
  {
   return Doc_convergence_history;
  }
 
 /// \short Write convergence history into file with specified filename
 /// (automatically switches on doc). Optional second argument is a string
 /// that can be used (as a zone title) to identify what case
 /// we're running (e.g. what combination of linear solver and
 /// preconditioner or parameter values are used).
 void open_convergence_history_file_stream(const std::string& file_name,
                                           const std::string& zone_title="")
  {
   // start docing
   Doc_convergence_history=true;
   
   // Close if it's open
   if (Output_file_stream.is_open())
    {
     Output_file_stream.close();
    }

   // Open new one
   Output_file_stream.open(file_name.c_str());

   // Write tecplot zone header
   Output_file_stream << "VARIABLES=\"iterations\", \"scaled residual\"" 
                      << std::endl;   
   Output_file_stream << "ZONE T=\"" << zone_title << "\""
                      << std::endl;   
   Output_file_stream << 0 << " " << 1.0 << std::endl;   
  }

 /// Close convergence history output stream
 void close_convergence_history_file_stream()
  {
   if (Output_file_stream.is_open()) Output_file_stream.close();
  }

 ///  \short returns the time taken to assemble the jacobian matrix and 
 /// residual vector
 double jacobian_setup_time()
  {
   return Jacobian_setup_time;
  }

 /// \short return the time taken to solve the linear system
 double linear_solver_solution_time()
  {
   return Solution_time;
  }

 /// \short returns the the time taken to setup the preconditioner 
 virtual double preconditioner_setup_time()
  {
   return Preconditioner_setup_time;
  }

 /// \short boolean indicating whether a preconditioner should be set up
 /// before solve
 bool& setup_preconditioner_before_solve()
  {
   return Setup_preconditioner_before_solve;
  }

  protected:

 /// \short Flag indicating if the convergence history is to be
 /// documented
 bool Doc_convergence_history;

 /// Output file stream for convergence history
 std::ofstream Output_file_stream;
 
 /// \short Default preconditioner:  The base
 /// class for preconditioners is a fully functional (if trivial!)
 /// preconditioner.
 static IdentityPreconditioner Default_preconditioner;
  
 ///Convergence tolerance
 double Tolerance;
 
 ///Maximum number of iterations
 unsigned Max_iter;
 
 /// Pointer to the preconditioner
 Preconditioner* Preconditioner_pt;

 /// Jacobian setup time
 double Jacobian_setup_time;

 /// linear solver solution time
 double Solution_time;

 /// Preconditioner setup time
 double Preconditioner_setup_time;

 /// \short indicates whether the preconditioner should be setup before solve.
 /// Default = true;
 bool Setup_preconditioner_before_solve;
};

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////





//======================================================================
/// \short The conjugate gradient method.
//======================================================================
template<typename MATRIX>
class CG : public IterativeLinearSolver 
{

  public:
 
 ///Constructor
 CG() : Iterations(0), Matrix_pt(0), Resolving(false), 
  Matrix_can_be_deleted(true)
  {}
 
 
 /// Destructor (cleanup storage)
 virtual ~CG()
  {
   clean_up_memory();
  }
 
 /// Broken copy constructor
 CG(const CG&) 
  { 
   BrokenCopy::broken_copy("CG");
  } 
 
 /// Broken assignment operator
 void operator=(const CG&) 
  {
   BrokenCopy::broken_assign("CG");
  }
 
 /// Overload disable resolve so that it cleans up memory too
 void disable_resolve()
  {
   LinearSolver::disable_resolve();
   clean_up_memory();
  }
 
 
 /// \short Solver: Takes pointer to problem and returns the results vector
 /// which contains the solution of the linear system defined by
 /// the problem's fully assembled Jacobian and residual vector.
 void solve(Problem* const &problem_pt, DoubleVector &result);
 
 /// \short Linear-algebra-type solver: Takes pointer to a matrix and rhs 
 /// vector and returns the solution of the linear system. Problem pointer 
 /// defaults to NULL and can be omitted in linear-algebra-type solves in which
 /// the preconditioner doesn't (mustn't!) require a pointer to an
 /// associated Problem.
 void solve(DoubleMatrixBase* const &matrix_pt,
            const DoubleVector &rhs,
            DoubleVector &solution)
  {
   // Store the matrix if required
   if ((Enable_resolve)&&(!Resolving))
    {
     Matrix_pt=dynamic_cast<MATRIX*>(matrix_pt);

     // Matrix has been passed in from the outside so we must not
     // delete it
     Matrix_can_be_deleted=false;
    }

   // set the distribution
   if (dynamic_cast<DistributableLinearAlgebraObject*>(matrix_pt))
    {
     // the solver has the same distribution as the matrix if possible
     Distribution_pt->rebuild(dynamic_cast<DistributableLinearAlgebraObject*>
                              (matrix_pt)->distribution_pt());
    }
   else
    {
     // the solver has the same distribution as the RHS
     Distribution_pt->rebuild(rhs.distribution_pt());
    }

   //Call the helper function with no problem
   this->solve_helper(matrix_pt,rhs,solution,0);
  }
 
 /// \short Re-solve the system defined by the last assembled Jacobian
 /// and the rhs vector specified here. Solution is returned in the 
 /// vector result.
 void resolve(const DoubleVector &rhs, DoubleVector &result);
 
 /// Number of iterations taken 
 unsigned iterations() 
  {
   return Iterations;
  }
 
 
  private:

 /// General interface to solve function 
 void solve_helper(DoubleMatrixBase* const &matrix_pt,
                   const DoubleVector &rhs,
                   DoubleVector &solution,
                   Problem* problem_pt);

 
 /// Cleanup data that's stored for resolve (if any has been stored)
 void clean_up_memory()
  {
   if ((Matrix_pt!=0)&&(Matrix_can_be_deleted))
    {
     delete Matrix_pt;
     Matrix_pt=0;
    }
  }
 
 /// Number of iterations taken
 unsigned Iterations;
 
 /// Pointer to matrix
 MATRIX* Matrix_pt;
 
 /// \short Boolean flag to indicate if the solve is done in re-solve mode,
 /// bypassing setup of matrix and preconditioner
 bool Resolving;
 
 /// \short Boolean flag to indicate if the matrix pointed to be Matrix_pt
 /// can be deleted.
 bool Matrix_can_be_deleted;
};




///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////





//======================================================================
/// \short The conjugate gradient method.
//======================================================================
template<typename MATRIX>
class BiCGStab : public IterativeLinearSolver 
{

 public:

  ///Constructor
 BiCGStab() : Iterations(0), Matrix_pt(0), Resolving(false), 
  Matrix_can_be_deleted(true)
   {}
  

 /// Destructor (cleanup storage)
 virtual ~BiCGStab()
  {
   clean_up_memory();
  }
 
 /// Broken copy constructor
 BiCGStab(const BiCGStab&) 
  { 
   BrokenCopy::broken_copy("BiCGStab");
  } 
 
 /// Broken assignment operator
 void operator=(const BiCGStab&) 
  {
   BrokenCopy::broken_assign("BiCGStab");
  }

 

 /// Overload disable resolve so that it cleans up memory too
 void disable_resolve()
  {
   LinearSolver::disable_resolve();
   clean_up_memory();
  }
 
 /// \short Solver: Takes pointer to problem and returns the results vector
 /// which contains the solution of the linear system defined by
 /// the problem's fully assembled Jacobian and residual vector.
 void solve(Problem* const &problem_pt, DoubleVector &result);
 
 /// \short Linear-algebra-type solver: Takes pointer to a matrix and rhs 
 /// vector and returns the solution of the linear system. Problem pointer 
 /// defaults to NULL and can be omitted in linear-algebra-type solves in which
 /// the preconditioner doesn't (mustn't!) require a pointer to an
 /// associated Problem.
 void solve(DoubleMatrixBase* const &matrix_pt,
            const DoubleVector& rhs,
            DoubleVector &solution)
 {
  // Store the matrix if required
  if ((Enable_resolve)&&(!Resolving))
   {
    Matrix_pt=dynamic_cast<MATRIX*>(matrix_pt);
    
    // Matrix has been passed in from the outside so we must not
    // delete it
    Matrix_can_be_deleted=false;
   }

 // set the distribution
 if (dynamic_cast<DistributableLinearAlgebraObject*>(matrix_pt))
  {
   // the solver has the same distribution as the matrix if possible
   Distribution_pt->rebuild(dynamic_cast<DistributableLinearAlgebraObject*>
                            (matrix_pt)->distribution_pt());
  }
 else
  {
   // the solver has the same distribution as the RHS
   Distribution_pt->rebuild(rhs.distribution_pt());
  }

  //Call the helper function with no problem
  this->solve_helper(matrix_pt,rhs,solution,0);
 }
 
 
 /// \short Re-solve the system defined by the last assembled Jacobian
 /// and the rhs vector specified here. Solution is returned in the 
 /// vector result.
 void resolve(const DoubleVector &rhs, 
              DoubleVector &result);
 

 /// Number of iterations taken 
 unsigned iterations() 
  {
   return Iterations;
  }

 
  private:

  /// General interface to solve function 
 void solve_helper(DoubleMatrixBase* const &matrix_pt,
                   const DoubleVector &rhs,
                   DoubleVector &solution,
                   Problem* problem_pt);
 
 /// Cleanup data that's stored for resolve (if any has been stored)
 void clean_up_memory()
  {
   if ((Matrix_pt!=0)&&(Matrix_can_be_deleted))
    {
     delete Matrix_pt;
     Matrix_pt=0;
    }
  }
 
 /// Number of iterations taken
 unsigned Iterations;

 /// Pointer to matrix
 MATRIX* Matrix_pt;
 
 /// \short Boolean flag to indicate if the solve is done in re-solve mode,
 /// bypassing setup of matrix and preconditioner
 bool Resolving;
  
 /// \short Boolean flag to indicate if the matrix pointed to be Matrix_pt
 /// can be deleted.
 bool Matrix_can_be_deleted;

};




///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////



//=========================================================================
///\short The Gauss Seidel method
//=========================================================================
template<typename MATRIX>
class GS : public IterativeLinearSolver 
{

 public:

 ///Constructor
 GS() : Iterations(0), Matrix_pt(0), Resolving(false), 
  Matrix_can_be_deleted(true)
  {}


 /// Destructor (cleanup storage)
 virtual ~GS()
  {
   clean_up_memory();
  }


 /// Broken copy constructor
 GS(const GS&) 
  { 
   BrokenCopy::broken_copy("GS");
  } 
 
 /// Broken assignment operator
 void operator=(const GS&) 
  {
   BrokenCopy::broken_assign("GS");
  }
 
 /// Overload disable resolve so that it cleans up memory too
 void disable_resolve()
  {
   LinearSolver::disable_resolve();
   clean_up_memory();
  }
  
 /// \short Solver: Takes pointer to problem and returns the results vector
 /// which contains the solution of the linear system defined by
 /// the problem's fully assembled Jacobian and residual vector.
 void solve(Problem* const &problem_pt, DoubleVector &result);
 
 /// \short Linear-algebra-type solver: Takes pointer to a matrix and rhs
 /// vector and returns the solution of the linear system. Problem pointer
 /// defaults to NULL and can be omitted in linear-algebra-type solves in which
 /// the preconditioner doesn't (mustn't!) require a pointer to an
 /// associated Problem.
 void solve(DoubleMatrixBase* const &matrix_pt,
            const DoubleVector &rhs,
            DoubleVector &solution)
 {
  // setup the distribution
  Distribution_pt->rebuild(rhs.distribution_pt());

  // Store the matrix if required
  if ((Enable_resolve)&&(!Resolving))
   {
    Matrix_pt=dynamic_cast<MATRIX*>(matrix_pt);

    // Matrix has been passed in from the outside so we must not
    // delete it
    Matrix_can_be_deleted=false;
   }
  
  //Call the helper function with no problem
  this->solve_helper(matrix_pt,rhs,solution,0);
 }
 
 /// \short Re-solve the system defined by the last assembled Jacobian
 /// and the rhs vector specified here. Solution is returned in the
 /// vector result.
 void resolve(const DoubleVector &rhs,
              DoubleVector &result);


 /// \short returns the the time taken to setup the preconditioner 
 double preconditioner_setup_time()
  {
   throw OomphLibError(
    "Gauss Seidel is not a preconditionable iterative solver",
    "GS<MATRIX>::preconditioner_setup_time()",
    OOMPH_EXCEPTION_LOCATION);
   return 0;
  }


 /// Number of iterations taken 
 unsigned iterations() 
  {
   return Iterations;
  }
 

  private:

  /// General interface to solve function 
 void solve_helper(DoubleMatrixBase* const &matrix_pt,
                   const DoubleVector &rhs,
                   DoubleVector &solution,
                   Problem* problem_pt);
 

 /// Cleanup data that's stored for resolve (if any has been stored)
 void clean_up_memory()
  {
   if ((Matrix_pt!=0)&&(Matrix_can_be_deleted))
    {
     delete Matrix_pt;
     Matrix_pt=0;
    }
  }
 
 /// Number of iterations taken
 unsigned Iterations;

 /// Pointer to matrix
 MATRIX* Matrix_pt;
 
 /// \short Boolean flag to indicate if the solve is done in re-solve mode,
 /// bypassing setup of matrix and preconditioner
 bool Resolving;
 
 /// \short Boolean flag to indicate if the matrix pointed to be Matrix_pt
 /// can be deleted.
 bool Matrix_can_be_deleted;

};


///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////



//======================================================================
/// \short The GMRES method.
//======================================================================
template<typename MATRIX>
class GMRES : public IterativeLinearSolver 
{

 public:

  ///Constructor
 GMRES() : Iterations(0), Matrix_pt(0), Resolving(false), 
  Matrix_can_be_deleted(true)
   {
    Preconditioner_LHS = true;
    Iteration_restart = false;
   }
  
 /// Destructor (cleanup storage)
 virtual ~GMRES()
  {
   clean_up_memory();
  }
 
 /// Broken copy constructor
 GMRES(const GMRES&) 
  { 
   BrokenCopy::broken_copy("GMRES");
  } 
 
 /// Broken assignment operator
 void operator=(const GMRES&) 
  {
   BrokenCopy::broken_assign("GMRES");
  }
 
 /// Overload disable resolve so that it cleans up memory too
 void disable_resolve()
  {
   LinearSolver::disable_resolve();
   clean_up_memory();
  }
  
 /// \short Solver: Takes pointer to problem and returns the results vector
 /// which contains the solution of the linear system defined by
 /// the problem's fully assembled Jacobian and residual vector.
 void solve(Problem* const &problem_pt, DoubleVector &result);
 
 /// \short Linear-algebra-type solver: Takes pointer to a matrix and rhs
 /// vector and returns the solution of the linear system. Problem pointer
 /// defaults to NULL and can be omitted in linear-algebra-type solves in which
 /// the preconditioner doesn't (mustn't!) require a pointer to an
 /// associated Problem.
 void solve(DoubleMatrixBase* const &matrix_pt,
            const DoubleVector &rhs,
            DoubleVector &solution)
 {
  // setup the distribution
  Distribution_pt->rebuild(rhs.distribution_pt());

  // Store the matrix if required
  if ((Enable_resolve)&&(!Resolving))
   {
    Matrix_pt=dynamic_cast<MATRIX*>(matrix_pt);
    
    // Matrix has been passed in from the outside so we must not
    // delete it
    Matrix_can_be_deleted=false;
   }

  //Call the helper function with no problem
  this->solve_helper(matrix_pt,rhs,solution,0);
 }
 
 /// \short Re-solve the system defined by the last assembled Jacobian
 /// and the rhs vector specified here. Solution is returned in the
 /// vector result.
 void resolve(const DoubleVector &rhs,
              DoubleVector &result);

 /// Number of iterations taken 
 unsigned iterations() 
  {
   return Iterations;
  }

 /// \short access function indicating whether restarted GMRES is used
 bool iteration_restart() const
  {
   return Iteration_restart;
  }

 /// \short switches on iteration restarting and takes as an argument the 
 /// number of iterations after which the construction of the orthogonalisation
 /// basis vectors should be restarted
 void enable_iteration_restart(const unsigned& restart)
  {
   Restart = restart;
   Iteration_restart = true;
  }

 /// switches off iteration restart
 void disable_iteration_restart()
  {
   Iteration_restart = false;
  }

 /// \short Access function to Preconditioner_LHS, the flag which determines
 /// if left hand or right hand preconditioning is used
 bool& preconditioner_LHS() {return Preconditioner_LHS;}
 
  private:

 /// General interface to solve function 
 void solve_helper(DoubleMatrixBase* const &matrix_pt,
                   const DoubleVector &rhs,
                   DoubleVector &solution,
                   Problem* problem_pt);
 
 /// Cleanup data that's stored for resolve (if any has been stored)
 void clean_up_memory()
  {
   if ((Matrix_pt!=0)&&(Matrix_can_be_deleted))
    {
     delete Matrix_pt;
     Matrix_pt=0;
    }
  }

 /// Helper function to update the result vector
 void update(const unsigned& k, const Vector<Vector<double> >& H, 
             const Vector<double>& s, const Vector<DoubleVector>& v,
             DoubleVector &x)
  {
   // Make a local copy of s
   Vector<double> y(s);
   
   // Backsolve:  
   for (int i = int(k); i >= 0; i--) 
    {
     y[i] /= H[i][i];
     for (int j = i - 1; j >= 0; j--)
      {
       y[j] -= H[i][j] * y[i];
      }
    }

   unsigned n_x=x.nrow();
   DoubleVector temp(x.distribution_pt(),0.0);
   DoubleVector z(x.distribution_pt(),0.0);

   // x = Vy
   double* temp_pt = temp.values_pt();
   for (unsigned j = 0; j <= k; j++)
    {
     double* vj_pt = v[j].values_pt();
     for (unsigned i = 0; i < n_x; i++)
      {
       temp_pt[i] += vj_pt[i] * y[j];
      }
    }
   
   if(Preconditioner_LHS)
    { 
     x += temp;
    }
   else
    {
     //x=x0+M^{-1}Vy by saad p270
     preconditioner_pt()->preconditioner_solve(temp,z);
     x+=z;
    }   
  }
 
 /// \short Helper function: Generate plane rotation
 void generate_plane_rotation(double &dx, double &dy, double &cs, double &sn)
  {
   if (dy == 0.0) 
    {
     cs = 1.0;
     sn = 0.0;
    } 
   else if (fabs(dy) > fabs(dx)) 
    {
     double temp = dx / dy;
     sn = 1.0 / sqrt( 1.0 + temp*temp );
     cs = temp * sn;
    } 
   else 
    {
     double temp = dy / dx;
     cs = 1.0 / sqrt( 1.0 + temp*temp );
     sn = temp * cs;
    }
  }

 /// \short Helper function: Apply plane rotation
 void apply_plane_rotation(double &dx, double &dy, double &cs, double &sn)
  {
   double temp  =  cs * dx + sn * dy;
   dy = -sn * dx + cs * dy;
   dx = temp;
  }

 /// Number of iterations taken
 unsigned Iterations;

 /// \short The number of iterations before the iteration proceedure is 
 /// restarted if iteration restart is used
 unsigned Restart;

 /// boolean indicating if iteration restarting is used
 bool Iteration_restart;

 /// Pointer to matrix
 MATRIX* Matrix_pt;
 
 /// \short Boolean flag to indicate if the solve is done in re-solve mode,
 /// bypassing setup of matrix and preconditioner
 bool Resolving;

 /// \short Boolean flag to indicate if the matrix pointed to be Matrix_pt
 /// can be deleted.
 bool Matrix_can_be_deleted;

 /// \short boolean indicating use of left hand preconditioning (if true)
 /// or right hand preconditioning (if false)
 bool Preconditioner_LHS;
};
}

#endif

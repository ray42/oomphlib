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
// Driver code for a simple test poisson problem using a mesh
// generated from an input file generated by the triangle mesh generator
// Triangle.

//Generic routines
#include "generic.h"

// Poisson equations
#include "poisson.h"

// The mesh
#include "meshes/triangle_mesh.h"

using namespace std;

using namespace oomph;



//====================================================================
/// Namespace for exact solution for Poisson equation with sharp step 
//====================================================================
namespace TanhSolnForPoisson
{

 /// Parameter for steepness of step
 double Alpha;

 /// Parameter for angle of step
 double Beta;

 /// Shift of step along x-direction
 double X_shift=8.0;
 
 /// Flag for zero TanhSolution
 bool TanhSoln=false;

 /// Exact solution as a Vector
 void get_exact_u(const Vector<double>& x, Vector<double>& u)
 {
  if (TanhSoln)
   {
    u[0]=tanh(1.0-Alpha*(Beta*(x[0]-X_shift)-x[1]));
   }
  else
   {
    u[0]=0.0;
   }
 }


 /// Exact solution as a scalar
 void get_exact_u(const Vector<double>& x, double& u)
 {
  if (TanhSoln)
   {
    u=tanh(1.0-Alpha*(Beta*(x[0]-X_shift)-x[1]));
   }
  else
   {
    u=0.0;
   }
 }


 /// Source function to make it an exact solution 
 void get_source(const Vector<double>& x, double& source)
 {
  if (TanhSoln)
   {
    source = 2.0*tanh(-1.0+Alpha*(Beta*(x[0]-X_shift)-x[1]))*
     (1.0-pow(tanh(-1.0+Alpha*(Beta*(x[0]-X_shift)-x[1])),2.0))*
     Alpha*Alpha*Beta*Beta+2.0*tanh(-1.0+Alpha*(Beta*(x[0]-X_shift)-x[1]))*
     (1.0-pow(tanh(-1.0+Alpha*(Beta*(x[0]-X_shift)-x[1])),2.0))*Alpha*Alpha;
   }
  else
   {
    source=10.0;
   }
 }

}






//====================================================================
/// Micky mouse Poisson problem.
//====================================================================

// Poisson problem
template<class ELEMENT> 
class PoissonProblem : public Problem
{

public:


 /// \short Constructor: Pass pointer to source function and names of
 /// two triangle input files
 PoissonProblem(PoissonEquations<2>::PoissonSourceFctPt source_fct_pt,
                const string& node_file_name,
                const string& element_file_name,
                const string& poly_file_name);

 /// Destructor (empty)
 ~PoissonProblem(){}


 /// Update the problem specs before solve: (Re)set boundary conditions
 void actions_before_newton_solve()
  {
   //Loop over the boundaries
   unsigned num_bound = mesh_pt()->nboundary();
   for(unsigned ibound=0;ibound<num_bound;ibound++)
    {
     // Loop over the nodes on boundary
     unsigned num_nod=mesh_pt()->nboundary_node(ibound);
     for (unsigned inod=0;inod<num_nod;inod++)
      {
       Node* nod_pt=mesh_pt()->boundary_node_pt(ibound,inod);
       double u;
       Vector<double> x(2);
       x[0]=nod_pt->x(0);
       x[1]=nod_pt->x(1);
       TanhSolnForPoisson::get_exact_u(x,u);
       nod_pt->set_value(0,u);
      }
    }
  }

 /// Update the problem specs before solve (empty)
 void actions_after_newton_solve()
  {}


 /// Access function for the specific mesh
 TriangleMesh<ELEMENT>* mesh_pt() 
  {
   return dynamic_cast<TriangleMesh<ELEMENT>*>(Problem::mesh_pt());
  }

  /// Doc the solution
 void doc_solution(DocInfo& doc_info);

private:

 /// Pointer to source function
 PoissonEquations<2>::PoissonSourceFctPt Source_fct_pt;

};



//========================================================================
/// Constructor for Poisson problem
//========================================================================
template<class ELEMENT>
PoissonProblem<ELEMENT>::
 PoissonProblem(PoissonEquations<2>::PoissonSourceFctPt source_fct_pt,
                const string& node_file_name,
                const string& element_file_name,
                const string& poly_file_name)
        : Source_fct_pt(source_fct_pt)
{ 

 // Setup parameters for exact tanh solution

 // Steepness of step
 TanhSolnForPoisson::Alpha=1.0; 

 // Orientation of step
 TanhSolnForPoisson::Beta=1.4;

 //Create mesh
  Problem::mesh_pt() = new TriangleMesh<ELEMENT>(node_file_name,
                                                 element_file_name,
                                                 poly_file_name);

 // Set the boundary conditions for this problem: All nodes are
 // free by default -- just pin the ones that have Dirichlet conditions
 // here. 
 unsigned num_bound = mesh_pt()->nboundary();
 for(unsigned ibound=0;ibound<num_bound;ibound++)
 {
   unsigned num_nod= mesh_pt()->nboundary_node(ibound);
   for (unsigned inod=0;inod<num_nod;inod++)
   {
     mesh_pt()->boundary_node_pt(ibound,inod)->pin(0);
   }
 }

 // Complete the build of all elements so they are fully functional

 //Find number of elements in mesh
 unsigned n_element = mesh_pt()->nelement();

 // Loop over the elements to set up element-specific 
 // things that cannot be handled by constructor
 for(unsigned i=0;i<n_element;i++)
  {
   // Upcast from GeneralElement to the present element
   ELEMENT* el_pt = dynamic_cast<ELEMENT*>(mesh_pt()->element_pt(i));

   //Set the source function pointer
   el_pt->source_fct_pt() = Source_fct_pt;
  }

 // Setup equation numbering scheme
 cout <<"Number of equations: " << assign_eqn_numbers() << std::endl; 

}



//========================================================================
/// Doc the solution
//========================================================================
template<class ELEMENT>
void PoissonProblem<ELEMENT>::doc_solution(DocInfo& doc_info)
{ 

 ofstream some_file;
 char filename[100];

 // Number of plot points
 unsigned npts;
 npts=5;



 // Output boundaries
 //------------------
 sprintf(filename,"%s/boundaries.dat",doc_info.directory().c_str());
 some_file.open(filename);
 mesh_pt()->output_boundaries(some_file);
 some_file.close();


 // Output solution
 //----------------
 sprintf(filename,"%s/soln%i.dat",doc_info.directory().c_str(),
         doc_info.number());
 some_file.open(filename);
 mesh_pt()->output(some_file,npts);
 some_file.close();

 // Exact solution available?
 if (TanhSolnForPoisson::TanhSoln)
  {
   

   // Output exact solution 
   //----------------------
   sprintf(filename,"%s/exact_soln%i.dat",doc_info.directory().c_str(),
           doc_info.number());
   some_file.open(filename);
   mesh_pt()->output_fct(some_file,npts,TanhSolnForPoisson::get_exact_u); 
   some_file.close();
   
   
   // Doc error
   //----------
   double error,norm;
   sprintf(filename,"%s/error%i.dat",doc_info.directory().c_str(),
           doc_info.number());
   some_file.open(filename);
   mesh_pt()->compute_error(some_file,TanhSolnForPoisson::get_exact_u,
                            error,norm); 
   some_file.close();
   cout << "error: " << sqrt(error) << std::endl; 
   cout << "norm : " << sqrt(norm) << std::endl << std::endl;

  }

 }

 



//========================================================================
/// Demonstrate how to solve Poisson problem
//========================================================================
int main(int argc, char* argv[])
{
 // Store command line arguments
 CommandLineArgs::setup(argc,argv);

 // Check number of command line arguments: Need exactly four.
 if (argc!=4)
  {
   std::string error_message =
    "Wrong number of command line arguments.\n";
   error_message +=
    "Must specify the following file names  \n";
   error_message += 
    "filename.node then filename.ele then filename.poly\n";

   throw OomphLibError(error_message,
                       OOMPH_CURRENT_FUNCTION,
                       OOMPH_EXCEPTION_LOCATION);
  }



 // Convert arguments to strings that specify the input file names
 string node_file_name(argv[1]);
 string element_file_name(argv[2]);
 string poly_file_name(argv[3]);

 // Label for output
 DocInfo doc_info;
 
 // Output directory
 doc_info.set_directory("RESLT");

 
 
 // Linear Elements
 //----------------
 {
  cout << std::endl << "Linear elements" << std::endl;
  cout <<              "===============" << std::endl << std::endl;
  
  //Set up the problem
  PoissonProblem<TPoissonElement<2,2> > 
   problem(&TanhSolnForPoisson::get_source,
           node_file_name,
           element_file_name,
           poly_file_name);
 
  // Use TanhSolution?
  TanhSolnForPoisson::TanhSoln=true;
  
  // Solve once with forcing that makes TanhSolution an exact solution
  // and once with constant source fct and zero bc
  unsigned nstep=2;
  for (unsigned istep=0;istep<nstep;istep++)
   {
    // Solve the problem
    problem.newton_solve();
    
    //Output solution
    problem.doc_solution(doc_info);
    
    //Increment counter for solutions 
    doc_info.number()++;
    
    // Use TanhSolution?
    TanhSolnForPoisson::TanhSoln=false;   
   }
 }


 // Quadratic Elements
 //-------------------
 {
  cout << std::endl  << "Quadratic elements" << std::endl;
  cout <<               "===================" << std::endl << std::endl;

  //Set up the problem
  PoissonProblem<TPoissonElement<2,3> > 
   problem(&TanhSolnForPoisson::get_source,
           node_file_name,
           element_file_name,
           poly_file_name);
 
  // Use TanhSolution?
  TanhSolnForPoisson::TanhSoln=true;
  
  // Solve once with forcing that makes TanhSolution an exact solution
  // and once with constant source fct and zero bc
  unsigned nstep=2;
  for (unsigned istep=0;istep<nstep;istep++)
   {
    // Solve the problem
    problem.newton_solve();
    
    //Output solution
    problem.doc_solution(doc_info);
    
    //Increment counter for solutions 
    doc_info.number()++;
    
    // Use TanhSolution?
    TanhSolnForPoisson::TanhSoln=false;   
   }
 }


 // Cubic Elements
 //---------------
 {
  cout << std::endl << "Cubic elements" << std::endl;
  cout <<              "==============" << std::endl << std::endl;

  //Set up the problem
  PoissonProblem<TPoissonElement<2,4> > 
   problem(&TanhSolnForPoisson::get_source,
           node_file_name,
           element_file_name,
           poly_file_name);
 
  // Use TanhSolution?
  TanhSolnForPoisson::TanhSoln=true;
  
  // Solve once with forcing that makes TanhSolution an exact solution
  // and once with constant source fct and zero bc
  unsigned nstep=2;
  for (unsigned istep=0;istep<nstep;istep++)
   {
    // Solve the problem
    problem.newton_solve();
    
    //Output solution
    problem.doc_solution(doc_info);
    
    //Increment counter for solutions 
    doc_info.number()++;
    
    // Use TanhSolution?
    TanhSolnForPoisson::TanhSoln=false;   
   }
 }


 

}




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
//Header file for KL shell elements
#ifndef OOMPH_KIRCHHOFF_LOVE_SHELL_ELEMENTS_HEADER
#define OOMPH_KIRCHHOFF_LOVE_SHELL_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//OOMPH-LIB header
#include "../generic/hermite_elements.h"
#include "../generic/geom_objects.h"
#include "../generic/fsi.h"
#include "../generic/stored_shape_function_elements.h"

namespace oomph
{

//========================================================================
/// A class for elements that solves the equations of Kirchhoff Love shell 
/// thin-shell theory -- hierher: Annotate
//========================================================================
class KirchhoffLoveShellEquations : public virtual SolidFiniteElement
{
  private:
 
 /// Static default value for the Poisson's ratio
 static double Default_nu_value;

 /// Static default value for the timescale ratio (1.0 for natural scaling)
 static double Default_lambda_sq_value;

 /// Static default value for the thickness ratio
 static double Default_h_value;

 /// Pointer to Poisson's ratio
 double *Nu_pt;

 /// Pointer to dimensionless wall thickness
 double *H_pt;

 /// Pointer to timescale ratio (non-dimensional density)
 double *Lambda_sq_pt;

 /// \short Pointer to membrane pre-stress terms -- should 
 /// probably generalise this to function pointers at some point
 DenseMatrix<double*> Prestress_pt;

 /// Static default for prestress (set to zero)
 static double Zero_prestress;

protected:

 /// Invert a DIM by DIM matrix
 inline double calculate_contravariant
  (double A[2][2], double Aup[2][2]);

 /// Default load function (zero traction)
 static void Zero_traction_fct(const Vector<double> &xi,
                               const Vector<double> &x,
                               const Vector<double> &N,
                               Vector<double> &load);

 /// \short Pointer to load vector function: Its arguments are: 
 /// Lagrangian coordinate, Eulerian coordinate, normal vector and 
 /// load vector itself (not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases)
 void (*Load_vector_fct_pt)(const Vector<double> &xi,
                            const Vector<double> &x,
                            const Vector<double> &N,
                            Vector<double> &load);

               
 /// \short Pointer to the GeomObject that specifies the undeformed midplane
 /// of the shell
 GeomObject* Undeformed_midplane_pt;


 /// short Helper function to Return the residuals for 
 /// the equations of KL shell theory. This is used to prevent
 /// a call of fill_in_contribution_to_residuals in 
 /// the function fill_in_contribution_to_jacobian that can
 /// lead to virtual inheritance woes if this element is ever
 /// used as part of a multi-physics element.
 void fill_in_contribution_to_residuals_shell(Vector<double> &residuals);

 /// \short Get the load vector for the computation of the rate of work
 /// done by the load. Here we simply forward this to
 /// load_vector(...) but allow it to be overloaded in derived classes
 /// to allow the computation of the rate of work due to constituent
 /// parts of the load vector (e.g. the fluid traction in an FSI 
 /// problem). Pass number of integration point (dummy), 
 /// Lagr. coordinate and normal vector and return the load vector
 /// (not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases). 
 virtual void load_vector_for_rate_of_work_computation(
  const unsigned &intpt,
  const Vector<double>& xi,
  const Vector<double>& x,
  const Vector<double>& N,
  Vector<double>& load)
  {
   load_vector(intpt, xi, x, N ,load);
  }
 

public:

 /// Constructor: Initialise physical parameter values to defaults
 KirchhoffLoveShellEquations() : Undeformed_midplane_pt(0)
  {
   //Set the default values of physical parameters
   Nu_pt = &Default_nu_value;
   Lambda_sq_pt = &Default_lambda_sq_value;
   H_pt = &Default_h_value;

   //Default load is zero traction
   Load_vector_fct_pt = &Zero_traction_fct;

   // Default prestress is zero
   Prestress_pt.resize(2,2);
   Prestress_pt(0,0)=&Zero_prestress;
   Prestress_pt(1,0)=&Zero_prestress;
   Prestress_pt(0,1)=&Zero_prestress;
   Prestress_pt(1,1)=&Zero_prestress;
  }

 /// Access to the load vector function pointer
 void (* &load_vector_fct_pt())(const Vector<double>& xi,
                                const Vector<double>& x,
                                const Vector<double>& N,
                                Vector<double>& load) 
  {return Load_vector_fct_pt;}

 /// \short Get the load vector: Pass number of integration point (dummy), 
 /// Lagr. coordinate and normal vector and return the load vector
 /// (not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases). This function is virtual so it can be 
 /// overloaded for FSI.
 virtual void load_vector(const unsigned &intpt,
                          const Vector<double>& xi,
                          const Vector<double>& x,
                          const Vector<double>& N,
                          Vector<double>& load)
  {
   Load_vector_fct_pt(xi,x,N,load);
  }
 

 /// \short Set pointer to (i,j)-th component of second Piola Kirchhoff 
 /// membrane prestress to specified value (automatically imposes 
 /// symmetry for off-diagonal entries)
 void set_prestress_pt(const unsigned& i,
                       const unsigned& j,
                       double* value_pt)
  {
   Prestress_pt(i,j)=value_pt;
   Prestress_pt(j,i)=value_pt;
  }

 /// \short Return (i,j)-th component of second Piola Kirchhoff membrane 
 /// prestress
 double prestress(const unsigned& i,
                  const unsigned& j)
  {
   return *Prestress_pt(i,j);
  }

 /// Return the Poisson's ratio
 const double &nu() const {return *Nu_pt;}

 /// Return the wall thickness to undeformed radius ratio
 const double &h() const {return *H_pt;}

 /// Return the timescale ratio (non-dimensional density)
 const double& lambda_sq() const {return *Lambda_sq_pt;}
 
 /// Return a pointer to the Poisson ratio
 double* &nu_pt() {return Nu_pt;}

 /// Return a pointer to the non-dim wall thickness
 double* &h_pt() {return H_pt;}

 /// Return a pointer to timescale ratio (nondim density)
 double* &lambda_sq_pt() {return Lambda_sq_pt;}

 /// \short Return a reference to geometric object that specifies the shell's
 /// undeformed geometry
 GeomObject*& undeformed_midplane_pt() {return Undeformed_midplane_pt;}
 
 /// \short Get normal vector
 void get_normal(const Vector<double>& s, Vector<double>& N);

 ///Overload the standard fill in residuals contribution
 void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Simply call the shell residuals
   fill_in_contribution_to_residuals_shell(residuals);
  }
 
 /// Return the jacobian is calculated by finite differences by default,
 void fill_in_contribution_to_jacobian(Vector<double> &residuals, 
                                   DenseMatrix<double> &jacobian);

 /// \short Get potential (strain) and kinetic energy of the element
 void get_energy(double& pot_en, double& kin_en);

 /// \short Get integral of instantaneous rate of work done on 
 /// the wall due to the load returned by the virtual 
 /// function load_vector_for_rate_of_work_computation(...). 
 /// In the current class
 /// the latter function simply de-references the external
 /// load but this can be overloaded in derived classes
 /// (e.g. in FSI elements) to determine the rate of work done
 /// by individual constituents of this load (e.g. the fluid load
 /// in an FSI problem). hierher spell out nondimensionalisation
 double load_rate_of_work();

 /// Generic FiniteElement output function
 void output(std::ostream &outfile) {FiniteElement::output(outfile);}

 /// Generic FiniteElement output function
 void output(std::ostream &outfile, const unsigned &n_plot)
  {FiniteElement::output(outfile,n_plot);}

 /// Generic FiniteElement output function
 void output(FILE* file_pt) {FiniteElement::output(file_pt);}

 /// Generic FiniteElement output function
 void output(FILE* file_pt, const unsigned &n_plot)
  {FiniteElement::output(file_pt,n_plot);}

}; 



//================================================================== 
/// Matrix inversion for 2 dimensions
//================================================================== 
double KirchhoffLoveShellEquations::calculate_contravariant
(double A[2][2], double Aup[2][2])
{
 //Calculate determinant
 double det = A[0][0]*A[1][1] - A[0][1]*A[1][0];

 //Calculate entries of the inverse
 Aup[0][0] = A[1][1]/det;
 Aup[0][1] = -A[0][1]/det;
 Aup[1][0] = -A[1][0]/det;
 Aup[1][1] = A[0][0]/det;

 //Return determinant
 return(det);
}





//=======================================================================
/// An element that solves the Kirchhoff-Love shell theory equations 
/// using Hermite interpolation (displacements 
/// and slopes are interpolated separately. The local and global
/// (Lagrangian) coordinates  are not assumed to be aligned.
/// N.B. It will be DOG SLOW.
//=======================================================================
class HermiteShellElement : public virtual SolidQHermiteElement<2>, 
 public KirchhoffLoveShellEquations
{
public:

 /// Constructor, there are no internal data points
 HermiteShellElement() : SolidQHermiteElement<2>(), 
  KirchhoffLoveShellEquations() 
  {
   //Set the number of dimensions at each node (3D nodes on 2D surface)
   set_nodal_dimension(3);
  }
 
 /// Overload the output function
 void output(std::ostream &outfile) {SolidQHermiteElement<2>::output(outfile);}

 /// Output function
 void output(std::ostream &outfile, const unsigned &n_plot);

 /// Overload the output function
 void output(FILE* file_pt) {SolidQHermiteElement<2>::output(file_pt);}

 /// Output function
 void output(FILE* file_pt, const unsigned &n_plot);

};



//=========================================================================
/// An element that solves the Kirchhoff-Love shell theory equations 
/// using Hermite interpolation (displacements 
/// and slopes are interpolated separately. The local and global
/// (Lagrangian) coordinates  are assumed to be aligned so that the
/// Jacobian of the mapping between these coordinates is diagonal.
/// This significantly simplifies (and speeds up) the computation of the
/// derivatives of the shape functions. 
//=========================================================================
class DiagHermiteShellElement : public HermiteShellElement, 
                                public SolidDiagQHermiteElement<2>
{

public:

 /// Constructor, there are no internal data points
 DiagHermiteShellElement() : HermiteShellElement(), 
  SolidDiagQHermiteElement<2>() {}

};



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////



//=======================================================================
/// Face geometry for the HermiteShell elements: 1D SolidQHermiteElement
//=======================================================================
template<>
class FaceGeometry<HermiteShellElement> : 
  public virtual SolidQHermiteElement<1>
{
};




////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//=========================================================================
/// Diag Hermite Kirchhoff Love shell "upgraded" to a FSIWallElement
/// (and thus, by inheritance, a GeomObject), so it can be used in FSI.
/// hierher: change in FSI Beam ?
//=========================================================================
class FSIDiagHermiteShellElement : public virtual DiagHermiteShellElement,
             public virtual FSIWallElement
{
  private:
 
 /// \short Get the load vector for the computation of the rate of work
 /// done by the load. Can switch between full load and fluid load only.
 /// Overloads the version in the shell element base class.
 /// Pass number of integration point (dummy)
 /// Lagr. coordinate and normal vector and return the load vector
 /// (not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases). 
 virtual void load_vector_for_rate_of_work_computation(
  const unsigned &intpt,
  const Vector<double>& xi,
  const Vector<double>& x,
  const Vector<double>& N,
  Vector<double>& load)
  {
   /// Get fluid-only load vector
   if (Compute_rate_of_work_by_load_with_fluid_load_only)
    {
     fluid_load_vector(intpt,N,load);
    }
   // Get full load vector as per default
   else
    {
     load_vector(intpt,xi,x,N,load);
    }
  }

 ///Boolean flag to indicate whether the normal is directed into the fluid
 bool Normal_points_into_fluid;
 
 /// \short Boolean flag to indicate if rate-of-work by load is to be
 ///based on the fluid traction only
 bool Compute_rate_of_work_by_load_with_fluid_load_only;

  public:
 
 /// \short Constructor: Create shell element as FSIWallElement (and thus,
 /// by inheritance, a GeomObject) with two Lagrangian coordinates
 /// and 3 Eulerian coordinates. By default, we assume that the
 /// normal vector computed by KirchhoffLoveShellEquations::get_normal(...)
 /// points into the fluid. 
 /// If this is not the case, overwrite this with the access function
 /// FSIDiagHermiteShellElement::normal_points_into_fluid().
 FSIDiagHermiteShellElement() : DiagHermiteShellElement(),
  Normal_points_into_fluid(true),
  Compute_rate_of_work_by_load_with_fluid_load_only(false)
  { 
   unsigned n_lagr=2;
   unsigned n_dim=3;
   setup_fsi_wall_element(n_lagr,n_dim);
  }
 
 /// \short Destructor: empty
 ~FSIDiagHermiteShellElement(){}
 
 /// \short Does the normal computed by
 /// KirchhoffLoveShellEquations::get_normal(...) point into the fluid?
 bool &normal_points_into_fluid() {return Normal_points_into_fluid;}

 /// \short Derivative of position vector w.r.t. the SolidFiniteElement's
 /// Lagrangian coordinates; evaluated at current time.
 void dposition_dlagrangian_at_local_coordinate(
  const Vector<double>& s, DenseMatrix<double> &drdxi) const;


 /// \short Get integral of instantaneous rate of work done on 
 /// the wall due to the fluid load returned by the function 
 /// fluid_load_vector(...).
 double fluid_load_rate_of_work()
  {
   Compute_rate_of_work_by_load_with_fluid_load_only=true;
   double tmp=load_rate_of_work();
   Compute_rate_of_work_by_load_with_fluid_load_only=false;
   return tmp;
  }   


 /// \short Get the load vector: Pass number of the integration point,
 /// Lagr. coordinate, Eulerian coordinate and normal vector
 /// and return the load vector. (Not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases). We first evaluate the load function defined via
 /// KirchhoffLoveShellEquations::load_vector_fct_pt() -- this
 /// represents the non-FSI load on the shell, e.g. an external
 /// pressure load. Then we add to this the FSI load due to
 /// the traction exerted by the adjacent FSIFluidElements, taking
 /// the sign of the normal into account.
 void load_vector(const unsigned& intpt,
                  const Vector<double>& xi,
                  const Vector<double>& x,
                  const Vector<double>& N,
                  Vector<double>& load)
  {
   //Initially call the standard Load_vector_fct_pt
   Load_vector_fct_pt(xi,x,N,load);

   //Memory for the FSI load
   Vector<double> fsi_load(3);

   //Get the fluid load on the wall stress scale
   fluid_load_vector(intpt,N,fsi_load);

   //If the normal is outer to the fluid switch the direction
   double sign = 1.0;
   if (!Normal_points_into_fluid) {sign = -1.0;}

   //Add the FSI load to the load vector
   for(unsigned i=0;i<3;i++)
    {
     load[i] += sign*fsi_load[i];
    }
  }

 /// \short Get the Jacobian and residuals. Wrapper to generic FSI version;
 /// that catches the case when we replace the Jacobian by the
 /// mass matrix (for the consistent assignment of initial conditions).
 virtual void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                               DenseMatrix<double> &jacobian)
  {
   //Call the basic shell jacobian
   DiagHermiteShellElement::
     fill_in_contribution_to_jacobian(residuals,jacobian);
   //Fill in the external interaction by finite differences
   this->fill_in_jacobian_from_external_interaction_by_fd(jacobian);
  }

 /// \short Find the local coordinate s in this element
 /// that corresponds to the global "intrinsic" coordinate \f$ \zeta \f$
 /// (here identical to the Lagrangian coordinate \f$ \xi \f$).
 /// If the coordinate is contained within this element, the
 /// geom_object_pt points to "this" element; if the zeta coordinate
 /// is not contained in this element geom_object_pt=NULL.
 void locate_zeta(const Vector<double> &zeta,
                  GeomObject* &geom_object_pt, Vector<double> &s,
                  const bool& use_coordinate_as_initial_guess=false);



 /// \short The number of "blocks" that degrees of freedom in this element
 /// are sub-divided into: Just the solid degrees of freedom themselves.
 unsigned ndof_types()
  {
   return 1;
  }

 /// \short Create a list of pairs for all unknowns in this element,
 /// so that the first entry in each pair contains the global equation
 /// number of the unknown, while the second one contains the number
 /// of the "block" that this unknown is associated with.
 /// (Function can obviously only be called if the equation numbering
 /// scheme has been set up.)
 void get_dof_numbers_for_unknowns(
  std::list<std::pair<unsigned long,unsigned> >& block_lookup_list);

};



////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////


//======================================================================
/// Element that allows the imposition of boundary
/// conditions for a shell that is clamped to a 2D plane
/// that is specified by its normal. Constraint is applied by
/// a Lagrange multiplier. 
/// \n
/// \b Note \b 1: Note that the introduction of the Lagrange
/// multiplier adds two additional values (relative to the number
/// of values before the addition of the FaceElement) to the nodes. 
/// This ensures that nodes that are shared by adjacent FaceElements
/// are not resized repeatedly but also means that this won't work
/// if two "edges" of the shell (that share a node) are subject
/// to different constraints, each applied with its own 
/// independent Lagrange multiplier. In such cases a modified
/// version of this class must be written.
/// \n
///  \b Note \b 2: The FaceGeometry for a HermiteShellElement is
/// the 1D two-node element SolidQHermiteElement<1> which has 
/// four shape functions (two nodes, two types -- representing the
/// shape functions that interpolate the value and the derivative).
/// These are the "correct" shape functions for the interpolation
/// of the Lagrange multiplier and the isoparametric representation 
/// of the geometry. However, when applying the contribution from the
/// constraint equation to the bulk equations, we have to take
/// all four types of dof into account so the element has to 
/// reset the number of positional dofs to four. To avoid any clashes
/// we overload (the relevant subset of) the access functions to the
/// shape functions and their derivatives and set the shape functions
/// associated with the spurious positional dofs to zero. This is a bit
/// hacky but the only way (?) this can be done...
//======================================================================
class ClampedHermiteShellBoundaryConditionElement : 
 public virtual FaceGeometry<HermiteShellElement>, 
 public virtual SolidFaceElement
{
 
public:

 /// \short Constructor, takes the pointer to the "bulk" element, the 
 /// index of the fixed local coordinate and its value represented
 /// by an integer (+/- 1), indicating that the face is located
 /// at the max. or min. value of the "fixed" local coordinate
 /// in the bulk element.
 ClampedHermiteShellBoundaryConditionElement(
  FiniteElement* const &bulk_el_pt, const int &face_index);
 

 ///\short  Broken empty constructor
 ClampedHermiteShellBoundaryConditionElement()
  {
   throw OomphLibError(
    "Don't call empty constructor for ClampedHermiteShellBoundaryConditionElement",
    "ClampedHermiteShellBoundaryConditionElement::ClampedHermiteShellBoundaryConditionElement()",
    OOMPH_EXCEPTION_LOCATION);
  }
 
 /// Broken copy constructor
 ClampedHermiteShellBoundaryConditionElement(
  const ClampedHermiteShellBoundaryConditionElement& dummy) 
  { 
   BrokenCopy::broken_copy(
    "ClampedHermiteShellBoundaryConditionElement");
  } 
 
 /// Broken assignment operator
 void operator=(const ClampedHermiteShellBoundaryConditionElement&) 
  {
   BrokenCopy::broken_assign(
    "ClampedHermiteShellBoundaryConditionElement");
  }

 /// \short Set normal vector to clamping plane
 void set_symmetry_line(const Vector<double>& normal_to_clamping_plane)
  {
   Normal_to_clamping_plane[0]=normal_to_clamping_plane[0];
   Normal_to_clamping_plane[1]=normal_to_clamping_plane[1];
   Normal_to_clamping_plane[2]=normal_to_clamping_plane[2];
  }

 /// Fill in the element's contribution to its residual vector
 void fill_in_contribution_to_residuals(Vector<double> &residuals);
 

 //////////////////////////////////////////////////////////////////
 // Note: We should also overload all other versions of shape(...)
 // and dshape(...) but these are the only ones that are used.
 //////////////////////////////////////////////////////////////////


 /// \short Calculate the geometric shape functions 
 /// at local coordinate s. Set any "superfluous" shape functions to zero.
 void shape(const Vector<double> &s, Shape &psi) const
  {
   // Initialise all of them to zero
   unsigned n=psi.nindex1();
   unsigned m=psi.nindex2();
   for (unsigned i=0;i<n;i++)
    {    
     for (unsigned j=0;j<m;j++)
      { 
       psi(i,j)=0.0;
      }
    }
   FaceGeometry<HermiteShellElement>::shape(s,psi);
  }


 /// \short Calculate the geometric shape functions 
 /// at local coordinate s. Set any "superfluous" shape functions to zero.
 void dshape_local(const Vector<double> &s, Shape &psi, DShape &dpsids) const
  {
   // Initialise all of them to zero
   unsigned n=psi.nindex1();
   unsigned m=psi.nindex2();
   for (unsigned i=0;i<n;i++)
    {    
     for (unsigned j=0;j<m;j++)
      { 
       psi(i,j)=0.0;
      }
    }
   unsigned n1=dpsids.nindex1();
   unsigned n2=dpsids.nindex2();
   unsigned n3=dpsids.nindex3();
   for (unsigned i=0;i<n1;i++)
    {    
     for (unsigned j=0;j<n2;j++)
      { 
       for (unsigned k=0;k<n3;k++)
        { 
         dpsids(i,j,k)=0.0;
        }
      }
    }
   FaceGeometry<HermiteShellElement>::dshape_local(s,psi,dpsids);
  }

 /// Output function -- forward to broken version in FiniteElement
 /// until somebody decides what exactly they want to plot here...
 void output(std::ostream &outfile) {FiniteElement::output(outfile);}

 /// \short Output function
 void output(std::ostream &outfile, const unsigned &n_plot);

 /// C-style output function -- forward to broken version in FiniteElement
 /// until somebody decides what exactly they want to plot here...
 void output(FILE* file_pt) {FiniteElement::output(file_pt);}

 /// \short C-style output function -- forward to broken version in 
 /// FiniteElement until somebody decides what exactly they want to plot 
 /// here...
 void output(FILE* file_pt, const unsigned &n_plot)
  {FiniteElement::output(file_pt,n_plot);}

 /// \short The number of "blocks" that degrees of freedom in this element
 /// are sub-divided into: Just the solid degrees of freedom themselves.
 unsigned ndof_types()
  {
   return 1;
  }

 /// \short Create a list of pairs for all unknowns in this element,
 /// so that the first entry in each pair contains the global equation
 /// number of the unknown, while the second one contains the number
 /// of the "block" that this unknown is associated with.
 /// (Function can obviously only be called if the equation numbering
 /// scheme has been set up.)
 void get_dof_numbers_for_unknowns(
  std::list<std::pair<unsigned long,unsigned> >& block_lookup_list);


private:

 /// \short Normal vector to the clamping plane
 Vector<double> Normal_to_clamping_plane;

}; 



}

#endif





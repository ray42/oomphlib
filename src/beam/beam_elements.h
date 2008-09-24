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
//Header file for KL beam elements
#ifndef OOMPH_KIRCHHOFF_LOVE_BEAM_ELEMENTS_HEADER
#define OOMPH_KIRCHHOFF_LOVE_BEAM_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//OOMPH-LIB header files
#include "../generic/hermite_elements.h"
#include "../generic/geom_objects.h"
#include "../generic/fsi.h"
#include "../generic/block_preconditioner.h"

namespace oomph
{

//=======================================================================
/// A class for elements that solve the equations of Kirchhoff-Love  
/// large-displacement (but linearly-elastic) thin-beam theory.
///
/// The variational principle has the form
/// \f[
/// \int_0^{L}   \left[ 
/// (\sigma_0 + \gamma) \ \delta  
/// \gamma +  
/// \frac{1}{12} \left(\frac{h}{R_0}\right)^2 \kappa 
///  \ \delta \kappa   - 
/// \left( \left(\frac{R_0}{h}\right)  {\bf f} - \Lambda^2 
/// \frac{\partial^2 {\bf R}_w}{\partial t^2} \right) \cdot  
/// \delta {\bf R}_w 
///  \right] \ d\xi  = 0,
/// \f]
/// where all lengths have been non-dimensionalised w.r.t. \f$ R_0 \f$.
/// The strain and and bending "tensors" \f$\gamma\f$ and \f$\kappa\f$
/// are computed relative to the shape of the beam's undeformed shape
/// which is specified as a GeomObject.
///
/// Time is scaled on the timescale \f$T\f$ and
/// \f[
/// \Lambda = \frac{a}{T} \sqrt{\frac{\rho}{E_{eff}}},
/// \f] 
/// the ratio of the timescale used in the non-dimensionalisation of the
/// equations to the natural timescale of the wall oscillations (in the
/// wall's in-plane mode). \f$ \Lambda^2 \f$ can be interpreted as
/// the non-dimensional wall density, therefore \f$ \Lambda=0\f$
/// corresponds to the case without wall inertia. 
///
///
/// Note that:
/// - the load vector \f$ {\bf f} \f$ is scaled
///   on the effective elastic modulus \f$ E_{eff}=E/(1-\nu^2)\f$ 
///   (rather than the 
///   bending stiffness). Rescale the result yourself if you prefer
///   another non-dimensionalisation (the current version yields the
///   the most compact maths).
/// - Poisson's ratio does not appear explicitly since it only occurs
///   in combination with Young's modulus \f$E\f$. 
/// 
/// Default values:
/// - the 2nd Piola Kirchhoff pre-stress \f$ \sigma_0 \f$ is zero.
/// - the wall thickness \f$ h/R_0\f$ is 1/20.
/// - the timescale ratio \f$ \Lambda^2\f$ is 1.
/// - the traction vector \f$ f \f$ evaluates to zero.  
///
/// Need to specify:
/// - the undeformed wall shape (as a GeomObject). 
///
/// The governing equations can be switched from the principle of
/// virtual displacements to a system of equations that forces the 
/// beam to deform into a shape specified by a SolidInitialCondition object.
/// If \c SolidFiniteElement::solid_ic_pt()!=0 we solve the
/// the equations
/// \f[
/// \int_0^{L}   \left(
/// \frac{\partial^i {\bf R}_{IC}}{\partial t^i} -  {\bf R}_w
/// \right) \psi_{jk} \ d\xi  = 0,
/// \f]
/// where  \f$ \partial^i {\bf R}_{IC}/\partial t^i\f$ is
/// implemented by the SolidInitialCondition object, pointed to by 
/// \c SolidFiniteElement::shell_ic_pt().
/// 
//=======================================================================
class KirchhoffLoveBeamEquations : public virtual SolidFiniteElement
{

private:

 /// Static default value for 2nd Piola Kirchhoff prestress
 static double Default_sigma0_value;

 /// Static default value for timescale ratio (1.0 -- for natural scaling) 
 static double Default_lambda_sq_value;

 /// Static default value for non-dim wall thickness
 static double Default_h_value;

 /// Pointer to axial prestress
 double* Sigma0_pt;

 /// Pointer to wall thickness
 double* H_pt;

 /// Pointer to Timescale ratio (non-dim. density)
 double *Lambda_sq_pt;

protected:

 /// Default load function (zero traction)
 static void Zero_traction_fct(const Vector<double>& xi,
                               const Vector<double> &x,
                               const Vector<double>& N,
                               Vector<double>& load);

 /// \short Pointer to load vector function: Its arguments are: 
 /// Lagrangian coordinate, Eulerian coordinate, normal vector and 
 /// load vector itself (not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases)
 void (*Load_vector_fct_pt)(const Vector<double>& xi,
                            const Vector<double> &x,
                            const Vector<double>& N,
                            Vector<double>& load);

 /// \short Pointer to the GeomObject that specifies the beam's
 /// undeformed midplane
 GeomObject* Undeformed_beam_pt;
 
public:

 /// \short Constructor. Set default values for all physical parameters
 /// and zero traction. 
 KirchhoffLoveBeamEquations() :   Undeformed_beam_pt(0)
  {
   //Set physical parameter pointers to the default values
   Sigma0_pt = &Default_sigma0_value;
   Lambda_sq_pt = &Default_lambda_sq_value;
   H_pt = &Default_h_value;
   // Zero traction
   Load_vector_fct_pt=&Zero_traction_fct;
  }


 /// Reference to the load vector function pointer
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
 virtual void load_vector(const unsigned& intpt,
                          const Vector<double>& xi,
                          const Vector<double>& x,
                          const Vector<double>& N,
                          Vector<double>& load)
  {
   Load_vector_fct_pt(xi,x,N,load);
  }

  /// Return the non-dimensional wall thickness
  const double &h() const {return *H_pt;}

  /// Return the timescale ratio (non-dimensional density)
  const double& lambda_sq() const {return *Lambda_sq_pt;}
  
  /// Return the axial prestress
  const double &sigma0() const {return *Sigma0_pt;}
    
  /// Return a pointer to axial prestress
  double* &sigma0_pt() {return Sigma0_pt;}
  
  /// Return a pointer to non-dim. wall thickness
  double* &h_pt() {return H_pt;}
  
  /// Return a pointer to timescale ratio (nondim density)
  double* &lambda_sq_pt() {return Lambda_sq_pt;}

  /// \short Return a Pointer to geometric object that specifies the beam's
  /// undeformed geometry
  GeomObject*& undeformed_beam_pt() {return Undeformed_beam_pt;}
      
  /// Get normal vector on wall
  void get_normal(const Vector<double>& s, Vector<double>& N)
   {
    Vector<double> r(2);
    get_normal(s,r,N);
   }
  
  
  /// Get position vector to and normal vector on wall
  void get_normal(const Vector<double>& s, 
                  Vector<double>& r,
                  Vector<double>& N);
  
  /// \short Get position vector to and non-unit tangent vector on wall:
  /// dr/ds
  void get_non_unit_tangent(const Vector<double>& s,
                            Vector<double>& r,
                            Vector<double>& drds);

  /// \short Return the residuals for the equations of Kirchhoff-Love beam
  /// theory with linear constitutive equations; if  Solid_ic_pt!=0, we
  /// assign residuals which force the assignement of an initial shape/
  /// veloc/accel to the dofs. This overloads the standard interface.
  void fill_in_contribution_to_residuals(Vector<double> &residuals)
   {
    fill_in_contribution_to_residuals_beam(residuals);
   }


  /// \short Return the residuals for the equations of Kirchhoff-Love beam
  /// theory with linear constitutive equations; if  Solid_ic_pt!=0, we
  /// assign residuals which force the assignement of an initial shape/
  /// veloc/accel to the dofs.
  void fill_in_contribution_to_residuals_beam(Vector<double> &residuals);

  
  /// Get FE jacobian and residuals (Jacobian done by finite differences)
  virtual void fill_in_contribution_to_jacobian(Vector<double> &residuals, 
                                            DenseMatrix<double> &jacobian);
  
  /// \short Get potential (strain) and kinetic energy of the element
  void get_energy(double& pot_en, double& kin_en);

  /// \short Get the energy due to stretching and bending and the
  /// kinetic energy of the element
  void get_energy(double &stretch, double &bend, double &kin_en);

}; 


//=========================================================================
/// \short Hermite Kirchhoff Love beam. Implements KirchhoffLoveBeamEquations
/// using 2-node Hermite elements as the underlying geometrical elements.
//=========================================================================
class HermiteBeamElement : public virtual SolidQHermiteElement<1>, 
 public KirchhoffLoveBeamEquations
{
  public:

 /// Constructor (empty)
 HermiteBeamElement() : SolidQHermiteElement<1>(), 
  KirchhoffLoveBeamEquations() 
  {
   //Set the number of dimensions at each node (2D node on 1D surface)
   set_nodal_dimension(2);
  }

 /// Output function
 void output(std::ostream &outfile);

 /// Output function with specified number of plot points
 void output(std::ostream &outfile, const unsigned &n_plot);

 /// \short Output at previous time (t=0: present; t>0: previous) 
 /// with specified number of plot points
 void output(const unsigned& t, std::ostream &outfile, const unsigned &n_plot);

 /// C-style output function
 void output(FILE* file_pt);

 /// C-style output function with specified number of plot points
 void output(FILE* file_pt, const unsigned &n_plot);

 /// \short C-style output at previous time (t=0: present; t>0: previous)
 /// with specified number of plot points
 void output(const unsigned& t, FILE* file_pt, const unsigned &n_plot);

};

//=========================================================================
/// Hermite Kirchhoff Love beam "upgraded" to a FSIWallElement (and thus, 
/// by inheritance, a GeomObject), so it can be used in FSI. 
//=========================================================================
class FSIHermiteBeamElement : public virtual HermiteBeamElement, 
                              public virtual FSIWallElement
{
  private:

 //Boolean flag to indicate whether the normal is directed into the fluid
 bool Normal_points_into_fluid;
 
  public:
 
 /// \short Constructor: Create beam element as FSIWallElement (and thus,
 /// by inheritance, a GeomObject). By default, we assume that the
 /// normal vector computed by KirchhoffLoveBeamEquations::get_normal(...)
 /// points into the fluid. If this is not the case, overwrite this
 /// with the access function FSIHermiteBeamElement::normal_points_into_fluid()
 FSIHermiteBeamElement() : HermiteBeamElement(), 
  Normal_points_into_fluid(true) 
  {
   unsigned n_lagr=1;
   unsigned n_dim=2;
   setup_fsi_wall_element(n_lagr,n_dim);
  } 
 
 /// \short Destructor: empty
 ~FSIHermiteBeamElement(){}

 /// \short Does the normal computed by 
 /// KirchhoffLoveBeamEquations::get_normal(...) point into the fluid?
 bool &normal_points_into_fluid() {return Normal_points_into_fluid;}

 /// \short How many items of Data does the shape of the object depend on?
 /// Same as # of nodes.
 unsigned ngeom_data() const
  {
   // Geom data = variable position data with one per node
   return nnode();
  }
 
 /// \short Return pointer to the j-th Data item that the object's 
 /// shape depends on. (Redirects to the nodes' positional Data).
 Data* geom_data_pt(const unsigned& j)
  {
   return static_cast<SolidNode*>(node_pt(j))->variable_position_pt();
  }
 
 /// \short Position vector at local coordinate s 
 void position(const Vector<double>& s, Vector<double>& r) const
  {
   // Position Vector
   interpolated_x(s,r);
  }

 /// \short Position vector at local coordinate s at discrete
 /// previous time (t=0: present time; t>0: previous time)   
 void position(const unsigned& t, const Vector<double>& s, 
               Vector<double>& r) const
  {
   // Position vector at previous time level t
   interpolated_x(t,s,r);
  }

 
 /// \short Derivative of position vector w.r.t. the SolidFiniteElement's
 /// Lagrangian coordinates; evaluated at current time.
 void dposition_dlagrangian_at_local_coordinate(
  const Vector<double>& s, DenseMatrix<double> &drdxi) const;


 /// \short Derivative of position vector w.r.t. to the GeomObject's
 /// intrinsic coordinates which are identical to the SolidFiniteElement's
 /// local coordinates, so dR_i/dzeta_alpha = dR_i/ds_alpha= drds(alpha,i). 
 /// Evaluated at current time.
 void dposition(const Vector<double>& s, 
                DenseMatrix<double> &drds) const
  {
   throw OomphLibError(
    "Broken -- who calls this \n",
    "FSIHermiteBeamElement::dposition()",
    OOMPH_EXCEPTION_LOCATION);
  }

 /// \short 2nd derivative of position vector w.r.t. to coordinates: 
 /// d^2R_i/dxi_alpha dxi_beta = ddrdxi(alpha,beta,i). 
 /// Evaluated at current time. (broken)
 void d2position(const Vector<double> &xi, 
               RankThreeTensor<double> &ddrdxi) const
  {
   throw OomphLibError(
    "This version of d2position() hasn't been coded up (yet) \n",
    "FSIHermiteBeamElement::d2position()",
    OOMPH_EXCEPTION_LOCATION);
  }

 /// \short Posn vector and its  1st & 2nd derivatives
 /// w.r.t. to coordinates:  dR_i/dxi_alpha = drdxi(alpha,i)
 /// d^2R_i/dxi_alpha dxi_beta = ddrdxi(alpha,beta,i).
 /// Evaluated at current time. (broken)
 void d2position(const Vector<double>& xi, Vector<double>& r,
               DenseMatrix<double> &drdxi,
               RankThreeTensor<double> &ddrdxi) const
  {
   throw OomphLibError(
    "This version of d2position() hasn't been coded up (yet) \n",
    "FSIHermiteBeamElement::d2position()",
    OOMPH_EXCEPTION_LOCATION);
  }

 /// \short Velocity vector at local coordinate s 
 void veloc(const Vector<double>& s, Vector<double>& veloc)
  {
   // First time deriv of position vector
   interpolated_dxdt(s,1,veloc);
  }


 /// \short Acceleration vector at local coordinate s 
 void accel(const Vector<double>& s, Vector<double>& accel)
  {
   // Second time deriv of osition Vector
   interpolated_dxdt(s,2,accel);
  }

 /// \short Get the load vector: Pass number of the integration point,
 /// Lagr. coordinate, Eulerian coordinate and normal vector 
 /// and return the load vector. (Not all of the input arguments will be
 /// required for all specific load functions but the list should
 /// cover all cases). We first evaluate the load function defined via
 /// KirchhoffLoveBeamEquations::load_vector_fct_pt() -- this 
 /// represents the non-FSI load on the beam, e.g. an external 
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
   Vector<double> fsi_load(2);

   //Get the fluid load on the wall stress scale
   fluid_load_vector(intpt,N,fsi_load);

   //If the normal is outer to the fluid switch the direction
   double sign = 1.0;
   if (!Normal_points_into_fluid) {sign = -1.0;}

   //Add the FSI load to the load vector
   for(unsigned i=0;i<2;i++)
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
   //Call the element's residuals vector
   fill_in_contribution_to_residuals_beam(residuals);
   
   //Solve for the consistent acceleration in Newmark scheme?
   if(Solve_for_consistent_newmark_accel_flag)
    {
     fill_in_jacobian_for_newmark_accel(jacobian);
     return;
    }
   
   FSIWallElement::
    fill_in_jacobian_from_solid_position_and_external_by_fd(jacobian);  
  }

 /// \short In the GeomObject incarnation of the FSIWallElement
 /// the Lagrangian coordinate \f$ \xi \f$ used in the solid mechanics
 /// is equal to the "global" intrinsic coordinate \f$ \zeta \f$ in 
 /// the compound GeomObject (the wall mesh) this element is a
 /// member of. This function computes zeta as a function of s,
 /// the local coordinate in the element which acts as the
 /// intrinsic coordinate for the element itself. (Are you still
 /// with me?)
 void interpolated_zeta(const Vector<double> &s, Vector<double> &zeta)
  const {interpolated_xi(s,zeta);}

 /// \short Find the local coordinate s in this element
 /// that corresponds to the global "intrinsic" coordinate \f$ \zeta \f$
 /// (here identical to the Lagrangian coordinate \f$ \xi \f$). 
 /// If the coordinate is contained within this element, the
 /// geom_object_pt points to "this" element; if the zeta coordinate
 /// is not contained in this element geom_object_pt=NULL.
 void locate_zeta(const Vector<double> &zeta,
                  GeomObject* &geom_object_pt, Vector<double> &s);
 


 /// \short The number of "blocks" that degrees of freedom in this element
 /// are sub-divided into: Just the solid degrees of freedom themselves.
 unsigned nblock_types()
  {
   return 1;
  }
 
 /// \short Create a list of pairs for all unknowns in this element,
 /// so that the first entry in each pair contains the global equation
 /// number of the unknown, while the second one contains the number
 /// of the "block" that this unknown is associated with.
 /// (Function can obviously only be called if the equation numbering
 /// scheme has been set up.) 
 void get_block_numbers_for_unknowns(
  std::list<std::pair<unsigned long,unsigned> >& block_lookup_list);

};



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////



//=======================================================================
/// Face geometry for the HermiteBeam elements: Solid point element
//=======================================================================
template<>
class FaceGeometry<HermiteBeamElement> : public virtual SolidPointElement
{
};



///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////



//======================================================================
/// Element that allows the imposition of boundary
/// conditions for a beam that is clamped but can slide
/// along a line which is specified by a position vector
/// to that line and the normal vector to it. The endpoint
/// of the beam is forced to stay on that line and meet
/// it at a right angle. This is achieved with Lagrange multipliers.
//======================================================================
class ClampedSlidingHermiteBeamBoundaryConditionElement 
:  public virtual FaceGeometry<HermiteBeamElement>, 
   public virtual SolidFaceElement
{
 
public:

 /// \short Constructor, takes the pointer to the "bulk" element, the 
 /// index of the fixed local coordinate and its value represented
 /// by an integer (+/- 1), indicating that the face is located
 /// at the max. or min. value of the "fixed" local coordinate
 /// in the bulk element.
 ClampedSlidingHermiteBeamBoundaryConditionElement(
  FiniteElement* const &bulk_el_pt, const int& face_index);

 ///\short  Broken empty constructor
 ClampedSlidingHermiteBeamBoundaryConditionElement()
  {
   throw OomphLibError(
    "Don't call empty constructor for ClampedSlidingHermiteBeamBoundaryConditionElement ",
    "ClampedSlidingHermiteBeamBoundaryConditionElement::ClampedSlidingHermiteBeamBoundaryConditionElement ()",
    OOMPH_EXCEPTION_LOCATION);
  }
 
 
 /// Broken copy constructor
 ClampedSlidingHermiteBeamBoundaryConditionElement(
  const ClampedSlidingHermiteBeamBoundaryConditionElement & dummy) 
  { 
   BrokenCopy::broken_copy(
    "ClampedSlidingHermiteBeamBoundaryConditionElement");
  } 
 
 /// Broken assignment operator
 void operator=(const ClampedSlidingHermiteBeamBoundaryConditionElement&) 
  {
   BrokenCopy::broken_assign(
    "ClampedSlidingHermiteBeamBoundaryConditionElement");
  }


 /// \short Set vectors to some point on the symmetry line, and 
 /// normal to that line along which the end of the beam is sliding.
 void set_symmetry_line(const Vector<double>& vector_to_symmetry_line,
                        const Vector<double>& normal_to_symmetry_line)
  {
   Vector_to_symmetry_line[0]=vector_to_symmetry_line[0];
   Vector_to_symmetry_line[1]=vector_to_symmetry_line[1];
   Normal_to_symmetry_line[0]=normal_to_symmetry_line[0];
   Normal_to_symmetry_line[1]=normal_to_symmetry_line[1];
  }


 /// Fill in the element's contribution to its residual vector
 void fill_in_contribution_to_residuals(Vector<double> &residuals);

 
 /// Output function -- forward to broken version in FiniteElement
 /// until somebody decides what exactly they want to plot here...
 void output(std::ostream &outfile) {FiniteElement::output(outfile);}

 /// \short Output function -- forward to broken version in FiniteElement
 /// until somebody decides what exactly they want to plot here...
 void output(std::ostream &outfile, const unsigned &n_plot)
  {FiniteElement::output(outfile,n_plot);}

 /// C-style output function -- forward to broken version in FiniteElement
 /// until somebody decides what exactly they want to plot here...
 void output(FILE* file_pt) {FiniteElement::output(file_pt);}

 /// \short C-style output function -- forward to broken version in 
 /// FiniteElement until somebody decides what exactly they want to plot 
 /// here...
 void output(FILE* file_pt, const unsigned &n_plot)
  {FiniteElement::output(file_pt,n_plot);}

private:

 /// \short Vector to some point on the symmetry line along which the
 /// end of the beam is sliding
 Vector<double> Vector_to_symmetry_line;

 /// \short Normal vector to the symmetry line along which the 
 /// end of the beam is sliding
 Vector<double> Normal_to_symmetry_line;

}; 



}

#endif






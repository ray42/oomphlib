//LIC// ====================================================================
//LIC// This file forms part of oomph-lib, the object-oriented, 
//LIC// multi-physics finite-element library, available 
//LIC// at http://www.oomph-lib.org.
//LIC// 
//LIC//    Version 1.0; svn revision $LastChangedRevision$
//LIC//
//LIC// $LastChangedDate$
//LIC// 
//LIC// Copyright (C) 2006-2016 Matthias Heil and Andrew Hazel
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
// Header file for elements that are used to apply prescribed flux
// boundary conditions to the generalised Helmholtz equations
#ifndef OOMPH_GENERALISED_HELMHOLTZ_FLUX_ELEMENTS_HEADER
#define OOMPH_GENERALISED_HELMHOLTZ_FLUX_ELEMENTS_HEADER


// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

// oomph-lib ncludes
#include "../generic/Qelements.h"
#include "math.h"
#include <complex>

namespace oomph
{



//======================================================================
/// \short A class for elements that allow the post-processing of 
/// radiated power and flux on the boundaries of GeneralisedHelmholtz elements.
/// The element geometry is obtained from the  FaceGeometry<ELEMENT> 
/// policy class. 
//======================================================================
template <class ELEMENT>
class GeneralisedHelmholtzPowerElement : public virtual FaceGeometry<ELEMENT>, 
public virtual FaceElement
{
  
public:


 /// \short Constructor, takes the pointer to the "bulk" element and the 
 /// index of the face to which the element is attached.
 GeneralisedHelmholtzPowerElement(FiniteElement* const &bulk_el_pt, 
                                  const int& face_index); 
 
 ///\short  Broken empty constructor
 GeneralisedHelmholtzPowerElement()
  {
   throw OomphLibError(
    "Don't call empty constructor for GeneralisedHelmholtzPowerElement",
    OOMPH_CURRENT_FUNCTION,
    OOMPH_EXCEPTION_LOCATION);
  }

 /// Broken copy constructor
 GeneralisedHelmholtzPowerElement(const GeneralisedHelmholtzPowerElement& dummy) 
  { 
   BrokenCopy::broken_copy("GeneralisedHelmholtzPowerElement");
  } 
 
 /// Broken assignment operator
 void operator=(const GeneralisedHelmholtzPowerElement&) 
  {
   BrokenCopy::broken_assign("GeneralisedHelmholtzPowerElement");
  }


 /// \short Specify the value of nodal zeta from the face geometry
 /// The "global" intrinsic coordinate of the element when
 /// viewed as part of a geometric object should be given by
 /// the FaceElement representation, by default (needed to break
 /// indeterminacy if bulk element is SolidElement)
 double zeta_nodal(const unsigned &n, const unsigned &k,           
                   const unsigned &i) const 
 {return FaceElement::zeta_nodal(n,k,i);}     




 
 /// \short Return the index at which the unknown value
 /// is stored. 
 virtual inline std::complex<unsigned> u_index_helmholtz() const 
 {return std::complex<unsigned>(U_index_helmholtz.real(),
                                U_index_helmholtz.imag());}
 

  /// \short Compute the element's contribution to the time-averaged 
  /// radiated power over the artificial boundary
  double global_power_contribution()
  {   
   // Dummy output file
   std::ofstream outfile;
   return global_power_contribution(outfile);
  }
  
  /// \short Compute the element's contribution to the time-averaged 
  /// radiated power over the artificial boundary. Also output the
  /// power density in the specified 
  ///output file if it's open.
  double global_power_contribution(std::ofstream& outfile)
  {   
   // pointer to the corresponding bulk element
   ELEMENT* bulk_elem_pt = dynamic_cast<ELEMENT*>(this->bulk_element_pt()); 
   
   // Number of nodes in bulk element
   unsigned nnode_bulk=bulk_elem_pt->nnode();
   const unsigned n_node_local = nnode();
   
   //get the dim of the bulk and local nodes
   const unsigned bulk_dim= bulk_elem_pt->dim();    
   const unsigned local_dim=this->dim();
   
   //Set up memory for the shape and test functions
   Shape psi(n_node_local);
   
   //Set up memory for the shape functions
   Shape psi_bulk(nnode_bulk);
   DShape dpsi_bulk_dx(nnode_bulk,bulk_dim);
   
   //Set up memory for the outer unit normal
   Vector< double > unit_normal(bulk_dim);    
   
   //Set the value of n_intpt
   const unsigned n_intpt = integral_pt()->nweight();
   
   //Set the Vector to hold local coordinates
   Vector<double> s(local_dim);
   double power=0.0;    
   
   // Output?
   if (outfile.is_open())
    {
     outfile << "ZONE\n";
    }

   //Loop over the integration points
   //--------------------------------
   for(unsigned ipt=0;ipt<n_intpt;ipt++)
    { 
     //Assign values of s
     for(unsigned i=0;i<local_dim;i++)
      {
       s[i] = integral_pt()->knot(ipt,i);
      }
     //get the outer_unit_ext vector      
     this->outer_unit_normal(s,unit_normal); 
     
     //Get the integral weight
     double w = integral_pt()->weight(ipt);
     
     // Get jacobian of mapping
     double J=J_eulerian(s);
     
     //Premultiply the weights and the Jacobian
     double W = w*J;
     
     // Get local coordinates in bulk element by copy construction
     Vector<double> s_bulk(local_coordinate_in_bulk(s));
     
     //Call the derivatives of the shape  functions
     //in the bulk -- must do this via s because this point
     //is not an integration point the bulk element!
     (void)bulk_elem_pt->dshape_eulerian(s_bulk,psi_bulk,dpsi_bulk_dx);
     this->shape(s,psi);
     
     // Derivs of Eulerian coordinates w.r.t. local coordinates
     std::complex<double>  dphi_dn(0.0,0.0);
     Vector<std::complex <double> > 
      interpolated_dphidx(bulk_dim,std::complex<double>(0.0,0.0));
     std::complex<double> interpolated_phi(0.0,0.0);
     Vector<double> x(bulk_dim);

     //Calculate function value and derivatives:
     //-----------------------------------------
     // Loop over nodes
     for(unsigned l=0;l<nnode_bulk;l++) 
      {
       //Get the nodal value of the helmholtz unknown
       const std::complex<double> phi_value(
        bulk_elem_pt->nodal_value(l,bulk_elem_pt->u_index_helmholtz().real()),
        bulk_elem_pt->nodal_value(l,bulk_elem_pt->u_index_helmholtz().imag()));
              
       //Loop over directions
       for(unsigned i=0;i<bulk_dim;i++)
        {
         interpolated_dphidx[i] += phi_value*dpsi_bulk_dx(l,i);
        }
      } // End of loop over the bulk_nodes
     
     for(unsigned l=0;l<n_node_local;l++) 
      {
       //Get the nodal value of the helmholtz unknown
       const std::complex<double> phi_value(
        nodal_value(l,u_index_helmholtz().real()),
        nodal_value(l,u_index_helmholtz().imag()));
       
       interpolated_phi += phi_value*psi(l);
      }
     
     //define dphi_dn 
     for(unsigned i=0;i<bulk_dim;i++)
      {
       dphi_dn += interpolated_dphidx[i]*unit_normal[i];
      }

     // Power density
     double integrand=0.5*
      (interpolated_phi.real()*dphi_dn.imag()-
       interpolated_phi.imag()*dphi_dn.real());
     
     // Output?
     if (outfile.is_open())
      {
       interpolated_x(s,x);
       double phi=atan2(x[1],x[0]);
       outfile << x[0] << " "
               << x[1] << " "
               << phi << " "
               << integrand << "\n";
      }

     // ...add to integral
     power+=integrand*W;
    }  
   
   return  power;
  }




  /// \short Compute the element's contribution to the time-averaged 
  /// flux
  std::complex<double> global_flux_contribution()
  {   
   // Dummy output file
   std::ofstream outfile;
   return global_flux_contribution(outfile);
  }
  
  /// \short  Compute the element's contribution to the integral of 
  /// the flux over the artificial boundary. Also output the
  /// flux  in the specified 
  ///output file if it's open.
  std::complex<double> global_flux_contribution(std::ofstream& outfile)
  {   
   // pointer to the corresponding bulk element
   ELEMENT* bulk_elem_pt = dynamic_cast<ELEMENT*>(this->bulk_element_pt()); 
   
   // Number of nodes in bulk element
   unsigned nnode_bulk=bulk_elem_pt->nnode();
   const unsigned n_node_local = nnode();
   
   //get the dim of the bulk and local nodes
   const unsigned bulk_dim= bulk_elem_pt->dim();    
   const unsigned local_dim=this->dim();
   
   //Set up memory for the shape and test functions
   Shape psi(n_node_local);
   
   //Set up memory for the shape functions
   Shape psi_bulk(nnode_bulk);
   DShape dpsi_bulk_dx(nnode_bulk,bulk_dim);
   
   //Set up memory for the outer unit normal
   Vector<double> unit_normal(bulk_dim);    
   
   //Set the value of n_intpt
   const unsigned n_intpt = integral_pt()->nweight();
   
   //Set the Vector to hold local coordinates
   Vector<double> s(local_dim);
   std::complex<double> flux (0.0,0.0);    
   
   // Output?
   if (outfile.is_open())
    {
     outfile << "ZONE\n";
    }

   //Loop over the integration points
   //--------------------------------
   for(unsigned ipt=0;ipt<n_intpt;ipt++)
    { 
     //Assign values of s
     for(unsigned i=0;i<local_dim;i++)
      {
       s[i] = integral_pt()->knot(ipt,i);
      }
     //get the outer_unit_ext vector      
     this->outer_unit_normal(s,unit_normal); 
     
     //Get the integral weight
     double w = integral_pt()->weight(ipt);
     
     // Get jacobian of mapping
     double J=J_eulerian(s);
     
     //Premultiply the weights and the Jacobian
     double W = w*J;
     
     // Get local coordinates in bulk element by copy construction
     Vector<double> s_bulk(local_coordinate_in_bulk(s));
     
     //Call the derivatives of the shape  functions
     //in the bulk -- must do this via s because this point
     //is not an integration point the bulk element!
     (void)bulk_elem_pt->dshape_eulerian(s_bulk,psi_bulk,dpsi_bulk_dx);
     this->shape(s,psi);
     
     // Derivs of Eulerian coordinates w.r.t. local coordinates
     std::complex<double>  dphi_dn(0.0,0.0);
     Vector<std::complex <double> > 
      interpolated_dphidx(bulk_dim,std::complex<double>(0.0,0.0));
     Vector<double> x(bulk_dim);

     //Calculate function value and derivatives:
     //-----------------------------------------
     // Loop over nodes
     for(unsigned l=0;l<nnode_bulk;l++) 
      {
       //Get the nodal value of the helmholtz unknown
       const std::complex<double> phi_value(
        bulk_elem_pt->nodal_value(l,bulk_elem_pt->u_index_helmholtz().real()),
        bulk_elem_pt->nodal_value(l,bulk_elem_pt->u_index_helmholtz().imag()));
              
       //Loop over directions
       for(unsigned i=0;i<bulk_dim;i++)
        {
         interpolated_dphidx[i] += phi_value*dpsi_bulk_dx(l,i);
        }
      } // End of loop over the bulk_nodes
     
     
     //define dphi_dn 
     for(unsigned i=0;i<bulk_dim;i++)
      {
       dphi_dn += interpolated_dphidx[i]*unit_normal[i];
      }

     // Output?
     if (outfile.is_open())
      {
       interpolated_x(s,x);
       outfile << x[0] << " "
               << x[1] << " "
               << dphi_dn.real() << " " 
               << dphi_dn.imag() << "\n";
      }

     // ...add to integral
     flux+=dphi_dn*W;
    }  
   
   return  flux;
  }


protected:




 /// \short The index at which the real and imag part of the unknown is stored 
 /// at the nodes
 std::complex<unsigned> U_index_helmholtz;
  
  ///The spatial dimension of the problem
 unsigned Dim;


}; 

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



//===========================================================================
/// Constructor, takes the pointer to the "bulk" element, the 
/// index of the fixed local coordinate and its value represented
/// by an integer (+/- 1), indicating that the face is located
/// at the max. or min. value of the "fixed" local coordinate
/// in the bulk element.
//===========================================================================
template<class ELEMENT>
GeneralisedHelmholtzPowerElement<ELEMENT>::
GeneralisedHelmholtzPowerElement(FiniteElement* const &bulk_el_pt, 
                   const int &face_index) : 
  FaceGeometry<ELEMENT>(), FaceElement()
  { 
#ifdef PARANOID
   {
    //Check that the element is not a refineable 3d element
    ELEMENT* elem_pt = dynamic_cast<ELEMENT*>(bulk_el_pt);
    //If it's three-d
    if(elem_pt->dim()==3)
     {
      //Is it refineable
      RefineableElement* ref_el_pt=dynamic_cast<RefineableElement*>(elem_pt);
      if(ref_el_pt!=0)
       {
        if (this->has_hanging_nodes())
         {
          throw OomphLibError(
           "This flux element will not work correctly if nodes are hanging\n",
           OOMPH_CURRENT_FUNCTION,
           OOMPH_EXCEPTION_LOCATION);
         }
       }
     }
   }
#endif   

   // Let the bulk element build the FaceElement, i.e. setup the pointers 
   // to its nodes (by referring to the appropriate nodes in the bulk
   // element), etc.
   bulk_el_pt->build_face_element(face_index,this);
 
   // Extract the dimension of the problem from the dimension of 
   // the first node
   Dim = this->node_pt(0)->ndim();

   //Set up U_index_helmholtz. Initialise to zero, which probably won't change
   //in most cases, oh well, the price we pay for generality
   U_index_helmholtz = std::complex<unsigned>(0,1);

   //Cast to the appropriate GeneralisedHelmholtzEquation so that we can
   //find the index at which the variable is stored
   //We assume that the dimension of the full problem is the same
   //as the dimension of the node, if this is not the case you will have
   //to write custom elements, sorry
   switch(Dim)
    {
     //One dimensional problem
    case 1:
    {
     GeneralisedHelmholtzEquations<1>* eqn_pt = 
      dynamic_cast<GeneralisedHelmholtzEquations<1>*>(bulk_el_pt);
     //If the cast has failed die
     if(eqn_pt==0)
      {
       std::string error_string =
        "Bulk element must inherit from GeneralisedHelmholtzEquations.";
       error_string += 
        "Nodes are one dimensional, but cannot cast the bulk element to\n";
       error_string += "GeneralisedHelmholtzEquations<1>\n.";
       error_string += 
        "If you desire this functionality, you must implement it yourself\n";
       
       throw OomphLibError(error_string,
                           OOMPH_CURRENT_FUNCTION,
                           OOMPH_EXCEPTION_LOCATION);
      }
     //Otherwise read out the value
     else
      {  
       //Read the index from the (cast) bulk element
       U_index_helmholtz = eqn_pt->u_index_helmholtz();
      }
    }
    break;
    
    //Two dimensional problem
    case 2:
    {
     GeneralisedHelmholtzEquations<2>* eqn_pt = 
      dynamic_cast<GeneralisedHelmholtzEquations<2>*>(bulk_el_pt);
     //If the cast has failed die
     if(eqn_pt==0)
      {
       std::string error_string =
        "Bulk element must inherit from GeneralisedHelmholtzEquations.";
       error_string += 
        "Nodes are two dimensional, but cannot cast the bulk element to\n";
       error_string += "GeneralisedHelmholtzEquations<2>\n.";
       error_string += 
        "If you desire this functionality, you must implement it yourself\n";
       
       throw OomphLibError(error_string,
                           OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
      }
     else
      {
       //Read the index from the (cast) bulk element
       U_index_helmholtz = eqn_pt->u_index_helmholtz();
      }   
    }
    
    break;
    
    //Three dimensional problem
    case 3: 
    {
     GeneralisedHelmholtzEquations<3>* eqn_pt = 
      dynamic_cast<GeneralisedHelmholtzEquations<3>*>(bulk_el_pt);
     //If the cast has failed die
     if(eqn_pt==0)
      {
       std::string error_string =
        "Bulk element must inherit from GeneralisedHelmholtzEquations.";
       error_string += 
        "Nodes are three dimensional, but cannot cast the bulk element to\n";
       error_string += "GeneralisedHelmholtzEquations<3>\n.";
       error_string += 
        "If you desire this functionality, you must implement it yourself\n";
       
       throw OomphLibError(error_string,
                           OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION); 
      }
     else 
      {
       //Read the index from the (cast) bulk element
       U_index_helmholtz = eqn_pt->u_index_helmholtz();
      }
    }
    break;

    //Any other case is an error
    default:
     std::ostringstream error_stream; 
     error_stream <<  "Dimension of node is " << Dim 
                  << ". It should be 1,2, or 3!" << std::endl;
     
     throw OomphLibError(error_stream.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
     break;
    }
  }



//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////




//======================================================================
/// \short A class for elements that allow the imposition of an 
/// applied flux on the boundaries of GeneralisedHelmholtz elements.
/// The element geometry is obtained from the  FaceGeometry<ELEMENT> 
/// policy class.
//======================================================================
template <class ELEMENT>
class GeneralisedHelmholtzFluxElement : public virtual FaceGeometry<ELEMENT>, 
public virtual FaceElement
{
  
public:

 /// \short Function pointer to the prescribed-flux function fct(x,f(x)) -- 
 /// x is a Vector and  the flux is a complex. NOTE THAT f(x) represents
 /// c^2 du/dn!
 typedef void (*GeneralisedHelmholtzPrescribedFluxFctPt)
  (const Vector<double>& x, std::complex<double>& flux);

 /// \short Constructor, takes the pointer to the "bulk" element and the 
 /// index of the face to which the element is attached.
 GeneralisedHelmholtzFluxElement(FiniteElement* const &bulk_el_pt, 
                    const int& face_index); 

 ///\short  Broken empty constructor
 GeneralisedHelmholtzFluxElement()
  {
   throw OomphLibError(
    "Don't call empty constructor for GeneralisedHelmholtzFluxElement",
    OOMPH_CURRENT_FUNCTION,
    OOMPH_EXCEPTION_LOCATION);
  }

 /// Broken copy constructor
 GeneralisedHelmholtzFluxElement(const GeneralisedHelmholtzFluxElement& dummy) 
  { 
   BrokenCopy::broken_copy("GeneralisedHelmholtzFluxElement");
  } 
 
 /// Broken assignment operator
 void operator=(const GeneralisedHelmholtzFluxElement&) 
  {
   BrokenCopy::broken_assign("GeneralisedHelmholtzFluxElement");
  }


 /// Access function for the prescribed-flux function pointer
 GeneralisedHelmholtzPrescribedFluxFctPt& flux_fct_pt() {return Flux_fct_pt;}


 /// Add the element's contribution to its residual vector
 inline void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
   //Call the generic residuals function with flag set to 0
   //using a dummy matrix argument
   fill_in_generic_residual_contribution_helmholtz_flux(
    residuals,GeneralisedElement::Dummy_matrix,0);
  }


 /// \short Add the element's contribution to its residual vector and its 
 /// Jacobian matrix
 inline void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                          DenseMatrix<double> &jacobian)
  {
   //Call the generic routine with the flag set to 1
   fill_in_generic_residual_contribution_helmholtz_flux(residuals,jacobian,1);
  }


 /// \short Specify the value of nodal zeta from the face geometry
 /// The "global" intrinsic coordinate of the element when
 /// viewed as part of a geometric object should be given by
 /// the FaceElement representation, by default (needed to break
 /// indeterminacy if bulk element is SolidElement)
 double zeta_nodal(const unsigned &n, const unsigned &k,           
                   const unsigned &i) const 
 {return FaceElement::zeta_nodal(n,k,i);}     


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

 
 /// \short Return the index at which the unknown value
 /// is stored. 
 virtual inline std::complex<unsigned> u_index_helmholtz() const 
 {return std::complex<unsigned>(U_index_helmholtz.real(),U_index_helmholtz.imag());}
 

protected:

 /// \short Function to compute the shape and test functions and to return 
 /// the Jacobian of mapping between local and global (Eulerian)
 /// coordinates
 inline double shape_and_test(const Vector<double> &s, Shape &psi, Shape &test)
  const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   //Get the shape functions
   shape(s,psi);

   //Set the test functions to be the same as the shape functions
   for(unsigned i=0;i<n_node;i++) {test[i] = psi[i];}

   //Return the value of the jacobian
   return J_eulerian(s);
  }


 /// \short Function to compute the shape and test functions and to return 
 /// the Jacobian of mapping between local and global (Eulerian)
 /// coordinates
 inline double shape_and_test_at_knot(const unsigned &ipt,
                                      Shape &psi, Shape &test)
  const
  {
   //Find number of nodes
   unsigned n_node = nnode();

   //Get the shape functions
   shape_at_knot(ipt,psi);

   //Set the test functions to be the same as the shape functions
   for(unsigned i=0;i<n_node;i++) {test[i] = psi[i];}

   //Return the value of the jacobian
   return J_eulerian_at_knot(ipt);
  }


 /// Function to calculate the prescribed flux at a given spatial
 /// position
 void get_flux(const Vector<double>& x, std::complex<double>& flux)
  {
   //If the function pointer is zero return zero
   if(Flux_fct_pt == 0)
    {
     flux = std::complex<double>(0.0,0.0);
    }
   //Otherwise call the function
   else
    {
     (*Flux_fct_pt)(x,flux);
    }
  }


 /// \short The index at which the real and imag part of the unknown is stored 
 /// at the nodes
 std::complex<unsigned> U_index_helmholtz;
 
 
 /// \short Add the element's contribution to its residual vector.
 /// flag=1(or 0): do (or don't) compute the contribution to the
 /// Jacobian as well. 
 virtual void fill_in_generic_residual_contribution_helmholtz_flux(
  Vector<double> &residuals, DenseMatrix<double> &jacobian, 
  const unsigned& flag);
 
 
 /// Function pointer to the (global) prescribed-flux function
 GeneralisedHelmholtzPrescribedFluxFctPt Flux_fct_pt;

 ///The spatial dimension of the problem
 unsigned Dim;


}; 

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////



//===========================================================================
/// Constructor, takes the pointer to the "bulk" element, the 
/// index of the fixed local coordinate and its value represented
/// by an integer (+/- 1), indicating that the face is located
/// at the max. or min. value of the "fixed" local coordinate
/// in the bulk element.
//===========================================================================
template<class ELEMENT>
GeneralisedHelmholtzFluxElement<ELEMENT>::
GeneralisedHelmholtzFluxElement(FiniteElement* const &bulk_el_pt, 
                   const int &face_index) : 
  FaceGeometry<ELEMENT>(), FaceElement()
  { 
#ifdef PARANOID
   {
    //Check that the element is not a refineable 3d element
    ELEMENT* elem_pt = dynamic_cast<ELEMENT*>(bulk_el_pt);
    //If it's three-d
    if(elem_pt->dim()==3)
     {
      //Is it refineable
      RefineableElement* ref_el_pt=dynamic_cast<RefineableElement*>(elem_pt);
      if(ref_el_pt!=0)
       {
        if (this->has_hanging_nodes())
         {
          throw OomphLibError(
           "This flux element will not work correctly if nodes are hanging\n",
           OOMPH_CURRENT_FUNCTION,
           OOMPH_EXCEPTION_LOCATION);
         }
       }
     }
   }
#endif   

   // Let the bulk element build the FaceElement, i.e. setup the pointers 
   // to its nodes (by referring to the appropriate nodes in the bulk
   // element), etc.
   bulk_el_pt->build_face_element(face_index,this);
 
   // Initialise the prescribed-flux function pointer to zero
   Flux_fct_pt = 0;

   // Extract the dimension of the problem from the dimension of 
   // the first node
   Dim = this->node_pt(0)->ndim();

   //Set up U_index_helmholtz. Initialise to zero, which probably won't change
   //in most cases, oh well, the price we pay for generality
   U_index_helmholtz = std::complex<unsigned>(0,1);

   //Cast to the appropriate GeneralisedHelmholtzEquation so that we can
   //find the index at which the variable is stored
   //We assume that the dimension of the full problem is the same
   //as the dimension of the node, if this is not the case you will have
   //to write custom elements, sorry
   switch(Dim)
    {
     //One dimensional problem
    case 1:
    {
     GeneralisedHelmholtzEquations<1>* eqn_pt = 
      dynamic_cast<GeneralisedHelmholtzEquations<1>*>(bulk_el_pt);
     //If the cast has failed die
     if(eqn_pt==0)
      {
       std::string error_string =
        "Bulk element must inherit from GeneralisedHelmholtzEquations.";
       error_string += 
        "Nodes are one dimensional, but cannot cast the bulk element to\n";
       error_string += "GeneralisedHelmholtzEquations<1>\n.";
       error_string += 
        "If you desire this functionality, you must implement it yourself\n";
       
       throw OomphLibError(error_string,
                           OOMPH_CURRENT_FUNCTION,
                           OOMPH_EXCEPTION_LOCATION);
      }
     //Otherwise read out the value
     else
      {  
       //Read the index from the (cast) bulk element
       U_index_helmholtz = eqn_pt->u_index_helmholtz();
      }
    }
    break;
    
    //Two dimensional problem
    case 2:
    {
     GeneralisedHelmholtzEquations<2>* eqn_pt = 
      dynamic_cast<GeneralisedHelmholtzEquations<2>*>(bulk_el_pt);
     //If the cast has failed die
     if(eqn_pt==0)
      {
       std::string error_string =
        "Bulk element must inherit from GeneralisedHelmholtzEquations.";
       error_string += 
        "Nodes are two dimensional, but cannot cast the bulk element to\n";
       error_string += "GeneralisedHelmholtzEquations<2>\n.";
       error_string += 
        "If you desire this functionality, you must implement it yourself\n";
       
       throw OomphLibError(error_string,
                           OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION);
      }
     else
      {
       //Read the index from the (cast) bulk element
       U_index_helmholtz = eqn_pt->u_index_helmholtz();
      }   
    }
    
    break;
    
    //Three dimensional problem
    case 3: 
    {
     GeneralisedHelmholtzEquations<3>* eqn_pt = 
      dynamic_cast<GeneralisedHelmholtzEquations<3>*>(bulk_el_pt);
     //If the cast has failed die
     if(eqn_pt==0)
      {
       std::string error_string =
        "Bulk element must inherit from GeneralisedHelmholtzEquations.";
       error_string += 
        "Nodes are three dimensional, but cannot cast the bulk element to\n";
       error_string += "GeneralisedHelmholtzEquations<3>\n.";
       error_string += 
        "If you desire this functionality, you must implement it yourself\n";
       
       throw OomphLibError(error_string,
                           OOMPH_CURRENT_FUNCTION,
                        OOMPH_EXCEPTION_LOCATION); 
      }
     else 
      {
       //Read the index from the (cast) bulk element
       U_index_helmholtz = eqn_pt->u_index_helmholtz();
      }
    }
    break;

    //Any other case is an error
    default:
     std::ostringstream error_stream; 
     error_stream <<  "Dimension of node is " << Dim 
                  << ". It should be 1,2, or 3!" << std::endl;
     
     throw OomphLibError(error_stream.str(),
                         OOMPH_CURRENT_FUNCTION,
                         OOMPH_EXCEPTION_LOCATION);
     break;
    }
  }


//===========================================================================
/// Compute the element's residual vector and the (zero) Jacobian matrix.
//===========================================================================
template<class ELEMENT>
void GeneralisedHelmholtzFluxElement<ELEMENT>::
fill_in_generic_residual_contribution_helmholtz_flux(
 Vector<double> &residuals, DenseMatrix<double> &jacobian, 
 const unsigned& flag)
{
 //Find out how many nodes there are
 const unsigned n_node = nnode();
  
 //Set up memory for the shape and test functions
 Shape psif(n_node), testf(n_node);
 
 //Set the value of Nintpt
 const unsigned n_intpt = integral_pt()->nweight();
 
 //Set the Vector to hold local coordinates
 Vector<double> s(Dim-1);
 
 //Integers to hold the local equation and unknown numbers
 int local_eqn_real=0 ,local_eqn_imag=0;

 //Loop over the integration points
 //--------------------------------
 for(unsigned ipt=0;ipt<n_intpt;ipt++)
  {

   //Assign values of s
   for(unsigned i=0;i<(Dim-1);i++) {s[i] = integral_pt()->knot(ipt,i);}
   
   //Get the integral weight
   double w = integral_pt()->weight(ipt);
   
   //Find the shape and test functions and return the Jacobian
   //of the mapping
   double J = shape_and_test(s,psif,testf);
   
   //Premultiply the weights and the Jacobian
   double W = w*J;
   
   //Need to find position to feed into flux function, initialise to zero
   Vector<double> interpolated_x(Dim,0.0);
   
   //Calculate Eulerian position of integration point
   for(unsigned l=0;l<n_node;l++) 
    {
     for(unsigned i=0;i<Dim;i++)
      {
       interpolated_x[i] += nodal_position(l,i)*psif[l];
      }
    }
   
   //Get the imposed flux
   std::complex<double> flux(0.0,0.0);
   get_flux(interpolated_x,flux);
   
   //Now add to the appropriate equations
   //Loop over the test functions
   for(unsigned l=0;l<n_node;l++)
    {  
     local_eqn_real = nodal_local_eqn(l,U_index_helmholtz.real());
     /*IF it's not a boundary condition*/
     if(local_eqn_real >= 0)
      {
       //Add the prescribed flux terms
       residuals[local_eqn_real] -= flux.real()*testf[l]*W;
       
       // Imposed traction doesn't depend upon the solution, 
       // --> the Jacobian is always zero, so no Jacobian
       // terms are required
      }
     local_eqn_imag = nodal_local_eqn(l,U_index_helmholtz.imag());
     /*IF it's not a boundary condition*/
     if(local_eqn_imag >= 0)
      {
       //Add the prescribed flux terms
       residuals[local_eqn_imag] -= flux.imag()*testf[l]*W;
       
       // Imposed traction doesn't depend upon the solution, 
       // --> the Jacobian is always zero, so no Jacobian
       // terms are required
      }   
    }
  }
}


}

#endif


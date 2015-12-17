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
// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

//oomph-lib includes
#include "biharmonic_flux_elements.h"


namespace oomph
{
//=============================================================================
/// Constructor, takes the pointer to the "bulk" element, the 
/// index of the fixed local coordinate and its value represented
/// by an integer (+/- 1), indicating that the face is located
/// at the max. or min. value of the "fixed" local coordinate
/// in the bulk element. And the macro element position of the flux element
/// along the edge of the problem
//=============================================================================
template<>
BiharmonicFluxElement<2>::
BiharmonicFluxElement(FiniteElement* const &bulk_el_pt, 
                      const int &face_index, const unsigned& b) : 
  FaceGeometry<BiharmonicElement<2> >(), FaceElement()
  { 

   // Let the bulk element build the FaceElement, i.e. setup the pointers 
   // to its nodes (by referring to the appropriate nodes in the bulk
   // element), etc.
   bulk_el_pt->build_face_element(face_index,this);

   // Initialise the prescribed-flux function pointer to zero
   Flux0_fct_pt = 0;
   Flux1_fct_pt = 0;
	  
   // set the number of nodal degrees of freedom for the face element
   Nface_nodal_dof = 2;

   //
   Boundary = b;
 
  }


//=============================================================================
/// \short Calculate the Jacobian of the mapping between local and global
/// coordinates at the position s for face elements for two dimensional 
/// problems
/// The jacobian of the 1D face element is computed which is dt/ds_t 
//=============================================================================
template<>
double BiharmonicFluxElement<2>::J_eulerian(const Vector<double> &s) const
{

 //Find the number of nodes
 unsigned n_node = this->nnode();

 //Set up dummy memory for the shape functions
 Shape psi(n_node,Nface_nodal_dof);
 DShape dpsids(n_node,Nface_nodal_dof,1);

 //Get the shape functions and local derivatives
 this->dshape_local(s,psi,dpsids);
 
 // computed dx_i/ds_t along edge element
 Vector<double> interpolated_dxds_t(2);

 //Get the bulk position type corresponding to the slope
 const unsigned slope_index = this->bulk_position_type(1);
 for(unsigned l=0;l<n_node;l++)
  {
   for (unsigned d = 0; d < 2; d++)
    {
     interpolated_dxds_t[d] += this->node_pt(l)->x_gen(0,d) * dpsids(l,0,0)
      + this->node_pt(l)->x_gen(slope_index,d)*dpsids(l,1,0);
    }
  }

 // dt/ds_t = dx_0/ds_t*t_0 + dx_1/ds_t*t_1
 // 
 // given : t_0 = dx_0/ds_t / ( (dx_0/ds_t)^2 + (dx_1/ds_t)^2 )^0.5
 //         t_1 = dx_1/ds_t / ( (dx_0/ds_t)^2 + (dx_1/ds_t)^2 )^0.5
 //
 // then : dt/ds_t = ( (dx_0/ds_t)^2 + (dx_1/ds_t)^2 )^0.5
 //
 // (note : it is gaurantee that dt/ds_t is +ve, therefore do not need PARANOID
 //         check of jacobian)
 double dtds_t = sqrt(interpolated_dxds_t[0]*interpolated_dxds_t[0] +
  interpolated_dxds_t[1]*interpolated_dxds_t[1]);
 
 //Return the Jacobian
 return dtds_t;
}



//=============================================================================
/// \short Compute the elemental contribution to the residual vector for 2D
/// problem type 0 biharmonic flux elements
//=============================================================================
template<>
void BiharmonicFluxElement<2>::
fill_in_generic_residual_contribution_biharmonic_flux(Vector<double> &residual)
{
 //Find out how many nodes there are in the face element
 unsigned n_node = this->nnode();

 //set up memory for shape functions
 Shape psi(n_node,Nface_nodal_dof);

 //setup memory for derivative of shape functions
 DShape dpsi(n_node,Nface_nodal_dof,1);

  //Set the value of Nintpt
 unsigned n_intpt = this->integral_pt()->nweight();
 
 // local coordinate position of integration point in face element
 Vector<double> s(1);
   
 // edge sign adjust
 int edge_sign = //-int(this->s_fixed_value())*int((s_fixed_index-0.5)*2);
                 - this->normal_sign(); 

 //Flip for the different normal conventions (TIDY THIS UP)
 if((this->face_index() == 2) || (this->face_index() == -2))
  {edge_sign *= -1;}

 // Get the bulk position type corresponding to the slope
 const unsigned slope_index = this->bulk_position_type(1); 
 
 // Ge the position type corresponding to the outer slope
 const unsigned outer_slope_index = 3 - slope_index;

//Loop over the integration points
 //--------------------------------
 for(unsigned ipt=0;ipt<n_intpt;ipt++)
  {
   
   //Get the integral weight
   double w = this->integral_pt()->weight(ipt);
   
   // value of local coordinate s at integration point
   s[0] = this->integral_pt()->knot(ipt,0);

   // get the (1D) shape fn
   this->dshape_local(s,psi,dpsi);

   // compute dx_i/ds_t and dx_i/ds_n at ipt
   Vector<double> dxds_t(2,0.0);
   Vector<double> dxds_n(2,0.0);
   for (unsigned n = 0; n < n_node; n++)
    {
     for (unsigned d = 0; d < 2; d++)
      {
       for (unsigned k = 0; k < Nface_nodal_dof; k++)
        {
         dxds_t[d] += dpsi(n,k,0)*node_pt(n)->x_gen(slope_index*k,d);
         dxds_n[d] += psi(n,k)*node_pt(n)->x_gen(outer_slope_index+
                                                 slope_index*k,d);
        }
      }
    }
  
   // compute ds_n/dn
   double ds_ndn = -edge_sign*sqrt(dxds_t[0]*dxds_t[0]+dxds_t[1]*dxds_t[1])/
    (-dxds_n[0]*dxds_t[1]+dxds_t[0]*dxds_n[1]);
  
   // compute ds_t/dn
   double ds_tdn = edge_sign*(dxds_t[1]*dxds_n[1]+dxds_t[0]*dxds_n[0])/
    (sqrt(dxds_t[0]*dxds_t[0]+dxds_t[1]*dxds_t[1])*
     (-dxds_n[0]*dxds_t[1]+dxds_t[0]*dxds_n[1]));

   // compute interpolated_m at s
   double interpolated_m = 0.0;
   Vector<double> m(2);
   for (unsigned n = 0; n < n_node; n++)
    {
     this->node_pt(n)->get_coordinates_on_boundary(Boundary,m);
     for (unsigned k = 0; k < Nface_nodal_dof; k++)
      {
       interpolated_m += psi(n,k)*m[k];
      }
    }

   // get the fluxes
   double flux0 = 0.0; 
   get_flux0(interpolated_m,flux0);
   double flux1 = 0.0;
   get_flux1(interpolated_m,flux1);
  
   // get J
   double J = J_eulerian(s);

   //Premultiply the weights and the Jacobian
   double W = w*J;

   //Now add to the appropriate equations
 

   //Loop over the test function nodes
   for(unsigned n=0;n<n_node;n++)
    {
     // loop over the face element position types
     for (unsigned k = 0;k < Nface_nodal_dof;k++)
      {

       // apply contribution for flux0
       
       // determine bulk position type
       unsigned bulk_p_type = slope_index*k;

       // local equation number
       int local_eqn = this->nodal_local_eqn(n,bulk_p_type);

       // if node not pinned
       if (local_eqn >= 0)
        {        
         // add contribution to residual
         residual[local_eqn] += flux1*psi(n,k)*W;
        }

       // apply first contribution for flux1
       
       // if node not pinned
       if (local_eqn >= 0)
        {        
         // add contribution to residual
         residual[local_eqn] -= flux0*dpsi(n,k,0)*ds_tdn*W;
        }

       // apply second contribution for flux1

       // determine bulk position type
       bulk_p_type = outer_slope_index + slope_index*k;

       // local equation number
       local_eqn = this->nodal_local_eqn(n,bulk_p_type);

       // if node not pinned
       if (local_eqn >= 0)
        {        
         // add contribution to residual
         residual[local_eqn] -= flux0*psi(n,k)*ds_ndn*W;
        }
      }
    }
  }
}

// Ensure Flux elements are build
template class BiharmonicFluxElement<2>;

}









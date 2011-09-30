//Header file for elements that are used to integrate the shear stress
//along either side wall

#ifndef OOMPH_POLAR_STRESS_INTEGRAL_ELEMENTS_HEADER
#define OOMPH_POLAR_STRESS_INTEGRAL_ELEMENTS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


//OOMPH-LIB headers
#include "../generic/Qelements.h"

namespace oomph
{

//======================================================================
///A class for elements that allow the imposition of an applied traction
///to the Navier--Stokes equations 
///The geometrical information can be read from the FaceGeometery<ELEMENT> 
///class and and thus, we can be generic enough without the need to have
///a separate equations class
//======================================================================
template <class ELEMENT>
class PolarStressIntegralElement : public virtual FaceGeometry<ELEMENT>, 
public virtual FaceElement
{
 
private:

 ///Pointer to an imposed traction function
 void (*Traction_fct_pt)(const double& time, const Vector<double> &x, 
                       Vector<double> &result);

 ///The highest dimension of the problem 
 unsigned Dim;

protected:


 /// \short Access function that returns the local equation numbers
 /// for velocity components.
 /// u_local_eqn(n,i) = local equation number or < 0 if pinned.
 /// The default is to asssume that n is the local node number
 /// and the i-th velocity component is the i-th unknown stored at the node.
 virtual inline int u_local_eqn(const unsigned &n, const unsigned &i)
  {return nodal_local_eqn(n,i);}

 ///\short Function to compute the shape and test functions and to return 
 ///the Jacobian of mapping 
 inline double shape_and_test_at_knot(const unsigned &ipt, 
                                      Shape &psi, Shape &test)
  const
  {
   //Find number of nodes
   unsigned n_node = nnode();
   //Calculate the shape functions
   shape_at_knot(ipt,psi);
   //Set the test functions to be the same as the shape functions
   for(unsigned i=0;i<n_node;i++) {test[i] = psi[i];}
   //Return the value of the jacobian
   return J_eulerian_at_knot(ipt);
  }

 /// Pointer to the angle alpha
 double *Alpha_pt;

 //Traction elements need to know whether they're at the inlet or outlet
 //as the unit outward normal has a differing sign dependent on
 //the boundary
 // phi=-1, phi=1
 int Boundary;

public:

 /// Alpha
 const double &alpha() const {return *Alpha_pt;}

 /// Pointer to Alpha
 double* &alpha_pt() {return Alpha_pt;}

 /// Boundary
 const int boundary() const {return Boundary;}

 /// Function to set boundary
 void set_boundary(int bound) {Boundary=bound;}

 ///Function to calculate the shear stress along boundary
 double get_shear_stress();

 ///Constructor, which takes a "bulk" element and the value of the index
 ///and its limit
 PolarStressIntegralElement(FiniteElement* const &element_pt, 
                            const int &face_index) : 
 FaceGeometry<ELEMENT>(), FaceElement()
  { 
#ifdef PARANOID
 {
  //Check that the element is not a refineable 3d element
  ELEMENT* elem_pt = new ELEMENT;
  //If it's three-d
  if(elem_pt->dim()==3)
   {
    //Is it refineable
    if(dynamic_cast<RefineableElement*>(elem_pt))
     {
      //Issue a warning
      OomphLibWarning(
       "This flux element will not work correctly if nodes are hanging\n",
       "PolarStressIntegralElement::Constructor",
       OOMPH_EXCEPTION_LOCATION);
     }
   }
 }
#endif

   //Attach the geometrical information to the element. N.B. This function
   //also assigns nbulk_value from the required_nvalue of the bulk element
   element_pt->build_face_element(face_index,this);
  
   //Set the dimension from the dimension of the first node
   Dim = this->node_pt(0)->ndim();

 }

 /// Destructor should not delete anything
 ~PolarStressIntegralElement() { }
 
 ///This function returns just the residuals
 inline void fill_in_contribution_to_residuals(Vector<double> &residuals)
  {
    //Do nothing
  }
 
 ///This function returns the residuals and the jacobian
 inline void fill_in_contribution_to_jacobian(Vector<double> &residuals,
                                          DenseMatrix<double> &jacobian)
  {
    //Do nothing
  }

 ///\short Compute the element's residual Vector and the jacobian matrix
 /// Plus the mass matrix especially for eigenvalue problems
 void fill_in_contribution_to_jacobian_and_mass_matrix(
  Vector<double> &residuals,
  DenseMatrix<double> &jacobian,DenseMatrix<double> &mass_matrix)
  {   
    //Do nothing
  }
 
 ///Overload the output function
 void output(std::ostream &outfile) {FiniteElement::output(outfile);}

///Output function: x,y,[z],u,v,[w],p in tecplot format
void output(std::ostream &outfile, const unsigned &nplot)
 {FiniteElement::output(outfile,nplot);}

 /// local velocities
 double u(const unsigned &l, const unsigned &i )
  {return nodal_value(l,i);}

 /// local position
 double x(const unsigned &l, const unsigned &i )
  {return nodal_position(l,i);}

}; 

//============================================================================
/// Function that returns the shear stress
//============================================================================
template<class ELEMENT>
double PolarStressIntegralElement<ELEMENT>::
get_shear_stress()
{
 //Storage for shear stress
 double dudphi,shear_contribution=0.0;

 //Set the value of n_intpt
 unsigned n_intpt = integral_pt()->nweight();

 //Storage for local coordinate
 Vector<double> s_local(1);
 //Storage for local coordinate in bulk
 Vector<double> s_bulk(2);

 //Find out how many nodes there are
 unsigned n_node = nnode();

 //Set up memory for the shape and test functions
 Shape psif(n_node), testf(n_node);
 
 //Loop over the integration points
 for(unsigned ipt=0;ipt<n_intpt;ipt++)
  {
   //Get the integral weight
   double w = integral_pt()->weight(ipt);
   
   //Find the shape and test functions and return the Jacobian
   //of the mapping
   double J = shape_and_test_at_knot(ipt,psif,testf);
   
   //Premultiply the weights and the Jacobian
   double W = w*J;

   //Need to find position to feed into Traction function
   Vector<double> interpolated_x(Dim);
   
   //Initialise to zero
   for(unsigned i=0;i<Dim;i++) 
    {
     interpolated_x[i] = 0.0;
    }
   
   //Calculate velocities and derivatives
   for(unsigned l=0;l<n_node;l++) 
    {
     //Loop over velocity components
     for(unsigned i=0;i<Dim;i++) 
      {
	interpolated_x[i] += nodal_position(l,i)*psif[l];
      }
    }

   //Get the local coordinate
   s_local[0] = integral_pt()->knot(ipt,0);

   //Get bulk coordinate
   s_bulk = this->local_coordinate_in_bulk(s_local);

   // Upcast from GeneralisedElement to the present element
   ELEMENT* el_pt = dynamic_cast<ELEMENT*>(this->Bulk_element_pt);

   //Get du_dphi from bulk element
   dudphi = el_pt->interpolated_dudx_pnst(s_bulk,0,1);  

   //The contribution to the unweighted shear stress
   shear_contribution += dudphi*W;
                       //dudphi*interpolated_x[0]*W;

  } //End of loop over integration points

 return shear_contribution;

} //End of get_shear_stress

} //End of namespace oomph

#endif
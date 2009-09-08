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
//Header file for a class that is used to represent a mesh
//as a geometric object

//Include guards to prevent multiple inclusion of the header
#ifndef OOMPH_MESH_AS_GEOMETRIC_OBJECT_HEADER
#define OOMPH_MESH_AS_GEOMETRIC_OBJECT_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
  #include <oomph-lib-config.h>
#endif

#include<limits.h>
#include<float.h>

//Include the geometric object header file
#include "geom_objects.h"

namespace oomph
{

//========================================================================
/// This class provides a GeomObject representation of a given
/// finite element mesh. The template parameters specify the
/// Lagrangian and Eulerian coordinates of the mesh. 
/// The consitituent elements of the mesh must have their own
/// GeomObject representations and they become sub-objects
/// in this compound GeomObject.
//========================================================================
template<unsigned DIM_LAGRANGIAN, unsigned DIM_EULERIAN, class ELEMENT>
class MeshAsGeomObject : public GeomObject
{

private:

 /// \short  Helper function for constructor: Oass the pointer to the mesh, 
 /// communicator and boolean
 ///to specify whether to calculate coordinate extrema or not
 void construct_it(Mesh* const &mesh_pt, OomphCommunicator* comm_pt,
                   const bool& compute_extreme_bin_coords);

 /// \short Vector of pointers to Data items that affects the object's shape
 Vector<Data*> Geom_data_pt;

 ///Internal storage for the elements that constitute the object
 Vector<ELEMENT*> Sub_geom_object_pt;

 ///Storage for paired objects and coords in each bin 
 Vector<Vector<std::pair<ELEMENT*,Vector<double> > > > Bin_object_coord_pairs;

 ///Storage for min and max coordinates of the bin structure
 Vector<double> Minmax_coords;

 ///Number of bins in x direction
 unsigned Nbin_x;

 ///Number of bins in y direction
 unsigned Nbin_y;

 ///Number of bins in z direction
 unsigned Nbin_z;

 ///Current spiralling level
 unsigned Current_spiral_level;

 ///Communicator
 OomphCommunicator* Communicator_pt;

public:
 
// using namespace Multi_domain_functions;
 
 
 ///\short Constructor, pass the pointer to the mesh
  MeshAsGeomObject(Mesh* const &mesh_pt) :
 GeomObject(DIM_LAGRANGIAN,DIM_EULERIAN)
  {
   OomphCommunicator* comm_pt=0;
   bool compute_extreme_bin_coords=true;
   this->construct_it(mesh_pt,comm_pt,compute_extreme_bin_coords);
  }


 ///\short Constructor, pass the pointer to the mesh and communicator
  MeshAsGeomObject(Mesh* const &mesh_pt,
                   OomphCommunicator* comm_pt) :
 GeomObject(DIM_LAGRANGIAN,DIM_EULERIAN)
  {
   bool compute_extreme_bin_coords=true;
   this->construct_it(mesh_pt,comm_pt,compute_extreme_bin_coords);
  }

 ///\short Constructor, pass the pointer to the mesh and 
 /// boolean to bypass the computation of the extreme coordinates
 ///of the bin used in the locate_zeta method
 MeshAsGeomObject(Mesh* const &mesh_pt,
                  const bool& compute_extreme_bin_coords) :
 GeomObject(DIM_LAGRANGIAN,DIM_EULERIAN)
  {
   OomphCommunicator* comm_pt=0;
   this->construct_it(mesh_pt,comm_pt,compute_extreme_bin_coords);
  }


 ///\short Constructor, pass the pointer to the mesh, communicator, and 
 /// boolean to bypass the computation of the extreme coordinates
 ///of the bin used in the locate_zeta method
 MeshAsGeomObject(Mesh* const &mesh_pt,
                  OomphCommunicator* comm_pt,
                  const bool& compute_extreme_bin_coords) :
 GeomObject(DIM_LAGRANGIAN,DIM_EULERIAN)
  {
   this->construct_it(mesh_pt,comm_pt,compute_extreme_bin_coords);
  }
 
 /// Empty constructor
 MeshAsGeomObject(){} 

 /// Broken copy constructor
 MeshAsGeomObject(const MeshAsGeomObject&) 
  { 
   BrokenCopy::broken_copy("MeshAsGeomObject");
  } 
 
 /// Broken assignment operator
 void operator=(const MeshAsGeomObject&) 
  {
   BrokenCopy::broken_assign("MeshAsGeomObject");
  }



 /// How many items of Data does the shape of the object depend on?
 unsigned ngeom_data() const {return Geom_data_pt.size();}
 
 /// \short Return pointer to the j-th Data item that the object's 
 /// shape depends on 
 Data* geom_data_pt(const unsigned& j) {return Geom_data_pt[j];}

 /// \short Find the sub geometric object and local coordinate therein that
 /// corresponds to the intrinsic coordinate zeta. If sub_geom_object_pt=0
 /// on return from this function, none of the constituent sub-objects 
 /// contain the required coordinate.
 /// Setting the optional bool argument to true means that each
 /// time the sub-object's locate_zeta function is called, the coordinate
 /// argument "s" is used as the initial guess
 void locate_zeta(const Vector<double>& zeta, 
                  GeomObject*& sub_geom_object_pt, 
                  Vector<double>& s,
                  const bool& use_coordinate_as_initial_guess=false);

 /// \short Return the position as a function of the intrinsic coordinate zeta.
 /// This provides an (expensive!) default implementation in which
 /// we loop over all the constituent sub-objects and check if they
 /// contain zeta and then evaluate their position() function.
 void position(const Vector<double> &zeta, Vector<double> &r) const
  {
   // Call position function at current timestep:
   unsigned t=0;
   position(t,zeta,r);
  }


 /// \short Parametrised position on object: r(zeta). Evaluated at
 /// previous timestep. t=0: current time; t>0: previous
 /// timestep. This provides an (expensive!) default implementation in which
 /// we loop over all the constituent sub-objects and check if they
 /// contain zeta and then evaluate their position() function. 
 void position(const unsigned& t, const Vector<double>& zeta,
               Vector<double>& r) const
  {
   // Storage for the GeomObject that contains the zeta coordinate
   // and the intrinsic coordinate within it.
   GeomObject* sub_geom_object_pt;
   Vector<double> s(DIM_LAGRANGIAN);

   //Find the sub object containing zeta, and the local intrinsic coordinate
   //within it
   const_cast<MeshAsGeomObject*>(this)->locate_zeta(zeta,sub_geom_object_pt,s);
   if(sub_geom_object_pt == 0) 
    {
     std::ostringstream error_message;
     error_message << "Cannot locate zeta ";
     for(unsigned i=0;i<DIM_LAGRANGIAN;i++)
      {
       error_message << zeta[i] << " ";
      }
     error_message << std::endl;
     throw OomphLibError(error_message.str(),
                         "MeshAsGeomObject::position()",
                         OOMPH_EXCEPTION_LOCATION);
    }
   //Call that sub-object's position function
   sub_geom_object_pt->position(t,s,r);

  } // end of position

 ///Return the derivative of the position
 void dposition(const Vector<double> &xi, DenseMatrix<double> &drdxi) const
  {
   throw OomphLibError("dposition() not implemented",
                       "MeshAsGeomObject::dposition()",
                       OOMPH_EXCEPTION_LOCATION);
  }

 ///Access function to current spiral level
 unsigned& current_spiral_level() {return Current_spiral_level;}

 ///Access function for min coordinate in x direction
 double& x_min() {return Minmax_coords[0];}

 ///Access function for max coordinate in x direction
 double& x_max() {return Minmax_coords[1];}

 ///Access function for min coordinate in y direction
 double& y_min() {return Minmax_coords[2];}

 ///Access function for max coordinate in y direction
 double& y_max() {return Minmax_coords[3];}

 ///Access function for min coordinate in z direction
 double& z_min() {return Minmax_coords[4];}

 ///Access function for max coordinate in z direction
 double& z_max() {return Minmax_coords[5];}

 ///Get the min and max coordinates for the mesh, in each dimension
 void get_min_and_max_coordinates(Mesh* const &mesh_pt);

 ///Initialise and populate the "bin" structure for locating coordinates
 void create_bins_of_objects();

 ///Flush the storage for the binning method
 void flush_bins_of_objects()
  {
   Bin_object_coord_pairs.clear();
  }

 ///Calculate the bin numbers of all the neighbours to "bin" given the level
 void get_neighbouring_bins_helper(const unsigned& bin, const unsigned& level,
                                   Vector<unsigned>& neighbour_bin);

};

}

#endif

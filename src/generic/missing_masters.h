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
//Header file for multi-domain functions, including the class
//ElementWithExternalElement which stores pointers to external elements

// Temporary flag to enable full annotation of multi domain
// comms (but keep alive because it would be such a bloody pain to
// rewrite it if things ever go wrong again...)
//#define ANNOTATE_MULTI_DOMAIN_COMMUNICATION

//Include guards to prevent multiple inclusion of the header
#ifndef OOMPH_MISSING_MASTERS_HEADER
#define OOMPH_MISSING_MASTERS_HEADER

// Config header generated by autoconfig
#ifdef HAVE_CONFIG_H
#include <oomph-lib-config.h>
#endif

//Oomph-lib headers
#include "geom_objects.h"
#include "problem.h"
#include "shape.h"

#include "mesh.h"
#include "mesh_as_geometric_object.h"
#include "algebraic_elements.h"
#include "macro_element_node_update_element.h"
#include "Qelements.h"
#include "element_with_external_element.h"


namespace oomph
{

//======================================================================
// Namespace for global missing master constructing functions
//======================================================================
namespace Missing_masters_functions
 {

#ifdef ANNOTATE_MISSING_MASTERS_COMMUNICATION

  // Temporary vector of strings to enable full annotation of multi domain
  // comms (but keep alive because it would be such a bloody pain to
  // rewrite it if things ever go wrong again...)
  //BENFLAG: This is left over from the multi-domain stuff and should work
  //         in the same way, but it has not been tested.
  extern Vector<std::string> Flat_packed_unsigneds_string;

#endif

  /// \short Boolean to indicate whether to doc timings or not.
  extern bool Doc_timings;

  /// \short Boolean to indicate whether to document basic info (to screen)
  ///        during setup_multi_domain_interaction() routines
  extern bool Doc_stats;

  /// \short Boolean to indicate whether to document further info (to screen)
  ///        during setup_multi_domain_interaction() routines
  extern bool Doc_full_stats;


  // Functions for multi-domain method

#ifdef OOMPH_HAS_MPI
  // Helper functions for external haloed node identification


  /// \short Helper function to add external haloed nodes, inc. masters
  /// of external haloed nodes
  void add_external_haloed_node_to_storage(int& iproc, Node* nod_pt,
                                           Mesh* const &mesh_pt,
                                           int& n_cont_inter_values,
                                           Vector<unsigned>& send_unsigneds,
                                           Vector<double>& send_doubles);

  
  /// \short Recursively add any master nodes (and their master nodes etc) 
  /// of external haloed nodes
  void recursively_add_masters_of_external_haloed_node(
   int& iproc, Node* nod_pt,
   Mesh* const &mesh_pt,
   int& n_cont_inter_values,
   Vector<unsigned>& send_unsigneds,
   Vector<double>& send_doubles);
  

  /// \short Helper function to add external haloed node that is not a master
  void add_external_haloed_node_helper(int& iproc, Node* nod_pt,
                                       Mesh* const &mesh_pt,
                                       int& n_cont_inter_values,
                                       Vector<unsigned>& send_unsigneds,
                                       Vector<double>& send_doubles);

  /// \short Helper function to add external haloed node that is a master
  void add_external_haloed_master_node_helper(int& iproc,Node* master_nod_pt,
                                              Mesh* const &mesh_pt,
                                              int& n_cont_inter_values,
                                              Vector<unsigned>& send_unsigneds,
                                              Vector<double>& send_doubles);

  /// \short Helper function to get the required nodal information from an
  /// external haloed node so that a fully-functional external halo
  /// node (and therefore element) can be created on the receiving process
  void get_required_nodal_information_helper(int& iproc, Node* nod_pt,
                                             Mesh* const &mesh_pt,
                                             int& n_cont_inter_values,
                                             Vector<unsigned>& send_unsigneds,
                                             Vector<double>& send_doubles);

  /// \short Helper function to get the required master nodal information from
  /// an external haloed master node so that a fully-functional external halo
  /// master node (and possible element) can be created on the receiving proc
  void get_required_master_nodal_information_helper
   (int& iproc, Node* master_nod_pt,
    Mesh* const &mesh_pt, int& n_cont_inter_values,
    Vector<unsigned>& send_unsigneds,
    Vector<double>& send_doubles);
  
  // Helper functions for external halo node identification
  
  /// \short Helper function to add external halo nodes, including any masters,
  /// based on information received from the haloed process
  template<class EXT_ELEMENT>
   void add_external_halo_node_to_storage(Node* &new_nod_pt,
                                          Mesh* const &mesh_pt,
                                          unsigned& loc_p,
                                          unsigned& node_index,
                                          FiniteElement* const &new_el_pt,
                                          int& n_cont_inter_values,
                                          unsigned &counter_for_recv_unsigneds,
                                          Vector<unsigned>& recv_unsigneds,
                                          unsigned &counter_for_recv_doubles,
                                          Vector<double>& recv_doubles);
  
  /// \short Recursively add masters of external halo nodes (and their 
  /// masters, etc) based on information received from the haloed process
  template<class EXT_ELEMENT>
   void recursively_add_masters_of_external_halo_node_to_storage
   (Node* &new_nod_pt, Mesh* const &mesh_pt, unsigned& loc_p,
    unsigned& node_index, int& n_cont_inter_values,
    unsigned &counter_for_recv_unsigneds,
    Vector<unsigned>& recv_unsigneds,
    unsigned &counter_for_recv_doubles,
    Vector<double>& recv_doubles);
   

  /// \short Helper function to add external halo node that is not a master
   void add_external_halo_node_helper(Node* &new_nod_pt,
                                      Mesh* const &mesh_pt,
                                      unsigned& loc_p,
                                      unsigned& node_index,
                                      FiniteElement* const &new_el_pt,
                                      int& n_cont_inter_values,
                                      unsigned &counter_for_recv_unsigneds,
                                      Vector<unsigned>& recv_unsigneds,
                                      unsigned &counter_for_recv_doubles,
                                      Vector<double>& recv_doubles);

  /// \short Helper function to add external halo node that is a master
  template<class EXT_ELEMENT>
   void add_external_halo_master_node_helper(Node* &new_master_nod_pt,
                                             Node* &new_nod_pt,
                                             Mesh* const &mesh_pt,
                                             unsigned& loc_p,
                                             int& n_cont_inter_values,
                                             unsigned &counter_for_recv_unsigneds,
                                             Vector<unsigned>& recv_unsigneds,
                                             unsigned &counter_for_recv_doubles,
                                             Vector<double>& recv_doubles);


  /// \short Helper function which constructs a new external halo node 
  /// (on an element) with the information sent from the haloed process
   void construct_new_external_halo_node_helper(Node* &new_nod_pt,
                                                unsigned& loc_p,
                                                unsigned& node_index,
                                                FiniteElement* const 
                                                &new_el_pt,
                                                Mesh* const &mesh_pt,
                                                unsigned &counter_for_recv_unsigneds,
                                                Vector<unsigned>& recv_unsigneds,
                                                unsigned &counter_for_recv_doubles,
                                                Vector<double>& recv_doubles);

  /// \short Helper function which constructs a new external halo master node
  /// with the information sent from the haloed process
  template<class EXT_ELEMENT>
   void construct_new_external_halo_master_node_helper
   (Node* &new_master_nod_pt,Node* &nod_pt,unsigned& loc_p,
    Mesh* const &mesh_pt,
    unsigned &counter_for_recv_unsigneds,
    Vector<unsigned>& recv_unsigneds,
    unsigned &counter_for_recv_doubles,
    Vector<double>& recv_doubles);

#endif

 }


}

#endif




 

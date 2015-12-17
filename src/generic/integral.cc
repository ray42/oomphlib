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
//Static data for the Gaussian integration rules

//oomph-lib header
#include "integral.h"

namespace oomph
{

//Need to define the static data here

//============================================================
// Define the positions and weights of the 1D Gauss points 
//============================================================

//------------------------------------------------------------
//N=2
//------------------------------------------------------------
const double Gauss<1,2>::Knot[2][1]=
{{-0.577350269189626},{0.577350269189626}};

const double Gauss<1,2>::Weight[2]={1.0,1.0}; 

//------------------------------------------------------------
//N=3
//------------------------------------------------------------
const double Gauss<1,3>::Knot[3][1]= 
{{-0.774596669241483},{0.0},{0.774596669241483}};

const double Gauss<1,3>::Weight[3]={(5.0/9.0),(8.0/9.0),(5.0/9.0)};

//------------------------------------------------------------
//N=4
//------------------------------------------------------------
const double Gauss<1,4>::Knot[4][1]= 
{{-0.861136311594053},{-0.339981043584856}, {0.339981043584856},
 {0.861136311594053}};

const double Gauss<1,4>::Weight[4]={0.347854845137448, 0.652145154862546, 0.652145154862546, 0.347854845137448};



//============================================================
// Define the positions and weights of the 2D Gauss points 
//============================================================

//------------------------------------------------------------
//N=2
//------------------------------------------------------------
const double Gauss<2,2>::Knot[4][2]={
 {-0.577350269189626,-0.577350269189626},
 {-0.577350269189626,0.577350269189626},
 {0.577350269189626,-0.577350269189626},
 {0.577350269189626,0.577350269189626}};
const double Gauss<2,2>::Weight[4] = {1.0,1.0,1.0,1.0};

//------------------------------------------------------------
//N=3
//------------------------------------------------------------
const double Gauss<2,3>::Knot[9][2]= {
  {-0.774596669241483,-0.774596669241483},
  {-0.774596669241483,0.0},
  {-0.774596669241483,0.774596662941483},
  {0.0,-0.774596669241483},
  {0.0,0.0},
  {0.0,0.774596662941483},
  {0.774596662941483,-0.774596669241483},
  {0.774596662941483,0.0},
  {0.774596662941483,0.774596662941483}};
const double Gauss<2,3>::Weight[9] = {(25.0/81.0),(40.0/81.0),(25.0/81.0),
  (40.0/81.0),(64.0/81.0),(40.0/81.0),(25.0/81.0),(40.0/81.0),(25.0/81.0)};

//------------------------------------------------------------
//N=4
//------------------------------------------------------------
const double Gauss<2,4>::Knot[16][2]= {
 {-0.861136311594053,-0.861136311594053},
 {-0.339981043584856,-0.861136311594053},
 { 0.339981043584856,-0.861136311594053},
 { 0.861136311594053,-0.861136311594053},

 {-0.861136311594053,-0.339981043584856},
 {-0.339981043584856,-0.339981043584856},
 { 0.339981043584856,-0.339981043584856},
 { 0.861136311594053,-0.339981043584856},

 {-0.861136311594053, 0.339981043584856},
 {-0.339981043584856, 0.339981043584856},
 { 0.339981043584856, 0.339981043584856},
 { 0.861136311594053, 0.339981043584856},

 {-0.861136311594053, 0.861136311594053},
 {-0.339981043584856, 0.861136311594053},
 { 0.339981043584856, 0.861136311594053},
 { 0.861136311594053, 0.861136311594053}};

// Quick sanity check: they sum to 4  :)
const double Gauss<2,4>::Weight[16] = 
{0.1210029932855979 , 0.2268518518518480 , 0.2268518518518480, 0.1210029932855979,  
 0.2268518518518480 , 0.4252933030106941 , 0.4252933030106941, 0.2268518518518480,
 0.2268518518518480 , 0.4252933030106941 , 0.4252933030106941, 0.2268518518518480,
 0.1210029932855979 , 0.2268518518518480 , 0.2268518518518480, 0.1210029932855979};



//============================================================
// Define the positions and weights of the 3D Gauss points 
// (produced with utilities/gauss_weights.cc)
//============================================================

//------------------------------------------------------------
//N=2
//------------------------------------------------------------
const double Gauss<3,2>::Knot[8][3]=
{{-0.57735026918963,-0.57735026918963,-0.57735026918963},{-0.57735026918963,-0.57735026918963,0.57735026918963},{-0.57735026918963,0.57735026918963,-0.57735026918963},{-0.57735026918963,0.57735026918963,0.57735026918963},{0.57735026918963,-0.57735026918963,-0.57735026918963},{0.57735026918963,-0.57735026918963,0.57735026918963},{0.57735026918963,0.57735026918963,-0.57735026918963},{0.57735026918963,0.57735026918963,0.57735026918963}};
const double Gauss<3,2>::Weight[8] = {1,1,1,1,1,1,1,1};

//------------------------------------------------------------
//N=3
//------------------------------------------------------------
const double Gauss<3,3>::Knot[27][3]= 
{{-0.77459666924148,-0.77459666924148,-0.77459666924148},{-0.77459666924148,-0.77459666924148,0},{-0.77459666924148,-0.77459666924148,0.77459666924148},{-0.77459666924148,0,-0.77459666924148},{-0.77459666924148,0,0},{-0.77459666924148,0,0.77459666924148},{-0.77459666924148,0.77459666924148,-0.77459666924148},{-0.77459666924148,0.77459666924148,0},{-0.77459666924148,0.77459666924148,0.77459666924148},{0,-0.77459666924148,-0.77459666924148},{0,-0.77459666924148,0},{0,-0.77459666924148,0.77459666924148},{0,0,-0.77459666924148},{0,0,0},{0,0,0.77459666924148},{0,0.77459666924148,-0.77459666924148},{0,0.77459666924148,0},{0,0.77459666924148,0.77459666924148},{0.77459666924148,-0.77459666924148,-0.77459666924148},{0.77459666924148,-0.77459666924148,0},{0.77459666924148,-0.77459666924148,0.77459666924148},{0.77459666924148,0,-0.77459666924148},{0.77459666924148,0,0},{0.77459666924148,0,0.77459666924148},{0.77459666924148,0.77459666924148,-0.77459666924148},{0.77459666924148,0.77459666924148,0},{0.77459666924148,0.77459666924148,0.77459666924148}};


const double Gauss<3,3>::Weight[27] = {0.17146776406035,0.27434842249657,0.17146776406035,0.27434842249657,0.43895747599451,0.27434842249657,0.17146776406035,0.27434842249657,0.17146776406035,0.27434842249657,0.43895747599451,0.27434842249657,0.43895747599451,0.70233196159122,0.43895747599451,0.27434842249657,0.43895747599451,0.27434842249657,0.17146776406035,0.27434842249657,0.17146776406035,0.27434842249657,0.43895747599451,0.27434842249657,0.17146776406035,0.27434842249657,0.17146776406035};



//------------------------------------------------------------
//N=4
//------------------------------------------------------------
const double Gauss<3,4>::Knot[64][3]= 
{{-0.86113631159405,-0.86113631159405,-0.86113631159405},{-0.86113631159405,-0.86113631159405,-0.33998104358486},{-0.86113631159405,-0.86113631159405,0.33998104358486},{-0.86113631159405,-0.86113631159405,0.86113631159405},{-0.86113631159405,-0.33998104358486,-0.86113631159405},{-0.86113631159405,-0.33998104358486,-0.33998104358486},{-0.86113631159405,-0.33998104358486,0.33998104358486},{-0.86113631159405,-0.33998104358486,0.86113631159405},{-0.86113631159405,0.33998104358486,-0.86113631159405},{-0.86113631159405,0.33998104358486,-0.33998104358486},{-0.86113631159405,0.33998104358486,0.33998104358486},{-0.86113631159405,0.33998104358486,0.86113631159405},{-0.86113631159405,0.86113631159405,-0.86113631159405},{-0.86113631159405,0.86113631159405,-0.33998104358486},{-0.86113631159405,0.86113631159405,0.33998104358486},{-0.86113631159405,0.86113631159405,0.86113631159405},{-0.33998104358486,-0.86113631159405,-0.86113631159405},{-0.33998104358486,-0.86113631159405,-0.33998104358486},{-0.33998104358486,-0.86113631159405,0.33998104358486},{-0.33998104358486,-0.86113631159405,0.86113631159405},{-0.33998104358486,-0.33998104358486,-0.86113631159405},{-0.33998104358486,-0.33998104358486,-0.33998104358486},{-0.33998104358486,-0.33998104358486,0.33998104358486},{-0.33998104358486,-0.33998104358486,0.86113631159405},{-0.33998104358486,0.33998104358486,-0.86113631159405},{-0.33998104358486,0.33998104358486,-0.33998104358486},{-0.33998104358486,0.33998104358486,0.33998104358486},{-0.33998104358486,0.33998104358486,0.86113631159405},{-0.33998104358486,0.86113631159405,-0.86113631159405},{-0.33998104358486,0.86113631159405,-0.33998104358486},{-0.33998104358486,0.86113631159405,0.33998104358486},{-0.33998104358486,0.86113631159405,0.86113631159405},{0.33998104358486,-0.86113631159405,-0.86113631159405},{0.33998104358486,-0.86113631159405,-0.33998104358486},{0.33998104358486,-0.86113631159405,0.33998104358486},{0.33998104358486,-0.86113631159405,0.86113631159405},{0.33998104358486,-0.33998104358486,-0.86113631159405},{0.33998104358486,-0.33998104358486,-0.33998104358486},{0.33998104358486,-0.33998104358486,0.33998104358486},{0.33998104358486,-0.33998104358486,0.86113631159405},{0.33998104358486,0.33998104358486,-0.86113631159405},{0.33998104358486,0.33998104358486,-0.33998104358486},{0.33998104358486,0.33998104358486,0.33998104358486},{0.33998104358486,0.33998104358486,0.86113631159405},{0.33998104358486,0.86113631159405,-0.86113631159405},{0.33998104358486,0.86113631159405,-0.33998104358486},{0.33998104358486,0.86113631159405,0.33998104358486},{0.33998104358486,0.86113631159405,0.86113631159405},{0.86113631159405,-0.86113631159405,-0.86113631159405},{0.86113631159405,-0.86113631159405,-0.33998104358486},{0.86113631159405,-0.86113631159405,0.33998104358486},{0.86113631159405,-0.86113631159405,0.86113631159405},{0.86113631159405,-0.33998104358486,-0.86113631159405},{0.86113631159405,-0.33998104358486,-0.33998104358486},{0.86113631159405,-0.33998104358486,0.33998104358486},{0.86113631159405,-0.33998104358486,0.86113631159405},{0.86113631159405,0.33998104358486,-0.86113631159405},{0.86113631159405,0.33998104358486,-0.33998104358486},{0.86113631159405,0.33998104358486,0.33998104358486},{0.86113631159405,0.33998104358486,0.86113631159405},{0.86113631159405,0.86113631159405,-0.86113631159405},{0.86113631159405,0.86113631159405,-0.33998104358486},{0.86113631159405,0.86113631159405,0.33998104358486},{0.86113631159405,0.86113631159405,0.86113631159405}};



const double Gauss<3,4>::Weight[64] = 
{0.042091477490529,0.078911515795068,0.078911515795068,0.042091477490529,0.078911515795068,0.14794033605678,0.14794033605678,0.078911515795068,0.078911515795068,0.14794033605678,0.14794033605678,0.078911515795068,0.042091477490529,0.078911515795068,0.078911515795068,0.042091477490529,0.078911515795068,0.14794033605678,0.14794033605678,0.078911515795068,0.14794033605678,0.27735296695391,0.27735296695391,0.14794033605678,0.14794033605678,0.27735296695391,0.27735296695391,0.14794033605678,0.078911515795068,0.14794033605678,0.14794033605678,0.078911515795068,0.078911515795068,0.14794033605678,0.14794033605678,0.078911515795068,0.14794033605678,0.27735296695391,0.27735296695391,0.14794033605678,0.14794033605678,0.27735296695391,0.27735296695391,0.14794033605678,0.078911515795068,0.14794033605678,0.14794033605678,0.078911515795068,0.042091477490529,0.078911515795068,0.078911515795068,0.042091477490529,0.078911515795068,0.14794033605678,0.14794033605678,0.078911515795068,0.078911515795068,0.14794033605678,0.14794033605678,0.078911515795068,0.042091477490529,0.078911515795068,0.078911515795068,0.042091477490529};

//1D Triangles, which are just the same as 1D Gauss elements, but scaled
//so their coordinate runs from 0 to 1, rather than -1 to 1

//------------------------------------------------------------
//N=2
//------------------------------------------------------------
const double TGauss<1,2>::Knot[2][1]=
{{0.5*(-0.577350269189626+1.0)},{0.5*(0.577350269189626+1.0)}};

const double TGauss<1,2>::Weight[2]={0.5,0.5}; 

//------------------------------------------------------------
//N=3
//------------------------------------------------------------
const double TGauss<1,3>::Knot[3][1]= 
{{0.5*(-0.774596669241483+1.0)},{0.5},{0.5*(0.774596669241483+1.0)}};

const double TGauss<1,3>::Weight[3]={(5.0/18.0),(8.0/18.0),(5.0/18.0)};

//------------------------------------------------------------
//N=4
//------------------------------------------------------------
const double TGauss<1,4>::Knot[4][1]= 
{{0.5*(-0.861136311594053+1.0)},{0.5*(-0.339981043584856+1.0)}, 
 {0.5*(0.339981043584856+1.0)}, {0.5*(0.861136311594053+1.0)}};

const double TGauss<1,4>::Weight[4]={0.5*0.347854845137448, 
0.5*0.652145154862546, 0.5*0.652145154862546, 0.5*0.347854845137448};

//------------------------------------------------------------
//N=5
//------------------------------------------------------------
const double TGauss<1,5>::Knot[5][1]= {};

const double TGauss<1,5>::Weight[5]= {};

//// Define the positions and weights of the 2D Gauss points for triangles 
//
//------------------------------------------------------------
// "Full integration" weights for linear triangles
// accurate up to second order (Bathe p 467)
//------------------------------------------------------------
const double TGauss<2,2>::Knot[3][2]={
 {0.1666666666667,0.1666666666667},
 {0.6666666666667,0.1666666666667},
 {0.1666666666667,0.6666666666667}};

const double TGauss<2,2>::Weight[3] = {
0.1666666666667,
0.1666666666667,
0.1666666666667};

//------------------------------------------------------------
// "Full integration" weights for quadratic triangles
// accurate up to fifth order (Bathe p 467)
//------------------------------------------------------------
const double TGauss<2,3>::Knot[7][2]={
 {0.1012865073235,0.1012865073235},
 {0.7974269853531,0.1012865073235},
 {0.1012865073235,0.7974269853531},
 {0.4701420641051,0.0597158717898},
 {0.4701420641051,0.4701420641051},
 {0.0597158717898,0.4701420641051},
 {0.3333333333333,0.3333333333333}};

const double TGauss<2,3>::Weight[7] = {
0.5*0.1259391805448,
0.5*0.1259391805448,
0.5*0.1259391805448,
0.5*0.1323941527885,
0.5*0.1323941527885,
0.5*0.1323941527885,
0.5*0.225};



//------------------------------------------------------------
//"Full integration" weights for cubic triangles
// accurate up to seventh order (Bathe p 467)
//------------------------------------------------------------
const double TGauss<2,4>::Knot[13][2]={
 {0.0651301029022,0.0651301029022},
 {0.8697397941956,0.0651301029022},
 {0.0651301029022,0.8697397941956},
 {0.3128654960049,0.0486903154253},
 {0.6384441885698,0.3128654960049},
 {0.0486903154253,0.6384441885698},
 {0.6384441885698,0.0486903154253},
 {0.3128654960049,0.6384441885698},
 {0.0486903154253,0.3128654960049},
 {0.2603459660790,0.2603459660790},
 {0.4793080678419,0.2603459660790},
 {0.2603459660790,0.4793080678419},
 {0.3333333333333,0.3333333333333}};


const double TGauss<2,4>::Weight[13] = {
0.5*0.0533472356088,
0.5*0.0533472356088,
0.5*0.0533472356088,
0.5*0.0771137608903,
0.5*0.0771137608903,
0.5*0.0771137608903,
0.5*0.0771137608903,
0.5*0.0771137608903,
0.5*0.0771137608903,
0.5*0.1756152574332,
0.5*0.1756152574332,
0.5*0.1756152574332,
0.5*-0.1495700444677};

//------------------------------------------------------------
//"Full integration" weights for 2D triangles
// accurate up to order 11
// http://people.sc.fsu.edu/~jburkardt/datasets/quadrature_rules_tri/quadrature_rules_tri.html
// [TOMS706_37, order 37, degree of precision 13, a rule from ACM TOMS algorithm #706.]
//------------------------------------------------------------
 const double TGauss<2,13>::Knot[37][2]={
  {0.333333333333333333333333333333,  0.333333333333333333333333333333},
  {0.950275662924105565450352089520,  0.024862168537947217274823955239},
  {0.024862168537947217274823955239,  0.950275662924105565450352089520},
  {0.024862168537947217274823955239, 0.024862168537947217274823955239},
  {0.171614914923835347556304795551,  0.414192542538082326221847602214},
  {0.414192542538082326221847602214,  0.171614914923835347556304795551},
  {0.414192542538082326221847602214,  0.414192542538082326221847602214},
  {0.539412243677190440263092985511,  0.230293878161404779868453507244},
  {0.230293878161404779868453507244,  0.539412243677190440263092985511},
  {0.230293878161404779868453507244,  0.230293878161404779868453507244},
  {0.772160036676532561750285570113,  0.113919981661733719124857214943},
  {0.113919981661733719124857214943,  0.772160036676532561750285570113},
  {0.113919981661733719124857214943,  0.113919981661733719124857214943},
  {0.009085399949835353883572964740,  0.495457300025082323058213517632},
  {0.495457300025082323058213517632,  0.009085399949835353883572964740},
  {0.495457300025082323058213517632,  0.495457300025082323058213517632},
  {0.062277290305886993497083640527,  0.468861354847056503251458179727},
  {0.468861354847056503251458179727,  0.062277290305886993497083640527},
  {0.468861354847056503251458179727,  0.468861354847056503251458179727},
  {0.022076289653624405142446876931,  0.851306504174348550389457672223},
  {0.022076289653624405142446876931,  0.126617206172027096933163647918},
  {0.851306504174348550389457672223,  0.022076289653624405142446876931},
  {0.851306504174348550389457672223,  0.126617206172027096933163647918},
  {0.126617206172027096933163647918,  0.022076289653624405142446876931},
  {0.126617206172027096933163647918,  0.851306504174348550389457672223},
  {0.018620522802520968955913511549,  0.689441970728591295496647976487},
  {0.018620522802520968955913511549,  0.291937506468887771754472382212},
  {0.689441970728591295496647976487,  0.018620522802520968955913511549},
  {0.689441970728591295496647976487,  0.291937506468887771754472382212},
  {0.291937506468887771754472382212,  0.018620522802520968955913511549},
  {0.291937506468887771754472382212,  0.689441970728591295496647976487},
  {0.096506481292159228736516560903,  0.635867859433872768286976979827},
  {0.096506481292159228736516560903,  0.267625659273967961282458816185},
  {0.635867859433872768286976979827,  0.096506481292159228736516560903},
  {0.635867859433872768286976979827,  0.267625659273967961282458816185},
  {0.267625659273967961282458816185,  0.096506481292159228736516560903},
  {0.267625659273967961282458816185,  0.635867859433872768286976979827}
 };

 const double TGauss<2,13>::Weight[37] = 
 {
  0.051739766065744133555179145422,
  0.008007799555564801597804123460,
  0.008007799555564801597804123460,
  0.008007799555564801597804123460,
  0.046868898981821644823226732071,
  0.046868898981821644823226732071,
  0.046868898981821644823226732071,
  0.046590940183976487960361770070,
  0.046590940183976487960361770070,
  0.046590940183976487960361770070,
  0.031016943313796381407646220131,
  0.031016943313796381407646220131,
  0.031016943313796381407646220131,
  0.010791612736631273623178240136,
  0.010791612736631273623178240136,
  0.010791612736631273623178240136,
  0.032195534242431618819414482205,
  0.032195534242431618819414482205,
  0.032195534242431618819414482205,
  0.015445834210701583817692900053,
  0.015445834210701583817692900053,
  0.015445834210701583817692900053,
  0.015445834210701583817692900053,
  0.015445834210701583817692900053,
  0.015445834210701583817692900053,
  0.017822989923178661888748319485,
  0.017822989923178661888748319485,
  0.017822989923178661888748319485,
  0.017822989923178661888748319485,
  0.017822989923178661888748319485,
  0.017822989923178661888748319485,
  0.037038683681384627918546472190,
  0.037038683681384627918546472190,
  0.037038683681384627918546472190,
  0.037038683681384627918546472190,
  0.037038683681384627918546472190,
  0.037038683681384627918546472190
 };

//------------------------------------------------------------
//"Full integration" weights for 2D triangles
// accurate up to order 15
// http://people.sc.fsu.edu/~jburkardt/datasets/quadrature_rules_tri/quadrature_rules_tri.html
// [GAUSS8X8, order 64, degree of precision 15, (essentially a product of two 8 point 1D Gauss-Legendre rules).]
//------------------------------------------------------------
  const double TGauss<2,5>::Knot[64][2]={
   {0.9553660447100000,        0.8862103848242247e-3},
   {0.9553660447100000,        0.4537789678039195e-2},
   {0.9553660447100000,        0.1058868260117431e-1},
   {0.9553660447100000,        0.1822327082910602e-1},
   {0.9553660447100000,        0.2641068446089399e-1},
   {0.9553660447100000,        0.3404527268882569e-1},
   {0.9553660447100000,        0.4009616561196080e-1},
   {0.9553660447100000,        0.4374774490517578e-1},
   {0.8556337429600001,        0.2866402391985981e-2},
   {0.8556337429600001,        0.1467724979327651e-1},
   {0.8556337429600001,        0.3424855503358430e-1},
   {0.8556337429600001,        0.5894224214571626e-1},
   {0.8556337429600001,        0.8542401489428375e-1},
   {0.8556337429600001,        0.1101177020064157},
   {0.8556337429600001,        0.1296890072467235},
   {0.8556337429600001,        0.1414998546480140},
   {0.7131752428600000,        0.5694926133044352e-2},
   {0.7131752428600000,        0.2916054411712861e-1},
   {0.7131752428600000,        0.6804452564827500e-1},
   {0.7131752428600000,        0.1171055801775613},
   {0.7131752428600000,        0.1697191769624387},
   {0.7131752428600000,        0.2187802314917250},
   {0.7131752428600000,        0.2576642130228714},
   {0.7131752428600000,        0.2811298310069557},
   {0.5451866848000000,        0.9030351006711630e-2},
   {0.5451866848000000,        0.4623939674940125e-1},
   {0.5451866848000000,        0.1078970888004545},
   {0.5451866848000000,        0.1856923986620134},
   {0.5451866848000000,        0.2691209165379867},   
   {0.5451866848000000,        0.3469162263995455},  
   {0.5451866848000000,        0.4085739184505988},    
   {0.5451866848000000,        0.4457829641932884},   
   {0.3719321645800000,        0.1247033193690498e-1},
   {0.3719321645800000,        0.6385362269957356e-1},
   {0.3719321645800000,        0.1489989161403976},    
   {0.3719321645800000,        0.2564292182833579},   
   {0.3719321645800000,        0.3716386171366422},  
   {0.3719321645800000,        0.4790689192796024},    
   {0.3719321645800000,        0.5642142127204264},    
   {0.3719321645800000,        0.6155975034830951},    
   {0.2143084794000000,        0.1559996151584746e-1},
   {0.2143084794000000,        0.7987871227492103e-1},
   {0.2143084794000000,        0.1863925811641285},    
   {0.2143084794000000,        0.3207842387034378},    
   {0.2143084794000000,        0.4649072818965623},    
   {0.2143084794000000,        0.5992989394358715},    
   {0.2143084794000000,        0.7058128083250790},    
   {0.2143084794000000,        0.7700915590841526},    
   {0.9132360790000005e-1,    0.1804183496379599e-1},
   {0.9132360790000005e-1,    0.9238218584838476e-1},
   {0.9132360790000005e-1,    0.2155687489628060},    
   {0.9132360790000005e-1,    0.3709968314854498},    
   {0.9132360790000005e-1,    0.5376795606145502},    
   {0.9132360790000005e-1,    0.6931076431371940},    
   {0.9132360790000005e-1,    0.8162942062516152},    
   {0.9132360790000005e-1,    0.8906345571362040},    
   {0.1777991514999999e-1,    0.1950205026019779e-1},
   {0.1777991514999999e-1,    0.9985913490381848e-1},
   {0.1777991514999999e-1,    0.2330157982952792},    
   {0.1777991514999999e-1,    0.4010234473667467},   
   {0.1777991514999999e-1,    0.5811966374832533},    
   {0.1777991514999999e-1,    0.7492042865547208},    
   {0.1777991514999999e-1,    0.8823609499461815},    
   {0.1777991514999999e-1,    0.9627180345898023}
  };
  const double TGauss<2,5>::Weight[64] = 
  {
   0.3335674062677772e-3,
   0.7327880811491046e-3,
   0.1033723454167925e-2,
   0.1195112498415193e-2,
   0.1195112498415193e-2,
   0.1033723454167925e-2,
   0.7327880811491046e-3,
   0.3335674062677772e-3,
   0.1806210919443461e-2,
   0.3967923151181667e-2,
   0.5597437146194232e-2,
   0.6471331443180639e-2,
   0.6471331443180639e-2,
   0.5597437146194232e-2,
   0.3967923151181667e-2,
   0.1806210919443461e-2,
   0.4599755803015752e-2,
   0.1010484287526739e-1,
   0.1425461651131868e-1,
   0.1648010431039818e-1,
   0.1648010431039818e-1,
   0.1425461651131868e-1,
   0.1010484287526739e-1,
   0.4599755803015752e-2,
   0.8017259531156730e-2,
   0.1761248886287915e-1,
   0.2484544071087993e-1,
   0.2872441038508419e-1,
   0.2872441038508419e-1,
   0.2484544071087993e-1,
   0.1761248886287915e-1,
   0.8017259531156730e-2,
   0.1073501897357062e-1,
   0.2358292149331603e-1,
   0.3326776143412911e-1,
   0.3846165753898425e-1,
   0.3846165753898425e-1,
   0.3326776143412911e-1,
   0.2358292149331603e-1,
   0.1073501897357062e-1,
   0.1138879740452669e-1,
   0.2501915606814251e-1,
   0.3529381699354388e-1,
   0.4080402900378691e-1,
   0.4080402900378691e-1,
   0.3529381699354388e-1,
   0.2501915606814251e-1,
   0.1138879740452669e-1,
   0.9223845391285393e-2,
   0.2026314273544469e-1,
   0.2858464328177232e-1,
   0.3304739223149761e-1,
   0.3304739223149761e-1,
   0.2858464328177232e-1,
   0.2026314273544469e-1,
   0.9223845391285393e-2,
   0.4509812715921713e-2,
   0.9907253959306707e-2,
   0.1397588340693756e-1,
   0.1615785427783403e-1,
   0.1615785427783403e-1,
   0.1397588340693756e-1,
   0.9907253959306707e-2,
   0.4509812715921713e-2
  };

//// Define the positions and weights of the 3D Gauss points for tets

//------------------------------------------------------------
// "Full integration" weights for linear tets
// accurate up to second order (e.g. from German Zienkiwicz p 200)
//------------------------------------------------------------
const double TGauss<3,2>::Knot[4][3]={
 {0.138196601125011,0.138196601125011,0.585410196624969},
 {0.138196601125011,0.585410196624969,0.138196601125011},
 {0.585410196624969,0.138196601125011,0.138196601125011},
 {0.138196601125011,0.138196601125011,0.138196601125011}};


const double TGauss<3,2>::Weight[4] = {
0.0416666666667,
0.0416666666667,
0.0416666666667,
0.0416666666667};





//------------------------------------------------------------
//"Full integration" weights for quadratic tets
// accurate up to fifth order 
// The numbers are from Keast CMAME 55 pp339-348 (1986)
//------------------------------------------------------------
const double TGauss<3,3>::Knot[11][3]={
 {0.25,0.25,0.25},
 {0.785714285714286,0.071428571428571,0.071428571428571},
 {0.071428571428571,0.071428571428571,0.071428571428571},
 {0.071428571428571,0.785714285714286,0.071428571428571},
 {0.071428571428571,0.071428571428571,0.785714285714286},
 {0.399403576166799,0.399403576166799,0.100596423833201},
 {0.399403576166799,0.100596423833201,0.399403576166799},
 {0.100596423833201,0.399403576166799,0.399403576166799},
 {0.399403576166799,0.100596423833201,0.100596423833201},
 {0.100596423833201,0.399403576166799,0.100596423833201},
 {0.100596423833201,0.100596423833201,0.399403576166799}};


const double TGauss<3,3>::Weight[11] ={
-0.01315555555556,
 0.00762222222222,
 0.00762222222222,
 0.00762222222222,
 0.00762222222222,
 0.02488888888889,
 0.02488888888889,
 0.02488888888889,
 0.02488888888889,
 0.02488888888889,
 0.02488888888889
};

//------------------------------------------------------------
//"Full integration" weights for quartic tets
// accurate up to eighth order 
// The numbers are from Keast CMAME 55 pp339-348 (1986)
//------------------------------------------------------------
const double TGauss<3,5>::Knot[45][3]={
 {2.50000000000000000e-01, 2.50000000000000000e-01, 2.50000000000000000e-01},
 {1.27470936566639015e-01, 1.27470936566639015e-01, 1.27470936566639015e-01},
 {1.27470936566639015e-01, 1.27470936566639015e-01, 6.17587190300082967e-01},
 {1.27470936566639015e-01, 6.17587190300082967e-01, 1.27470936566639015e-01},
 {6.17587190300082967e-01, 1.27470936566639015e-01, 1.27470936566639015e-01},
 {3.20788303926322960e-02, 3.20788303926322960e-02, 3.20788303926322960e-02},
 {3.20788303926322960e-02, 3.20788303926322960e-02, 9.03763508822103123e-01},
 {3.20788303926322960e-02, 9.03763508822103123e-01, 3.20788303926322960e-02},
 {9.03763508822103123e-01, 3.20788303926322960e-02, 3.20788303926322960e-02},
 {4.97770956432810185e-02, 4.97770956432810185e-02, 4.50222904356718978e-01}, 
 {4.97770956432810185e-02, 4.50222904356718978e-01, 4.50222904356718978e-01},
 {4.50222904356718978e-01, 4.50222904356718978e-01, 4.97770956432810185e-02}, 
 {4.50222904356718978e-01, 4.97770956432810185e-02, 4.97770956432810185e-02},
 {4.97770956432810185e-02, 4.50222904356718978e-01, 4.97770956432810185e-02}, 
 {4.50222904356718978e-01, 4.97770956432810185e-02, 4.50222904356718978e-01},
 {1.83730447398549945e-01, 1.83730447398549945e-01, 3.16269552601450060e-01}, 
 {1.83730447398549945e-01, 3.16269552601450060e-01, 3.16269552601450060e-01},
 {3.16269552601450060e-01, 3.16269552601450060e-01, 1.83730447398549945e-01}, 
 {3.16269552601450060e-01, 1.83730447398549945e-01, 1.83730447398549945e-01},
 {1.83730447398549945e-01, 3.16269552601450060e-01, 1.83730447398549945e-01}, 
 {3.16269552601450060e-01, 1.83730447398549945e-01, 3.16269552601450060e-01},
 {2.31901089397150906e-01, 2.31901089397150906e-01, 2.29177878448171174e-02}, 
 {2.31901089397150906e-01, 2.29177878448171174e-02, 5.13280033360881072e-01}, 
 {2.29177878448171174e-02, 5.13280033360881072e-01, 2.31901089397150906e-01}, 
 {5.13280033360881072e-01, 2.31901089397150906e-01, 2.31901089397150906e-01},
 {2.31901089397150906e-01, 5.13280033360881072e-01, 2.31901089397150906e-01}, 
 {5.13280033360881072e-01, 2.31901089397150906e-01, 2.29177878448171174e-02},
 {2.31901089397150906e-01, 2.29177878448171174e-02, 2.31901089397150906e-01}, 
 {2.31901089397150906e-01, 5.13280033360881072e-01, 2.29177878448171174e-02}, 
 {2.29177878448171174e-02, 2.31901089397150906e-01, 5.13280033360881072e-01}, 
 {5.13280033360881072e-01, 2.29177878448171174e-02, 2.31901089397150906e-01}, 
 {2.29177878448171174e-02, 2.31901089397150906e-01, 2.31901089397150906e-01}, 
 {2.31901089397150906e-01, 2.31901089397150906e-01, 5.13280033360881072e-01},
 {3.79700484718286102e-02, 3.79700484718286102e-02, 7.30313427807538396e-01}, 
 {3.79700484718286102e-02, 7.30313427807538396e-01, 1.93746475248804382e-01}, 
 {7.30313427807538396e-01, 1.93746475248804382e-01, 3.79700484718286102e-02}, 
 {1.93746475248804382e-01, 3.79700484718286102e-02, 3.79700484718286102e-02},
 {3.79700484718286102e-02, 1.93746475248804382e-01, 3.79700484718286102e-02}, 
 {1.93746475248804382e-01, 3.79700484718286102e-02, 7.30313427807538396e-01},
 {3.79700484718286102e-02, 7.30313427807538396e-01, 3.79700484718286102e-02}, 
 {3.79700484718286102e-02, 1.93746475248804382e-01, 7.30313427807538396e-01},
 {7.30313427807538396e-01, 3.79700484718286102e-02, 1.93746475248804382e-01}, 
 {1.93746475248804382e-01, 7.30313427807538396e-01, 3.79700484718286102e-02},
 {7.30313427807538396e-01, 3.79700484718286102e-02, 3.79700484718286102e-02}, 
 {3.79700484718286102e-02,  3.79700484718286102e-02, 1.93746475248804382e-01} 
};

const double TGauss<3,5>::Weight[45] ={
-3.93270066412926145e-02,  4.08131605934270525e-03, 
 4.08131605934270525e-03,  4.08131605934270525e-03, 
 4.08131605934270525e-03,  6.58086773304341943e-04, 
 6.58086773304341943e-04,  6.58086773304341943e-04, 
 6.58086773304341943e-04,  4.38425882512284693e-03, 
 4.38425882512284693e-03,  4.38425882512284693e-03, 
 4.38425882512284693e-03,  4.38425882512284693e-03, 
 4.38425882512284693e-03,  1.38300638425098166e-02, 
 1.38300638425098166e-02,  1.38300638425098166e-02, 
 1.38300638425098166e-02,  1.38300638425098166e-02, 
 1.38300638425098166e-02,  4.24043742468372453e-03, 
 4.24043742468372453e-03,  4.24043742468372453e-03, 
 4.24043742468372453e-03,  4.24043742468372453e-03, 
 4.24043742468372453e-03,  4.24043742468372453e-03, 
 4.24043742468372453e-03,  4.24043742468372453e-03, 
 4.24043742468372453e-03,  4.24043742468372453e-03, 
 4.24043742468372453e-03,  2.23873973961420164e-03, 
 2.23873973961420164e-03,  2.23873973961420164e-03, 
 2.23873973961420164e-03,  2.23873973961420164e-03, 
 2.23873973961420164e-03,  2.23873973961420164e-03, 
 2.23873973961420164e-03,  2.23873973961420164e-03, 
 2.23873973961420164e-03,  2.23873973961420164e-03, 
 2.23873973961420164e-03}; 

}





/*===========================================================================*\
*                                                                            *
*                              OpenFlipper                                   *
 *           Copyright (c) 2001-2015, RWTH-Aachen University                 *
 *           Department of Computer Graphics and Multimedia                  *
 *                          All rights reserved.                             *
 *                            www.openflipper.org                            *
 *                                                                           *
 *---------------------------------------------------------------------------*
 * This file is part of OpenFlipper.                                         *
 *---------------------------------------------------------------------------*
 *                                                                           *
 * Redistribution and use in source and binary forms, with or without        *
 * modification, are permitted provided that the following conditions        *
 * are met:                                                                  *
 *                                                                           *
 * 1. Redistributions of source code must retain the above copyright notice, *
 *    this list of conditions and the following disclaimer.                  *
 *                                                                           *
 * 2. Redistributions in binary form must reproduce the above copyright      *
 *    notice, this list of conditions and the following disclaimer in the    *
 *    documentation and/or other materials provided with the distribution.   *
 *                                                                           *
 * 3. Neither the name of the copyright holder nor the names of its          *
 *    contributors may be used to endorse or promote products derived from   *
 *    this software without specific prior written permission.               *
 *                                                                           *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS       *
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED *
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A           *
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER *
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,  *
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,       *
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR        *
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF    *
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING      *
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS        *
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              *
*                                                                            *
\*===========================================================================*/

/*===========================================================================*\
*                                                                            *
*   $Revision$                                                       *
*   $LastChangedBy$                                                *
*   $Date$                     *
*                                                                            *
\*===========================================================================*/


//=============================================================================
#ifndef HOLEFILLER_HH
#define HOLEFILLER_HH
//=============================================================================

#include <vector>
#include <float.h>
#include <cmath>
#include <OpenMesh/Core/Utils/vector_cast.hh>
#include "OpenMeshUtils.h"
#include <OpenMesh/Tools/Smoother/JacobiLaplaceSmootherT.hh>
#include "Mesh.h"

//=============================================================================

class HoleFiller
{
public:

  import_om_abbreviations( Mesh );
  
  // A weight is a tuple of area and maximum dihedral angle
  //

  class Weight {
  public:
    
    Weight() : angle_( 180 ), area_( FLT_MAX ) {}
    Weight( Scalar _angle, Scalar _area ) : angle_( _angle ), area_( _area ) {}
    ~Weight() {}

    Scalar angle() const { return angle_; }
    Scalar area()  const { return area_; }

    Weight operator+( const Weight & _other ) const {
      return Weight( std::max( angle(), _other.angle() ),
		     area() + _other.area() );
    }

    bool operator<( const Weight & _rhs ) const {
      return ( angle() < _rhs.angle() ||
	       ( angle() == _rhs.angle() && area() < _rhs.area() ) );
    }

  private:
    Scalar angle_;
    Scalar area_;
  };


  // Ctors
  HoleFiller( Mesh & _mesh );
  ~HoleFiller();

  // Identify and fill all holes of the mesh.
  void fill_all_holes( int _stages = 3 );


  // Fill a hole which is identified by one of its boundary edges.
  void fill_hole( EH _eh, int _stages = 3 );

  // Fair a filling
  void fairing( std::vector< FH >& _faceHandles );

  // Remove degenerated faces from the filling
  void removeDegeneratedFaces( std::vector< FH >& _faceHandles );

private:

  // Refine a face
  bool refine( FH _fh );

  // Relax an edge
  bool relax_edge( EH _eh );

  // Test whether a point _x lies in the circumsphere of _a,_b,_c.
  bool in_circumsphere( const Point & _x,
			const Point & _a,
			const Point & _b,
			const Point & _c ) const;

  // Create the triangulation for polygon (_i,...,_j).
  bool fill( int _i, int _j );

  // Compute the weight of the triangle (_i,_j,_k).
  Weight weight( int _i, int _j, int _k );

  // Does edge (_u,_v) already exist?
  bool exists_edge( VH _u, VH _w );

  // Compute the area of the triangle (_a,_b,_c).
  Scalar area( VH _a, VH _b, VH _c );

  // Compute the dihedral angle (in degrees) between triangle
  // (_u,_v,_a) and triangle (_v,_u,_b).
  Scalar dihedral_angle( VH _u, VH _v, VH _a, VH _b );


  // The mesh, with each vertex we associate a scale factor that is
  // needed for remeshing

  Mesh & mesh_;  
  OpenMesh::VPropHandleT< Scalar > scale_;

  /*
                 HOLE
  
          boundary_vertex_
                 |
                 V
       ==*=======*=======*==  BOUNDARY 
        / \     / \     / \   
       /   \   /   \   /   \  
            \ /     \ /      
	     *       * <- opposite_vertex_
  */


  typedef std::vector< VH >                 VHVec;
  typedef std::vector< VH >::iterator       VHVecIter;
  typedef std::vector< VH >::const_iterator CVHVecIter;

  typedef std::vector< FH >                 FHVec;
  typedef std::vector< FH >::iterator       FHVecIter;
  typedef std::vector< FH >::const_iterator CFHVecIter;


  // This vector contains all vertices of the hole (in order)
  VHVec boundary_vertex_;

  // This vector contains all vertices that are opposite to an edge of the hole
  VHVec opposite_vertex_;

  // This vector contains all edges of the hole (in order)
  std::vector< EH > hole_edge_;

  // This vector stores handles to all triangles of the current hole
  std::vector< FH > hole_triangle_;

  // These are the two central arrays that are needed for the dynamic
  // programming approach to hole filling.
  //   w_[i][j] : stores the minimal weight that can be achieved
  //              for a triangulation of the polygon
  //              boundary_vertex_[i],...,boundary_vertex_[j]
  //   l_[i][j] : stores the third index of the triangle
  //                <boundary_vertex_[i],boundary_vertex_[l_[i][j]],
  //                 boundary_vertex_[j]>
  //              that is needed for reconstructing the minimal triangulation

  std::vector< std::vector< Weight > > w_;
  std::vector< std::vector< int    > > l_;
};
//=============================================================================
#endif // HOLEFILLER_HH defined
//=============================================================================


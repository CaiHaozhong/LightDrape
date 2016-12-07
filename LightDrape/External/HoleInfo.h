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
*   $Revision: 11127 $                                                       *
*   $LastChangedBy: moebius $                                                *
*   $Date: 2011-03-15 16:18:28 +0100 (Di, 15 Mär 2011) $                     *
*                                                                            *
\*===========================================================================*/

#ifndef HOLEINFO_HH
#define HOLEINFO_HH


// #include <OpenFlipper/common/perObjectData.hh>
// #include <OpenFlipper/common/Types.hh>

#include "HoleFiller.h"

class HoleInfo
{
  
  public :
    typedef std::vector< Mesh::EdgeHandle > Hole;

  private :
    // the mesh
    Mesh* mesh_;
    
    // list of holes
    std::vector< Hole > holes_;

    // holeFiller
    HoleFiller* filler_;
 
  public :

    /// Konstruktor
    HoleInfo(Mesh* _mesh);
  
    /// Destruktor
    ~HoleInfo();

    /** Find all holes in the mesh
     *  and store them in the internal list of holes.
     */
    void getHoles();

    /// fill hole with given index
    void fillHole(int _index, int _stages = 3 );

    /// fill hole with given boundary edgeHandle
    void fillHole( Mesh::EdgeHandle _eh, int _stages = 3 );

    /// fill all holes
    void fillAllHoles( int _stages = 3 );

    /// select a hole with given index
    void selectHole(int _index);

    /// Collect information to fly to a hole
    void getHolePostitionInfo(const int _index, Mesh::Normal& _holeNormal, Mesh::Point& _holeCenter) const;

    /** Collect bounding box size and boundary length of holes
     *
     * @param _index            Index of the hole
     * @param _edges            Count of boundary edges
     * @param _diagonal         Length of the hole bounding box diagonal
     * @param _boundary_Length  Boundary length of the hole
     */
    void getHoleInfo(const unsigned int _index, size_t& _edges, Mesh::Scalar& _diagonal, Mesh::Scalar& _boundaryLength) const;

    std::vector< std::vector< Mesh::EdgeHandle > >* holes();
};

#endif //HOLEINFO_HH

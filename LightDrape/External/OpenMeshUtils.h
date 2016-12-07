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
//
// Often used abbreviations for OpenMesh entitites.
//
//=============================================================================

#ifndef OMAbbreviations_HH
#define OMAbbreviations_HH

#define import_om_abbreviations( Mesh )    \
\
  typedef Mesh::Point Point;   \
  typedef Mesh::Scalar Scalar; \
\
  typedef Mesh::VertexHandle   VH; \
  typedef Mesh::EdgeHandle     EH; \
  typedef Mesh::HalfedgeHandle HH; \
  typedef Mesh::FaceHandle     FH; \
\
  typedef Mesh::VertexIter     VI; \
  typedef Mesh::HalfedgeIter   HI; \
  typedef Mesh::EdgeIter       EI; \
  typedef Mesh::FaceIter       FI; \
\
  typedef Mesh::Vertex    Vertex;   \
  typedef Mesh::Halfedge  Halfedge; \
  typedef Mesh::Edge      Edge;     \
  typedef Mesh::Face      Face;     \
\
  typedef Mesh::ConstVertexIter    CVI; \
  typedef Mesh::ConstHalfedgeIter  CHI; \
  typedef Mesh::ConstEdgeIter      CEI; \
  typedef Mesh::ConstFaceIter      CFI; \
\
  typedef Mesh::VertexVertexIter    VVI;  \
  typedef Mesh::VertexOHalfedgeIter VOHI; \
  typedef Mesh::VertexIHalfedgeIter VIHI; \
  typedef Mesh::VertexEdgeIter      VEI;  \
  typedef Mesh::VertexFaceIter      VFI;  \
  typedef Mesh::FaceVertexIter      FVI;  \
  typedef Mesh::FaceHalfedgeIter    FHI;  \
  typedef Mesh::FaceEdgeIter        FEI;  \
  typedef Mesh::FaceFaceIter        FFI;  \
\
  typedef Mesh::ConstVertexVertexIter    CVVI;  \
  typedef Mesh::ConstVertexOHalfedgeIter CVOHI; \
  typedef Mesh::ConstVertexIHalfedgeIter CVIHI; \
  typedef Mesh::ConstVertexEdgeIter      CVEI;  \
  typedef Mesh::ConstVertexFaceIter      CVFI;  \
  typedef Mesh::ConstFaceVertexIter      CFVI;  \
  typedef Mesh::ConstFaceHalfedgeIter    CFHI;  \
  typedef Mesh::ConstFaceEdgeIter        CFEI;  \
  typedef Mesh::ConstFaceFaceIter        CFFI;  \

#endif

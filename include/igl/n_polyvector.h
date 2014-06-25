// This file is part of libigl, a simple c++ geometry processing library.
//
// Copyright (C) 2014 Olga Diamanti <olga.diam@gmail.com>
//
// This Source Code Form is subject to the terms of the Mozilla Public License
// v. 2.0. If a copy of the MPL was not distributed with this file, You can
// obtain one at http://mozilla.org/MPL/2.0/.

#ifndef IGL_N_POLYVECTOR
#define IGL_N_POLYVECTOR
#include "igl_inline.h"

#include <Eigen/Core>
#include <vector>

namespace igl {
  //todo
  /// Given 2 vectors centered on origin calculate the rotation matrix from first to the second
  
  // Inputs:
  //   v0, v1         the two #3 by 1 vectors
  //   normalized     boolean, if false, then the vectors are normalized prior to the calculation
  // Output:
  //                  3 by 3 rotation matrix that takes v0 to v1
  //
  template <typename DerivedV, typename DerivedF>
  IGL_INLINE void n_polyvector(const Eigen::PlainObjectBase<DerivedV> &V,
                               const Eigen::PlainObjectBase<DerivedF> &F,
                               const Eigen::VectorXi &isConstrained,
                               const Eigen::Matrix<typename DerivedV::Scalar, Eigen::Dynamic, Eigen::Dynamic> &cfW,
                               Eigen::Matrix<typename DerivedV::Scalar, Eigen::Dynamic, Eigen::Dynamic> &output);
  
};


#ifdef IGL_HEADER_ONLY
#include "n_polyvector.cpp"
#endif


#endif /* defined(IGL_N_POLYVECTOR) */

/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_LQ_HPP
#define ELEM_LQ_HPP

#include "./LQ/ApplyQ.hpp"
#include "./LQ/Householder.hpp"
#include "./LQ/Explicit.hpp"

namespace elem {

// On exit, the lower triangle of A is overwritten by L, and the Householder
// transforms that determine Q are stored above the diagonal of A with an 
// implicit one on the diagonal. 
//
// In the complex case, the column-vector t stores the unit-magnitude complex 
// rotations that map the norms of the implicit Householder vectors to their
// coefficient:  
//                psi_j = 2 tau_j / ( u_j^H u_j ),
// where tau_j is the j'th entry of t and u_j is the j'th unscaled Householder
// reflector.

template<typename F> 
inline void
LQ( Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("LQ"))
    Matrix<F> t;
    lq::Householder( A, t );
}

template<typename F> 
inline void
LQ( DistMatrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("LQ"))
    DistMatrix<F,MD,STAR> t(A.Grid());
    lq::Householder( A, t );
}

template<typename F> 
inline void
LQ( Matrix<F>& A, Matrix<F>& t )
{
    DEBUG_ONLY(CallStackEntry cse("LQ"))
    lq::Householder( A, t );
}

template<typename F> 
inline void
LQ( DistMatrix<F>& A, DistMatrix<F,MD,STAR>& t )
{
    DEBUG_ONLY(
        CallStackEntry cse("LQ");
        if( A.Grid() != t.Grid() )
            LogicError("{A,t} must be distributed over the same grid");
    )
    lq::Householder( A, t );
}

} // namespace elem

#endif // ifndef ELEM_LQ_HPP

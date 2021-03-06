/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_RQ_PANEL_HPP
#define ELEM_RQ_PANEL_HPP

#include ELEM_GEMV_INC
#include ELEM_GER_INC
#include ELEM_REFLECTOR_INC
#include ELEM_ZEROS_INC

namespace elem {
namespace rq {

template<typename F> 
inline void
PanelHouseholder( Matrix<F>& A, Matrix<F>& t )
{
    DEBUG_ONLY(CallStackEntry cse("rq::PanelHouseholder"))
    const Int m = A.Height();
    const Int n = A.Width();
    const Int minDim = Min(m,n);
    t.Resize( minDim, 1 );

    const Int iOff = ( n>=m ? 0   : m-n );
    const Int jOff = ( n>=m ? n-m : 0   );

    Matrix<F> z01;
    for( Int k=minDim-1; k>=0; --k )
    {
        const Int ki = k + iOff;
        const Int kj = k + jOff;
        auto a10     = ViewRange( A, ki, 0,  ki+1, kj   );
        auto alpha11 = ViewRange( A, ki, kj, ki+1, kj+1 );
        auto A0L     = ViewRange( A, 0,  0,  ki,   kj+1 );
        auto a1L     = ViewRange( A, ki, 0,  ki+1, kj+1 );

        // Find tau and v such that
        //  |a10 alpha11| /I - tau |v^T| |conj(v) 1|\ = |0 beta|
        //                \        |1  |            /
        const F tau = RightReflector( alpha11, a10 );
        t.Set( k, 0, tau );

        // Temporarily set a1L = | v 1 |
        const F alpha = alpha11.Get(0,0);
        alpha11.Set(0,0,1);

        // A2R := A2R Hous(a1L^T,tau)
        //      = A2R (I - tau a1L^T conj(a1L))
        //      = A2R - tau (A2R a1L^T) conj(a1L)
        Zeros( z01, A0L.Height(), 1 );
        Gemv( NORMAL, F(1), A0L, a1L, F(0), z01 );
        Ger( -tau, z01, a1L, A0L );

        // Reset alpha11's value
        alpha11.Set(0,0,alpha);
    }
}

template<typename F> 
inline void
PanelHouseholder( Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("rq::PanelHouseholder"))
    Matrix<F> t;
    PanelHouseholder( A, t );
}

template<typename F> 
inline void
PanelHouseholder( DistMatrix<F>& A, DistMatrix<F,MD,STAR>& t )
{
    DEBUG_ONLY(
        CallStackEntry cse("rq::PanelHouseholder");
        if( A.Grid() != t.Grid() )
            LogicError("{A,t} must be distributed over the same grid");
        if( !A.DiagonalAlignedWith( t, A.Width()-A.Height() ) ) 
            LogicError("t must be aligned with A's main diagonal");
    )
    const Int m = A.Height();
    const Int n = A.Width();
    const Int minDim = Min(m,n);
    t.Resize( minDim, 1 );

    const Int iOff = ( n>=m ? 0   : m-n );
    const Int jOff = ( n>=m ? n-m : 0   );

    const Grid& g = A.Grid();
    DistMatrix<F,STAR,MR  > a1L_STAR_MR(g);
    DistMatrix<F,MC,  STAR> z01_MC_STAR(g);

    for( Int k=minDim-1; k>=0; --k )
    {
        const Int ki = k + iOff;
        const Int kj = k + jOff;
        auto a10     = ViewRange( A, ki, 0,  ki+1, kj   );
        auto alpha11 = ViewRange( A, ki, kj, ki+1, kj+1 );
        auto A0L     = ViewRange( A, 0,  0,  ki,   kj+1 );
        auto a1L     = ViewRange( A, ki, 0,  ki+1, kj+1 );

        // Find tau and v such that
        //  |a10 alpha11| /I - tau |v^T| |conj(v) 1|\ = |0 beta|
        //                \        |1  |            /
        const F tau = RightReflector( alpha11, a10 );
        t.Set( k, 0, tau );

        // Temporarily set a1L = | v 1 |
        F alpha = 0;
        if( alpha11.IsLocal(0,0) )
        {
            alpha = alpha11.GetLocal(0,0);
            alpha11.SetLocal(0,0,1);
        }

        // A2R := A2R Hous(a1L^T,tau)
        //      = A2R (I - tau a1L^T conj(a1L))
        //      = A2R - tau (A2R a1L^T) conj(a1L)
        a1L_STAR_MR = a1L;
        Zeros( z01_MC_STAR, A0L.Height(), 1 );
        LocalGemv( NORMAL, F(1), A0L, a1L_STAR_MR, F(0), z01_MC_STAR );
        z01_MC_STAR.SumOver( A0L.RowComm() );
        Ger
        ( -tau, z01_MC_STAR.LockedMatrix(), a1L_STAR_MR.LockedMatrix(),
          A0L.Matrix() ); 

        // Reset alpha11's value
        if( alpha11.IsLocal(0,0) )
            alpha11.SetLocal(0,0,alpha);
    }
}

template<typename F> 
inline void
PanelHouseholder( DistMatrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("rq::PanelHouseholder"))
    DistMatrix<F,MD,STAR> t(A.Grid());
    PanelHouseholder( A, t );
}

} // namespace rq
} // namespace elem

#endif // ifndef ELEM_RQ_PANEL_HPP

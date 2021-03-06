/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   Copyright (c) 2013, The University of Texas at Austin
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_TRMM_RLT_HPP
#define ELEM_TRMM_RLT_HPP

#include ELEM_AXPY_INC
#include ELEM_MAKETRIANGULAR_INC
#include ELEM_SCALE_INC
#include ELEM_SETDIAGONAL_INC
#include ELEM_TRANSPOSE_INC

#include ELEM_GEMM_INC

#include ELEM_ZEROS_INC

namespace elem {
namespace internal {

template<typename T>
inline void
LocalTrmmAccumulateRLT
( UnitOrNonUnit diag, T alpha,
  const DistMatrix<T>& L,
  const DistMatrix<T,MR,STAR>& XTrans_MR_STAR,
        DistMatrix<T,MC,STAR>& ZTrans_MC_STAR )
{
    DEBUG_ONLY(
        CallStackEntry cse("internal::LocalTrmmAccumulateRLT");
        if( L.Grid() != XTrans_MR_STAR.Grid() ||
            XTrans_MR_STAR.Grid() != ZTrans_MC_STAR.Grid() )
            LogicError("{L,X,Z} must be distributed over the same grid");
        if( L.Height() != L.Width() ||
            L.Height() != XTrans_MR_STAR.Height() ||
            L.Height() != ZTrans_MC_STAR.Height() ||
            XTrans_MR_STAR.Width() != ZTrans_MC_STAR.Width() )
            LogicError
            ("Nonconformal LocalTrmmAccumulateRLT:\n",
             "  L ~ ",L.Height()," x ",L.Width(),"\n",
             "  X^H/T[MR,* ] ~ ",XTrans_MR_STAR.Height()," x ",
                                 XTrans_MR_STAR.Width(),"\n",
             "  Z^H/T[MC,* ] ~ ",ZTrans_MC_STAR.Height()," x ",
                                 ZTrans_MC_STAR.Width());
        if( XTrans_MR_STAR.ColAlign() != L.RowAlign() ||
            ZTrans_MC_STAR.ColAlign() != L.ColAlign() )
            LogicError("Partial matrix distributions are misaligned");
    )
    const Grid& g = L.Grid();

    // Matrix views
    DistMatrix<T>
        LTL(g), LTR(g),  L00(g), L01(g), L02(g),
        LBL(g), LBR(g),  L10(g), L11(g), L12(g),
                         L20(g), L21(g), L22(g);

    DistMatrix<T> D11(g);

    DistMatrix<T,MR,STAR>
        XTTrans_MR_STAR(g),  X0Trans_MR_STAR(g),
        XBTrans_MR_STAR(g),  X1Trans_MR_STAR(g),
                             X2Trans_MR_STAR(g);

    DistMatrix<T,MC,STAR>
        ZTTrans_MC_STAR(g),  Z0Trans_MC_STAR(g),
        ZBTrans_MC_STAR(g),  Z1Trans_MC_STAR(g),
                             Z2Trans_MC_STAR(g);

    const Int ratio = Max( g.Height(), g.Width() );
    PushBlocksizeStack( ratio*Blocksize() );

    LockedPartitionDownDiagonal
    ( L, LTL, LTR,
         LBL, LBR, 0 );
    LockedPartitionDown
    ( XTrans_MR_STAR, XTTrans_MR_STAR,
                      XBTrans_MR_STAR, 0 );
    PartitionDown
    ( ZTrans_MC_STAR, ZTTrans_MC_STAR,
                      ZBTrans_MC_STAR, 0 );
    while( LTL.Height() < L.Height() )
    {
        LockedRepartitionDownDiagonal
        ( LTL, /**/ LTR,  L00, /**/ L01, L02,
         /*************/ /******************/
               /**/       L10, /**/ L11, L12,
          LBL, /**/ LBR,  L20, /**/ L21, L22 );

        LockedRepartitionDown
        ( XTTrans_MR_STAR,  X0Trans_MR_STAR,
         /***************/ /***************/
                            X1Trans_MR_STAR,
          XBTrans_MR_STAR,  X2Trans_MR_STAR );

        RepartitionDown
        ( ZTTrans_MC_STAR,  Z0Trans_MC_STAR,
         /***************/ /***************/
                            Z1Trans_MC_STAR,
          ZBTrans_MC_STAR,  Z2Trans_MC_STAR );

        D11.AlignWith( L11 );
        //--------------------------------------------------------------------//
        D11 = L11;
        MakeTriangular( LOWER, D11 );
        if( diag == UNIT )
            SetDiagonal( D11, T(1) );
        LocalGemm
        ( NORMAL, NORMAL, alpha, D11, X1Trans_MR_STAR, T(1), Z1Trans_MC_STAR );
        LocalGemm
        ( NORMAL, NORMAL, alpha, L21, X1Trans_MR_STAR, T(1), Z2Trans_MC_STAR );
        //--------------------------------------------------------------------//

        SlideLockedPartitionDownDiagonal
        ( LTL, /**/ LTR,  L00, L01, /**/ L02,
               /**/       L10, L11, /**/ L12,
         /*************/ /******************/
          LBL, /**/ LBR,  L20, L21, /**/ L22 );

        SlideLockedPartitionDown
        ( XTTrans_MR_STAR,   X0Trans_MR_STAR,
                             X1Trans_MR_STAR,
         /****************/ /***************/
          XBTrans_MR_STAR,   X2Trans_MR_STAR );

        SlidePartitionDown
        ( ZTTrans_MC_STAR,  Z0Trans_MC_STAR,
                            Z1Trans_MC_STAR,
         /***************/ /***************/
          ZBTrans_MC_STAR,  Z2Trans_MC_STAR );
    }
    PopBlocksizeStack();
}

template<typename T>
inline void
TrmmRLTA
( Orientation orientation, UnitOrNonUnit diag,
  T alpha, const DistMatrix<T>& L,
                 DistMatrix<T>& X )
{
    DEBUG_ONLY(
        CallStackEntry cse("internal::TrmmRLTA");
        if( L.Grid() != X.Grid() )
            LogicError("{L,X} must be distributed over the same grid");
    )
    const Grid& g = L.Grid();
    const bool conjugate = ( orientation == ADJOINT );

    DistMatrix<T>
        XT(g),  X0(g),
        XB(g),  X1(g),
                X2(g);

    DistMatrix<T,MR,  STAR> X1Trans_MR_STAR(g);
    DistMatrix<T,MC,  STAR> Z1Trans_MC_STAR(g);
    DistMatrix<T,MC,  MR  > Z1Trans(g);
    DistMatrix<T,MR,  MC  > Z1Trans_MR_MC(g);

    X1Trans_MR_STAR.AlignWith( L );
    Z1Trans_MC_STAR.AlignWith( L );

    PartitionDown
    ( X, XT,
         XB, 0 );
    while( XT.Height() < X.Height() )
    {
        RepartitionDown
        ( XT,  X0,
         /**/ /**/
               X1,
          XB,  X2 );

        Z1Trans_MR_MC.AlignWith( X1 );
        //--------------------------------------------------------------------//
        X1.TransposeColAllGather( X1Trans_MR_STAR, conjugate );
        Zeros( Z1Trans_MC_STAR, X1.Width(), X1.Height() );
        LocalTrmmAccumulateRLT
        ( diag, alpha, L, X1Trans_MR_STAR, Z1Trans_MC_STAR );

        Z1Trans.RowSumScatterFrom( Z1Trans_MC_STAR );
        Z1Trans_MR_MC = Z1Trans;
        Transpose( Z1Trans_MR_MC.Matrix(), X1.Matrix(), conjugate );
        //--------------------------------------------------------------------//

        SlidePartitionDown
        ( XT,  X0,
               X1,
         /**/ /**/
          XB,  X2 );
    }
}

template<typename T>
inline void
TrmmRLTC
( Orientation orientation, 
  UnitOrNonUnit diag,
  T alpha, const DistMatrix<T>& L,
                 DistMatrix<T>& X )
{
    DEBUG_ONLY(
        CallStackEntry cse("internal::TrmmRLTC");
        if( L.Grid() != X.Grid() )
            LogicError("L and X must be distributed over the same grid");
        if( orientation == NORMAL )
            LogicError("TrmmRLTC expects an Adjoint/Transpose option");
        if( L.Height() != L.Width() || X.Width() != L.Height() )
            LogicError
            ("Nonconformal TrmmRLTC: \n",
             "  L ~ ",L.Height()," x ",L.Width(),"\n",
             "  X ~ ",X.Height()," x ",X.Width());
    )
    const Grid& g = L.Grid();
    const bool conjugate = ( orientation == ADJOINT );

    // Matrix views
    DistMatrix<T> 
        LTL(g), LTR(g),  L00(g), L01(g), L02(g),
        LBL(g), LBR(g),  L10(g), L11(g), L12(g),
                         L20(g), L21(g), L22(g);

    DistMatrix<T> XL(g), XR(g),
                  X0(g), X1(g), X2(g);

    // Temporary distributions
    DistMatrix<T,MR,  STAR> L10Trans_MR_STAR(g);
    DistMatrix<T,STAR,STAR> L11_STAR_STAR(g);
    DistMatrix<T,VC,  STAR> X1_VC_STAR(g);
    DistMatrix<T,MC,  STAR> D1_MC_STAR(g);

    // Start the algorithm
    Scale( alpha, X );
    LockedPartitionUpDiagonal
    ( L, LTL, LTR,
         LBL, LBR, 0 );
    PartitionLeft( X, XL, XR, 0 );
    while( XL.Width() > 0 )
    {
        LockedRepartitionUpDiagonal
        ( LTL, /**/ LTR,  L00, L01, /**/ L02,
               /**/       L10, L11, /**/ L12,
         /*************/ /******************/
          LBL, /**/ LBR,  L20, L21, /**/ L22 );

        RepartitionLeft
        ( XL,     /**/ XR,
          X0, X1, /**/ X2 );

        L10Trans_MR_STAR.AlignWith( X0 );
        D1_MC_STAR.AlignWith( X1 );
        //--------------------------------------------------------------------//
        X1_VC_STAR = X1;
        L11_STAR_STAR = L11;
        LocalTrmm
        ( RIGHT, LOWER, orientation, diag, T(1), L11_STAR_STAR, X1_VC_STAR );
        X1 = X1_VC_STAR;
 
        L10.TransposeColAllGather( L10Trans_MR_STAR, conjugate );
        LocalGemm( NORMAL, NORMAL, T(1), X0, L10Trans_MR_STAR, D1_MC_STAR );
        X1.RowSumScatterUpdate( T(1), D1_MC_STAR );
        //--------------------------------------------------------------------//

        SlideLockedPartitionUpDiagonal
        ( LTL, /**/ LTR,  L00, /**/ L01, L02,
         /*************/ /******************/
               /**/       L10, /**/ L11, L12,
          LBL, /**/ LBR,  L20, /**/ L21, L22 );

        SlidePartitionLeft
        ( XL, /**/     XR,
          X0, /**/ X1, X2 );
    }
}

// Right Lower Adjoint/Transpose (Non)Unit Trmm
//   X := X tril(L)^T,
//   X := X tril(L)^H,
//   X := X trilu(L)^T, or
//   X := X trilu(L)^H
template<typename T>
inline void
TrmmRLT
( Orientation orientation, 
  UnitOrNonUnit diag,
  T alpha, const DistMatrix<T>& L,
                 DistMatrix<T>& X )
{
    DEBUG_ONLY(CallStackEntry cse("internal::TrmmRLT"))
    // TODO: Come up with a better routing mechanism
    if( L.Height() > 5*X.Height() )
        TrmmRLTA( orientation, diag, alpha, L, X );
    else
        TrmmRLTC( orientation, diag, alpha, L, X );
}

} // namespace internal
} // namespace elem

#endif // ifndef ELEM_TRMM_RLT_HPP

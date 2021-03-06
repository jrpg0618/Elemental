/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_TRSM_RLT_HPP
#define ELEM_TRSM_RLT_HPP

#include ELEM_GEMM_INC

namespace elem {
namespace internal {

// Right Lower (Conjugate)Transpose (Non)Unit Trsm
//   X := X tril(L)^-T,
//   X := X tril(L)^-H,
//   X := X trilu(L)^-T, or
//   X := X trilu(L)^-H
template<typename F>
inline void
TrsmRLT
( Orientation orientation, UnitOrNonUnit diag,
  F alpha, const DistMatrix<F>& L, DistMatrix<F>& X,
  bool checkIfSingular )
{
    DEBUG_ONLY(
        CallStackEntry cse("internal::TrsmRLT");
        if( orientation == NORMAL )
            LogicError("TrsmRLT expects a (Conjugate)Transpose option");
    )
    const Grid& g = L.Grid();

    // Matrix views
    DistMatrix<F> 
        LTL(g), LTR(g),  L00(g), L01(g), L02(g),
        LBL(g), LBR(g),  L10(g), L11(g), L12(g),
                         L20(g), L21(g), L22(g);

    DistMatrix<F> XL(g), XR(g),
                  X0(g), X1(g), X2(g);

    // Temporary distributions
    DistMatrix<F,STAR,STAR> L11_STAR_STAR(g);
    DistMatrix<F,VR,  STAR> L21_VR_STAR(g);
    DistMatrix<F,STAR,MR  > L21Trans_STAR_MR(g);
    DistMatrix<F,VC,  STAR> X1_VC_STAR(g);
    DistMatrix<F,STAR,MC  > X1Trans_STAR_MC(g);

    // Start the algorithm
    Scale( alpha, X );
    LockedPartitionDownDiagonal
    ( L, LTL, LTR,
         LBL, LBR, 0 );
    PartitionRight( X, XL, XR, 0 );
    while( XR.Width() > 0 )
    {
        LockedRepartitionDownDiagonal
        ( LTL, /**/ LTR,  L00, /**/ L01, L02,
         /*************/ /******************/
               /**/       L10, /**/ L11, L12,
          LBL, /**/ LBR,  L20, /**/ L21, L22 );

        RepartitionRight
        ( XL, /**/     XR,
          X0, /**/ X1, X2 );

        X1_VC_STAR.AlignWith( X2 );
        X1Trans_STAR_MC.AlignWith( X2 );
        L21_VR_STAR.AlignWith( X2 );
        L21Trans_STAR_MR.AlignWith( X2 );
        //--------------------------------------------------------------------//
        L11_STAR_STAR = L11; 
        X1_VC_STAR = X1;  
        
        LocalTrsm
        ( RIGHT, LOWER, orientation, diag, 
          F(1), L11_STAR_STAR, X1_VC_STAR, checkIfSingular );

        X1_VC_STAR.TransposePartialColAllGather( X1Trans_STAR_MC );
        X1.TransposeRowFilterFrom( X1Trans_STAR_MC );
        L21_VR_STAR = L21;
        L21_VR_STAR.TransposePartialColAllGather
        ( L21Trans_STAR_MR, (orientation==ADJOINT) ); 

        // X2[MC,MR] -= X1[MC,*] (L21[MR,*])^(T/H)
        //            = X1^T[* ,MC] (L21^(T/H))[*,MR]
        LocalGemm
        ( TRANSPOSE, NORMAL, 
          F(-1), X1Trans_STAR_MC, L21Trans_STAR_MR, F(1), X2 );
        //--------------------------------------------------------------------//

        SlideLockedPartitionDownDiagonal
        ( LTL, /**/ LTR,  L00, L01, /**/ L02,
               /**/       L10, L11, /**/ L12,
         /*************/ /******************/
          LBL, /**/ LBR,  L20, L21, /**/ L22 );

        SlidePartitionRight
        ( XL,     /**/ XR,
          X0, X1, /**/ X2 );
    }
}

} // namespace internal
} // namespace elem

#endif // ifndef ELEM_TRSM_RLT_HPP

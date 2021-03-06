/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_DETERMINANT_CHOLESKY_HPP
#define ELEM_DETERMINANT_CHOLESKY_HPP

#include ELEM_CHOLESKY_INC

namespace elem {
namespace hpd_det {

template<typename F>
inline SafeProduct<F> 
AfterCholesky( UpperOrLower uplo, const Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("hpd_det::AfterCholesky"))
    typedef Base<F> R;
    const Int n = A.Height();

    Matrix<F> d;
    A.GetDiagonal( d );
    SafeProduct<F> det( n );
    det.rho = F(1);

    const R scale = R(n)/R(2);
    for( Int i=0; i<n; ++i )
    {
        const R delta = RealPart(d.Get(i,0));
        det.kappa += Log(delta)/scale;
    }

    return det;
}

template<typename F>
inline SafeProduct<F> 
Cholesky( UpperOrLower uplo, Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("hpd_det::Cholesky"))
    SafeProduct<F> det( A.Height() );
    try
    {
        elem::Cholesky( uplo, A );
        det = hpd_det::AfterCholesky( uplo, A );
    }
    catch( NonHPDMatrixException& e )
    {
        det.rho = 0;
        det.kappa = 0;
    }
    return det;
}

template<typename F> 
inline SafeProduct<F> 
AfterCholesky( UpperOrLower uplo, const DistMatrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("hpd_det::AfterCholesky"))
    typedef Base<F> R;
    const Int n = A.Height();
    const Grid& g = A.Grid();

    DistMatrix<F,MD,STAR> d(g);
    A.GetDiagonal( d );
    R localKappa = 0; 
    if( d.Participating() )
    {
        const R scale = R(n)/R(2);
        const Int nLocalDiag = d.LocalHeight();
        for( Int iLoc=0; iLoc<nLocalDiag; ++iLoc )
        {
            const R delta = RealPart(d.GetLocal(iLoc,0));
            localKappa += Log(delta)/scale;
        }
    }
    SafeProduct<F> det( n );
    det.kappa = mpi::AllReduce( localKappa, g.VCComm() );
    det.rho = F(1);

    return det;
}

template<typename F> 
inline SafeProduct<F> 
Cholesky( UpperOrLower uplo, DistMatrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("hpd_det::Cholesky"))
    SafeProduct<F> det( A.Height() );
    try
    {
        elem::Cholesky( uplo, A );
        det = hpd_det::AfterCholesky( uplo, A );
    }
    catch( NonHPDMatrixException& e )
    {
        det.rho = 0;
        det.kappa = 0;
    }
    return det;
}

} // namespace hpd_det
} // namespace elem

#endif // ifndef ELEM_DETERMINANT_CHOLESKY_HPP

/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_NORM_MAX_HPP
#define ELEM_NORM_MAX_HPP

namespace elem {

template<typename F> 
inline BASE(F)
MaxNorm( const Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("MaxNorm"))
    typedef Base<F> Real;
    Real maxAbs = 0;
    const Int height = A.Height();
    const Int width = A.Width();
    for( Int j=0; j<width; ++j )
    {
        for( Int i=0; i<height; ++i )
        {
            const Real thisAbs = Abs(A.Get(i,j));
            maxAbs = std::max( maxAbs, thisAbs );
        }
    }
    return maxAbs;
}

template<typename F>
inline BASE(F)
HermitianMaxNorm( UpperOrLower uplo, const Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("HermitianMaxNorm"))
    if( A.Height() != A.Width() )
        LogicError("Hermitian matrices must be square.");

    typedef Base<F> Real;
    Real maxAbs = 0;
    const Int height = A.Height();
    const Int width = A.Width();
    if( uplo == UPPER )
    {
        for( Int j=0; j<width; ++j )
        {
            for( Int i=0; i<=j; ++i )
            {
                const Real thisAbs = Abs(A.Get(i,j));
                maxAbs = std::max( maxAbs, thisAbs );
            }
        }
    }
    else
    {
        for( Int j=0; j<width; ++j )
        {
            for( Int i=j; i<height; ++i )
            {
                const Real thisAbs = Abs(A.Get(i,j));
                maxAbs = std::max( maxAbs, thisAbs );
            }
        }
    }
    return maxAbs;
}

template<typename F>
inline BASE(F)
SymmetricMaxNorm( UpperOrLower uplo, const Matrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("SymmetricMaxNorm"))
    return HermitianMaxNorm( uplo, A );
}

template<typename F,Dist U,Dist V>
inline BASE(F)
MaxNorm( const DistMatrix<F,U,V>& A )
{
    DEBUG_ONLY(CallStackEntry cse("MaxNorm"))
    typedef Base<F> Real;
    Real norm=0;
    if( A.Participating() )
    {
        Real localMaxAbs = 0;
        const Int localHeight = A.LocalHeight();
        const Int localWidth = A.LocalWidth();
        for( Int jLoc=0; jLoc<localWidth; ++jLoc )
        {
            for( Int iLoc=0; iLoc<localHeight; ++iLoc )
            {
                const Real thisAbs = Abs(A.GetLocal(iLoc,jLoc));
                localMaxAbs = std::max( localMaxAbs, thisAbs );
            }
        }
        norm = mpi::AllReduce( localMaxAbs, mpi::MAX, A.DistComm() );
    }
    mpi::Broadcast( norm, A.Root(), A.CrossComm() );
    return norm;
}

template<typename F>
inline BASE(F)
HermitianMaxNorm( UpperOrLower uplo, const DistMatrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("HermitianMaxNorm"))
    if( A.Height() != A.Width() )
        LogicError("Hermitian matrices must be square.");

    const Int r = A.Grid().Height();
    const Int c = A.Grid().Width();
    const Int colShift = A.ColShift();
    const Int rowShift = A.RowShift();

    typedef Base<F> Real;
    Real localMaxAbs = 0;
    const Int localWidth = A.LocalWidth();
    if( uplo == UPPER )
    {
        for( Int jLoc=0; jLoc<localWidth; ++jLoc )
        {
            Int j = rowShift + jLoc*c;
            Int numUpperRows = Length(j+1,colShift,r);
            for( Int iLoc=0; iLoc<numUpperRows; ++iLoc )
            {
                const Real thisAbs = Abs(A.GetLocal(iLoc,jLoc));
                localMaxAbs = std::max( localMaxAbs, thisAbs );
            }
        }
    }
    else
    {
        for( Int jLoc=0; jLoc<localWidth; ++jLoc )
        {
            Int j = rowShift + jLoc*c;
            Int numStrictlyUpperRows = Length(j,colShift,r);
            for( Int iLoc=numStrictlyUpperRows;
                 iLoc<A.LocalHeight(); ++iLoc )
            {
                const Real thisAbs = Abs(A.GetLocal(iLoc,jLoc));
                localMaxAbs = std::max( localMaxAbs, thisAbs );
            }
        }
    }

    return mpi::AllReduce( localMaxAbs, mpi::MAX, A.Grid().VCComm() );
}

template<typename F>
inline BASE(F)
SymmetricMaxNorm( UpperOrLower uplo, const DistMatrix<F>& A )
{
    DEBUG_ONLY(CallStackEntry cse("SymmetricMaxNorm"))
    return HermitianMaxNorm( uplo, A );
}

} // namespace elem

#endif // ifndef ELEM_NORM_MAX_HPP

/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_IDENTITY_HPP
#define ELEM_IDENTITY_HPP

#include ELEM_ZERO_INC

namespace elem {

template<typename T> 
inline void
MakeIdentity( Matrix<T>& I )
{
    DEBUG_ONLY(CallStackEntry cse("MakeIdentity"))
    Zero( I );
    const Int m = I.Height();
    const Int n = I.Width();
    for( Int j=0; j<std::min(m,n); ++j )
        I.Set( j, j, T(1) );
}

template<typename T,Dist U,Dist V>
inline void
MakeIdentity( DistMatrix<T,U,V>& I )
{
    DEBUG_ONLY(CallStackEntry cse("MakeIdentity"))
    Zero( I.Matrix() );

    const Int localHeight = I.LocalHeight();
    const Int localWidth = I.LocalWidth();
    const Int colShift = I.ColShift();
    const Int rowShift = I.RowShift();
    const Int colStride = I.ColStride();
    const Int rowStride = I.RowStride();
    for( Int jLoc=0; jLoc<localWidth; ++jLoc )
    {
        const Int j = rowShift + jLoc*rowStride;
        for( Int iLoc=0; iLoc<localHeight; ++iLoc )
        {
            const Int i = colShift + iLoc*colStride;
            if( i == j )
                I.SetLocal( iLoc, jLoc, T(1) );
        }
    }
}

template<typename T>
inline void
Identity( Matrix<T>& I, Int m, Int n )
{
    DEBUG_ONLY(CallStackEntry cse("Identity"))
    I.Resize( m, n );
    MakeIdentity( I );
}

#ifndef SWIG
template<typename T>
inline Matrix<T>
Identity( Int m, Int n )
{
    Matrix<T> I( m, n );
    MakeIdentity( I );
    return I;
}
#endif

template<typename T,Dist U,Dist V>
inline void
Identity( DistMatrix<T,U,V>& I, Int m, Int n )
{
    DEBUG_ONLY(CallStackEntry cse("Identity"))
    I.Resize( m, n );
    MakeIdentity( I );
}

#ifndef SWIG
template<typename T,Dist U=MC,Dist V=MR>
inline DistMatrix<T,U,V>
Identity( const Grid& g, Int m, Int n )
{
    DistMatrix<T,U,V> I( m, n, g );
    MakeIdentity( I );
    return I;
}
#endif

} // namespace elem

#endif // ifndef ELEM_IDENTITY_HPP

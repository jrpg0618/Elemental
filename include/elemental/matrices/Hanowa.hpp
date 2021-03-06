/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_HANOWA_HPP
#define ELEM_HANOWA_HPP

#include "./Diagonal.hpp"

namespace elem {

template<typename T>
inline void
Hanowa( Matrix<T>& A, Int n, T mu )
{
    DEBUG_ONLY(CallStackEntry cse("Hanowa"))
    if( n % 2 != 0 )
        LogicError("n must be an even integer");
    A.Resize( n, n );
    const Int m = n/2;
    std::vector<T> d(m);

    for( Int j=0; j<m; ++j )
        d[j] = mu;
    auto ABlock = View( A, 0, 0, m, m );
    Diagonal( ABlock, d );
    ABlock = View( A, m, m, m, m );
    Diagonal( ABlock, d );

    for( Int j=0; j<m; ++j )
        d[j] = -(j+1);
    ABlock = View( A, 0, m, m, m );
    Diagonal( ABlock, d );

    for( Int j=0; j<m; ++j )
        d[j] = j+1;
    ABlock = View( A, m, 0, m, m );
    Diagonal( ABlock, d );
}

#ifndef SWIG
template<typename T>
inline Matrix<T>
Hanowa( Int n, T mu )
{
    Matrix<T> A;
    Hanowa( A, n, mu );
    return A;
}
#endif

template<typename T,Dist U,Dist V>
inline void
Hanowa( DistMatrix<T,U,V>& A, Int n, T mu )
{
    DEBUG_ONLY(CallStackEntry cse("Hanowa"))
    if( n % 2 != 0 )
        LogicError("n must be an even integer");
    A.Resize( n, n );
    const Int m = n/2;
    std::vector<T> d(m);

    for( Int j=0; j<m; ++j )
        d[j] = mu;
    auto ABlock = View( A, 0, 0, m, m );
    Diagonal( ABlock, d );
    ABlock = View( A, m, m, m, m );
    Diagonal( ABlock, d );

    for( Int j=0; j<m; ++j )
        d[j] = -(j+1);
    ABlock = View( A, 0, m, m, m );
    Diagonal( ABlock, d );

    for( Int j=0; j<m; ++j )
        d[j] = j+1;
    ABlock = View( A, m, 0, m, m );
    Diagonal( ABlock, d );
}

#ifndef SWIG
template<typename T,Dist U=MC,Dist V=MR>
inline DistMatrix<T,U,V>
Hanowa( const Grid& g, Int n, T mu )
{
    DistMatrix<T,U,V> A(g);
    Hanowa( A, n, mu );
    return A;
}
#endif

// TODO: MakeHanowa?

} // namespace elem

#endif // ifndef ELEM_HANOWA_HPP

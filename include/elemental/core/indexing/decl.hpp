/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_INDEXING_DECL_HPP
#define ELEM_INDEXING_DECL_HPP

namespace elem {

Int LastOffset( Int n, Int bsize );

Int DiagonalLength( Int height, Int width, Int offset=0 );

Int GCD( Int a, Int b ); 
Int GCD_( Int a, Int b ); 

Int Length( Int n, Int shift, Int numProcs );
Int Length_( Int n, Int shift, Int numProcs );

Int Length( Int n, Int rank, Int firstRank, Int numProcs );
Int Length_( Int n, Int rank, Int firstRank, Int numProcs );

Int MaxLength( Int n, Int numProcs );
Int MaxLength_( Int n, Int numProcs );

Int Shift( Int rank, Int firstRank, Int numProcs );
Int Shift_( Int rank, Int firstRank, Int numProcs );

Unsigned Log2( Unsigned n );
bool PowerOfTwo( Unsigned n );

} // namespace elem

#endif // ifndef ELEM_INDEXING_DECL_HPP

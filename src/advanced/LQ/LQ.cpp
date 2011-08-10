/*
   Copyright (c) 2009-2011, Jack Poulson
   All rights reserved.

   This file is part of Elemental.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

    - Redistributions of source code must retain the above copyright notice,
      this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    - Neither the name of the owner nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/
#include "elemental/basic_internal.hpp"
#include "elemental/advanced_internal.hpp"
using namespace std;
using namespace elemental;

// On exit, the upper triangle of A is overwritten by L, and the Householder
// transforms that determine Q are stored above the diagonal of A with an 
// implicit one on the diagonal. 
//
// In the complex case, the column-vector s stores the unit-magnitude complex 
// rotations that map the norms of the implicit Householder vectors to their
// coefficient:  
//                tau_j = 2 psi_j / ( u_j^H u_j ),
// where psi_j is the j'th entry of s and u_j is the j'th unscaled Householder
// reflector.

template<typename R> // representation of a real number
void
elemental::advanced::LQ
( DistMatrix<R,MC,MR>& A )
{
#ifndef RELEASE
    PushCallStack("advanced::LQ");
#endif
    // TODO
    throw std::logic_error("This routine is not yet written.");
#ifndef RELEASE
    PopCallStack();
#endif
}

#ifndef WITHOUT_COMPLEX
template<typename R> // representation of a real number
void
elemental::advanced::LQ
( DistMatrix<complex<R>,MC,  MR  >& A, 
  DistMatrix<complex<R>,Star,Star>& t )
{
#ifndef RELEASE
    PushCallStack("advanced::LQ");
    if( A.Grid() != t.Grid() )
        throw logic_error( "A and s must be distributed over the same grid." );
#endif
    // TODO
    throw std::logic_error("This routine is not yet written.");
#ifndef RELEASE
    PopCallStack();
#endif
}
#endif

template void
elemental::advanced::LQ
( DistMatrix<float,MC,MR>& A );

template void
elemental::advanced::LQ
( DistMatrix<double,MC,MR>& A );

#ifndef WITHOUT_COMPLEX
template void
elemental::advanced::LQ
( DistMatrix<scomplex,MC,  MR  >& A,
  DistMatrix<scomplex,Star,Star>& t );

template void
elemental::advanced::LQ
( DistMatrix<dcomplex,MC,  MR  >& A,
  DistMatrix<dcomplex,Star,Star>& t );
#endif

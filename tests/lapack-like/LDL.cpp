/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
// NOTE: It is possible to simply include "elemental.hpp" instead
#include "elemental-lite.hpp"
#include ELEM_DIAGONALSCALE_INC
#include ELEM_HEMM_INC
#include ELEM_SYMM_INC
#include ELEM_TRMM_INC
#include ELEM_LDL_INC
#include ELEM_FROBENIUSNORM_INC
#include ELEM_INFINITYNORM_INC
#include ELEM_ONENORM_INC
#include ELEM_HERMITIANUNIFORMSPECTRUM_INC
using namespace std;
using namespace elem;

template<typename F> 
void TestCorrectness
( bool conjugated, bool print, 
  const DistMatrix<F>& A,
  const DistMatrix<F,MD,STAR>& dSub,
  const DistMatrix<Int,VC,STAR>& p,
  const DistMatrix<F>& AOrig )
{
    typedef Base<F> Real;
    const Grid& g = A.Grid();
    const Int m = AOrig.Height();

    DistMatrix<F> X(g), Y(g);
    Uniform( X, m, 100 );
    Y = X;

    // Test correctness by comparing the application of AOrig against a 
    // random set of 100 vectors to the application of tril(A) tril(A)^H
    if( print )
        Print( X, "X" );
    ldl::MultiplyAfter( A, dSub, p, Y, conjugated );
    if( print )
        Print( Y, "P' L B L' P X" );
    Symm( LEFT, LOWER, F(-1), AOrig, X, F(1), Y, conjugated );
    if( print )
        Print( Y, "P' L B L' P X - A X" );
    const Real oneNormOfError = OneNorm( Y );
    const Real infNormOfError = InfinityNorm( Y );
    const Real frobNormOfError = FrobeniusNorm( Y );
    const Real infNormOfA = HermitianInfinityNorm( LOWER, AOrig );
    const Real frobNormOfA = HermitianFrobeniusNorm( LOWER, AOrig );
    const Real oneNormOfX = OneNorm( X );
    const Real infNormOfX = InfinityNorm( X );
    const Real frobNormOfX = FrobeniusNorm( X );
    if( g.Rank() == 0 )
    {
        cout << "||A||_1 = ||A||_oo   = " << infNormOfA << "\n"
             << "||A||_F              = " << frobNormOfA << "\n"
             << "||X||_1              = " << oneNormOfX << "\n"
             << "||X||_oo             = " << infNormOfX << "\n"
             << "||X||_F              = " << frobNormOfX << "\n"
             << "||A X - L D L^[T/H] X||_1  = " << oneNormOfError << "\n"
             << "||A X - L D L^[T/H] X||_oo = " << infNormOfError << "\n"
             << "||A X - L D L^[T/H] X||_F  = " << frobNormOfError << endl;
    }
}

template<typename F> 
void TestLDL
( bool conjugated, bool testCorrectness, bool print, 
  Int m, const Grid& g )
{
    DistMatrix<F> A(g), AOrig(g);
    if( conjugated )
        HermitianUniformSpectrum( A, m, -100, 100 );
    else
        Uniform( A, m, m );
    if( testCorrectness )
    {
        if( g.Rank() == 0 )
        {
            cout << "  Making copy of original matrix...";
            cout.flush();
        }
        AOrig = A;
        if( g.Rank() == 0 )
            cout << "DONE" << endl;
    }
    if( print )
        Print( A, "A" );

    if( g.Rank() == 0 )
    {
        cout << "  Starting LDL^[T/H] factorization...";
        cout.flush();
    }
    mpi::Barrier( g.Comm() );
    const double startTime = mpi::Time();
    DistMatrix<F,MD,STAR> dSub(g);
    DistMatrix<Int,VC,STAR> p(g);
    if( conjugated )
        LDLH( A, dSub, p );
    else
        LDLT( A, dSub, p );
    mpi::Barrier( g.Comm() );
    const double runTime = mpi::Time() - startTime;
    const double realGFlops = 1./3.*Pow(double(m),3.)/(1.e9*runTime);
    const double gFlops = ( IsComplex<F>::val ? 4*realGFlops : realGFlops );
    if( g.Rank() == 0 )
    {
        cout << "DONE.\n"
             << "  Time = " << runTime << " seconds. GFlops = " 
             << gFlops << endl;
    }
    if( print )
        Print( A, "A after factorization" );
    if( testCorrectness )
        TestCorrectness( conjugated, print, A, dSub, p, AOrig );
}

int 
main( int argc, char* argv[] )
{
    Initialize( argc, argv );
    mpi::Comm comm = mpi::COMM_WORLD;
    const Int commRank = mpi::CommRank( comm );
    const Int commSize = mpi::CommSize( comm );

    try
    {
        Int r = Input("--gridHeight","process grid height",0);
        const Int m = Input("--height","height of matrix",100);
        const Int nb = Input("--nb","algorithmic blocksize",96);
        const Int nbLocal = Input("--nbLocal","local blocksize",32);
        const bool conjugated = Input("--conjugate","conjugate LDL?",false);
        const bool testCorrectness = Input
            ("--correctness","test correctness?",true);
        const bool print = Input("--print","print matrices?",false);
        ProcessInput();
        PrintInputReport();

        if( r == 0 )
            r = Grid::FindFactor( commSize );
        const Grid g( comm, r );
        SetBlocksize( nb );
        SetLocalTrrkBlocksize<double>( nbLocal );
        SetLocalTrrkBlocksize<Complex<double>>( nbLocal );
        ComplainIfDebug();
        if( commRank == 0 )
            cout << "Will test LDL" << (conjugated?"^H":"^T") << endl;

        if( commRank == 0 )
        {
            cout << "---------------------\n"
                 << "Testing with doubles:\n"
                 << "---------------------" << endl;
        }
        TestLDL<double>( conjugated, testCorrectness, print, m, g );

        if( commRank == 0 )
        {
            cout << "--------------------------------------\n"
                 << "Testing with double-precision complex:\n"
                 << "--------------------------------------" << endl;
        }
        TestLDL<Complex<double>>( conjugated, testCorrectness, print, m, g );
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}

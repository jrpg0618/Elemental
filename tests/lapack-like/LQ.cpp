/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
// NOTE: It is possible to simply include "elemental.hpp" instead
#include "elemental-lite.hpp"
#include ELEM_MAKETRIANGULAR_INC
#include ELEM_LQ_INC
#include ELEM_FROBENIUSNORM_INC
#include ELEM_INFINITYNORM_INC
#include ELEM_ONENORM_INC
#include ELEM_IDENTITY_INC
#include ELEM_UNIFORM_INC
using namespace std;
using namespace elem;

template<typename F> 
void TestCorrectness
( bool print,
  const DistMatrix<F>& A,
  const DistMatrix<F,MD,STAR>& t,
        DistMatrix<F>& AOrig )
{
    typedef Base<F> Real;
    const Grid& g = A.Grid();
    const Int m = A.Height();
    const Int n = A.Width();
    const Int minDim = std::min(m,n);

    if( g.Rank() == 0 )
        cout << "  Testing orthogonality of Q..." << endl;

    // Form Z := Q Q^H as an approximation to identity
    auto Z = Identity<F>( g, m, n );
    lq::ApplyQ( RIGHT, NORMAL, A, t, Z );
    lq::ApplyQ( RIGHT, ADJOINT, A, t, Z );
    auto ZUpper = View( Z, 0, 0, minDim, minDim );

    // Form X := I - Q Q^H
    auto X = Identity<F>( g, minDim, minDim );
    Axpy( F(-1), ZUpper, X );

    Real oneNormOfError = OneNorm( X );
    Real infNormOfError = InfinityNorm( X );
    Real frobNormOfError = FrobeniusNorm( X );
    if( g.Rank() == 0 )
    {
        cout << "    ||Q Q^H - I||_1  = " << oneNormOfError << "\n"
             << "    ||Q Q^H - I||_oo = " << infNormOfError << "\n"
             << "    ||Q Q^H - I||_F  = " << frobNormOfError << endl;
    }

    if( g.Rank() == 0 )
        cout << "  Testing if A = LQ..." << endl;

    // Form L Q
    auto L( A );
    MakeTriangular( LOWER, L );
    lq::ApplyQ( RIGHT, NORMAL, A, t, L );

    // Form L Q - A
    Axpy( F(-1), AOrig, L );
    
    const Real oneNormOfA = OneNorm( AOrig );
    const Real infNormOfA = InfinityNorm( AOrig );
    const Real frobNormOfA = FrobeniusNorm( AOrig );
    oneNormOfError = OneNorm( L );
    infNormOfError = InfinityNorm( L );
    frobNormOfError = FrobeniusNorm( L );
    if( g.Rank() == 0 )
    {
        cout << "    ||A||_1       = " << oneNormOfA << "\n"
             << "    ||A||_oo      = " << infNormOfA << "\n"
             << "    ||A||_F       = " << frobNormOfA << "\n"
             << "    ||A - LQ||_1  = " << oneNormOfError << "\n"
             << "    ||A - LQ||_oo = " << infNormOfError << "\n"
             << "    ||A - LQ||_F  = " << frobNormOfError << endl;
    }
}

template<typename F>
void TestLQ( bool testCorrectness, bool print, Int m, Int n, const Grid& g )
{
    DistMatrix<F> A(g), AOrig(g);
    Uniform( A, m, n );

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
    DistMatrix<F,MD,STAR> t(g);

    if( g.Rank() == 0 )
    {
        cout << "  Starting LQ factorization...";
        cout.flush();
    }
    mpi::Barrier( g.Comm() );
    const double startTime = mpi::Time();
    LQ( A, t );
    mpi::Barrier( g.Comm() );
    const double runTime = mpi::Time() - startTime;
    const double mD = double(m);
    const double nD = double(n);
    const double realGFlops = (8.*mD*mD*nD - 8./3.*mD*mD*mD)/(1.e9*runTime);
    const double gFlops = ( IsComplex<F>::val ? 4*realGFlops : realGFlops );
    if( g.Rank() == 0 )
    {
        cout << "DONE. " << endl
             << "  Time = " << runTime << " seconds. GFlops = " 
             << gFlops << endl;
    }
    if( print )
        Print( A, "A after factorization" );
    if( testCorrectness )
        TestCorrectness( print, A, t, AOrig );
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
        Int r = Input("--gridHeight","height of process grid",0);
        const Int m = Input("--height","height of matrix",100);
        const Int n = Input("--width","width of matrix",100);
        const Int nb = Input("--nb","algorithmic blocksize",96);
        const bool testCorrectness = Input
            ("--correctness","test correctness?",true);
        const bool print = Input("--print","print matrices?",false);
        ProcessInput();
        PrintInputReport();

        if( r == 0 )
            r = Grid::FindFactor( commSize );
        const Grid g( comm, r );
        SetBlocksize( nb );
        ComplainIfDebug();
        if( commRank == 0 )
            cout << "Will test LQ" << endl;

        if( commRank == 0 )
        {
            cout << "---------------------\n"
                 << "Testing with doubles:\n"
                 << "---------------------" << endl;
        }
        TestLQ<double>( testCorrectness, print, m, n, g );

        if( commRank == 0 )
        {
            cout << "--------------------------------------\n"
                 << "Testing with double-precision complex:\n"
                 << "--------------------------------------" << endl;
        }
        TestLQ<Complex<double>>( testCorrectness, print, m, n, g );
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}

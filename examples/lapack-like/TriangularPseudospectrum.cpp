/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
// NOTE: It is possible to simply include "elemental.hpp" instead
#include "elemental-lite.hpp"
#include ELEM_FROBENIUSNORM_INC
#include ELEM_PSEUDOSPECTRUM_INC
#include ELEM_DEMMEL_INC
#include ELEM_FOXLI_INC
#include ELEM_GRCAR_INC
#include ELEM_LOTKIN_INC
#include ELEM_UNIFORM_INC
using namespace std;
using namespace elem;

typedef double Real;
typedef Complex<Real> C;

int
main( int argc, char* argv[] )
{
    Initialize( argc, argv );

    try 
    {
        const Int matType =
            Input("--matType","0:uniform,1:Demmel,2:Lotkin,3:Grcar,4:FoxLi",1);
        const Int n = Input("--size","height of matrix",100);
        const Real realCenter = Input("--realCenter","real center",0.);
        const Real imagCenter = Input("--imagCenter","imag center",0.);
        const Real xWidth = Input("--xWidth","x width of image",0.);
        const Real yWidth = Input("--yWidth","y width of image",0.);
        const Int xSize = Input("--xSize","number of x samples",100);
        const Int ySize = Input("--ySize","number of y samples",100);
        const bool lanczos = Input("--lanczos","use Lanczos?",true);
        const Int krylovSize = Input("--krylovSize","num Lanczos vectors",10);
        const bool reorthog = Input("--reorthog","reorthog basis?",true);
        const bool deflate = Input("--deflate","deflate converged?",true);
        const Int maxIts = Input("--maxIts","maximum two-norm iter's",1000);
        const Real tol = Input("--tol","tolerance for norm estimates",1e-6);
        const Int numBands = Input("--numBands","num bands for Grcar",3);
        const Real omega = Input("--omega","frequency for Fox-Li",16*M_PI);
        const bool progress = Input("--progress","print progress?",true);
        const bool display = Input("--display","display matrices?",false);
        const bool write = Input("--write","write matrices?",false);
        const bool writePseudo = Input("--writePs","write pseudospec.",false);
        const Int formatInt = Input("--format","write format",2);
        const Int colorMapInt = Input("--colorMap","color map",0);
        ProcessInput();
        PrintInputReport();

        if( formatInt < 1 || formatInt >= FileFormat_MAX )
            LogicError("Invalid file format integer, should be in [1,",
                       FileFormat_MAX,")");

        FileFormat format = static_cast<FileFormat>(formatInt);
        ColorMap colorMap = static_cast<ColorMap>(colorMapInt);
        SetColorMap( colorMap );
        C center(realCenter,imagCenter);

        DistMatrix<C> A;
        switch( matType )
        {
        case 0: Uniform( A, n, n ); break;
        case 1: Demmel( A, n ); break;
        case 2: Lotkin( A, n ); break;
        case 3: Grcar( A, n, numBands ); break;
        case 4: FoxLi( A, n, omega ); break;
        default: LogicError("Invalid matrix type");
        }
        MakeTriangular( UPPER, A );
        if( display )
            Display( A, "A" );
        if( write )
            Write( A, "A", format );

        // Visualize the pseudospectrum by evaluating ||inv(A-sigma I)||_2 
        // for a grid of complex sigma's.
        DistMatrix<Real> invNormMap;
        DistMatrix<Int> itCountMap;
        if( xWidth != 0. && yWidth != 0. )
            itCountMap = TriangularPseudospectrum
            ( A, invNormMap, center, xWidth, yWidth, xSize, ySize,
              lanczos, krylovSize, reorthog, deflate, maxIts, tol, progress );
        else
            itCountMap = TriangularPseudospectrum
            ( A, invNormMap, center, xSize, ySize,                
              lanczos, krylovSize, reorthog, deflate, maxIts, tol, progress );
        const Int numIts = MaxNorm( itCountMap );
        if( mpi::WorldRank() == 0 )
            std::cout << "num iterations=" << numIts << std::endl;
        if( display )
        {
            Display( invNormMap, "invNormMap" );
            Display( itCountMap, "itCountMap" );
        }
        if( write || writePseudo )
        {
            Write( invNormMap, "invNormMap", format );
            Write( itCountMap, "itCountMap", format );
        }

        // Take the element-wise log
        const Int mLocal = invNormMap.LocalHeight();
        const Int nLocal = invNormMap.LocalWidth();
        for( Int jLoc=0; jLoc<nLocal; ++jLoc )
            for( Int iLoc=0; iLoc<mLocal; ++iLoc )
                invNormMap.SetLocal
                ( iLoc, jLoc, Log(invNormMap.GetLocal(iLoc,jLoc)) );
        if( display )
        {
            Display( invNormMap, "logInvNormMap" );
            if( GetColorMap() != GRAYSCALE_DISCRETE )
            {       
                auto colorMap = GetColorMap();
                SetColorMap( GRAYSCALE_DISCRETE );
                Display( invNormMap, "discreteLogInvNormMap" );
                SetColorMap( colorMap );
            }
        }
        if( write || writePseudo ) 
        {
            Write( invNormMap, "logInvNormMap", format );
            if( GetColorMap() != GRAYSCALE_DISCRETE )
            {
                auto colorMap = GetColorMap();
                SetColorMap( GRAYSCALE_DISCRETE );
                Write( invNormMap, "discreteLogInvNormMap", format );
                SetColorMap( colorMap );
            }
        }
    }
    catch( exception& e ) { ReportException(e); }

    Finalize();
    return 0;
}

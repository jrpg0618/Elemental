/*
   Copyright (c) 2009-2014, Jack Poulson
                      2013, Jed Brown 
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEM_GRID_DECL_HPP
#define ELEM_GRID_DECL_HPP

namespace elem {

class Grid
{
public:
    explicit Grid( mpi::Comm comm=mpi::COMM_WORLD );
    explicit Grid( mpi::Comm comm, int height );
    ~Grid();

    // Simple interface (simpler version of distributed-based interface)
    int Row() const;           // same as MCRank()
    int Col() const;           // same as MRRank()
    int Rank() const;          // same as VCRank()
    int Height() const;        // same as MCSize()
    int Width() const;         // same as MRSize()
    int Size() const;          // same as VCSize() and VRSize()
    mpi::Comm ColComm() const; // same as MCComm()
    mpi::Comm RowComm() const; // same as MRComm()
    mpi::Comm Comm() const;    // same as VCComm()

    // Distribution-based interface
    int MCRank() const;
    int MRRank() const;
    int VCRank() const;
    int VRRank() const;
    int MCSize() const;
    int MRSize() const;
    int VCSize() const;
    int VRSize() const;
    mpi::Comm MCComm() const;
    mpi::Comm MRComm() const;
    mpi::Comm VCComm() const;
    mpi::Comm VRComm() const;
    mpi::Comm MDComm() const;
    mpi::Comm MDPerpComm() const;

    // Advanced routines
    explicit Grid( mpi::Comm viewers, mpi::Group owners, int height );
    int GCD() const; // greatest common denominator of grid height and width
    int LCM() const; // lowest common multiple of grid height and width
    bool InGrid() const;
    int OwningRank() const;
    int ViewingRank() const;
    int VCToViewingMap( int VCRank ) const;
    mpi::Group OwningGroup() const;
    mpi::Comm OwningComm() const;
    mpi::Comm ViewingComm() const;
    int DiagPath() const;
    int DiagPath( int vectorColRank ) const;
    int DiagPathRank() const;
    int DiagPathRank( int vectorColRank ) const;
    int FirstVCRank( int diagPath ) const;

    static int FindFactor( int p );

private:
    int height_, width_, size_, gcd_;
    int matrixColRank_, matrixRowRank_;
    int vectorColRank_, vectorRowRank_;
    std::vector<int> diagPathsAndRanks_;

    mpi::Comm viewingComm_; // all processes that create the grid
    mpi::Group viewingGroup_;
    int viewingRank_; // our rank in the viewing communicator
    std::vector<int> vectorColToViewingMap_;

    // The processes that do and do not own data
    mpi::Group owningGroup_, notOwningGroup_;

    // Keep track of whether or not our process is in the grid. This is 
    // necessary to avoid calls like MPI_Comm_size when we're not in the
    // communicator's group. Note that we _can_ call MPI_Group_rank when not 
    // in the group and that the result is MPI_UNDEFINED.
    bool inGrid_;

    // Create a communicator for our (not-)owning team
    mpi::Comm owningComm_;
    int owningRank_;

    // These will only be valid if we are in the grid
    mpi::Comm cartComm_,  // the processes that are in the grid
              matrixColComm_, matrixRowComm_,
              matrixDiagComm_, matrixDiagPerpComm_,
              vectorColComm_, vectorRowComm_;

    void SetUpGrid();

    // Disable copying this class due to MPI_Comm/MPI_Group ownership issues
    // and potential performance loss from duplicating MPI communicators, e.g.,
    // on Blue Gene/P there is supposedly a performance loss
    const Grid& operator=( Grid& );
    Grid( const Grid& );
};

bool operator== ( const Grid& A, const Grid& B );
bool operator!= ( const Grid& A, const Grid& B );

// Return a grid constructed using mpi::COMM_WORLD.
const Grid& DefaultGrid();

} // namespace elem

#endif // ifndef ELEM_GRID_DECL_HPP

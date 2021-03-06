/*
   Copyright (c) 2009-2013, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#ifndef ELEMENTAL_CONFIG_H
#define ELEMENTAL_CONFIG_H

/* Build type and version information */
#define GIT_SHA1 "@GIT_SHA1@"
#define Elemental_VERSION_MAJOR "@Elemental_VERSION_MAJOR@"
#define Elemental_VERSION_MINOR "@Elemental_VERSION_MINOR@"
#define CMAKE_BUILD_TYPE "@CMAKE_BUILD_TYPE@"
#cmakedefine RELEASE

/* C compiler info */
#define CMAKE_C_COMPILER    "@CMAKE_C_COMPILER@"
#define MPI_C_COMPILER      "@MPI_C_COMPILER@"
#define MPI_C_INCLUDE_PATH  "@MPI_C_INCLUDE_PATH@"
#define MPI_C_COMPILE_FLAGS "@MPI_C_COMPILE_FLAGS@"
#define MPI_C_LINK_FLAGS    "@MPI_C_LINK_FLAGS@"
#define MPI_C_LIBRARIES     "@MPI_C_LIBRARIES@"

/* C++ compiler info */
#define CMAKE_CXX_COMPILER    "@CMAKE_CXX_COMPILER@"
#define CXX_FLAGS             "@CXX_FLAGS@"
#define MPI_CXX_COMPILER      "@MPI_CXX_COMPILER@"
#define MPI_CXX_INCLUDE_PATH  "@MPI_CXX_INCLUDE_PATH@"
#define MPI_CXX_COMPILE_FLAGS "@MPI_CXX_COMPILE_FLAGS@"
#define MPI_CXX_LINK_FLAGS    "@MPI_CXX_LINK_FLAGS@"
#define MPI_CXX_LIBRARIES     "@MPI_CXX_LIBRARIES@"

/* Math libraries */
#define MATH_LIBS "@MATH_LIBS@"
#cmakedefine BLAS_POST
#cmakedefine LAPACK_POST
#cmakedefine HAVE_FLA_BSVD

/* Basic configuration options */
#define RESTRICT @RESTRICT@
#cmakedefine HAVE_OPENMP
#cmakedefine HAVE_OMP_COLLAPSE
#cmakedefine HAVE_QT5
#cmakedefine HAVE_F90_INTERFACE
#cmakedefine AVOID_COMPLEX_MPI
#cmakedefine HAVE_UNIFORM_INT
#cmakedefine HAVE_UNIFORM_REAL
#cmakedefine HAVE_STEADYCLOCK
#cmakedefine HAVE_MPI_REDUCE_SCATTER_BLOCK
#cmakedefine HAVE_MPI_IN_PLACE
#cmakedefine HAVE_MPI_LONG_LONG
#cmakedefine HAVE_MPI_COMM_SET_ERRHANDLER
#cmakedefine HAVE_MPI_INIT_THREAD
#cmakedefine HAVE_MPI_QUERY_THREAD
#cmakedefine HAVE_MPI3_NONBLOCKING_COLLECTIVES
#cmakedefine HAVE_MPIX_NONBLOCKING_COLLECTIVES
#cmakedefine REDUCE_SCATTER_BLOCK_VIA_ALLREDUCE
#cmakedefine USE_BYTE_ALLGATHERS
#cmakedefine USE_64BIT_INTS

/* Advanced configuration options */
#cmakedefine CACHE_WARNINGS
#cmakedefine UNALIGNED_WARNINGS
#cmakedefine VECTOR_WARNINGS
#cmakedefine POOL_MEMORY
#cmakedefine AVOID_OMP_FMA

#endif /* ELEMENTAL_CONFIG_H */

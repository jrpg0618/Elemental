/*
   Copyright (c) 2009-2014, Jack Poulson
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/
#pragma once
#ifndef ELEMENTAL_HPP
#define ELEMENTAL_HPP

#include "elemental/include-paths.hpp"

#include "elemental/config.h"
#ifdef HAVE_F90_INTERFACE
# include "elemental/FCMangle.h"
#endif

#include "elemental/core.hpp"
#include "elemental/blas-like.hpp"
#include "elemental/lapack-like.hpp"
#include "elemental/convex.hpp"
#include "elemental/control.hpp"
#include "elemental/matrices.hpp"

#include "elemental/io.hpp"

#endif // ifndef ELEMENTAL_HPP

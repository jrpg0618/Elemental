/*
   Copyright (c) 2009-2014, Jack Poulson
                      2013, Michael C. Grant
   All rights reserved.

   This file is part of Elemental and is under the BSD 2-Clause License, 
   which can be found in the LICENSE file in the root directory, or at 
   http://opensource.org/licenses/BSD-2-Clause
*/

%module elem_convex

%include "common.swg"
%import "elem.i"

// Convex optimization
// ===================
 
%include "elemental/convex/LogBarrier.hpp"
%include "elemental/convex/LogDetDiv.hpp"
%include "elemental/convex/SVT.hpp"
%include "elemental/convex/SoftThreshold.hpp"

namespace elem {
OVERLOAD0(LogBarrier)
OVERLOAD0(LogDetDiv)
OVERLOAD0(SVT)
OVERLOAD0(SoftThreshold)
}

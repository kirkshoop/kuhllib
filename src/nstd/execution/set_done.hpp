// nstd/execution/set_done.hpp                                        -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2021 Dietmar Kuehl http://www.dietmar-kuehl.de         
//                                                                       
//  Permission is hereby granted, free of charge, to any person          
//  obtaining a copy of this software and associated documentation       
//  files (the "Software"), to deal in the Software without restriction, 
//  including without limitation the rights to use, copy, modify,        
//  merge, publish, distribute, sublicense, and/or sell copies of        
//  the Software, and to permit persons to whom the Software is          
//  furnished to do so, subject to the following conditions:             
//                                                                       
//  The above copyright notice and this permission notice shall be       
//  included in all copies or substantial portions of the Software.      
//                                                                       
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,      
//  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      
//  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             
//  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          
//  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         
//  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         
//  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        
//  OTHER DEALINGS IN THE SOFTWARE. 
// ----------------------------------------------------------------------------

#ifndef INCLUDED_NSTD_EXECUTION_SET_DONE
#define INCLUDED_NSTD_EXECUTION_SET_DONE

#include "nstd/functional/tag_invoke.hpp"
#include "nstd/utility/forward.hpp"

// ----------------------------------------------------------------------------

namespace nstd::execution::inline customization_points {
    inline constexpr struct set_done_t {
        template <typename Receiver>
        constexpr auto operator()(Receiver&& receiver) const
            noexcept(noexcept(::nstd::tag_invoke(*this, ::nstd::utility::forward<Receiver>(receiver))))
            -> void //-dk:TODO verify if expression-equivalent may mean it can have a [reference?] return type
            requires requires(Receiver&& receiver) {
                ::nstd::tag_invoke(*this, ::nstd::utility::forward<Receiver>(receiver));
            }
        {
            return ::nstd::tag_invoke(*this, ::nstd::utility::forward<Receiver>(receiver));
        }
    } set_done;
}

// ----------------------------------------------------------------------------

#endif

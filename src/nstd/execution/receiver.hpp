// nstd/execution/receiver.hpp                                        -*-C++-*-
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

#ifndef INCLUDED_NSTD_EXECUTION_RECEIVER
#define INCLUDED_NSTD_EXECUTION_RECEIVER

#include "nstd/execution/set_value.hpp"
#include "nstd/execution/set_error.hpp"
#include "nstd/execution/set_done.hpp"
#include "nstd/type_traits/remove_cvref.hpp"
#include "nstd/utility/forward.hpp"
#include "nstd/utility/move.hpp"
#include <concepts>
#include <exception>

// ----------------------------------------------------------------------------

namespace nstd::execution {
    template <typename Receiver, typename Error = ::std::exception_ptr>
    concept receiver
        =  ::std::move_constructible<::nstd::type_traits::remove_cvref_t<Receiver>>
        && ::std::constructible_from<::nstd::type_traits::remove_cvref_t<Receiver>, Receiver>
        && requires(::nstd::type_traits::remove_cvref_t<Receiver>&& rec, Error&& err) {
            { ::nstd::execution::set_done(::nstd::utility::move(rec)) } noexcept;
            { ::nstd::execution::set_error(::nstd::utility::move(rec), ::nstd::utility::forward<Error>(err)) } noexcept;
        }
        ;
}

// ----------------------------------------------------------------------------

#endif

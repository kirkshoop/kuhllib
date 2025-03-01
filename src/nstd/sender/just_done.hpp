// nstd/sender/just_done.hpp                                          -*-C++-*-
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

#ifndef INCLUDED_NSTD_SENDER_JUST_DONE
#define INCLUDED_NSTD_SENDER_JUST_DONE

#include "nstd/execution/sender_base.hpp"
#include "nstd/execution/connect.hpp"
#include "nstd/execution/set_done.hpp"
#include "nstd/execution/start.hpp"
#include "nstd/type_traits/remove_cvref.hpp"
#include "nstd/utility/move.hpp"
#include "nstd/utility/forward.hpp"
#include <exception>

// ----------------------------------------------------------------------------

namespace nstd::net {
    class just_done;
    template <typename Receiver> struct just_done_state;
}

// ----------------------------------------------------------------------------

template <typename Receiver>
struct nstd::net::just_done_state
{
    ::nstd::type_traits::remove_cvref_t<Receiver> d_receiver;

    friend auto tag_invoke(::nstd::execution::start_t, just_done_state& state) noexcept -> void {
        ::nstd::execution::set_done(::nstd::utility::move(state.d_receiver));
    }
};

// ----------------------------------------------------------------------------

class nstd::net::just_done
    : public ::nstd::execution::piped_sender_base
{
public:
    template <template <typename...> class V, template <typename...> class T>
    using value_types = V<T<int>>;
    template <template <typename...> class V>
    using error_types = V<::std::exception_ptr>;
    static constexpr bool sends_done = true;

    template <typename Receiver>
    friend auto tag_invoke(::nstd::execution::connect_t, just_done&&, Receiver&& receiver)
         -> ::nstd::net::just_done_state<Receiver> {
        return ::nstd::net::just_done_state<Receiver>{
            ::nstd::utility::forward<Receiver>(receiver)
            };
    }
};

// ----------------------------------------------------------------------------

#endif
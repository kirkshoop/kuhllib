// nstd/execution/sender_traits.hpp                                   -*-C++-*-
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

#ifndef INCLUDED_NSTD_EXECUTION_SENDER_TRAITS
#define INCLUDED_NSTD_EXECUTION_SENDER_TRAITS

#include "nstd/hidden_names/has_sender_types.hpp"
#include "nstd/hidden_names/derives_from_sender_base.hpp"
#include <concepts>

// ----------------------------------------------------------------------------

namespace nstd::execution {
    template <typename>
    struct sender_traits
    {
        using not_specialized = void;
    };

    template <::nstd::hidden_names::has_sender_types Sender>
    struct sender_traits<Sender>
    {
        template <template <typename...> class Tuple, template <typename...> class Variant>
        using value_types = typename Sender::template value_types<Tuple, Variant>;
        template <template <typename...> class Tuple>
        using error_types = typename Sender::template error_types<Tuple>;
        static constexpr bool sends_done = Sender::sends_done;
    };

    template <::nstd::hidden_names::derives_from_sender_base Sender>
    struct sender_traits<Sender>
    {
    };
}

// ----------------------------------------------------------------------------

#endif

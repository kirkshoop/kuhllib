// nstd/sender/just_error.t.cpp                                       -*-C++-*-
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

#include "nstd/sender/just_error.hpp"
#include "nstd/execution/connect.hpp"
#include "nstd/execution/set_value.hpp"
#include "nstd/execution/set_error.hpp"
#include "nstd/execution/set_done.hpp"
#include "nstd/execution/start.hpp"
#include "kuhl/test.hpp"
#include <optional>

namespace test_declarations {}
namespace TD  = test_declarations;
namespace EX  = ::nstd::execution;
namespace NET = ::nstd::net;
namespace KT  = ::kuhl::test;

// ----------------------------------------------------------------------------

namespace test_declarations {
    struct receiver {
        ::std::optional<int>* ptr;

        friend void tag_invoke(EX::set_value_t, receiver&&, int) noexcept {}
        friend void tag_invoke(EX::set_error_t, receiver&&, std::exception_ptr) noexcept {}
        friend void tag_invoke(EX::set_error_t, receiver&& r, int v) noexcept { *r.ptr = v; }
        friend void tag_invoke(EX::set_done_t, receiver&&) noexcept {}
    };

    struct string_receiver {
        ::std::optional<::std::string>* ptr;

        friend void tag_invoke(EX::set_value_t, string_receiver&&, ::std::string_view) noexcept {}
        friend void tag_invoke(EX::set_error_t, string_receiver&&, std::exception_ptr) noexcept {}
        friend void tag_invoke(EX::set_error_t, string_receiver&&, char const*) noexcept {}
        friend void tag_invoke(EX::set_error_t, string_receiver&& r, ::std::string_view v) noexcept { *r.ptr = v; }
        friend void tag_invoke(EX::set_done_t, string_receiver&&) noexcept {}
    };
}

// ----------------------------------------------------------------------------

static KT::testcase const tests[] = {
    KT::expect_success("just_error usage", []{
            ::std::optional<int> value;
            auto state = EX::connect(NET::just_error(17), TD::receiver{&value});
            EX::start(state);
            return value.value_or(0) == 17
                ;
        }),
    KT::expect_success("just_error specialization", []{
            ::std::optional<::std::string> value;
            auto state = EX::connect(NET::just_error("foo"), TD::string_receiver{&value});
            EX::start(state);
            return value.value_or("") == "foo"
                ;
        }),
};

static KT::add_tests suite("just_error", ::tests);

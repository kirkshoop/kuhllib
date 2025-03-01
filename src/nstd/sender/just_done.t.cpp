// nstd/sender/just_done.t.cpp                                        -*-C++-*-
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

#include "nstd/sender/just_done.hpp"
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
        bool* ptr;
        friend auto tag_invoke(EX::set_value_t, receiver&&, int) noexcept -> void {}
        friend auto tag_invoke(EX::set_error_t, receiver&&, std::exception_ptr) noexcept -> void {}
        friend auto tag_invoke(EX::set_done_t, receiver&& r) noexcept -> void { *r.ptr = true; }
    };
}

static KT::testcase const tests[] = {
    KT::expect_success("just_done usage", []{
            bool value(false);
            auto state = EX::connect(NET::just_done(), TD::receiver{&value});
            EX::start(state);
            return value
                ;
        }),
};

static KT::add_tests suite("just_done", ::tests);

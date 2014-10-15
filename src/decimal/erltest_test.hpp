// erltest_test.hpp                                                   -*-C++-*-
// ----------------------------------------------------------------------------
//  Copyright (C) 2013 Dietmar Kuehl http://www.dietmar-kuehl.de         
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

#ifndef INCLUDED_ERLTEST_TEST
#define INCLUDED_ERLTEST_TEST

#include <iostream>
#include <iomanip>
#include <cstdlib>

// ----------------------------------------------------------------------------

namespace erl
{
    namespace test
    {
        class context;
        class testcase;

        erl::test::testcase
        expect_success(char const*, bool(*)());
        erl::test::testcase
        expect_success(char const*, bool(*)(erl::test::context&));

        erl::test::testcase
        expect_failure(char const*, bool(*)());
        erl::test::testcase
        expect_failure(char const*, bool(*)(erl::test::context&));

        template <typename Exception>
        erl::test::testcase
        expect_exception(char const*, bool(*)());
        template <typename Exception>
        erl::test::testcase
        expect_exception(char const*, bool(*)(erl::test::context&));

        int run_tests(char const*, int, char*[],
                      erl::test::testcase const*, erl::test::testcase const*);
        template <std::size_t NoTests>
        int run_tests(char const*, int, char*[], 
                      erl::test::testcase const (&tests)[NoTests]);

        template <typename T>
        void use(T const&);
    }
}

// ----------------------------------------------------------------------------

class erl::test::context
{
    // for now, this is intentionally left blank
};

class erl::test::testcase
{
private:
    char const* d_name;
    bool (*d_no_context)();
    bool (*d_context)(erl::test::context&);
    bool (testcase::*d_run_test)(std::ostream&, erl::test::context&) const;

    bool expect_success_with_context(std::ostream&      ,
                                     erl::test::context&) const;
    bool expect_success_without_context(std::ostream&      ,
                                        erl::test::context&) const;
    bool expect_failure_with_context(std::ostream&      ,
                                     erl::test::context&) const;
    bool expect_failure_without_context(std::ostream&      ,
                                        erl::test::context&) const;
    template <typename Exception>
    bool expect_exception_with_context(std::ostream&      ,
                                       erl::test::context&) const;
    template <typename Exception>
    bool expect_exception_without_context(std::ostream&      ,
                                          erl::test::context&) const;

public:
    testcase(bool, char const*, bool (*)());
    testcase(bool, char const*, bool (*)(erl::test::context&));
    template <typename Exception>
    testcase(Exception*, char const*, bool (*)());
    template <typename Exception>
    testcase(Exception*, char const*, bool (*)(erl::test::context&));

    bool run(std::ostream&, std::size_t, char const*) const;
};

// ----------------------------------------------------------------------------

inline
erl::test::testcase::testcase(bool success, char const* name, bool (*test)())
    : d_name(name)
    , d_no_context(test)
    , d_context()
    , d_run_test(success
                 ? &testcase::expect_success_without_context
                 : &testcase::expect_failure_without_context)
{
}

inline
erl::test::testcase::testcase(bool success, char const* name,
                              bool (*test)(erl::test::context&))
    : d_name(name)
    , d_no_context()
    , d_context(test)
    , d_run_test(success
                 ? &testcase::expect_success_with_context
                 : &testcase::expect_failure_with_context)
{
}

template <typename Exception>
inline
erl::test::testcase::testcase(Exception*, char const* name, bool (*test)())
    : d_name(name)
    , d_no_context(test)
    , d_context()
    , d_run_test(&testcase::expect_exception_without_context<Exception>)
{
}

template <typename Exception>
inline
erl::test::testcase::testcase(Exception*, char const* name,
                              bool (*test)(erl::test::context&))
    : d_name(name)
    , d_no_context()
    , d_context(test)
    , d_run_test(&testcase::expect_exception_with_context<Exception>)
{
}

// ----------------------------------------------------------------------------

inline
bool
erl::test::testcase::expect_success_with_context(std::ostream&       out,
                                                 erl::test::context& context) const
{
    try {
        bool result(this->d_context(context));
        out << (result? "OK": "fail") << '\n';
        return result;
    }
    catch (...) {
        out << "unexpected exception\n";
        return false;
    }
}

inline
bool
erl::test::testcase::expect_failure_with_context(std::ostream&       out,
                                                 erl::test::context& context) const
{
    try {
        bool result(this->d_context(context));
        out << (result? "OK (unexpected)": "fail (expected)") << '\n';
        return result;
    }
    catch (...) {
        out << "exception (fail expected)\n";
        return false;
    }
}

inline
bool
erl::test::testcase::expect_success_without_context(std::ostream&       out,
                                                    erl::test::context& ) const
{
    try {
        bool result(this->d_no_context());
        out << (result? "OK": "fail") << '\n';
        return result;
    }
    catch (...) {
        out << "unexpected exception\n";
        return false;
    }
}

inline
bool
erl::test::testcase::expect_failure_without_context(std::ostream&       out,
                                                    erl::test::context& ) const
{
    try {
        bool result(this->d_no_context());
        out << (result? "OK (unexpected)": "fail (expected)") << '\n';
        return result;
    }
    catch (...) {
        out << "exception (fail expected)\n";
        return false;
    }
}

template <typename Exception>
inline
bool
erl::test::testcase::expect_exception_with_context(
                                             std::ostream&       out,
                                             erl::test::context& context) const
{
    try {
        bool result(this->d_context(context));
        out << (result
                      ? "fail (OK instead of exception)"
                      : "fail (instead of exception)") << '\n';
        return false;
    }
    catch (Exception const&) {
        out << "OK (received exception)\n";
        return true;
    }
    catch (...) {
        out << "fail (different exception)\n";
        return false;
    }
}

template <typename Exception>
inline
bool
erl::test::testcase::expect_exception_without_context(
                                                    std::ostream&       out,
                                                    erl::test::context& ) const
{
    try {
        bool result(this->d_no_context());
        out << (result
                      ? "fail (OK instead of exception)"
                      : "fail (instead of exception)") << '\n';
        return false;
    }
    catch (Exception const&) {
        out << "OK (received exception)\n";
        return true;
    }
    catch (...) {
        out << "fail (different exception)\n";
        return false;
    }
}

// ----------------------------------------------------------------------------

inline
bool
erl::test::testcase::run(std::ostream& out,
                         std::size_t index,
                         char const* name) const
{
    out << std::right << std::setfill(' ') << std::setw(3) << index << ": "
        << std::left  << std::setw(30) << name
        << std::setw(40) << this->d_name;
    erl::test::context context;
    return (this->*(this->d_run_test))(out, context);
}

// ----------------------------------------------------------------------------

inline
erl::test::testcase
erl::test::expect_success(char const* name, bool (*test)())
{
    return erl::test::testcase(true, name, test);
}

inline
erl::test::testcase
erl::test::expect_success(char const* name, bool (*test)(erl::test::context&))
{
    return erl::test::testcase(true, name, test);
}

inline
erl::test::testcase
erl::test::expect_failure(char const* name, bool (*test)())
{
    return erl::test::testcase(false, name, test);
}

inline
erl::test::testcase
erl::test::expect_failure(char const* name, bool (*test)(erl::test::context&))
{
    return erl::test::testcase(false, name, test);
}

template <typename Exception>
erl::test::testcase
erl::test::expect_exception(char const* name, bool(*test)())
{
    return erl::test::testcase(static_cast<Exception*>(0), name, test);
}

template <typename Exception>
erl::test::testcase
erl::test::expect_exception(char const* name, bool(*test)(erl::test::context&))
{
    return erl::test::testcase(static_cast<Exception*>(0), name, test);
}

// ----------------------------------------------------------------------------
//-dk:TODO: run_tests() should reside in a library
//-dk:TODO: run_tests() should evaluate its argument to decide what to do

inline
int
erl::test::run_tests(char const* name, int, char*[],
                     erl::test::testcase const* begin,
                     erl::test::testcase const* end)
{
    std::ptrdiff_t success(0);
    for (erl::test::testcase const* it(begin); it != end; ++it) {
        it->run(std::cout, it - begin + 1, name) && ++success;
    }
    std::cout << name << ": "
              << success << "/" << (end - begin) << " succeeded\n";
    return success == (end - begin)? EXIT_SUCCESS: EXIT_FAILURE;
}

template <std::size_t NoTests>
int
erl::test::run_tests(char const* name, int ac, char* av[],
                     erl::test::testcase const (&tests)[NoTests])
{
    return erl::test::run_tests(name, ac, av, tests, tests + NoTests);
}

// ----------------------------------------------------------------------------

template <typename T>
void erl::test::use(T const&)
{
}

// ----------------------------------------------------------------------------

#endif

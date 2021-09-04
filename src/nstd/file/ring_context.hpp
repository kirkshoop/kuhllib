// nstd/file/ring_context.hpp                                         -*-C++-*-
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

#ifndef INCLUDED_NSTD_FILE_RING_CONTEXT
#define INCLUDED_NSTD_FILE_RING_CONTEXT

#include "nstd/file/descriptor.hpp"
#include "nstd/file/mapped_memory.hpp"
#include "nstd/file/ring.hpp"
#include <linux/io_uring.h>
#include <chrono>
#include <cstddef>
#include <cstdint>

// ----------------------------------------------------------------------------

namespace nstd::file {
    class ring_context; // I/O context base on io_uring
}

// ----------------------------------------------------------------------------

class nstd::file::ring_context
{
public:
    using scheduler_type = int;
private:
    ::nstd::file::descriptor            d_fd;
    ::nstd::file::mapped_memory         d_smem; // submission ring memory
    ::nstd::file::mapped_memory         d_cmem; // potentially needed completion ring memory
    ::nstd::file::mapped_memory         d_emem; // submission element memory
    ::nstd::file::ring<unsigned int>    d_submission;
    ::nstd::file::ring<::io_uring_cqe>  d_completion;
    ::io_uring_sqe*                     d_submission_elements;
    ::std::atomic<unsigned int>         d_outstanding;
    ::std::atomic<bool>                 d_stopped{false};

    auto intern_submit(::std::size_t) -> void;
    auto process_result() -> ::std::size_t;

public:
    class io_base;
    class run_guard;

    template <typename Op>
    auto submit(Op op) -> void;

    //-dk:TODO class executor_type;
    using count_type = ::std::size_t;
    enum queue_size: int { max = ::std::numeric_limits<int>::max() }; // extension

    ring_context();
    explicit ring_context(queue_size size); // extension
    explicit ring_context(int);
    ring_context(ring_context const&) = delete;
    auto operator=(ring_context const&) -> ring_context& = delete;
    ~ring_context();

    auto setup(queue_size size) -> int; // extension; return better error?
    auto is_setup() const -> bool;      // extension

    //-dk:TODO auto get_executor() noexcept -> ::nstd::file::ring_context::executor_type;

    auto run() -> ::nstd::file::ring_context::count_type;
    template <typename Rep, typename Period>
        auto run_for(::std::chrono::duration<Rep, Period> const&)
            -> ::nstd::file::ring_context::count_type;
    template <typename Clock, typename Duration>
        auto run_until(::std::chrono::time_point<Clock, Duration> const&)
            -> ::nstd::file::ring_context::count_type;
    auto run_one() -> ::nstd::file::ring_context::count_type;
    template <typename Rep, typename Period>
        auto run_one_for(::std::chrono::duration<Rep, Period> const&)
            -> ::nstd::file::ring_context::count_type;
    template <typename Clock, typename Duration>
        auto run_one_until(::std::chrono::time_point<Clock, Duration> const&)
            -> ::nstd::file::ring_context::count_type;

    auto poll() -> ::nstd::file::ring_context::count_type;
    auto poll_one() -> ::nstd::file::ring_context::count_type;
    auto stop() -> void;
    auto stopped() const noexcept -> bool;
    auto restart() -> void;
};

// ----------------------------------------------------------------------------

class nstd::file::ring_context::io_base
{
protected:
    ~io_base() = default;
    virtual auto do_result(::std::int32_t, ::std::uint32_t) -> void = 0;

public:
    auto result(::std::int32_t res, ::std::uint32_t flags) -> void { this->do_result(res, flags); }
};

// ----------------------------------------------------------------------------

template <typename Op>
auto nstd::file::ring_context::submit(Op op) ->void
{
    auto tail(this->d_submission.tail());
    auto index(this->d_submission.mask(tail));
    op(this->d_submission_elements[index]);
    this->d_submission.d_array[index] = index;
    ++this->d_outstanding;
    this->d_submission.advance_tail();
    this->intern_submit(1u);
}

// ----------------------------------------------------------------------------

#endif

//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <system_error>

// class error_category

// const error_category& system_category();

// XFAIL: stdlib=apple-libc++ && target={{.+}}-apple-macosx10.{{9|10|11|12}}
// XFAIL: LIBCXX-PICOLIBC-FIXME

#include <system_error>
#include <cassert>
#include <string>
#include <cerrno>

#include "test_macros.h"

// See https://llvm.org/D65667
struct StaticInit {
    const std::error_category* ec;
    ~StaticInit() {
        std::string str = ec->name();
        assert(str == "system") ;
    }
};
static StaticInit foo;

int main(int, char**)
{
    {
        const std::error_category& e_cat1 = std::system_category();
        std::error_condition e_cond = e_cat1.default_error_condition(5);
        assert(e_cond.value() == 5);
        assert(e_cond.category() == std::generic_category());
        e_cond = e_cat1.default_error_condition(5000);
        assert(e_cond.value() == 5000);
        assert(e_cond.category() == std::system_category());
    }

    // Test the result of message(int cond) when given a bad error condition
    {
            errno = E2BIG; // something that message will never generate
            const std::error_category& e_cat1 = std::system_category();
            const std::string msg = e_cat1.message(-1);
            // Exact message format varies by platform.
#if defined(_AIX)
            LIBCPP_ASSERT(msg.rfind("Error -1 occurred", 0) == 0);
#else
            LIBCPP_ASSERT(msg.rfind("Unknown error", 0) == 0);
#endif
            assert(errno == E2BIG);
    }

    {
        foo.ec = &std::system_category();
        std::string m = foo.ec->name();
        assert(m == "system");
    }

    return 0;
}

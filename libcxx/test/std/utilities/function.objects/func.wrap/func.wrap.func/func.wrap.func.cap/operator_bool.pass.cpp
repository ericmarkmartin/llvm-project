//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

// <functional>

// class function<R(ArgTypes...)>

// explicit operator bool() const

#include <functional>
#include <cassert>

int g(int) {return 0;}

int main()
{
    {
    std::function<int(int)> f;
    assert(!f);
    f = g;
    assert(f);
    }
}

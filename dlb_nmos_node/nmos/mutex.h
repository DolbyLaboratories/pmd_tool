/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Changed condition variable implementation - switched from boost to std library.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (c) 2019-2025, Dolby Laboratories Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef NMOS_MUTEX_H
#define NMOS_MUTEX_H

#include <condition_variable>
#include "bst/shared_mutex.h"

namespace nmos
{
    typedef bst::shared_mutex mutex;

    typedef bst::shared_lock<mutex> read_lock;
    typedef std::unique_lock<mutex> write_lock;

    // locking strategy tag structs must be usable for both read_lock and write_lock
#ifndef BST_SHARED_MUTEX_BOOST
    typedef std::adopt_lock_t adopt_lock_t;
    typedef std::defer_lock_t defer_lock_t;
    typedef std::try_to_lock_t try_to_lock_t;
    using std::adopt_lock;
    using std::defer_lock;
    using std::try_to_lock;
#else
    struct adopt_lock_t : boost::adopt_lock_t, std::adopt_lock_t {};
    struct defer_lock_t : boost::defer_lock_t, std::defer_lock_t {};
    struct try_to_lock_t : boost::try_to_lock_t, std::try_to_lock_t {};
    const adopt_lock_t adopt_lock;
    const defer_lock_t defer_lock;
    const try_to_lock_t try_to_lock;
#endif

    typedef std::condition_variable_any condition_variable;
    typedef std::cv_status cv_status;

    template <typename Func>
    auto with_read_lock(nmos::mutex& mutex, Func&& func) -> decltype(func())
    {
        nmos::read_lock lock(mutex);
        return func();
    }

    template <typename Func>
    auto with_write_lock(nmos::mutex& mutex, Func&& func) -> decltype(func())
    {
        nmos::write_lock lock(mutex);
        return func();
    }
}

#endif

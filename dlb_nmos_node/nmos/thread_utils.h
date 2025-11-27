/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Simplified condition variable wait implementations by removing
 * try-catch exception handling and updated chrono namespace from bst::chrono to std::chrono.
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

#ifndef NMOS_THREAD_UTILS_H
#define NMOS_THREAD_UTILS_H

#include <thread>

namespace nmos
{
    // Utility types, constants and functions extending std functionality (could be extracted from the nmos module)
    namespace details
    {
        // temporarily unlock a mutex
        template <typename BasicLockable>
        class reverse_lock_guard
        {
        public:
            typedef BasicLockable mutex_type;
            explicit reverse_lock_guard(mutex_type& m) : m(m) { m.unlock(); }
            ~reverse_lock_guard() { m.lock(); }
            reverse_lock_guard(const reverse_lock_guard&) = delete;
            reverse_lock_guard& operator=(const reverse_lock_guard&) = delete;
        private:
            mutex_type& m;
        };

        // std::condition_variable::wait_until when max time_point is specified seems unreliable
        // note, all parameters are template parameters in order to also handle e.g. boost::condition_variable_any
        template <typename ConditionVariable, typename Lock, typename TimePoint, typename Predicate>
        inline bool wait_until(ConditionVariable& condition, Lock& lock, const TimePoint& tp, Predicate predicate)
        {
            if ((TimePoint::max)() == tp)
            {
                condition.wait(lock, predicate);
                return true;
            }
            else
            {
                return condition.wait_until(lock, tp, predicate);
            }
        }

        template <typename ConditionVariable, typename Lock, typename Rep, typename Period, typename Predicate>
        inline bool wait_for(ConditionVariable& condition, Lock& lock, const std::chrono::duration<Rep, Period>& duration, Predicate predicate)
        {
            if ((std::chrono::duration<Rep, Period>::max)() == duration)
            {
                condition.wait(lock, predicate);
                return true;
            }
            else
            {
                // using wait_until as a workaround for bug in VS2015, resolved in VS2017
                // see https://developercommunity.visualstudio.com/content/problem/274532/bug-in-visual-studio-2015-implementation-of-stdcon.html
                return condition.wait_until(lock, std::chrono::steady_clock::now() + duration, predicate);
            }
        }

        template <typename ConditionVariable, typename Lock, typename TimePoint>
        inline auto wait_until(ConditionVariable& condition, Lock& lock, const TimePoint& tp) -> decltype(condition.wait_until(lock, tp))
        {
            if ((TimePoint::max)() == tp)
            {
                condition.wait(lock);
                typedef decltype(condition.wait_until(lock, tp)) cv_status;
                return cv_status::no_timeout;
            }
            else
            {
                return condition.wait_until(lock, tp);
            }
        }

        template <typename ConditionVariable, typename Lock, typename Rep, typename Period>
        inline auto wait_for(ConditionVariable& condition, Lock& lock, const std::chrono::duration<Rep, Period>& duration) -> decltype(condition.wait_for(lock, duration))
        {
            if ((std::chrono::duration<Rep, Period>::max)() == duration)
            {
                condition.wait(lock);
                typedef decltype(condition.wait_for(lock, duration)) cv_status;
                return cv_status::no_timeout;
            }
            else
            {
                return condition.wait_until(lock, std::chrono::steady_clock::now() + duration);
            }
        }

        // RAII helper for starting and later joining threads which may require unblocking before join
        template <typename Function, typename PreJoin>
        class thread_guard
        {
        public:
            thread_guard(Function f, PreJoin pre_join) : thread(f), pre_join(pre_join) {}
            thread_guard(thread_guard&& rhs) : thread(std::move(rhs.thread)), pre_join(std::move(rhs.pre_join)) {}
            ~thread_guard() { pre_join(); if (thread.joinable()) thread.join(); }
            thread_guard(const thread_guard&) = delete;
            thread_guard& operator=(const thread_guard&) = delete;
            thread_guard& operator=(thread_guard&&) = delete;
        private:
            std::thread thread;
            PreJoin pre_join;
        };

        template <typename Function, typename PreJoin>
        inline thread_guard<Function, PreJoin> make_thread_guard(Function f, PreJoin pj)
        {
            return{ f, pj };
        }
    }
}

#endif

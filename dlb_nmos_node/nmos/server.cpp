/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Improved error handling in server open/close operations by adding
 * wait_nothrow helper function and better task completion handling.
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

#include "nmos/server.h"
#include "nmos/model.h"

namespace nmos
{
    server::server(nmos::base_model& model)
        : model(model)
    {}

    namespace details
    {
        void wait_nothrow(pplx::task<void> t) { try { t.wait(); } catch (...) {} }
    }

    pplx::task<void> server::open()
    {
        return pplx::create_task([&]
        {
            start_threads();
            return open_listeners();
        }).then([&](pplx::task<void> finally)
        {
            try
            {
                return finally.get();
            }
            catch (...)
            {
                stop_threads();
                throw;
            }
        });
    }

    pplx::task<void> server::close()
    {
        return close_listeners().then([&](pplx::task<void> finally)
        {
            stop_threads();
            return finally;
        });
    }

    void server::start_threads()
    {
        // Start up threads

        for (auto& thread_function : thread_functions)
        {
            // hm, this is intended to be one-shot, so could move thread_function into the thread?
            threads.push_back(std::thread(thread_function));
        }
    }

    pplx::task<void> server::open_listeners()
    {
        return pplx::create_task([&]
        {
            // Open the API ports

            std::vector<pplx::task<void>> tasks;

            for (auto& http_listener : http_listeners)
            {
                if (0 <= http_listener.uri().port()) tasks.push_back(http_listener.open());
            }

            for (auto& ws_listener : ws_listeners)
            {
                if (0 <= ws_listener.uri().port()) tasks.push_back(ws_listener.open());
            }

            return pplx::when_all(tasks.begin(), tasks.end()).then([tasks](pplx::task<void> finally)
            {
                for (auto& task : tasks) details::wait_nothrow(task);
                finally.wait();
            });
        });
    }

    pplx::task<void> server::close_listeners()
    {
        return pplx::create_task([&]
        {
            // Close the API ports

            std::vector<pplx::task<void>> tasks;

            for (auto& http_listener : http_listeners)
            {
                if (0 <= http_listener.uri().port()) tasks.push_back(http_listener.close());
            }
            for (auto& ws_listener : ws_listeners)
            {
                if (0 <= ws_listener.uri().port()) tasks.push_back(ws_listener.close());
            }

            return pplx::when_all(tasks.begin(), tasks.end()).then([tasks](pplx::task<void> finally)
            {
                for (auto& task : tasks) details::wait_nothrow(task);
                finally.wait();
            });
        });
    }

    void server::stop_threads()
    {
        // Signal shutdown
        model.controlled_shutdown();

        // Join all threads

        for (auto& thread : threads)
        {
            if (thread.joinable())
            {
                // hm, ignoring possibility of exceptions?
                thread.join();
            }
        }

        threads.clear();
    }
}

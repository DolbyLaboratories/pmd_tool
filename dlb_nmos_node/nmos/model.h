/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed IS-12 control protocol resources from node model.
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

#ifndef NMOS_MODEL_H
#define NMOS_MODEL_H

#include "nmos/mutex.h"
#include "nmos/resources.h"
#include "nmos/settings.h"
#include "nmos/thread_utils.h"

// NMOS Node and Registry models
namespace nmos
{
    struct base_model
    {
        // mutex to be used to protect the members of the model from simultaneous access by multiple threads
        mutable nmos::mutex mutex;

        // condition to be used to wait for, and notify other threads about, changes to any member of the model
        // including the shutdown flag
        mutable nmos::condition_variable condition;

        // condition to be used to wait until, and notify other threads when, shutdown is initiated
        // by setting the shutdown flag
        mutable nmos::condition_variable shutdown_condition;

        // application-wide configuration
        nmos::settings settings;

        // flag indicating whether shutdown has been initiated
        bool shutdown = false;

        // convenience functions
        // (the mutex and conditions may be used directly as well)

        nmos::read_lock read_lock() const { return nmos::read_lock{ mutex }; }
        nmos::write_lock write_lock() const { return nmos::write_lock{ mutex }; }
        void notify() const { return condition.notify_all(); }

        template <class ReadOrWriteLock>
        void wait(ReadOrWriteLock& lock)
        {
            condition.wait(lock);
        }

        template <class ReadOrWriteLock, class Predicate>
        void wait(ReadOrWriteLock& lock, Predicate pred)
        {
            condition.wait(lock, pred);
        }

        template <class ReadOrWriteLock, class TimePoint>
        cv_status wait_until(ReadOrWriteLock& lock, const TimePoint& abs_time)
        {
            return details::wait_until(condition, lock, abs_time);
        }

        template <class ReadOrWriteLock, class TimePoint, class Predicate>
        bool wait_until(ReadOrWriteLock& lock, const TimePoint& abs_time, Predicate pred)
        {
            return details::wait_until(condition, lock, abs_time, pred);
        }

        template <class ReadOrWriteLock, class Duration>
        cv_status wait_for(ReadOrWriteLock& lock, const Duration& rel_time)
        {
            return details::wait_for(condition, lock, rel_time);
        }

        template <class ReadOrWriteLock, class Duration, class Predicate>
        bool wait_for(ReadOrWriteLock& lock, const Duration& rel_time, Predicate pred)
        {
            return details::wait_for(condition, lock, rel_time, pred);
        }

        void controlled_shutdown()
        {
            {
                auto lock = write_lock();
                shutdown = true;
            }
            notify();
            shutdown_condition.notify_all();
        }
    };

    struct model : base_model
    {
        // IS-04 resources for this node
        nmos::resources node_resources;
    };

    struct node_model : model
    {
        // IS-05 senders and receivers for this node
        // see nmos/connection_resources.h
        nmos::resources connection_resources;

        // IS-07 sources for this node
        // see nmos/events_resources.h
        nmos::resources events_resources;

        // IS-08 inputs and outputs for this node
        // see nmos/channelmapping_resources.h
        nmos::resources channelmapping_resources;
    };

    struct registry_model : model
    {
        // Resources added by IS-04 Registration API
        nmos::resources registry_resources;

        // Global configuration resource for IS-09 System API
        nmos::resource system_global_resource;
    };
}

#endif

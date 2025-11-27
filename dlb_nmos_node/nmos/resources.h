/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Updated documentation URLs from specs.amwa.tv to GitHub repository links.
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

#ifndef NMOS_RESOURCES_H
#define NMOS_RESOURCES_H

#include <functional>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/composite_key.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include "nmos/resource.h"

// This declares a container type suitable for managing a node's resources, or all the resources in a registry,
// with indices to support the operations required by the Node, Query and Registration APIs.
namespace nmos
{
    // The implementation is currently left public and high-level operations are provided as free functions while
    // the interface settles down...

    namespace tags
    {
        struct id;
        struct type;
        struct created;
        struct updated;
    }

    namespace details
    {
        typedef boost::multi_index::member<resource, id, &resource::id> id_extractor;
        typedef boost::multi_index::composite_key<resource, boost::multi_index::const_mem_fun<resource, bool, &resource::has_data>, boost::multi_index::member<resource, type, &resource::type>> type_extractor;
        typedef boost::tuple<bool, type> type_extractor_tuple;
        typedef boost::multi_index::member<resource, tai, &resource::created> created_extractor;
        typedef boost::multi_index::member<resource, tai, &resource::updated> updated_extractor;

        // extant resources have non-null data
        inline type_extractor_tuple has_data(const type& type) { return type_extractor_tuple{ true, type }; }
    }

    // the id index ensures resource id is unique
    // the type index is a composite index incorporating whether the resource has been deleted or expired
    // the created/updated indices ensure uniqueness to satisfy the requirements of Query API cursor-based paging
    // and are in descending order to simplify implementation
    typedef boost::multi_index_container<
        resource,
        boost::multi_index::indexed_by<
            boost::multi_index::hashed_unique<boost::multi_index::tag<tags::id>, details::id_extractor>,
            boost::multi_index::ordered_non_unique<boost::multi_index::tag<tags::type>, details::type_extractor>,
            boost::multi_index::ordered_unique<boost::multi_index::tag<tags::created>, details::created_extractor, std::greater<details::created_extractor::result_type>>,
            boost::multi_index::ordered_unique<boost::multi_index::tag<tags::updated>, details::updated_extractor, std::greater<details::updated_extractor::result_type>>
        >
    > resources;

    // Resource creation/update/deletion operations

    // returns the most recent timestamp in the specified resources
    tai most_recent_update(const resources& resources);

    inline tai strictly_increasing_update(const resources& resources, tai update = tai_now())
    {
        const auto most_recent = most_recent_update(resources);
        return update > most_recent ? update : tai_from_time_point(time_point_from_tai(most_recent) + tai_clock::duration(1));
    }

    // returns the least health of extant and non-extant resources
    // note, this is now O(N), not O(1), since resource health is mutable and therefore unindexed
    std::pair<health, health> least_health(const resources& resources);

    // insert a resource (join_sub_resources can be false if related resources are known to be inserted in order)
    std::pair<resources::iterator, bool> insert_resource(resources& resources, resource&& resource, bool join_sub_resources = false);

    // modify a resource
    bool modify_resource(resources& resources, const id& id, std::function<void(resource&)> modifier);

    // erase the resource with the specified id from the specified resources (if present)
    // and return the count of the number of resources erased (including sub-resources)
    // resources may optionally be initially "erased" by setting data to null, and remain in this non-extant state until they are explicitly forgotten (or reinserted)
    resources::size_type erase_resource(resources& resources, const id& id, bool forget_now = true);

    // forget all erased resources which expired *before* the specified time from the specified resources
    // and return the count of the number of resources forgotten
    // resources may optionally be initially "erased" by setting data to null, and remain in this non-extant state until they are explicitly forgotten (or reinserted)
    resources::size_type forget_erased_resources(resources& resources, const health& forget_health = health_forever);

    // erase all resources which expired *before* the specified time from the specified resources
    // and return the count of the number of resources erased
    // resources may optionally be initially "erased" by setting data to null, and remain in this non-extant state until they are explicitly forgotten (or reinserted)
    // by default, the updated timestamp is not modified but this may be overridden
    resources::size_type erase_expired_resources(resources& resources, const health& expire_health, bool forget_now = true, bool set_updated = false);

    // find the resource with the specified id in the specified resources (if present) and
    // set the health of the resource and all of its sub-resources, to prevent them expiring
    // note, since health is mutable, no need for the resources parameter to be non-const
    void set_resource_health(const resources& resources, const id& id, health health = health_now());

    // Other helper functions for resources

    // get the super-resource id and type, according to the guidelines on referential integrity
    // see https://github.com/AMWA-TV/nmos-discovery-registration/blob/v1.2.1/docs/4.1.%20Behaviour%20-%20Registration.md#referential-integrity
    std::pair<id, type> get_super_resource(const api_version& version, const type& type, const web::json::value& data);

    inline std::pair<id, type> get_super_resource(const resource& resource)
    {
        return get_super_resource(resource.version, resource.type, resource.data);
    }

    bool has_resource(const resources& resources, const std::pair<id, type>& id_type);

    // find resource by id
    resources::const_iterator find_resource(const resources& resources, const id& id);
    resources::iterator find_resource(resources& resources, const id& id);

    // find resource by id, and matching type
    resources::const_iterator find_resource(const resources& resources, const std::pair<id, type>& id_type);
    resources::iterator find_resource(resources& resources, const std::pair<id, type>& id_type);

    // find resource by type and predicate
    template <typename Predicate>
    inline resources::const_iterator find_resource_if(const resources& resources, type type, Predicate pred)
    {
        auto& by_type = resources.get<tags::type>();
        const auto type_resources = by_type.equal_range(details::has_data(type));
        auto resource = std::find_if(type_resources.first, type_resources.second, pred);
        return type_resources.second != resource ? resources.project<0>(resource) : resources.end();
    }

    // strictly, this just returns a node (or the end iterator)
    resources::const_iterator find_self_resource(const resources& resources);
    resources::iterator find_self_resource(resources& resources);

    // get the id of each resource with the specified super-resource
    std::set<nmos::id> get_sub_resources(const resources& resources, const std::pair<id, type>& id_type);

    namespace details
    {
        // return true if the resource is "erased" but not forgotten
        bool is_erased_resource(const resources& resources, const std::pair<id, type>& id_type);
    }
}

#endif

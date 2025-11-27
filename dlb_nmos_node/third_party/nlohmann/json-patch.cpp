/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Fixed namespace syntax error (; to }), removed to_string()
 * calls on json_pointer objects, removed get_json() methods.
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

#include "json-patch.hpp"

#include <nlohmann/json-schema.hpp>

namespace
{

// originally from http://jsonpatch.com/, http://json.schemastore.org/json-patch
// with fixes
const nlohmann::json patch_schema = R"patch({
    "title": "JSON schema for JSONPatch files",
    "$schema": "http://json-schema.org/draft-04/schema#",
    "type": "array",

    "items": {
        "oneOf": [
            {
                "additionalProperties": false,
                "required": [ "value", "op", "path"],
                "properties": {
                    "path" : { "$ref": "#/definitions/path" },
                    "op": {
                        "description": "The operation to perform.",
                        "type": "string",
                        "enum": [ "add", "replace", "test" ]
                    },
                    "value": {
                        "description": "The value to add, replace or test."
                    }
                }
            },
            {
                "additionalProperties": false,
                "required": [ "op", "path"],
                "properties": {
                    "path" : { "$ref": "#/definitions/path" },
                    "op": {
                        "description": "The operation to perform.",
                        "type": "string",
                        "enum": [ "remove" ]
                    }
                }
            },
            {
                "additionalProperties": false,
                "required": [ "from", "op", "path" ],
                "properties": {
                    "path" : { "$ref": "#/definitions/path" },
                    "op": {
                        "description": "The operation to perform.",
                        "type": "string",
                        "enum": [ "move", "copy" ]
                    },
                    "from": {
                        "$ref": "#/definitions/path",
                        "description": "A JSON Pointer path pointing to the location to move/copy from."
                    }
                }
            }
        ]
    },
    "definitions": {
        "path": {
            "description": "A JSON Pointer path.",
            "type": "string"
        }
    }
})patch"_json;
}; // namespace

namespace nlohmann
{

json_patch::json_patch(json &&patch)
    : j_(std::move(patch))
{
	validateJsonPatch(j_);
}

json_patch::json_patch(const json &patch)
    : j_(std::move(patch))
{
	validateJsonPatch(j_);
}

json_patch &json_patch::add(const json::json_pointer &ptr, json value)
{
	j_.push_back(json{{"op", "add"}, {"path", ptr}, {"value", std::move(value)}});
	return *this;
}

json_patch &json_patch::replace(const json::json_pointer &ptr, json value)
{
	j_.push_back(json{{"op", "replace"}, {"path", ptr}, {"value", std::move(value)}});
	return *this;
}

json_patch &json_patch::remove(const json::json_pointer &ptr)
{
	j_.push_back(json{{"op", "remove"}, {"path", ptr}});
	return *this;
}

void json_patch::validateJsonPatch(json const &patch)
{
	// static put here to have it created at the first usage of validateJsonPatch
	static nlohmann::json_schema::json_validator patch_validator(patch_schema);

	patch_validator.validate(patch);

	for (auto const &op : patch)
		json::json_pointer(op["path"].get<std::string>());
}

} // namespace nlohmann

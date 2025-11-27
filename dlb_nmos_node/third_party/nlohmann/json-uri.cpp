/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Changed C++ style cast to C-style cast for character conversion.
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
/*
 * JSON schema validator for JSON for modern C++
 *
 * Copyright (c) 2016-2019 Patrick Boettcher <p@yai.se>.
 *
 * SPDX-License-Identifier: MIT
 *
 */
#include <nlohmann/json-schema.hpp>

#include <sstream>

namespace nlohmann
{

void json_uri::update(const std::string &uri)
{
	std::string pointer = ""; // default pointer is document-root

	// first split the URI into location and pointer
	auto pointer_separator = uri.find('#');
	if (pointer_separator != std::string::npos) {    // and extract the pointer-string if found
		pointer = uri.substr(pointer_separator + 1); // remove #

		// unescape %-values IOW, decode JSON-URI-formatted JSON-pointer
		std::size_t pos = pointer.size() - 1;
		do {
			pos = pointer.rfind('%', pos);
			if (pos == std::string::npos)
				break;

			if (pos >= pointer.size() - 2) {
				pos--;
				continue;
			}

			std::string hex = pointer.substr(pos + 1, 2);
			char ascii = (char) std::strtoul(hex.c_str(), nullptr, 16);
			pointer.replace(pos, 3, 1, ascii);

			pos--;
		} while (1);
	}

	auto location = uri.substr(0, pointer_separator);

	if (location.size()) { // a location part has been found

		// if it is an URN take it as it is
		if (location.find("urn:") == 0) {
			urn_ = location;

			// and clear URL members
			scheme_ = "";
			authority_ = "";
			path_ = "";

		} else { // it is an URL

			// split URL in protocol, hostname and path
			std::size_t pos = 0;
			auto proto = location.find("://", pos);
			if (proto != std::string::npos) { // extract the protocol

				urn_ = ""; // clear URN-member if URL is parsed

				scheme_ = location.substr(pos, proto - pos);
				pos = 3 + proto; // 3 == "://"

				auto authority = location.find("/", pos);
				if (authority != std::string::npos) { // and the hostname (no proto without hostname)
					authority_ = location.substr(pos, authority - pos);
					pos = authority;
				}
			}

			auto path = location.substr(pos);

			// URNs cannot of have paths
			if (urn_.size() && path.size())
				throw std::invalid_argument("Cannot add a path (" + path + ") to an URN URI (" + urn_ + ")");

			if (path[0] == '/') // if it starts with a / it is root-path
				path_ = path;
			else if (pos == 0) { // the URL contained only a path and the current path has no / at the end, strip last element until / and append
				auto last_slash = path_.rfind('/');
				path_ = path_.substr(0, last_slash) + '/' + path;
			} else // otherwise it is a subfolder
				path_.append(path);
		}
	}

	pointer_ = ""_json_pointer;
	identifier_ = "";

	if (pointer[0] == '/')
		pointer_ = json::json_pointer(pointer);
	else
		identifier_ = pointer;
}

std::string json_uri::location() const
{
	if (urn_.size())
		return urn_;

	std::stringstream s;

	if (scheme_.size() > 0)
		s << scheme_ << "://";

	s << authority_
	  << path_;

	return s.str();
}

std::string json_uri::to_string() const
{
	std::stringstream s;

	s << location() << " # ";

	if (identifier_ == "")
		s << pointer_.to_string();
	else
		s << identifier_;

	return s.str();
}

std::ostream &operator<<(std::ostream &os, const json_uri &u)
{
	return os << u.to_string();
}

std::string json_uri::escape(const std::string &src)
{
	std::vector<std::pair<std::string, std::string>> chars = {
	    {"~", "~0"},
	    {"/", "~1"}};

	std::string l = src;

	for (const auto &c : chars) {
		std::size_t pos = 0;
		do {
			pos = l.find(c.first, pos);
			if (pos == std::string::npos)
				break;
			l.replace(pos, 1, c.second);
			pos += c.second.size();
		} while (1);
	}

	return l;
}

} // namespace nlohmann

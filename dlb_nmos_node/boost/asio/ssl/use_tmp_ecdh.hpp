/*
 * Derived from: sony/nmos-cpp (Apache-2.0)
 * Modifications: Removed OpenSSL 3.0+ compatibility code and reverted to pre-3.0 implementation.
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
// based on https://github.com/chriskohlhoff/asio/pull/117

#ifndef BOOST_ASIO_SSL_USE_TMP_ECDH_HPP
#define BOOST_ASIO_SSL_USE_TMP_ECDH_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <boost/asio/ssl/context.hpp>

#include <boost/asio/detail/push_options.hpp>

#ifndef BOOST_ASIO_SYNC_OP_VOID
# define BOOST_ASIO_SYNC_OP_VOID void
# define BOOST_ASIO_SYNC_OP_VOID_RETURN(e) return
#endif

namespace boost {
namespace asio {
namespace ssl {

namespace use_tmp_ecdh_details {

struct bio_cleanup
{
    BIO* p;
    ~bio_cleanup() { if (p) ::BIO_free(p); }
};

struct x509_cleanup
{
    X509* p;
    ~x509_cleanup() { if (p) ::X509_free(p); }
};

struct evp_pkey_cleanup
{
    EVP_PKEY* p;
    ~evp_pkey_cleanup() { if (p) ::EVP_PKEY_free(p); }
};

struct ec_key_cleanup
{
    EC_KEY *p;
    ~ec_key_cleanup() { if (p) ::EC_KEY_free(p); }
};

inline
BOOST_ASIO_SYNC_OP_VOID do_use_tmp_ecdh(boost::asio::ssl::context& ctx,
    BIO* bio, boost::system::error_code& ec)
{
    ::ERR_clear_error();

    int nid = NID_undef;

    x509_cleanup x509 = { ::PEM_read_bio_X509(bio, NULL, 0, NULL) };
    if (x509.p)
    {
        evp_pkey_cleanup pkey = { ::X509_get_pubkey(x509.p) };
        if (pkey.p)
        {
            ec_key_cleanup key = { ::EVP_PKEY_get1_EC_KEY(pkey.p) };
            if (key.p)
            {
                const EC_GROUP *group = EC_KEY_get0_group(key.p);
                nid = EC_GROUP_get_curve_name(group);
            }
        }
    }

    ec_key_cleanup ec_key = { ::EC_KEY_new_by_curve_name(nid) };
    if (ec_key.p)
    {
        if (::SSL_CTX_set_tmp_ecdh(ctx.native_handle(), ec_key.p) == 1)
        {
            ec = boost::system::error_code();
            BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
        }
    }

    ec = boost::system::error_code(
        static_cast<int>(::ERR_get_error()),
        boost::asio::error::get_ssl_category());
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
}

inline
BOOST_ASIO_SYNC_OP_VOID use_tmp_ecdh_file(boost::asio::ssl::context& ctx,
    const std::string& certificate, boost::system::error_code& ec)
{
    ::ERR_clear_error();

    bio_cleanup bio = { ::BIO_new_file(certificate.c_str(), "r") };
    if (bio.p)
    {
        return do_use_tmp_ecdh(ctx, bio.p, ec);
    }

    ec = boost::system::error_code(
        static_cast<int>(::ERR_get_error()),
        boost::asio::error::get_ssl_category());
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
}

inline
void use_tmp_ecdh_file(boost::asio::ssl::context& ctx, const std::string& certificate)
{
    boost::system::error_code ec;
    use_tmp_ecdh_file(ctx, certificate, ec);
    boost::asio::detail::throw_error(ec, "use_tmp_ecdh_file");
}

inline
BOOST_ASIO_SYNC_OP_VOID use_tmp_ecdh(boost::asio::ssl::context& ctx,
    const boost::asio::const_buffer& certificate, boost::system::error_code& ec)
{
    ::ERR_clear_error();

    bio_cleanup bio = { ::BIO_new_mem_buf(const_cast<unsigned char*>(boost::asio::buffer_cast<const unsigned char*>(certificate)), static_cast<int>(boost::asio::buffer_size(certificate))) };
    if (bio.p)
    {
        return do_use_tmp_ecdh(ctx, bio.p, ec);
    }

    ec = boost::system::error_code(
        static_cast<int>(::ERR_get_error()),
        boost::asio::error::get_ssl_category());
    BOOST_ASIO_SYNC_OP_VOID_RETURN(ec);
}

inline
void use_tmp_ecdh(boost::asio::ssl::context& ctx, const boost::asio::const_buffer& certificate)
{
    boost::system::error_code ec;
    use_tmp_ecdh(ctx, certificate, ec);
    boost::asio::detail::throw_error(ec, "use_tmp_ecdh");
}

} // namespace use_tmp_ecdh_details

using use_tmp_ecdh_details::use_tmp_ecdh_file;
using use_tmp_ecdh_details::use_tmp_ecdh;

} // namespace ssl
} // namespace asio
} // namespace boost

#include <boost/asio/detail/pop_options.hpp>

#endif // BOOST_ASIO_SSL_USE_TMP_ECDH_HPP

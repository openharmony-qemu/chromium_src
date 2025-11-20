/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cerrno>

#include "arkweb/chromium_ext/services/network/url_loader_ext.h"

#include "base/logging.h"

namespace {

class UrlLoaderExtTest : public testing::Test {
public:
  UrlLoaderExtTest() = default;
  ~UrlLoaderExtTest() = default;
};

#if BUILDFLAG(ARKWEB_EXT_LOG_MESSAGE)
TEST_F(UrlLoaderExtTest, UrlLoaderExtTest_001) {
  GURL gUrl;
  net::HttpResponseInfo info;

  info.alpn_negotiated_protocol.clear();
  std::string protocol = network::GetProtocol(gUrl, info);
  EXPECT_TRUE(protocol.empty());

  info.alpn_negotiated_protocol = "unknown";
  protocol = network::GetProtocol(gUrl, info);
  EXPECT_EQ(protocol, "");

  info.alpn_negotiated_protocol = "unknown";
  info.was_fetched_via_spdy = true;
  protocol = network::GetProtocol(gUrl, info);
  EXPECT_EQ(protocol, "h2");

  info.was_fetched_via_spdy = false;
  GURL gUrl2("ftp://192.169.0.1");
  protocol = network::GetProtocol(gUrl2, info);
  EXPECT_EQ(protocol, "ftp");

  GURL gUrl3("https://www.examples.com");
  info.headers.reset();
  protocol = network::GetProtocol(gUrl3, info);
  EXPECT_EQ(protocol, "http");

  info.headers = base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/0.9 200 OK");
  protocol = network::GetProtocol(gUrl3, info);
  EXPECT_EQ(protocol, "http/0.9");

  info.headers.reset();
  info.headers = base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.0 200 OK");
  protocol = network::GetProtocol(gUrl3, info);
  EXPECT_EQ(protocol, "http/1.0");

  info.headers.reset();
  info.headers = base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.1 200 OK");
  protocol = network::GetProtocol(gUrl3, info);
  EXPECT_EQ(protocol, "http/1.1");
}

TEST_F(UrlLoaderExtTest, GetProtocol_AlpnProtocolValid_ReturnsDirectly) {
  GURL url("https://www.example.com");
  net::HttpResponseInfo info;

  info.alpn_negotiated_protocol = "h3";
  std::string protocol = network::GetProtocol(url, info);
  EXPECT_EQ(protocol, "h3");
}

TEST_F(UrlLoaderExtTest, GetProtocol_NonHttpScheme_ReturnsScheme) {
  GURL url("ws://example.com");
  net::HttpResponseInfo info;

  info.alpn_negotiated_protocol = "unknown";
  info.was_fetched_via_spdy = false;
  std::string protocol = network::GetProtocol(url, info);
  EXPECT_EQ(protocol, "ws");
}

TEST_F(UrlLoaderExtTest, GetProtocol_NoHeaders_ReturnsHttp) {
  GURL url("https://www.example.com");
  net::HttpResponseInfo info;

  info.alpn_negotiated_protocol = "unknown";
  info.was_fetched_via_spdy = false;
  info.headers.reset();
  std::string protocol = network::GetProtocol(url, info);
  EXPECT_EQ(protocol, "http");
}

TEST_F(UrlLoaderExtTest, GetProtocol_Http09_ReturnsHttp09) {
  GURL url("http://www.example.com");
  net::HttpResponseInfo info;

  info.alpn_negotiated_protocol = "unknown";
  info.was_fetched_via_spdy = false;
  info.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/0.9 200 OK");
  std::string protocol = network::GetProtocol(url, info);
  EXPECT_EQ(protocol, "http/0.9");
}

TEST_F(UrlLoaderExtTest, GetProtocol_Http10_ReturnsHttp10) {
  GURL url("http://www.example.com");
  net::HttpResponseInfo info;

  info.alpn_negotiated_protocol = "unknown";
  info.was_fetched_via_spdy = false;
  info.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/1.0 200 OK");
  std::string protocol = network::GetProtocol(url, info);
  EXPECT_EQ(protocol, "http/1.0");
}

TEST_F(UrlLoaderExtTest, GetProtocol_HttpOtherVersion_ReturnsHttp) {
  GURL url("http://www.example.com");
  net::HttpResponseInfo info;

  info.alpn_negotiated_protocol = "unknown";
  info.was_fetched_via_spdy = false;
  info.headers =
      base::MakeRefCounted<net::HttpResponseHeaders>("HTTP/2.0 200 OK");
  std::string protocol = network::GetProtocol(url, info);
  EXPECT_EQ(protocol, "http");
}

TEST_F(UrlLoaderExtTest, BoolToString) {
  EXPECT_EQ(network::BoolToString(true), "true");
  EXPECT_EQ(network::BoolToString(false), "false");
}
#endif
}  // namespace
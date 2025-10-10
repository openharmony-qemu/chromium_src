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

#include "services/network/public/cpp/request_destination.h"
#include "services/network/public/cpp/request_mode.h"
#include "services/network/public/mojom/fetch_api.mojom.h"
#include "services/network/sec_header_helpers.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace network {

class SecHeaderHelpersExtTest : public ::testing::Test {};

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
TEST_F(SecHeaderHelpersExtTest, UntrustworthyUrl_ReturnsEmpty) {
  GURL http_url("http://example.test/path");
  auto headers =
      GetFetchMetadataHeaders(http_url, mojom::RequestMode::kCors, false,
                              mojom::RequestDestination::kEmpty, absl::nullopt);
  EXPECT_TRUE(headers.empty());
}

TEST_F(SecHeaderHelpersExtTest, Trustworthy_NoInitiator_NoUser_DestEmpty) {
  GURL https_url("https://a.test/resource");
  auto headers =
      GetFetchMetadataHeaders(https_url, mojom::RequestMode::kCors, false,
                              mojom::RequestDestination::kEmpty, absl::nullopt);

  ASSERT_FALSE(headers.empty());
  auto it_site = headers.find(std::string("Sec-Fetch-Site"));
  auto it_mode = headers.find(std::string("Sec-Fetch-Mode"));
  auto it_user = headers.find(std::string("Sec-Fetch-User"));
  auto it_dest = headers.find(std::string("Sec-Fetch-Dest"));
  EXPECT_NE(it_site, headers.end());
  EXPECT_NE(it_mode, headers.end());
  EXPECT_EQ(it_user, headers.end());
  EXPECT_NE(it_dest, headers.end());

  EXPECT_EQ(it_mode->second, RequestModeToString(mojom::RequestMode::kCors));
  EXPECT_EQ(it_dest->second, "empty");
}

TEST_F(SecHeaderHelpersExtTest,
       Trustworthy_WithInitiator_WithUser_DestNonEmpty) {
  GURL https_url("https://a.test/resource");
  url::Origin initiator = url::Origin::Create(GURL("https://a.test"));
  auto headers =
      GetFetchMetadataHeaders(https_url, mojom::RequestMode::kNoCors, true,
                              mojom::RequestDestination::kImage, initiator);

  ASSERT_FALSE(headers.empty());
  auto it_site = headers.find(std::string("Sec-Fetch-Site"));
  auto it_mode = headers.find(std::string("Sec-Fetch-Mode"));
  auto it_user = headers.find(std::string("Sec-Fetch-User"));
  auto it_dest = headers.find(std::string("Sec-Fetch-Dest"));
  EXPECT_NE(it_site, headers.end());
  EXPECT_NE(it_mode, headers.end());
  EXPECT_NE(it_user, headers.end());
  EXPECT_NE(it_dest, headers.end());

  EXPECT_EQ(it_mode->second, RequestModeToString(mojom::RequestMode::kNoCors));
  EXPECT_EQ(it_user->second, "?1");
  EXPECT_EQ(it_dest->second,
            RequestDestinationToString(mojom::RequestDestination::kImage));
}

TEST_F(SecHeaderHelpersExtTest, Trustworthy_WithInitiator_NoUser_DestNonEmpty) {
  GURL https_url("https://b.test/asset.js");
  url::Origin initiator = url::Origin::Create(GURL("https://c.test"));
  auto headers =
      GetFetchMetadataHeaders(https_url, mojom::RequestMode::kSameOrigin, false,
                              mojom::RequestDestination::kScript, initiator);

  ASSERT_FALSE(headers.empty());
  EXPECT_NE(headers.find(std::string("Sec-Fetch-Site")), headers.end());
  EXPECT_NE(headers.find(std::string("Sec-Fetch-Mode")), headers.end());
  EXPECT_EQ(headers.find(std::string("Sec-Fetch-User")), headers.end());
  auto it_dest = headers.find(std::string("Sec-Fetch-Dest"));
  ASSERT_NE(it_dest, headers.end());
  EXPECT_EQ(it_dest->second,
            RequestDestinationToString(mojom::RequestDestination::kScript));
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_LOAD)

}  // namespace network
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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/loader/resource/script_resource_utils.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/testing/page_test_base.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_response.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"
#include "third_party/blink/renderer/core/loader/resource/script_resource.h"
#include "third_party/blink/renderer/platform/testing/main_thread_isolate.cc"

namespace blink {

class ScriptResourceUtilsTest : public testing::Test {
protected:
  void SetUp() override {
    Test::SetUp();
  }
  void TearDown() override {
    Test::TearDown();
  }
};

TEST_F(ScriptResourceUtilsTest, Constructor) {
  v8::Isolate* isolate = nullptr;
  KURL url("https://example.com/test.js");
  WTF::TextEncoding encoding("UTF-8");
  mojom::blink::ScriptType script_type = mojom::blink::ScriptType::kClassic;
  auto* script_resource = ScriptResource::CreateForTest(
    isolate,
    url,
    encoding,
    script_type
  );
  EXPECT_TRUE(script_resource);
  EXPECT_EQ(script_resource->GetInitialRequestScriptType(), script_type);
  ScriptResourceUtils utils(script_resource);
  EXPECT_EQ(utils.scriptResource, script_resource);
}

#if BUILDFLAG(ARKWEB_INJECT_OFFLINE_RESOURCE)
TEST_F(ScriptResourceUtilsTest, CreateForOfflineResource_ClassicScript) {
  KURL url("https://example.com/test.js");
  KURL origin_url("https://example.com/");
  ResourceResponse response;
  bool is_module = false;
  ScriptResource* resource = ScriptResourceUtils::CreateForOfflineResource(
    url, origin_url, response, is_module);
  ASSERT_NE(resource, nullptr);
  EXPECT_EQ(resource->GetResourceRequest().GetMode(), network::mojom::RequestMode::kNoCors);
  EXPECT_EQ(resource->GetResourceRequest().GetCredentialsMode(),
            network::mojom::CredentialsMode::kInclude);
  EXPECT_EQ(resource->GetInitialRequestScriptType(), mojom::blink::ScriptType::kClassic);
}

TEST_F(ScriptResourceUtilsTest, CreateForOfflineResource_ModuleScript) {
  KURL url("https://example.com/test.js");
  KURL origin_url("https://example.com/");
  ResourceResponse response;
  bool is_module = true;
  ScriptResource* resource = ScriptResourceUtils::CreateForOfflineResource(
    url, origin_url, response, is_module);
  ASSERT_NE(resource, nullptr);
  EXPECT_EQ(resource->GetResourceRequest().GetMode(), network::mojom::RequestMode::kCors);
  EXPECT_EQ(resource->GetResourceRequest().GetCredentialsMode(),
            network::mojom::CredentialsMode::kSameOrigin);
  EXPECT_EQ(resource->GetInitialRequestScriptType(), mojom::blink::ScriptType::kModule);
}

TEST_F(ScriptResourceUtilsTest, CreateForOfflineResource_WithCrossOriginHeader) {
  KURL url("https://example.com/test.js");
  KURL origin_url("https://example.com/");
  ResourceResponse response;
  response.HttpHeaderField(AtomicString("Cross-Origin"));
  bool is_module = false;
  ScriptResource* resource = ScriptResourceUtils::CreateForOfflineResource(
    url, origin_url, response, is_module);
  ASSERT_NE(resource, nullptr);
  EXPECT_EQ(resource->GetResourceRequest().GetMode(), network::mojom::RequestMode::kNoCors);
  EXPECT_EQ(resource->GetResourceRequest().GetCredentialsMode(),
            network::mojom::CredentialsMode::kInclude);
}

TEST_F(ScriptResourceUtilsTest, CreateForOfflineResource_WithCrossOriginUseCredentials) {
  KURL url("https://example.com/test.js");
  KURL origin_url("https://example.com/");
  ResourceResponse response;
  response.HttpHeaderField(AtomicString("use-credentials"));
  bool is_module = false;
  ScriptResource* resource = ScriptResourceUtils::CreateForOfflineResource(
    url, origin_url, response, is_module);
  ASSERT_NE(resource, nullptr);
  EXPECT_EQ(resource->GetResourceRequest().GetMode(), network::mojom::RequestMode::kNoCors);
  EXPECT_NE(resource->GetResourceRequest().GetCredentialsMode(),
            network::mojom::CredentialsMode::kSameOrigin);
}

TEST_F(ScriptResourceUtilsTest, CreateForOfflineResource_WithOriginUrl) {
  KURL url("https://example.com/test.js");
  KURL origin_url("https://trusted-origin.com/");
  ResourceResponse response;
  bool is_module = false;
  ScriptResource* resource = ScriptResourceUtils::CreateForOfflineResource(
    url, origin_url, response, is_module);
  ASSERT_NE(resource, nullptr);
}

TEST_F(ScriptResourceUtilsTest, CreateForOfflineResource_WithEmptyCrossOrigin) {
  KURL url("https://example.com/test.js");
  KURL origin_url("https://example.com/");
  ResourceResponse response;
  response.SetHttpHeaderField(AtomicString("Cross-Origin"), AtomicString());
  bool is_module = false;
  ScriptResource* resource = ScriptResourceUtils::CreateForOfflineResource(
    url, origin_url, response, is_module);
  ASSERT_NE(resource, nullptr);
  EXPECT_EQ(resource->GetResourceRequest().GetMode(), network::mojom::RequestMode::kNoCors);
  EXPECT_EQ(resource->GetResourceRequest().GetCredentialsMode(),
            network::mojom::CredentialsMode::kInclude);
}

TEST_F(ScriptResourceUtilsTest, CreateForOfflineResource_WithOtherCrossOriginValue) {
  KURL url("https://example.com/test.js");
  KURL origin_url("https://example.com/");
  ResourceResponse response;
  response.SetHttpHeaderField(AtomicString("Cross-Origin"), AtomicString("anonymous"));
  bool is_module = false;
  ScriptResource* resource = ScriptResourceUtils::CreateForOfflineResource(
    url, origin_url, response, is_module);
  ASSERT_NE(resource, nullptr);
  EXPECT_EQ(resource->GetResourceRequest().GetMode(), network::mojom::RequestMode::kCors);
  EXPECT_EQ(resource->GetResourceRequest().GetCredentialsMode(),
            network::mojom::CredentialsMode::kSameOrigin);
}

TEST_F(ScriptResourceUtilsTest, CreateForOfflineResource_ModuleWithCrossOrigin) {
  KURL url("https://example.com/test.js");
  KURL origin_url("https://example.com/");
  ResourceResponse response;
  response.SetHttpHeaderField(AtomicString("Cross-Origin"), AtomicString("use-credentials"));
  bool is_module = true;
  ScriptResource* resource = ScriptResourceUtils::CreateForOfflineResource(
    url, origin_url, response, is_module);
  ASSERT_NE(resource, nullptr);
  EXPECT_EQ(resource->GetResourceRequest().GetMode(), network::mojom::RequestMode::kCors);
  EXPECT_EQ(resource->GetResourceRequest().GetCredentialsMode(),
            network::mojom::CredentialsMode::kSameOrigin);
}
#endif

}  // namespace blink
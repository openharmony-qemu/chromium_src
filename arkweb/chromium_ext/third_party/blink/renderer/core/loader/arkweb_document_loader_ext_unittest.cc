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

#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/renderer/core/html/html_image_element.h"
#include "third_party/blink/renderer/core/html/image_document.h"
#define private public
#include "third_party/blink/renderer/core/loader/document_loader.h"
#undef private
#include "third_party/blink/renderer/core/loader/resource/image_resource.h"
#include "third_party/blink/renderer/core/loader/subresource_filter.h"
#include "third_party/blink/renderer/core/testing/core_unit_test_helper.h"
#include "third_party/blink/renderer/core/testing/dummy_page_holder.h"
#include "third_party/blink/renderer/core/testing/sim/sim_request.h"
#include "third_party/blink/renderer/core/testing/sim/sim_test.h"
#include "third_party/blink/renderer/platform/graphics/image.h"
#include "third_party/blink/renderer/platform/loader/fetch/memory_cache.h"
#include "third_party/blink/renderer/platform/testing/unit_test_helpers.h"
#include "third_party/blink/renderer/platform/testing/url_loader_mock_factory.h"
#include "third_party/blink/renderer/platform/testing/url_test_helpers.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/wtf/text/wtf_string.h"
#include "third_party/blink/renderer/platform/wtf/vector.h"

namespace blink {
#if BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)
scoped_refptr<const SharedBuffer> GetShareBufferForImageDocument(
    Document* document);
#endif

class FakeWebFilter : public WebDocumentSubresourceFilter {
 public:
  LoadPolicy GetLoadPolicy(const WebURL& resource_url,
                           network::mojom::RequestDestination) override {
    return kAllow;
  }

  LoadPolicy GetLoadPolicyForWebSocketConnect(const WebURL&) override {
    return kAllow;
  }

  LoadPolicy GetLoadPolicyForWebTransportConnect(const WebURL&) override {
    return kAllow;
  }

  void ReportDisallowedLoad() override {}

  bool ShouldLogToConsole() override { return false; }

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void ClearStatistics() override {}

  std::unique_ptr<std::string> GetElementHidingSelectors(
      const WebURL& document_url,
      bool need_common_selectors) override {
    return nullptr;
  }

  bool HasGenericHideTypeOption(
      const WebURL& document_url,
      const url::Origin& parent_document_origin) override {
    return false;
  }

  bool HasElemHideTypeOption(
      const WebURL& document_url,
      const url::Origin& parent_document_origin) override {
    return false;
  }

  bool HasDocumentTypeOption(
      const WebURL& document_url,
      const url::Origin& parent_document_origin) override {
    return false;
  }

  void DidMatchCssRule(const WebURL& document_url,
                       const std::string& dom_path,
                       bool is_for_report = false) override {}

  void SetDidFinishLoad(bool did_load_finished) override {}

  bool GetDidFinishLoad() override { return false; }

  std::unique_ptr<std::vector<std::string>> GetUserDomPathSelectors(
      const blink::WebURL& document_url,
      bool need_generic_selectors) override {
    return nullptr;
  }

  void set_activation_state(bool enabled) override {}
#endif
};

class TestExtraData : public WebDocumentLoader::ExtraData {
 public:
  std::unique_ptr<ExtraData> Clone() override {
    return std::make_unique<TestExtraData>();
  }
};

class ArkWebDocumentLoaderExtTest : public SimTest {
 public:
  void SetUp() override { SimTest::SetUp(); }

  void TearDown() override { SimTest::TearDown(); }

  static constexpr unsigned char kTinyPngData[] = {
      0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D,
      0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
      0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 0x15, 0xC4, 0x89, 0x00, 0x00, 0x00,
      0x0A, 0x49, 0x44, 0x41, 0x54, 0x78, 0x9C, 0x63, 0x60, 0x00, 0x00, 0x00,
      0x02, 0x00, 0x01, 0xE5, 0x27, 0xD4, 0xA2, 0x00, 0x00, 0x00, 0x00, 0x49,
      0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};
};

#if BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)
TEST_F(ArkWebDocumentLoaderExtTest,
       GetShareBufferForImageDocument_NullDocument) {
  scoped_refptr<const SharedBuffer> buffer =
      GetShareBufferForImageDocument(nullptr);
  EXPECT_EQ(buffer, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, GetShareBufferForImageDocument_Success) {
  const char* kImageUrl2 = "https://example.com/img2.png";
  SimRequest req(kImageUrl2, "image/png");
  LoadURL(kImageUrl2);
  WTF::Vector<char> data2;
  data2.Append(reinterpret_cast<const char*>(kTinyPngData),
               static_cast<wtf_size_t>(sizeof(kTinyPngData)));
  req.Complete(data2);

  Document& doc = GetDocument();
  ASSERT_TRUE(doc.IsImageDocument());
  EXPECT_EQ(doc.Url().GetString(), String::FromUTF8(kImageUrl2));

  scoped_refptr<const SharedBuffer> buf = GetShareBufferForImageDocument(&doc);
  EXPECT_NE(buf, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, OnGetImageFromCache_ImageDocument_Success) {
  const char* kImageUrl = "https://example.com/img.png";
  SimRequest request(kImageUrl, "image/png");
  LoadURL(kImageUrl);
  WTF::Vector<char> image_bytes;
  image_bytes.Append(reinterpret_cast<const char*>(kTinyPngData),
                     static_cast<wtf_size_t>(sizeof(kTinyPngData)));
  request.Complete(image_bytes);

  if (GetDocument().View()) {
    GetDocument().View()->UpdateAllLifecyclePhasesForTest();
  }
  test::RunPendingTasks();
  Compositor().BeginFrame();

  Document& doc = GetDocument();
  ASSERT_TRUE(doc.IsImageDocument());
  EXPECT_EQ(doc.Url().GetString(), String::FromUTF8(kImageUrl));
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  ASSERT_TRUE(doc.IsImageDocument());
  HTMLImageElement* img_elem =
      static_cast<ImageDocument*>(&doc)->ImageElement();
  auto* content = img_elem->CachedImage();
  ASSERT_NE(content, nullptr);
  blink::Image* image = content->GetImage();
  ASSERT_TRUE(image && !image->IsNull());
  ASSERT_TRUE(image->Data());

  DocumentLoader* active = frame->Loader().GetDocumentLoader();
  ASSERT_NE(active, nullptr);
  ArkWebDocumentLoaderExt* ext = active->AsArkWebDocumentLoaderExt();
  ASSERT_NE(ext, nullptr);

  url_test_helpers::UnregisterAllURLsAndClearMemoryCache();

  scoped_refptr<const SharedBuffer> buf =
      ext->OnGetImageFromCache(WTF::String::FromUTF8(kImageUrl));
  EXPECT_NE(buf, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, OnGetImageFromCache_InValid_Kurl) {
  const char* kImageUrl = "https://example.com/img.png";
  SimRequest request(kImageUrl, "image/png");
  LoadURL(kImageUrl);

  WTF::Vector<char> data;
  data.Append(reinterpret_cast<const char*>(kTinyPngData),
              static_cast<wtf_size_t>(sizeof(kTinyPngData)));
  request.Complete(data);

  Document& doc = GetDocument();
  ASSERT_TRUE(doc.IsImageDocument());
  EXPECT_EQ(doc.Url().GetString(), String::FromUTF8(kImageUrl));
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  auto nav_params = std::make_unique<WebNavigationParams>();
  nav_params->url = url_test_helpers::ToKURL(kImageUrl);
  auto policy_container = PolicyContainer::CreateEmpty();
  auto extra = std::make_unique<TestExtraData>();

  Persistent<ArkWebDocumentLoaderExt> loader =
      MakeGarbageCollected<ArkWebDocumentLoaderExt>(
          frame, WebNavigationType::kWebNavigationTypeOther,
          std::move(nav_params), std::move(policy_container), std::move(extra));

  url_test_helpers::UnregisterAllURLsAndClearMemoryCache();

  scoped_refptr<const SharedBuffer> buf =
      loader->OnGetImageFromCache(WTF::String::FromUTF8(kImageUrl));
  EXPECT_EQ(buf, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, OnGetImageFromCache_Resource_Empty) {
  const char* kImageUrl = "https://example.com/img.png";
  static const unsigned char kTinyPng[] = {};

  SimRequest request(kImageUrl, "image/png");
  LoadURL(kImageUrl);
  WTF::Vector<char> data;
  data.Append(reinterpret_cast<const char*>(kTinyPng),
              static_cast<wtf_size_t>(sizeof(kTinyPng)));
  request.Complete(data);

  Document& doc = GetDocument();
  ASSERT_TRUE(doc.IsImageDocument());
  EXPECT_EQ(doc.Url().GetString(), String::FromUTF8(kImageUrl));
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  DocumentLoader* active = frame->Loader().GetDocumentLoader();
  ASSERT_NE(active, nullptr);
  auto* ext = active->AsArkWebDocumentLoaderExt();
  ASSERT_NE(ext, nullptr);

  url_test_helpers::UnregisterAllURLsAndClearMemoryCache();

  scoped_refptr<const SharedBuffer> buf =
      ext->OnGetImageFromCache(WTF::String::FromUTF8(kImageUrl));
  EXPECT_EQ(buf, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, OnGetImageFromCache_ProtocolIs_HTTP_Invalid) {
  const char* kImageUrl = "http://example.com/img.png";
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  DocumentLoader* active = frame->Loader().GetDocumentLoader();
  ASSERT_NE(active, nullptr);
  ArkWebDocumentLoaderExt* ext = active->AsArkWebDocumentLoaderExt();
  ASSERT_NE(ext, nullptr);

  scoped_refptr<const SharedBuffer> buf =
      ext->OnGetImageFromCache(WTF::String::FromUTF8(kImageUrl));
  EXPECT_EQ(buf, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, OnGetImageFromCache_ProtocolIs_HTTPS_Invalid) {
  const char* kImageUrl = "https://example.com/img.png";
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  DocumentLoader* active = frame->Loader().GetDocumentLoader();
  ASSERT_NE(active, nullptr);
  ArkWebDocumentLoaderExt* ext = active->AsArkWebDocumentLoaderExt();
  ASSERT_NE(ext, nullptr);

  scoped_refptr<const SharedBuffer> buf =
      ext->OnGetImageFromCache(WTF::String::FromUTF8(kImageUrl));
  EXPECT_EQ(buf, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, OnGetImageFromCache_OtherProtocol) {
  const char* kImageUrl = "ftp://example.com/img.png";
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  DocumentLoader* active = frame->Loader().GetDocumentLoader();
  ASSERT_NE(active, nullptr);
  ArkWebDocumentLoaderExt* ext = active->AsArkWebDocumentLoaderExt();
  ASSERT_NE(ext, nullptr);

  scoped_refptr<const SharedBuffer> buf =
      ext->OnGetImageFromCache(WTF::String::FromUTF8(kImageUrl));
  EXPECT_EQ(buf, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, OnGetImageFromCache_SingleResource) {
  const char* kImageUrl = "https://example.com/img2.png";

  KURL kurl(kImageUrl);
  ImageResource* image_resource = ImageResource::CreateForTest(kurl);
  image_resource->NotifyStartLoad();
  ResourceResponse response(kurl);
  response.SetHttpStatusCode(200);
  response.SetMimeType(AtomicString("image/png"));
  image_resource->ResponseReceived(response);
  image_resource->AppendData(base::as_chars(base::span(kTinyPngData)));
  image_resource->FinishForTest();
  MemoryCache::Get()->Add(image_resource);

  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  DocumentLoader* active = frame->Loader().GetDocumentLoader();
  ASSERT_NE(active, nullptr);
  ArkWebDocumentLoaderExt* ext = active->AsArkWebDocumentLoaderExt();
  ASSERT_NE(ext, nullptr);

  scoped_refptr<const SharedBuffer> buf =
      ext->OnGetImageFromCache(WTF::String::FromUTF8(kImageUrl));
  EXPECT_NE(buf, nullptr);
}
#endif  // BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
TEST_F(ArkWebDocumentLoaderExtTest, NotifyLcpForBlankless_EmptyUrl) {
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  auto nav_params = std::make_unique<WebNavigationParams>();
  nav_params->url = KURL();
  auto policy_container = PolicyContainer::CreateEmpty();
  auto extra = std::make_unique<TestExtraData>();

  Persistent<ArkWebDocumentLoaderExt> loader =
      MakeGarbageCollected<ArkWebDocumentLoaderExt>(
          frame, WebNavigationType::kWebNavigationTypeOther,
          std::move(nav_params), std::move(policy_container), std::move(extra));

  loader->NotifyLcpForBlankless();
  EXPECT_TRUE(loader->original_url_.IsEmpty());
}

TEST_F(ArkWebDocumentLoaderExtTest, NotifyLcpForBlankless_AboutBlankUrl) {
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  auto nav_params = std::make_unique<WebNavigationParams>();
  nav_params->url = KURL("about:blank");
  auto policy_container = PolicyContainer::CreateEmpty();
  auto extra = std::make_unique<TestExtraData>();

  Persistent<ArkWebDocumentLoaderExt> loader =
      MakeGarbageCollected<ArkWebDocumentLoaderExt>(
          frame, WebNavigationType::kWebNavigationTypeOther,
          std::move(nav_params), std::move(policy_container), std::move(extra));

  loader->NotifyLcpForBlankless();
  EXPECT_TRUE(loader->original_url_.IsAboutBlankURL());
}

TEST_F(ArkWebDocumentLoaderExtTest, NotifyLcpForBlankless_NormalUrl_WithFrame) {
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  auto nav_params = std::make_unique<WebNavigationParams>();
  nav_params->url = url_test_helpers::ToKURL("https://example.com/");
  auto policy_container = PolicyContainer::CreateEmpty();
  auto extra = std::make_unique<TestExtraData>();

  Persistent<ArkWebDocumentLoaderExt> loader =
      MakeGarbageCollected<ArkWebDocumentLoaderExt>(
          frame, WebNavigationType::kWebNavigationTypeOther,
          std::move(nav_params), std::move(policy_container), std::move(extra));

  loader->NotifyLcpForBlankless();
  EXPECT_FALSE(loader->original_url_.IsEmpty());
  EXPECT_FALSE(loader->original_url_.IsAboutBlankURL());
}
#endif  // BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)

#if BUILDFLAG(ARKWEB_ADBLOCK)
TEST_F(ArkWebDocumentLoaderExtTest, GetWebSubresourceFilter_Null) {
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  auto nav_params = std::make_unique<WebNavigationParams>();
  nav_params->url = url_test_helpers::ToKURL("https://example.com/");
  auto policy_container = PolicyContainer::CreateEmpty();
  auto extra = std::make_unique<TestExtraData>();

  Persistent<ArkWebDocumentLoaderExt> loader =
      MakeGarbageCollected<ArkWebDocumentLoaderExt>(
          frame, WebNavigationType::kWebNavigationTypeOther,
          std::move(nav_params), std::move(policy_container), std::move(extra));
  WebDocumentSubresourceFilter* web_filter = loader->GetWebSubresourceFilter();
  EXPECT_EQ(web_filter, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, GetWebSubresourceFilter_WithFilter) {
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  auto nav_params = std::make_unique<WebNavigationParams>();
  nav_params->url = url_test_helpers::ToKURL("https://example.com/");
  auto policy_container = PolicyContainer::CreateEmpty();
  auto extra = std::make_unique<TestExtraData>();

  Persistent<ArkWebDocumentLoaderExt> loader =
      MakeGarbageCollected<ArkWebDocumentLoaderExt>(
          frame, WebNavigationType::kWebNavigationTypeOther,
          std::move(nav_params), std::move(policy_container), std::move(extra));

  loader->SetSubresourceFilter(new FakeWebFilter());
  WebDocumentSubresourceFilter* returned_filter =
      loader->GetWebSubresourceFilter();
  EXPECT_NE(returned_filter, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, GetWebUserSubresourceFilter_Null) {
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  auto nav_params = std::make_unique<WebNavigationParams>();
  nav_params->url = url_test_helpers::ToKURL("https://example.com/");
  auto policy_container = PolicyContainer::CreateEmpty();
  auto extra = std::make_unique<TestExtraData>();

  Persistent<ArkWebDocumentLoaderExt> loader =
      MakeGarbageCollected<ArkWebDocumentLoaderExt>(
          frame, WebNavigationType::kWebNavigationTypeOther,
          std::move(nav_params), std::move(policy_container), std::move(extra));

  WebDocumentSubresourceFilter* web_filter =
      loader->GetWebUserSubresourceFilter();
  EXPECT_EQ(web_filter, nullptr);
}

TEST_F(ArkWebDocumentLoaderExtTest, GetWebUserSubresourceFilter_WithFilter) {
  Document& doc = GetDocument();
  LocalFrame* frame = doc.GetFrame();
  ASSERT_NE(frame, nullptr);

  auto nav_params = std::make_unique<WebNavigationParams>();
  nav_params->url = url_test_helpers::ToKURL("https://example.com/");
  auto policy_container = PolicyContainer::CreateEmpty();
  auto extra = std::make_unique<TestExtraData>();

  Persistent<ArkWebDocumentLoaderExt> loader =
      MakeGarbageCollected<ArkWebDocumentLoaderExt>(
          frame, WebNavigationType::kWebNavigationTypeOther,
          std::move(nav_params), std::move(policy_container), std::move(extra));

  auto* filter = MakeGarbageCollected<SubresourceFilter>(
      frame->DomWindow(), std::make_unique<FakeWebFilter>());
  loader->SetUserSubresourceFilter(filter);

  WebDocumentSubresourceFilter* web_filter =
      loader->GetWebUserSubresourceFilter();
  EXPECT_NE(web_filter, nullptr);
}

#endif  // BUILDFLAG(ARKWEB_ADBLOCK)
}  // namespace blink

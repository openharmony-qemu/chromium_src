// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONTENT_WEB_TEST_RENDERER_FAKE_SUBRESOURCE_FILTER_H_
#define CONTENT_WEB_TEST_RENDERER_FAKE_SUBRESOURCE_FILTER_H_

#include <string>
#include <vector>

#include "third_party/blink/public/mojom/fetch/fetch_api_request.mojom-shared.h"
#include "third_party/blink/public/platform/web_document_subresource_filter.h"
#include "third_party/blink/public/platform/web_url_request.h"

namespace blink {
class WebURL;
}  // namespace blink

namespace content {

class FakeSubresourceFilter : public blink::WebDocumentSubresourceFilter {
 public:
  explicit FakeSubresourceFilter(
      std::vector<std::string> disallowed_path_suffixes,
      bool block_resources);
  ~FakeSubresourceFilter() override;

  FakeSubresourceFilter(const FakeSubresourceFilter&) = delete;
  FakeSubresourceFilter& operator=(const FakeSubresourceFilter&) = delete;

  // blink::WebDocumentSubresourceFilter:
  LoadPolicy GetLoadPolicy(const blink::WebURL& resource_url,
                           network::mojom::RequestDestination) override;
  LoadPolicy GetLoadPolicyForWebSocketConnect(
      const blink::WebURL& url) override;
  LoadPolicy GetLoadPolicyForWebTransportConnect(
      const blink::WebURL& url) override;
  void ReportDisallowedLoad() override;
  bool ShouldLogToConsole() override;
#if BUILDFLAG(ARKWEB_UNITTESTS)
  void ClearStatistics() override {}
  bool HasGenericHideTypeOption(
      const blink::WebURL& document_url,
      const url::Origin& parent_document_origin) override { return false; }
  bool HasElemHideTypeOption(
      const blink::WebURL& document_url,
      const url::Origin& parent_document_origin) override { return false; }
  bool HasDocumentTypeOption(
      const blink::WebURL& document_url,
      const url::Origin& parent_document_origin) override { return false; }
  void DidMatchCssRule(const blink::WebURL& document_url,
                       const std::string& dom_path,
                       //  unsigned rule_line_num = 0,
                       bool is_for_report = false) override {}
  void SetDidFinishLoad(bool did_load_finished) override {}
  bool GetDidFinishLoad() override { return false; }
  std::unique_ptr<std::vector<std::string>> GetUserDomPathSelectors(
      const blink::WebURL& document_url,
      bool need_generic_selectors) override { return nullptr; }
  void set_activation_state(bool enabled) override {}
   std::unique_ptr<std::string> GetElementHidingSelectors(
      const blink::WebURL& document_url,
      bool need_common_selectors) override { return nullptr; }
#endif // ARKWEB_UNITTESTS
 private:
  LoadPolicy GetLoadPolicyImpl(const blink::WebURL& url);

  const std::vector<std::string> disallowed_path_suffixes_;
  const bool block_subresources_;
};

}  // namespace content

#endif  // CONTENT_WEB_TEST_RENDERER_FAKE_SUBRESOURCE_FILTER_H_

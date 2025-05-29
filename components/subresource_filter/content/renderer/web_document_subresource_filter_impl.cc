// Copyright 2017 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/subresource_filter/content/renderer/web_document_subresource_filter_impl.h"

#include <memory>
#include <utility>

#include "base/check.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/memory/scoped_refptr.h"
#include "base/not_fatal_until.h"
#include "base/task/single_thread_task_runner.h"
#include "components/subresource_filter/content/shared/renderer/filter_utils.h"
#include "components/subresource_filter/core/common/constants.h"
#include "components/subresource_filter/core/common/load_policy.h"
#include "components/subresource_filter/core/common/memory_mapped_ruleset.h"
#include "components/subresource_filter/core/mojom/subresource_filter.mojom.h"
#include "components/url_pattern_index/proto/rules.pb.h"
#include "third_party/blink/public/platform/web_url.h"
#include "third_party/blink/public/platform/web_url_request.h"
#include "url/gurl.h"
#include "url/origin.h"

namespace subresource_filter {

namespace proto = url_pattern_index::proto;

namespace {

using WebLoadPolicy = blink::WebDocumentSubresourceFilter::LoadPolicy;

WebLoadPolicy ToWebLoadPolicy(LoadPolicy load_policy) {
  switch (load_policy) {
    case LoadPolicy::EXPLICITLY_ALLOW:
      [[fallthrough]];
    case LoadPolicy::ALLOW:
      return WebLoadPolicy::kAllow;
    case LoadPolicy::DISALLOW:
      return WebLoadPolicy::kDisallow;
    case LoadPolicy::WOULD_DISALLOW:
      return WebLoadPolicy::kWouldDisallow;
    default:
      NOTREACHED_IN_MIGRATION();
      return WebLoadPolicy::kAllow;
  }
}

void ProxyToTaskRunner(scoped_refptr<base::SingleThreadTaskRunner> task_runner,
                       base::OnceClosure callback) {
  task_runner->PostTask(FROM_HERE, std::move(callback));
}

}  // namespace

WebDocumentSubresourceFilterImpl::~WebDocumentSubresourceFilterImpl() = default;

WebDocumentSubresourceFilterImpl::WebDocumentSubresourceFilterImpl(
    url::Origin document_origin,
    mojom::ActivationState activation_state,
    scoped_refptr<const MemoryMappedRuleset> ruleset,
    base::OnceClosure first_disallowed_load_callback)
    : activation_state_(activation_state),
      filter_(std::move(document_origin),
              activation_state,
              std::move(ruleset),
              kSafeBrowsingRulesetConfig.uma_tag),
      first_disallowed_load_callback_(
          std::move(first_disallowed_load_callback)) {}

WebLoadPolicy WebDocumentSubresourceFilterImpl::GetLoadPolicy(
    const blink::WebURL& resourceUrl,
    network::mojom::RequestDestination request_destination) {
  return getLoadPolicyImpl(resourceUrl, ToElementType(request_destination));
}

#if BUILDFLAG(ARKWEB_ADBLOCK)
void WebDocumentSubresourceFilterImpl::ClearStatistics() {
  filter_.ClearStatistics();
}

void WebDocumentSubresourceFilterImpl::SetDidFinishLoad(
    bool did_load_finished) {
  filter_.SetDidFinishLoad(did_load_finished);
}

std::unique_ptr<std::string>
WebDocumentSubresourceFilterImpl::GetElementHidingSelectors(
    const blink::WebURL& document_url,
    bool need_generic_selectors) {
  if (filter_.activation_state().filtering_disabled_for_document) {
    return std::make_unique<std::string>();
  }

  return filter_.GetSelectors(document_url, !need_generic_selectors);
}

bool WebDocumentSubresourceFilterImpl::HasGenericHideTypeOption(
    const blink::WebURL& document_url,
    const url::Origin& parent_document_origin) {
  return filter_.HasGenericHideTypeOption(document_url, parent_document_origin);
}

bool WebDocumentSubresourceFilterImpl::HasElemHideTypeOption(
    const blink::WebURL& document_url,
    const url::Origin& parent_document_origin) {
  return filter_.HasElemHideTypeOption(document_url, parent_document_origin);
}

bool WebDocumentSubresourceFilterImpl::HasDocumentTypeOption(
    const blink::WebURL& document_url,
    const url::Origin& parent_document_origin) {
  return filter_.HasDocumentTypeOption(document_url, parent_document_origin);
}

void WebDocumentSubresourceFilterImpl::DidMatchCssRule(
    const blink::WebURL& document_url,
    const std::string& dom_path,
    bool is_for_report) {
  filter_.DidMatchCssRule(document_url, dom_path, is_for_report);
}

bool WebDocumentSubresourceFilterImpl::GetDidFinishLoad() {
  return filter_.GetDidFinishLoad();
}

std::unique_ptr<std::vector<std::string>>
WebDocumentSubresourceFilterImpl::GetUserDomPathSelectors(
    const blink::WebURL& document_url,
    bool need_generic_selectors) {
  if (filter_.activation_state().filtering_disabled_for_document) {
    return nullptr;
  }
  return filter_.GetUserDomPathSelectors(document_url, !need_generic_selectors);
}

void WebDocumentSubresourceFilterImpl::set_activation_state(bool enabled) {
  if (enabled) {
    LOG(WARNING) << "[AdBlock] set activation level to enable";
    activation_state_.activation_level = mojom::ActivationLevel::kEnabled;
    filter_.set_activation_state(activation_state_);
  } else {
    LOG(WARNING) << "[AdBlock] set activation level to disable";
    activation_state_.activation_level = mojom::ActivationLevel::kDisabled;
    filter_.set_activation_state(activation_state_);
  }
}
#endif

WebLoadPolicy
WebDocumentSubresourceFilterImpl::GetLoadPolicyForWebSocketConnect(
    const blink::WebURL& url) {
  CHECK(url.ProtocolIs("ws") || url.ProtocolIs("wss"),
        base::NotFatalUntil::M129);
  return getLoadPolicyImpl(url, proto::ELEMENT_TYPE_WEBSOCKET);
}

WebLoadPolicy
WebDocumentSubresourceFilterImpl::GetLoadPolicyForWebTransportConnect(
    const blink::WebURL& url) {
  return getLoadPolicyImpl(url, proto::ELEMENT_TYPE_WEBTRANSPORT);
}

void WebDocumentSubresourceFilterImpl::ReportDisallowedLoad() {
  if (!first_disallowed_load_callback_.is_null()) {
    std::move(first_disallowed_load_callback_).Run();
  }
}

bool WebDocumentSubresourceFilterImpl::ShouldLogToConsole() {
  return activation_state().enable_logging;
}

WebLoadPolicy WebDocumentSubresourceFilterImpl::getLoadPolicyImpl(
    const blink::WebURL& url,
    proto::ElementType element_type) {
  if (filter_.activation_state().filtering_disabled_for_document ||
      url.ProtocolIs(url::kDataScheme)) {
    ++filter_.statistics().num_loads_total;
    return WebLoadPolicy::kAllow;
  }

  // TODO(pkalinnikov): Would be good to avoid converting to GURL.
  return ToWebLoadPolicy(filter_.GetLoadPolicy(GURL(url), element_type));
}

WebDocumentSubresourceFilterImpl::BuilderImpl::BuilderImpl(
    url::Origin document_origin,
    mojom::ActivationState activation_state,
    base::File ruleset_file,
    base::OnceClosure first_disallowed_load_callback)
    : document_origin_(std::move(document_origin)),
      activation_state_(std::move(activation_state)),
      ruleset_file_(std::move(ruleset_file)),
      first_disallowed_load_callback_(
          std::move(first_disallowed_load_callback)),
      main_task_runner_(base::SingleThreadTaskRunner::GetCurrentDefault()) {}

WebDocumentSubresourceFilterImpl::BuilderImpl::~BuilderImpl() = default;

std::unique_ptr<blink::WebDocumentSubresourceFilter>
WebDocumentSubresourceFilterImpl::BuilderImpl::Build() {
  CHECK(ruleset_file_.IsValid(), base::NotFatalUntil::M129);
  scoped_refptr<MemoryMappedRuleset> ruleset =
      MemoryMappedRuleset::CreateAndInitialize(std::move(ruleset_file_));
  if (!ruleset) {
    return nullptr;
  }
  return std::make_unique<WebDocumentSubresourceFilterImpl>(
      document_origin_, activation_state_, std::move(ruleset),
      base::BindOnce(&ProxyToTaskRunner, main_task_runner_,
                     std::move(first_disallowed_load_callback_)));
}

}  // namespace subresource_filter

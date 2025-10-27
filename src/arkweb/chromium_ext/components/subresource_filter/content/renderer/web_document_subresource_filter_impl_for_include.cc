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

namespace subresource_filter {

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

}  // namespace subresource_filter
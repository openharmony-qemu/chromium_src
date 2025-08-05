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

namespace extensions {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
// static
ExtensionDevToolsInfoBarDelegate*
ExtensionDevToolsInfoBarDelegate::GetDelegateById(
    const ExtensionId& extension_id) {
  LOG(INFO) << " func:" << __FUNCTION__ << " ExtensionId:" << extension_id;
  Delegates& delegates = g_delegates.Get();
  const auto it = delegates.find(extension_id);
  if (it != delegates.end()) {
    return it->second;
  }
  return nullptr;
}

// static
std::list<ExtensionId>
ExtensionDevToolsInfoBarDelegate::GetDelegateIds() {
  LOG(INFO) << " func:" << __FUNCTION__;
  std::list<ExtensionId> delegate_ids;
  for (auto& item : g_delegates.Get()) {
    delegate_ids.push_back(item.first);
  }
  return delegate_ids;
}

// static
void
ExtensionDevToolsInfoBarDelegate::CancelConfirmInfoBar(const std::string& extension_id) {
  LOG(INFO) << " func:" << __FUNCTION__ << " ExtensionId:" << extension_id;
  Delegates& delegates = g_delegates.Get();
  const auto it = delegates.find(extension_id);
  if (it != delegates.end()) {
    if (it->second) {
      it->second->Cancel();
    }
  }
}

bool ExtensionDevToolsInfoBarDelegate::Cancel() {
  LOG(INFO) << " func:" << __FUNCTION__ << " ExtensionId:" << extension_id_;

  callback_list_.Notify();
  return ConfirmInfoBarDelegate::Cancel();
}
#endif  // ARKWEB_ARKWEB_EXTENSIONS

}  // namespace extensions

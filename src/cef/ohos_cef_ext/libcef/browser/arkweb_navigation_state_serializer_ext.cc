/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_cef_ext/libcef/browser/arkweb_navigation_state_serializer_ext.h"

#include <span>

#include "base/logging.h"
#include "third_party/blink/public/common/page_state/page_state.h"

namespace {
  const int kMaxNavigationEntrySize = 1024;
} // namespace

CefRefPtr<CefBinaryValue> NavigationStateSerializer::WriteNavigationStatus(
    content::WebContents& web_contents) {
  content::NavigationController& controller = web_contents.GetController();
  if (!web_contents.GetController().GetLastCommittedEntry() ||
      web_contents.GetController().GetLastCommittedEntry()->IsInitialEntry()) {
    LOG(ERROR) << "navigation controller invalid";
    return nullptr;
  }
  base::Pickle pickle;
  int entry_count = controller.GetEntryCount();
  int entry_index = controller.GetLastCommittedEntryIndex();
  pickle.WriteInt(entry_count);
  pickle.WriteInt(entry_index);
  for (int index = 0; index < entry_count; ++index) {
    WriteNavigationEntry(*controller.GetEntryAtIndex(index), &pickle);
  }
  return CefBinaryValue::Create(pickle.data(), pickle.size());
}

void NavigationStateSerializer::WriteNavigationEntry(
    content::NavigationEntry& entry,
    base::Pickle* pickle) {
  if (!pickle) {
    return;
  }
  pickle->WriteString(entry.GetURL().spec());
  pickle->WriteString(entry.GetVirtualURL().spec());

  const content::Referrer& referrer = entry.GetReferrer();
  pickle->WriteString(referrer.url.spec());
  pickle->WriteInt(static_cast<int>(referrer.policy));
  pickle->WriteString16(entry.GetTitle());
  pickle->WriteString(entry.GetPageState().ToEncodedData());
  pickle->WriteBool(static_cast<int>(entry.GetHasPostData()));
  pickle->WriteString(entry.GetOriginalRequestURL().spec());
  pickle->WriteString(entry.GetBaseURLForDataURL().spec());
  pickle->WriteBool(static_cast<int>(entry.GetIsOverridingUserAgent()));
  pickle->WriteInt64(entry.GetTimestamp().ToInternalValue());
  pickle->WriteInt(entry.GetHttpStatusCode());
}

bool NavigationStateSerializer::RestoreNavigationStatus(
    content::WebContents& web_contents,
    const CefRefPtr<CefBinaryValue>& state) {
  if (!state) {
    LOG(ERROR) << "web state is nullptr.";
    return false;
  }
  size_t state_size = state->GetSize();
  if (state_size <= 0) {
    LOG(ERROR) << "web state size invalid.";
    return false;
  }
  uint8_t temp_buffer[state_size];
  state->GetData(temp_buffer, state_size, 0);
  std::span<const uint8_t> dataSpan(temp_buffer, state_size);
  base::Pickle pickle = base::Pickle::WithUnownedBuffer(dataSpan);
  base::PickleIterator iterator(pickle);
  int entry_count = -1;
  int entry_index = -2;
  if (!iterator.ReadInt(&entry_count) || !iterator.ReadInt(&entry_index) ||
      entry_index >= entry_count || entry_index > kMaxNavigationEntrySize) {
    LOG(ERROR) << "web state size invalid.";
    return false;
  }

  std::unique_ptr<content::NavigationEntryRestoreContext> context =
      content::NavigationEntryRestoreContext::Create();
  std::vector<std::unique_ptr<content::NavigationEntry>> entries;
  entries.reserve(entry_count);
  for (int i = 0; i < entry_count; ++i) {
    std::unique_ptr<content::NavigationEntry> entry =
        content::NavigationEntry::Create();
    if (!RestoreNavigationEntry(&iterator, entry, context.get())) {
      return false;
    }
    entries.push_back(std::move(entry));
  }

  content::NavigationController& controller = web_contents.GetController();
  controller.Restore(entry_index, content::RestoreType::kRestored, &entries);
  controller.LoadIfNecessary();
  return true;
}

bool NavigationStateSerializer::RestoreNavigationEntry(
    base::PickleIterator* iterator,
    std::unique_ptr<content::NavigationEntry>& entry,
    content::NavigationEntryRestoreContext* context) {
  if (!iterator || !entry || !context) {
    return false;
  }
  std::string url;
  std::string virtual_url;
  std::string original_request_url;
  std::string base_url_for_data_url;
  std::string referrer_url;
  int policy;
  std::u16string title;
  std::string content_state;
  bool has_post_data;
  bool is_overriding_user_agent;
  int64_t timestamp;
  int http_status_code;

  if (!iterator->ReadString(&url) || !iterator->ReadString(&virtual_url) ||
      !iterator->ReadString(&referrer_url) || !iterator->ReadInt(&policy) ||
      !iterator->ReadString16(&title) ||
      !iterator->ReadString(&content_state) ||
      !iterator->ReadBool(&has_post_data) ||
      !iterator->ReadString(&original_request_url) ||
      !iterator->ReadString(&base_url_for_data_url) ||
      !iterator->ReadBool(&is_overriding_user_agent) ||
      !iterator->ReadInt64(&timestamp) ||
      !iterator->ReadInt(&http_status_code)) {
    LOG(ERROR) << "restore navigation entry failed.";
    return false;
  }

  GURL deserialized_url;
  deserialized_url = GURL(url);
  entry->SetURL(deserialized_url);
  entry->SetVirtualURL(GURL(virtual_url));
  entry->SetTitle(title);
  content::Referrer deserialized_referrer;
  deserialized_referrer.url = GURL(referrer_url);
  deserialized_referrer.policy = content::Referrer::ConvertToPolicy(policy);
  if (content_state.empty()) {
    entry->SetPageState(blink::PageState::CreateFromURL(deserialized_url),
                        context);
    entry->SetReferrer(deserialized_referrer);
  } else {
    entry->SetPageState(blink::PageState::CreateFromEncodedData(content_state),
                        context);
  }
  entry->SetHasPostData(has_post_data);
  entry->SetOriginalRequestURL(GURL(original_request_url));
  entry->SetBaseURLForDataURL(GURL(base_url_for_data_url));
  entry->SetIsOverridingUserAgent(is_overriding_user_agent);
  entry->SetTimestamp(base::Time::FromInternalValue(timestamp));
  entry->SetHttpStatusCode(http_status_code);
  return true;
}

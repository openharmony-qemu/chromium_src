/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "nweb_extension_reading_list_cef_delegate.h"

#include <map>

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "base/logging.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/reading_list/reading_list_event_router_ext.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "ohos_nweb_ex/core/extension/nweb_extension_reading_list_dispatcher.h"
#endif

namespace OHOS::NWeb {

namespace {

static int g_request_id = 0;

static std::map<int, AddEntryResultFunc> g_add_entry_result_funcs;

static std::map<int, QueryEntryResultFunc> g_query_entry_result_funcs;

static std::map<int, RemoveEntryResultFunc> g_remove_entry_result_funcs;

static std::map<int, UpdateEntryResultFunc> g_update_entry_result_funcs;

}  // namespace

NWebExtensionReadingListCefDelegate&
NWebExtensionReadingListCefDelegate::GetInstance() {
  static NWebExtensionReadingListCefDelegate instance;
  return instance;
}

bool NWebExtensionReadingListCefDelegate::AddEntry(
    const NWebAddEntryOptions& options,
    AddEntryResultFunc callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  int id = g_request_id++;
  g_add_entry_result_funcs[id] = std::move(callback);

  if (NWebExtensionReadingListDispatcher::GetInstance().AddEntry(id, options)) {
    LOG(INFO) << "succeed to call add entry,id is " << id;
    return true;
  }

  LOG(WARNING) << "failed to call add entry,id is " << id;
  g_add_entry_result_funcs.erase(id);
#endif
  return false;
}

bool NWebExtensionReadingListCefDelegate::QueryEntry(
    const NWebQueryEntryOptions& options,
    QueryEntryResultFunc callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  int id = g_request_id++;
  g_query_entry_result_funcs[id] = std::move(callback);

  if (NWebExtensionReadingListDispatcher::GetInstance().QueryEntry(id,
                                                                   options)) {
    LOG(INFO) << "succeed to call query entry,id is " << id;
    return true;
  }

  LOG(WARNING) << "failed to call query entry,id is " << id;
  g_query_entry_result_funcs.erase(id);
#endif
  return false;
}

bool NWebExtensionReadingListCefDelegate::RemoveEntry(
    const std::string& url,
    RemoveEntryResultFunc callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  int id = g_request_id++;
  g_remove_entry_result_funcs[id] = std::move(callback);

  if (NWebExtensionReadingListDispatcher::GetInstance().RemoveEntry(id, url)) {
    LOG(INFO) << "succeed to call remove entry,id is " << id;
    return true;
  }

  LOG(WARNING) << "failed to call remove entry,id is " << id;
  g_remove_entry_result_funcs.erase(id);
#endif
  return false;
}

bool NWebExtensionReadingListCefDelegate::UpdateEntry(
    const NWebUpdateEntryOptions& options,
    UpdateEntryResultFunc callback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  int id = g_request_id++;
  g_update_entry_result_funcs[id] = std::move(callback);

  if (NWebExtensionReadingListDispatcher::GetInstance().UpdateEntry(id,
                                                                    options)) {
    LOG(INFO) << "succeed to call update entry,id is " << id;
    return true;
  }

  LOG(WARNING) << "failed to call update entry,id is " << id;
  g_update_entry_result_funcs.erase(id);
#endif
  return false;
}

void NWebExtensionReadingListCefDelegate::OnEntryAdded(
    const NWebReadingListEntry& entry) {
  auto browser_context = GetBrowserContext();
  if (!browser_context) {
    return;
  }

  extensions::CefReadingListEventRouter::GetInstance().ReadingListDidAddEntry(
      entry, browser_context);
}

void NWebExtensionReadingListCefDelegate::OnEntryRemoved(
    const NWebReadingListEntry& entry) {
  auto browser_context = GetBrowserContext();
  if (!browser_context) {
    return;
  }

  extensions::CefReadingListEventRouter::GetInstance()
      .ReadingListWillRemoveEntry(entry, browser_context);
}

void NWebExtensionReadingListCefDelegate::OnEntryUpdated(
    const NWebReadingListEntry& entry) {
  auto browser_context = GetBrowserContext();
  if (!browser_context) {
    return;
  }

  extensions::CefReadingListEventRouter::GetInstance()
      .ReadingListDidUpdateEntry(entry, browser_context);
}

void NWebExtensionReadingListCefDelegate::AddEntryResult(
    int id,
    const std::string& error) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (g_add_entry_result_funcs.count(id)) {
    LOG(INFO) << "succeed to find add entry result func,id is " << id
              << ",error is " << error;
    std::move(g_add_entry_result_funcs[id]).Run(error);
    g_add_entry_result_funcs.erase(id);
    return;
  }

  LOG(WARNING) << "failed to find add entry result func,id is " << id
               << ",error is " << error;
#endif
}

void NWebExtensionReadingListCefDelegate::QueryEntryResult(
    int id,
    const std::string& error,
    const std::vector<NWebReadingListEntry>& entries) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (g_query_entry_result_funcs.count(id)) {
    LOG(INFO) << "succeed to find query entry result func,id is " << id
              << ",error is " << error;
    std::move(g_query_entry_result_funcs[id]).Run(error, entries);
    g_query_entry_result_funcs.erase(id);
    return;
  }

  LOG(WARNING) << "failed to find query entry result func,id is " << id
               << ",error is " << error;
#endif
}

void NWebExtensionReadingListCefDelegate::RemoveEntryResult(
    int id,
    const std::string& error) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (g_remove_entry_result_funcs.count(id)) {
    LOG(INFO) << "succeed to find remove entry result func,id is " << id
              << ",error is " << error;
    std::move(g_remove_entry_result_funcs[id]).Run(error);
    g_remove_entry_result_funcs.erase(id);
    return;
  }

  LOG(WARNING) << "failed to find remove entry result func,id is " << id
               << ",error is " << error;
#endif
}

void NWebExtensionReadingListCefDelegate::UpdateEntryResult(
    int id,
    const std::string& error) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
  if (g_update_entry_result_funcs.count(id)) {
    LOG(INFO) << "succeed to find update entry result func,id is " << id
              << ",error is " << error;
    std::move(g_update_entry_result_funcs[id]).Run(error);
    g_update_entry_result_funcs.erase(id);
    return;
  }

  LOG(WARNING) << "failed to find update entry result func,id is " << id
               << ",error is " << error;
#endif
}

}  // namespace OHOS::NWeb

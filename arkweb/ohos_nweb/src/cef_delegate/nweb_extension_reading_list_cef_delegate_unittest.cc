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


#include <atomic>
#include <gtest/gtest.h>
#include <string>

#include "nweb_extension_reading_list_cef_delegate.h"
#include "base/logging.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/reading_list/reading_list_event_router_ext.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_reading_list_dispatcher.h"
#endif

using namespace std;

namespace OHOS::NWeb {

bool g_func_add_entry = false;
bool g_result_add_entry = false;
int g_id_add_entry = 0;
#if BUILDFLAG(ARKWEB_NWEB_EX)
bool NotifyAddEntry(int id, const ArkWebPbBuffer* buffer) {
    g_id_add_entry = id;
    return g_result_add_entry;
}
#endif

bool g_func_query_entry = false;
bool g_result_query_entry = false;
int g_id_query_entry = 0;
#if BUILDFLAG(ARKWEB_NWEB_EX)
bool NotifyQueryEntry(int id, const ArkWebPbBuffer* buffer) {
    g_id_query_entry = id;
    return g_result_query_entry;
}
#endif

bool g_func_remove_entry = false;
bool g_result_remove_entry = false;
int g_id_remove_entry = 0;
#if BUILDFLAG(ARKWEB_NWEB_EX)
bool NotifyRemoveEntry(int id, const ArkWebPbBuffer* buffer) {
    g_id_remove_entry = id;
    return g_result_remove_entry;
}
#endif

bool g_func_update_entry = false;
bool g_result_update_entry = false;
int g_id_update_entry = 0;
#if BUILDFLAG(ARKWEB_NWEB_EX)
bool NotifyUpdateEntry(int id, const ArkWebPbBuffer* buffer) {
    g_id_update_entry = id;
    return g_result_update_entry;
}
#endif

class NWebExtensionReadingListCefDelegateTest : public testing::Test {
protected:
    void SetUp() override {
        delegate = &NWebExtensionReadingListCefDelegate::GetInstance();
        {
#if BUILDFLAG(ARKWEB_NWEB_EX)
            OnReadingListAddEntryFunc func_add = &NotifyAddEntry;
            NWebExtensionReadingListDispatcher::GetInstance().SetAddEntryFunc(func_add);

            OnReadingListQueryEntryFunc func_query = &NotifyQueryEntry;
            NWebExtensionReadingListDispatcher::GetInstance().SetQueryEntryFunc(func_query);

            OnReadingListRemoveEntryFunc func_remove = &NotifyRemoveEntry;
            NWebExtensionReadingListDispatcher::GetInstance().SetRemoveEntryFunc(func_remove);

            OnReadingListUpdateEntryFunc func_update = &NotifyUpdateEntry;
            NWebExtensionReadingListDispatcher::GetInstance().SetUpdateEntryFunc(func_update);
#endif
        }
    }

    void TearDown() override {
        delegate = nullptr;
    }

    NWebExtensionReadingListCefDelegate *delegate;
};

TEST_F(NWebExtensionReadingListCefDelegateTest, TestGetInstance) {
    auto &instance1 = NWebExtensionReadingListCefDelegate::GetInstance();
    auto &instance2 = NWebExtensionReadingListCefDelegate::GetInstance();
    EXPECT_EQ(&instance1, &instance2);
}

TEST_F(NWebExtensionReadingListCefDelegateTest, TestOnEntryAdded) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebReadingListEntry entry;
    entry.hasBeenRead = false;
    entry.creationTime = 0;
    entry.lastUpdateTime = 0;
    entry.url = "https://example.com";
    entry.title = "title example.com";
    delegate->OnEntryAdded(entry);
    EXPECT_EQ(entry.hasBeenRead, false);
#endif
}

TEST_F(NWebExtensionReadingListCefDelegateTest, TestOnEntryRemoved) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebReadingListEntry entry;
    entry.hasBeenRead = false;
    entry.creationTime = 0;
    entry.lastUpdateTime = 0;
    entry.url = "https://example.com";
    entry.title = "title example.com";
    delegate->OnEntryRemoved(entry);
    EXPECT_EQ(entry.hasBeenRead, false);
#endif
}

TEST_F(NWebExtensionReadingListCefDelegateTest, TestOnEntryUpdated) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    NWebReadingListEntry entry;
    entry.hasBeenRead = false;
    entry.creationTime = 0;
    entry.lastUpdateTime = 0;
    entry.url = "https://example.com";
    entry.title = "title example.com";
    delegate->OnEntryUpdated(entry);
    EXPECT_EQ(entry.hasBeenRead, false);
#endif
}

void AddEntryResult(const std::string& msg)
{
    g_func_add_entry = true;
}

TEST_F(NWebExtensionReadingListCefDelegateTest, TestddEntry_AddEntryResult) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    std::string error = "test entry result...";
    int id = 0;
    NWebAddEntryOptions options;
    options.url = "https://example.com";
    options.title = "title example.com";
    options.hasBeenRead = false;
    AddEntryResultFunc callback = base::BindRepeating(&AddEntryResult);

    g_result_add_entry = false;
    bool result = delegate->AddEntry(options, callback);
    EXPECT_EQ(result, false);

    g_result_add_entry = true;
    result = delegate->AddEntry(options, callback);
    EXPECT_EQ(result, true);

    g_func_add_entry = false;
    id = g_id_add_entry;
    delegate->AddEntryResult(id, error);
    EXPECT_EQ(g_func_add_entry, true);

    g_func_add_entry = false;
    id = g_id_add_entry + 2000;
    delegate->AddEntryResult(id, error);
    EXPECT_EQ(g_func_add_entry, false);
#endif
}

void QueryEntry(const std::string& msg,
                      const std::vector<NWebReadingListEntry>& entries)
{
    g_func_query_entry = true;
}

TEST_F(NWebExtensionReadingListCefDelegateTest, TestQueryEntry_QueryEntryResult) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    std::string error = "test query result...";
    int id = 0;
    NWebQueryEntryOptions options;
    options.url = "https://example.com";
    options.title = "title example.com";
    options.hasBeenRead = false;
    QueryEntryResultFunc callback = base::BindRepeating(&QueryEntry);

    g_result_query_entry = false;
    bool result = delegate->QueryEntry(options, callback);
    EXPECT_EQ(result, false);

    g_result_query_entry = true;
    result = delegate->QueryEntry(options, callback);
    EXPECT_EQ(result, true);

    g_func_query_entry = false;
    std::vector<NWebReadingListEntry> entries;
    id = g_id_query_entry;
    delegate->QueryEntryResult(id, error, entries);
    EXPECT_EQ(g_func_query_entry, true);

    g_func_query_entry = false;
    id = g_id_query_entry + 2000;
    delegate->QueryEntryResult(id, error, entries);
    EXPECT_EQ(g_func_query_entry, false);
#endif
}

void RemoveEntry(const std::string& msg)
{
    g_func_remove_entry = true;
}

TEST_F(NWebExtensionReadingListCefDelegateTest, TestRemoveEntry_RemoveEntryResult) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    std::string error = "test remove result...";
    int id = 0;
    std::string url = "https://example.com";
    RemoveEntryResultFunc callback = base::BindRepeating(&RemoveEntry);
    
    g_result_remove_entry = false;
    bool result = delegate->RemoveEntry(url, callback);
    EXPECT_EQ(result, false);

    g_result_remove_entry = true;
    result = delegate->RemoveEntry(url, callback);
    EXPECT_EQ(result, true);

    g_func_remove_entry = false;
    id = g_id_remove_entry;
    delegate->RemoveEntryResult(id, error);
    EXPECT_EQ(g_func_remove_entry, true);

    g_func_remove_entry = false;
    id = g_id_remove_entry + 2000;
    delegate->RemoveEntryResult(id, error);
    EXPECT_EQ(g_func_remove_entry, false);
#endif
}

void UpdateEntry(const std::string& msg)
{
    g_func_update_entry = true;
}

TEST_F(NWebExtensionReadingListCefDelegateTest, TestUpdateEntry_UpdateEntryResult) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    std::string error = "test update result...";
    int id = 0;
    NWebUpdateEntryOptions options;
    options.url = "https://example.com";
    options.title = "title example.com";
    options.hasBeenRead = false;
    UpdateEntryResultFunc callback = base::BindRepeating(&UpdateEntry);
    
    g_result_update_entry = false;
    bool result = delegate->UpdateEntry(options, callback);
    EXPECT_EQ(result, false);

    g_result_update_entry = true;
    result = delegate->UpdateEntry(options, callback);
    EXPECT_EQ(result, true);

    g_func_update_entry = false;
    id = g_id_update_entry;
    delegate->UpdateEntryResult(id, error);
    EXPECT_EQ(g_func_update_entry, true);

    g_func_update_entry = false;
    id = g_id_update_entry + 2000;
    delegate->UpdateEntryResult(id, error);
    EXPECT_EQ(g_func_update_entry, false);
#endif
}

}  // namespace
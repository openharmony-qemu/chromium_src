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

#include "nweb_extension_history_cef_delegate.h"

#include "base/logging.h"
#include "cef/libcef/browser/request_context_impl.h"
#include "cef/ohos_cef_ext/libcef/browser/extensions/api/history/cef_history_event_router.h"
#include "content/public/browser/browser_context.h"
#include "nweb_extension_utils.h"

#if BUILDFLAG(ARKWEB_NWEB_EX)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "ohos_nweb_ex/core/extension/nweb_extension_history_dispatcher.h"
#endif

using namespace std;

namespace OHOS::NWeb {

bool g_callback_history_delete_range = false;
bool g_func_history_delete_range = false;
int g_history_delete_requestId = 0;
#if BUILDFLAG(ARKWEB_NWEB_EX)
void NotifyDeleteRange(int request_id, const ArkWebPbBuffer* buffer) {
    g_history_delete_requestId = request_id;
    g_func_history_delete_range = true;
}
#endif

bool g_callback_get_visits = true;
bool g_func_history_get_visits = false;
int g_history_get_visits_requestId = 0;
#if BUILDFLAG(ARKWEB_NWEB_EX)
void NotifyGetVisits(int request_id, const ArkWebPbBuffer* buffer) {
    g_history_get_visits_requestId = request_id;
    g_func_history_get_visits = true;
}
#endif

class NWebExtensionHistoryCefDelegateTest : public testing::Test {
protected:
    void SetUp() override {
        delegate = NWebExtensionHistoryCefDelegate::GetInstance();
        {
#if BUILDFLAG(ARKWEB_NWEB_EX)
            callback_ = NWebExtensionHistoryDispatcher::GetInstance().CreateCallback();
#endif
        }
    }

    void TearDown() override {
        callback_ = nullptr;
        delegate = nullptr;
    }

    NWebExtensionHistoryCefDelegate *delegate;
    void* callback_ = nullptr;
};

TEST_F(NWebExtensionHistoryCefDelegateTest, TestGetInstance) {
    auto instance1 = NWebExtensionHistoryCefDelegate::GetInstance();
    auto instance2 = NWebExtensionHistoryCefDelegate::GetInstance();
    EXPECT_EQ(instance1, instance2);
}

void HistoryGetVisits(const std::string& error,
    const std::vector<NWebExtensionVisitItem>& results)
{
    g_callback_get_visits = true;
}

TEST_F(NWebExtensionHistoryCefDelegateTest, TestGetVisits_VisitsCallback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    std::string url = "https://example.com";
    std::string error = "test get visits callback...";
    std::vector<NWebExtensionVisitItem> results;
    HistoryGetVisitsCallback callback = base::BindRepeating(&HistoryGetVisits);

    bool result = delegate->GetVisits(url, callback);
    EXPECT_EQ(result, false);

    ((NWebExtensionHistoryApiCallback*)callback_)->get_visits_ = &NotifyGetVisits;
    result = delegate->GetVisits(url, callback);
    EXPECT_EQ(result, true);

    g_callback_get_visits = false;
    delegate->GetVisitsCallback(g_history_get_visits_requestId, error, results);
    EXPECT_EQ(g_callback_get_visits, true);

    g_callback_get_visits = false;
    int requestId = g_history_get_visits_requestId + 200;
    delegate->GetVisitsCallback(requestId, error, results);
    EXPECT_EQ(g_callback_get_visits, false);
#endif
}

void HistoryDeleteRange(const std::string& msg)
{
    g_callback_history_delete_range = true;
}

TEST_F(NWebExtensionHistoryCefDelegateTest, TestDeleteRange_DeleteRangeCallback) {
#if BUILDFLAG(ARKWEB_NWEB_EX)
    std::string error = "test delete range callback...";
    int64_t start_time = 1900;
    int64_t end_time = 2000;
    HistoryDeleteRangeCallback callback = base::BindRepeating(&HistoryDeleteRange);

    bool result = delegate->DeleteRange(start_time, end_time, callback);
    EXPECT_EQ(result, false);

    ((NWebExtensionHistoryApiCallback*)callback_)->delete_range_ = &NotifyDeleteRange;
    result = delegate->DeleteRange(start_time, end_time, callback);
    EXPECT_EQ(result, true);

    g_callback_history_delete_range = false;
    delegate->DeleteRangeCallback(g_history_delete_requestId, error);
    EXPECT_EQ(g_callback_history_delete_range, true);

    g_callback_history_delete_range = false;
    int requestId = g_history_delete_requestId + 200;
    delegate->DeleteRangeCallback(requestId, error);
    EXPECT_EQ(g_callback_history_delete_range, false);
#endif
}

}  // namespace
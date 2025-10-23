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
#include "nweb_web_storage_delegate.h"

#include <memory>
#include <thread>
#include <vector>
#include <gmock/gmock.h>

#include "base/logging.h"
#include "base/synchronization/waitable_event.h"
#include "base/time/time.h"
#include "cef/include/cef_waitable_event.h"
#include "cef/libcef/common/time_util.h"
#include "nweb_web_storage_origin_impl.h"
#include "url/gurl.h"
#include "gtest/gtest.h"

#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif

using namespace testing;
using namespace OHOS::NWeb;
using base::WaitableEvent;

TEST(NWebWebStorageDelegateTEST, DeleteAllData) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->DeleteAllData(false);
}

TEST(NWebWebStorageDelegateTEST, DeleteOrigin001) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    EXPECT_EQ(web_storage->DeleteOrigin(""), OHOS::NWeb::NWEB_INVALID_ORIGIN);
}

TEST(NWebWebStorageDelegateTEST, DeleteOrigin002) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    EXPECT_EQ(web_storage->DeleteOrigin("http://origin1"), -1);
}

TEST(NWebWebStorageDelegateTEST, GetOrigins001) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->GetOrigins(nullptr);
}

TEST(NWebWebStorageDelegateTEST, GetOrigins002) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    std::vector<std::shared_ptr<NWebWebStorageOrigin>> origins;
    web_storage->GetOrigins(origins);
}

TEST(NWebWebStorageDelegateTEST, GetOriginQuota001) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->GetOriginQuota("", nullptr);
}

TEST(NWebWebStorageDelegateTEST, GetOriginQuota002) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->GetOriginQuota("");
}

TEST(NWebWebStorageDelegateTEST, GetOriginUsage001) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->GetOriginUsage("", nullptr);
}

TEST(NWebWebStorageDelegateTEST, GetOriginUsage002) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->GetOriginUsage("");
}

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
TEST(NWebWebStorageDelegateTEST, GetPassword) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->GetPassword("", "", 1);
}

TEST(NWebWebStorageDelegateTEST, GetSavedPasswordsInfo) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->GetSavedPasswordsInfo(1);
}

TEST(NWebWebStorageDelegateTEST, MigratePasswordsInfo) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->MigratePasswordsInfo();
}

TEST(NWebWebStorageDelegateTEST, RegisterWebStorageExtensionCallback) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    std::shared_ptr<NWebStorageExtensionCallback>
        web_storage_extension_callback = nullptr;
    web_storage->RegisterWebStorageExtensionCallback(web_storage_extension_callback);
}

TEST(NWebWebStorageDelegateTEST, ClearPassword) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->ClearPassword();
}

TEST(NWebWebStorageDelegateTEST, RemovePassword) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->RemovePassword("", "");
}

TEST(NWebWebStorageDelegateTEST, RemovePasswordByUrl) {
    auto web_storage = std::make_shared<NWebWebStorageDelegate>();
    web_storage->RemovePasswordByUrl("");
}
#endif
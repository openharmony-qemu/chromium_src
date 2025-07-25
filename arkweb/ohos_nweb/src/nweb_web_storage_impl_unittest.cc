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


#include "nweb_web_storage_impl.h"

#include <gmock/gmock.h>

#include "arkweb/ohos_nweb_ex/build/features/features.h"
#include "nweb_hilog.h"
#include "nweb_web_storage_delegate.h"
#include "gtest/gtest.h"

using namespace OHOS::NWeb;

TEST(NWebWebStorageImplTEST, DeleteAllData) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->DeleteAllData(false);
}

TEST(NWebWebStorageImplTEST, DeleteOrigin) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    std::string origin = "";
    EXPECT_EQ(web_storage->DeleteOrigin(origin), OHOS::NWeb::NWEB_INVALID_ORIGIN);
}

TEST(NWebWebStorageImplTEST, GetOrigins001) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->GetOrigins(nullptr);
}

TEST(NWebWebStorageImplTEST, GetOrigins002) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    std::vector<std::shared_ptr<NWebWebStorageOrigin>> origins 
                                    = web_storage->GetOrigins();
    EXPECT_TRUE(origins.empty());
}

TEST(NWebWebStorageImplTEST, GetOriginQuota001) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->GetOriginQuota("", nullptr);

}

TEST(NWebWebStorageImplTEST, GetOriginQuota002) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    EXPECT_EQ(web_storage->GetOriginQuota(""), -1);
}

TEST(NWebWebStorageImplTEST, GetOriginUsage001) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->GetOriginQuota("", nullptr);
}

TEST(NWebWebStorageImplTEST, GetOriginUsage002) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    EXPECT_EQ(web_storage->GetOriginQuota(""), -1);
}

TEST(NWebWebStorageImplTEST, PutWebStorageCallback) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->PutWebStorageCallback(nullptr);
}

TEST(NWebWebStorageImplTEST, GetPassword) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    const std::string url = "";
    const std::string username = "";
    EXPECT_EQ(web_storage->GetPassword(url, username, 1), "");
}

TEST(NWebWebStorageImplTEST, GetSavedPasswords) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->GetSavedPasswords(1);
}

TEST(NWebWebStorageImplTEST, MigratePasswords) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->MigratePasswords();
}

TEST(NWebWebStorageImplTEST, ClearPassword) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->ClearPassword();
}

TEST(NWebWebStorageImplTEST, RemovePassword) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    const std::string url = "";
    const std::string username = "";
    web_storage->RemovePassword(url, username);
}

TEST(NWebWebStorageImplTEST, ModifyPassword) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    const std::string url = "";
    const std::string old1 = "";
    const std::string new1 = "";
    const std::string new2 = "";
    web_storage->ModifyPassword(url, old1, new1, new2);
}

TEST(NWebWebStorageImplTEST, RemovePasswordByUrl) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    const std::string url = "";
    web_storage->RemovePasswordByUrl(url);
}
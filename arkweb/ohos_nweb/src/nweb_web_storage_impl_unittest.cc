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

#include <gmock/gmock.h>
#include "arkweb/build/features/features.h"	
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "nweb_hilog.h"
#include "nweb_web_storage_delegate.h"
#include "gtest/gtest.h"

#define private public
#include "nweb_web_storage_impl.h"

using namespace OHOS::NWeb;

TEST(NWebWebStorageImplTEST, DeleteAllData) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->DeleteAllData(false);
}

TEST(NWebWebStorageImplTEST, DeleteAllData_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  web_storage->DeleteAllData(false);
}

TEST(NWebWebStorageImplTEST, DeleteOrigin) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    std::string origin = "";
    EXPECT_EQ(web_storage->DeleteOrigin(origin), OHOS::NWeb::NWEB_INVALID_ORIGIN);
}

TEST(NWebWebStorageImplTEST, DeleteOrigin_DelegateNotNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  std::string origin = "";
  web_storage->delegate_ = nullptr;
  EXPECT_EQ(web_storage->DeleteOrigin(origin), OHOS::NWeb::NWEB_ERR);
}

TEST(NWebWebStorageImplTEST, GetOrigins001) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->GetOrigins(nullptr);
}

TEST(NWebWebStorageImplTEST, GetOrigins001_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  web_storage->GetOrigins(nullptr);
}

TEST(NWebWebStorageImplTEST, GetOrigins002) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    std::vector<std::shared_ptr<NWebWebStorageOrigin>> origins 
                                    = web_storage->GetOrigins();
    EXPECT_TRUE(origins.empty());
}

TEST(NWebWebStorageImplTEST, GetOrigins002_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  std::vector<std::shared_ptr<NWebWebStorageOrigin>> origins
                                  = web_storage->GetOrigins();
  EXPECT_TRUE(origins.empty());
}

TEST(NWebWebStorageImplTEST, GetOriginQuota001) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->GetOriginQuota("", nullptr);
}

TEST(NWebWebStorageImplTEST, GetOriginQuota001_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  web_storage->GetOriginQuota("", nullptr);
}

TEST(NWebWebStorageImplTEST, GetOriginQuota002) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    EXPECT_EQ(web_storage->GetOriginQuota(""), -1);
}

TEST(NWebWebStorageImplTEST, GetOriginQuota002_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  EXPECT_EQ(web_storage->GetOriginQuota(""), -1);
}

TEST(NWebWebStorageImplTEST, GetOriginUsage001) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->GetOriginUsage("", nullptr);
}

TEST(NWebWebStorageImplTEST, GetOriginUsage002) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    EXPECT_EQ(web_storage->GetOriginUsage(""), -1);
}

TEST(NWebWebStorageImplTEST, GetOriginUsage003) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  EXPECT_EQ(web_storage->GetOriginUsage(""), -1);
}

TEST(NWebWebStorageImplTEST, GetOriginUsage004) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  ASSERT_NO_FATAL_FAILURE(web_storage->GetOriginUsage("", nullptr));
}

TEST(NWebWebStorageImplTEST, PutWebStorageCallback) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->PutWebStorageCallback(nullptr);
}

TEST(NWebWebStorageImplTEST, PutWebStorageCallback_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  web_storage->PutWebStorageCallback(nullptr);
}

TEST(NWebWebStorageImplTEST, GetPassword) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    const std::string url = "";
    const std::string username = "";
    EXPECT_EQ(web_storage->GetPassword(url, username, 1), "");
}

TEST(NWebWebStorageImplTEST, GetPassword002) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  const std::string url = "";
  const std::string username = "";
  web_storage->delegate_ = nullptr;
  EXPECT_EQ(web_storage->GetPassword(url, username, 1), "");
}

TEST(NWebWebStorageImplTEST, GetSavedPasswords) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->GetSavedPasswords(1);
}

TEST(NWebWebStorageImplTEST, GetSavedPasswords_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  web_storage->GetSavedPasswords(1);
}

TEST(NWebWebStorageImplTEST, MigratePasswords) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->MigratePasswords();
}

TEST(NWebWebStorageImplTEST, MigratePasswords_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  web_storage->MigratePasswords();
}

TEST(NWebWebStorageImplTEST, ClearPassword) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    web_storage->ClearPassword();
}

TEST(NWebWebStorageImplTEST, ClearPassword_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  web_storage->ClearPassword();
}

TEST(NWebWebStorageImplTEST, RemovePassword) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    const std::string url = "";
    const std::string username = "";
    web_storage->RemovePassword(url, username);
}

TEST(NWebWebStorageImplTEST, RemovePassword_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  const std::string url = "";
  const std::string username = "";
  web_storage->delegate_ = nullptr;
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

TEST(NWebWebStorageImplTEST, ModifyPassword_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  const std::string url = "";
  const std::string old1 = "";
  const std::string new1 = "";
  const std::string new2 = "";
  web_storage->delegate_ = nullptr;
  web_storage->ModifyPassword(url, old1, new1, new2);
}

TEST(NWebWebStorageImplTEST, RemovePasswordByUrl) {
    auto web_storage = std::make_shared<NWebWebStorageImpl>();
    const std::string url = "";
    web_storage->RemovePasswordByUrl(url);
}

TEST(NWebWebStorageImplTEST, RemovePasswordByUrl_DelegateNull) {
  auto web_storage = std::make_shared<NWebWebStorageImpl>();
  web_storage->delegate_ = nullptr;
  const std::string url = "";
  web_storage->RemovePasswordByUrl(url);
}
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

#include <cstring>
#include <cstdint>
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#define private public
#include "arkweb/ohos_adapter_ndk/keystore_adapter/keystore_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/keystore_adapter/keystore_adapter_impl.cpp"

using namespace OHOS::NWeb;
using namespace testing;

namespace {

class KeystoreAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void KeystoreAdapterImplTest::SetUpTestCase(void)
{}

void KeystoreAdapterImplTest::TearDownTestCase(void)
{}

void KeystoreAdapterImplTest::SetUp(void)
{}

void KeystoreAdapterImplTest::TearDown(void)
{}

TEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_InitParamSet_001)
{
    struct OH_Huks_ParamSet *paramSet;
    int32_t result = KeystoreAdapterImpl::GetInstance().InitParamSet(
        nullptr, g_genEncDecParams, sizeof(g_genEncDecParams) / sizeof(OH_Huks_Param));
    EXPECT_EQ(result, 401);
    result = KeystoreAdapterImpl::GetInstance().InitParamSet(
        &paramSet, nullptr, sizeof(g_genEncDecParams) / sizeof(OH_Huks_Param));
    EXPECT_EQ(result, 401);
    result = KeystoreAdapterImpl::GetInstance().InitParamSet(
        &paramSet, g_genEncDecParams, sizeof(g_genEncDecParams) / sizeof(OH_Huks_Param));
    EXPECT_EQ(result, 0);
}

TEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_EncryptKey_002)
{
    std::string alias = "test";
    std::string plainData = "web_test";
    std::string encryptString = KeystoreAdapterImpl::GetInstance().EncryptKey(alias, plainData);
    EXPECT_TRUE(encryptString.empty());
    std::string DecryptString = KeystoreAdapterImpl::GetInstance().DecryptKey(alias, encryptString);
    EXPECT_TRUE(DecryptString.empty());
    encryptString = KeystoreAdapterImpl::GetInstance().EncryptKey(alias, "");
    EXPECT_TRUE(encryptString.empty());
    DecryptString = KeystoreAdapterImpl::GetInstance().DecryptKey(alias, "test");
    EXPECT_TRUE(DecryptString.empty());

    std::string nullAlias = "";
    std::string nullPlainData = "";
    std::string nullEncrypt = KeystoreAdapterImpl::GetInstance().EncryptKey(nullAlias, nullPlainData);
    EXPECT_TRUE(nullEncrypt.empty());
    std::string nullDecrypt = KeystoreAdapterImpl::GetInstance().DecryptKey(nullAlias, nullPlainData);
    EXPECT_TRUE(DecryptString.empty());

    std::string long_str_alias(2048, 'a');
    nullEncrypt = KeystoreAdapterImpl::GetInstance().EncryptKey(long_str_alias, "test");
    EXPECT_TRUE(nullEncrypt.empty());
    nullDecrypt = KeystoreAdapterImpl::GetInstance().DecryptKey(long_str_alias, "test");
    EXPECT_TRUE(DecryptString.empty());
}

TEST_F(KeystoreAdapterImplTest, KeystoreAdapterImplTest_AssetQuery_003)
{
    std::string alias = "test";
    std::string encryptString = KeystoreAdapterImpl::GetInstance().AssetQuery(alias);
    EXPECT_TRUE(encryptString.empty());

    std::string nullAlias = "";
    std::string nullEncrypt = KeystoreAdapterImpl::GetInstance().AssetQuery(nullAlias);
    EXPECT_TRUE(nullEncrypt.empty());
}
}
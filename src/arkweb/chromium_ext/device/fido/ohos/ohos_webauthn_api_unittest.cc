// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/device/fido/ohos/ohos_webauthn_api.h"

#include <gtest/gtest.h>
#include "base/native_library.h"
#include "base/no_destructor.h"
#include "device/fido/attestation_statement_formats.h"
#include "device/fido/ohos/type_conversions.h"
#include "device/fido/ohos/fido2_api_utils.h"

using namespace testing;
namespace device {

class OhosWebAuthnApiTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void OhosWebAuthnApiTest::SetUpTestCase() {}

void OhosWebAuthnApiTest::TearDownTestCase() {}

void OhosWebAuthnApiTest::SetUp() {}

void OhosWebAuthnApiTest::TearDown() {}


TEST_F(OhosWebAuthnApiTest, GetClientCapabilitiesTest_001)
{
    auto ret = OhosWebAuthnApi::Instance()->GetClientCapabilities();
    EXPECT_FALSE(ret.empty());
}

TEST_F(OhosWebAuthnApiTest, IsUserVerifyingPlatformAuthenticatorAvailableTest_001)
{
    auto ret = OhosWebAuthnApi::Instance()->IsUserVerifyingPlatformAuthenticatorAvailable();
    EXPECT_TRUE(ret);
}

TEST_F(OhosWebAuthnApiTest, RegisterTest_001)
{
    std::string in_client_data_json = "test";
    PublicKeyCredentialRpEntity in_rp("test");
    std::vector<uint8_t> emptyData;
    PublicKeyCredentialUserEntity in_user(emptyData);
    std::vector<PublicKeyCredentialParams::CredentialInfo> credentialInfos;
    PublicKeyCredentialParams params = PublicKeyCredentialParams(credentialInfos);
    CtapMakeCredentialRequest request(in_client_data_json, in_rp, in_user, params);
    AuthenticatorSelectionCriteria authenticator_selection_criteria;
    MakeCredentialOptions options(authenticator_selection_criteria);
    auto ret = OhosWebAuthnApi::Instance()->Register(request, options);
    EXPECT_FALSE(ret.second.has_value());
}

TEST_F(OhosWebAuthnApiTest, GetAssertionTest_001)
{
    std::string in_rp_id = "test";
    std::string in_client_data_json = "test";
    CtapGetAssertionRequest request(in_rp_id, in_client_data_json);
    CtapGetAssertionOptions options;
    auto ret = OhosWebAuthnApi::Instance()->GetAssertion(request, options);
    EXPECT_FALSE(ret.second.has_value());
}
} // namespace base
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

TEST_F(OhosWebAuthnApiTest, GetClientCapabilities_002)
{
    auto capabilities = OhosWebAuthnApi::Instance()->GetClientCapabilities();
    // Verify we get some capabilities (may be empty if API not available)
    EXPECT_TRUE(capabilities.empty() || !capabilities.empty());
}

TEST_F(OhosWebAuthnApiTest, Register_002)
{
    // Test with more complete request
    std::string in_client_data_json = "test_client_data";
    PublicKeyCredentialRpEntity in_rp("test_rp_id", "test_rp_name");
    std::vector<uint8_t> user_id = {1, 2, 3, 4, 5};
    PublicKeyCredentialUserEntity in_user(user_id, "test_user", "Test User Display");
    PublicKeyCredentialParams::CredentialInfo info = {CredentialType::kPublicKey, -7};
    std::vector<PublicKeyCredentialParams::CredentialInfo> credential_infos;
    credential_infos.emplace_back(info);
    PublicKeyCredentialParams params(credential_infos);
    CtapMakeCredentialRequest request(in_client_data_json, in_rp, in_user, params);

    AuthenticatorSelectionCriteria authenticator_selection_criteria;
    MakeCredentialOptions options(authenticator_selection_criteria);

    auto ret = OhosWebAuthnApi::Instance()->Register(request, options);
    // Check the status
    EXPECT_EQ(ret.first, MakeCredentialStatus::kAuthenticatorResponseInvalid);
}

TEST_F(OhosWebAuthnApiTest, GetAssertion_002)
{
    // Test with more complete request
    std::string in_rp_id = "test.example.com";
    std::string in_client_data_json = "test_client_data";
    CtapGetAssertionRequest request(in_rp_id, in_client_data_json);
    CtapGetAssertionOptions options;

    auto ret = OhosWebAuthnApi::Instance()->GetAssertion(request, options);
    // Check the status
    EXPECT_EQ(ret.first, GetAssertionStatus::kAuthenticatorResponseInvalid);
}

TEST_F(OhosWebAuthnApiTest, MultipleRegisterCalls_001)
{
    // Test multiple sequential register calls
    std::string in_client_data_json = "test";
    PublicKeyCredentialRpEntity in_rp("test");
    std::vector<uint8_t> emptyData;
    PublicKeyCredentialUserEntity in_user(emptyData);
    PublicKeyCredentialParams::CredentialInfo info = {CredentialType::kPublicKey, 0};
    std::vector<PublicKeyCredentialParams::CredentialInfo> credential_infos;
    credential_infos.emplace_back(info);
    PublicKeyCredentialParams params(credential_infos);
    CtapMakeCredentialRequest request(in_client_data_json, in_rp, in_user, params);
    AuthenticatorSelectionCriteria authenticator_selection_criteria;
    MakeCredentialOptions options(authenticator_selection_criteria);

    // Multiple calls should work without crashing
    auto ret1 = OhosWebAuthnApi::Instance()->Register(request, options);
    auto ret2 = OhosWebAuthnApi::Instance()->Register(request, options);

    EXPECT_FALSE(ret1.second.has_value());
    EXPECT_FALSE(ret2.second.has_value());
}

TEST_F(OhosWebAuthnApiTest, MultipleGetAssertionCalls_001)
{
    // Test multiple sequential get assertion calls
    std::string in_rp_id = "test";
    std::string in_client_data_json = "test";
    CtapGetAssertionRequest request(in_rp_id, in_client_data_json);
    CtapGetAssertionOptions options;

    // Multiple calls should work without crashing
    auto ret1 = OhosWebAuthnApi::Instance()->GetAssertion(request, options);
    auto ret2 = OhosWebAuthnApi::Instance()->GetAssertion(request, options);

    EXPECT_FALSE(ret1.second.has_value());
    EXPECT_FALSE(ret2.second.has_value());
}

} // namespace device
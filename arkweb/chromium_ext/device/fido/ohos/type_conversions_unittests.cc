// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/device/fido/ohos/type_conversions.h"

#include <gtest/gtest.h>
#include <algorithm>
#include <sstream>
#include <string_view>
#include "base/strings/string_number_conversions.h"
#include "device/fido/attestation_statement.h"

using namespace testing;
namespace device {

std::string ToString(const char* c_str);
std::string_view TruncateString(const char* s);
template<>
std::string Convert(const FIDO2_AuthenticatorTransportArray& transports);
template<>
std::string Convert(const FIDO2_PublicKeyCredentialHintArray& hints);
template<>
const char* Convert(const ResidentKeyRequirement& resident_key);

class TypeConversionsTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void TypeConversionsTest::SetUpTestCase() {}

void TypeConversionsTest::TearDownTestCase() {}

void TypeConversionsTest::SetUp() {}

void TypeConversionsTest::TearDown() {}

TEST_F(TypeConversionsTest, FIDO2_ClientCapabilityTest_001)
{
    auto ret = Convert<std::string>(FIDO2_CONDITIONAL_CREATE);
    EXPECT_EQ(ret, "conditionalCreate");
    ret = Convert<std::string>(FIDO2_CONDITIONAL_GET);
    EXPECT_EQ(ret, "conditionalGet");
    ret = Convert<std::string>(FIDO2_HYBRID_TRANSPORT);
    EXPECT_EQ(ret, "hybridTransport");
    ret = Convert<std::string>(FIDO2_PASSKEY_PLATFORM_AUTHENTICATOR);
    EXPECT_EQ(ret, "passkeyPlatformAuthenticator");
    ret = Convert<std::string>(FIDO2_USER_VERIFYING_PLATFORM_AUTHENTICATOR);
    EXPECT_EQ(ret, "userVerifyingPlatformAuthenticator");
    ret = Convert<std::string>(FIDO2_RELATED_ORIGINS);
    EXPECT_EQ(ret, "relatedOrigins");
    ret = Convert<std::string>(FIDO2_SIGNAL_ALL_ACCEPTED_CREDENTIALS);
    EXPECT_EQ(ret, "signalAllAcceptedCredentials");
    ret = Convert<std::string>(FIDO2_SIGNAL_CURRENT_USER_DETAILS);
    EXPECT_EQ(ret, "signalCurrentUserDetails");
    ret = Convert<std::string>(FIDO2_SIGNAL_UNKNOWN_CREDENTIAL);
    EXPECT_EQ(ret, "signalUnknownCredential");
    ret = Convert<std::string>(FIDO2_EXTENSION_UVI);
    EXPECT_EQ(ret, "extension:uvi");
    ret = Convert<std::string>(static_cast<FIDO2_ClientCapability>(10));
    EXPECT_EQ(ret, "");
}

TEST_F(TypeConversionsTest, CredentialTypeTest_001)
{
    auto ret = Convert<FIDO2_PublicKeyCredentialType>(CredentialType::kPublicKey);
    EXPECT_EQ(ret, FIDO2_PUBLIC_KEY);
    ret = Convert<FIDO2_PublicKeyCredentialType>(static_cast<CredentialType>(1));
    EXPECT_EQ(ret, FIDO2_PUBLIC_KEY);
}

TEST_F(TypeConversionsTest, CredentialInfoTest_001)
{
    PublicKeyCredentialParams::CredentialInfo info = {CredentialType::kPublicKey, 0};
    auto ret = Convert<FIDO2_PublicKeyCredentialParameters>(info);
    EXPECT_EQ(ret.type, FIDO2_PUBLIC_KEY);
}

TEST_F(TypeConversionsTest, PublicKeyCredentialParamsTest_001)
{
    PublicKeyCredentialParams::CredentialInfo info = {CredentialType::kPublicKey, 0};
    std::vector<PublicKeyCredentialParams::CredentialInfo> credential_params;
    credential_params.emplace_back(info);
    PublicKeyCredentialParams params(credential_params);
    auto ret = Convert<std::vector<FIDO2_PublicKeyCredentialParameters>>(params);
    EXPECT_FALSE(ret.empty());
}

TEST_F(TypeConversionsTest, FidoTransportProtocolTest_001)
{
    auto ret = Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kUsbHumanInterfaceDevice);
    EXPECT_EQ(ret, FIDO2_USB);
    ret = Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kNearFieldCommunication);
    EXPECT_EQ(ret, FIDO2_NFC);
    ret = Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kBluetoothLowEnergy);
    EXPECT_EQ(ret, FIDO2_BLE);
    ret = Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kHybrid);
    EXPECT_EQ(ret, FIDO2_HYBRID);
    ret = Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kInternal);
    EXPECT_EQ(ret, FIDO2_INTERNAL);
    ret = Convert<FIDO2_AuthenticatorTransport>(FidoTransportProtocol::kDeprecatedAoa);
    EXPECT_EQ(ret, FIDO2_INTERNAL);
}

TEST_F(TypeConversionsTest, FIDO2_AuthenticatorTransportTest_001)
{
    auto ret = Convert<FidoTransportProtocol>(FIDO2_USB);
    EXPECT_EQ(ret, FidoTransportProtocol::kUsbHumanInterfaceDevice);
    ret = Convert<FidoTransportProtocol>(FIDO2_NFC);
    EXPECT_EQ(ret, FidoTransportProtocol::kNearFieldCommunication);
    ret = Convert<FidoTransportProtocol>(FIDO2_BLE);
    EXPECT_EQ(ret, FidoTransportProtocol::kBluetoothLowEnergy);
    ret = Convert<FidoTransportProtocol>(FIDO2_SMART_CARD);
    EXPECT_EQ(ret, FidoTransportProtocol::kHybrid);
    ret = Convert<FidoTransportProtocol>(FIDO2_HYBRID);
    EXPECT_EQ(ret, FidoTransportProtocol::kHybrid);
    ret = Convert<FidoTransportProtocol>(FIDO2_INTERNAL);
    EXPECT_EQ(ret, FidoTransportProtocol::kInternal);
    ret = Convert<FidoTransportProtocol>(static_cast<FIDO2_AuthenticatorTransport>(6));
    EXPECT_EQ(ret, FidoTransportProtocol::kInternal);
}

TEST_F(TypeConversionsTest, SetFidoTransportProtocolTest_001)
{
    FIDO2_AuthenticatorTransportArray transportArray;
    FIDO2_AuthenticatorTransport transport = FIDO2_AuthenticatorTransport::FIDO2_NFC;
    transportArray.transportNum = 1;
    transportArray.transports = nullptr;
    auto ret = Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(transportArray);
    EXPECT_EQ(ret, std::nullopt);
    transportArray.transportNum = 0;
    transportArray.transports = &transport;
    ret = Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(transportArray);
    EXPECT_EQ(ret, std::nullopt);
    transportArray.transportNum = 1;
    ret = Convert<std::optional<base::flat_set<FidoTransportProtocol>>>(transportArray);
    EXPECT_NE(ret, std::nullopt);
}

TEST_F(TypeConversionsTest, VecFIDO2_AuthenticatorTransportTest_001)
{
    base::flat_set<FidoTransportProtocol> transport;
    auto ret = Convert<std::vector<FIDO2_AuthenticatorTransport>>(transport);
    EXPECT_TRUE(ret.empty());
    transport.insert(FidoTransportProtocol::kInternal);
    ret = Convert<std::vector<FIDO2_AuthenticatorTransport>>(transport);
    EXPECT_FALSE(ret.empty());
}

TEST_F(TypeConversionsTest, UserVerificationRequirementTest_001)
{
    auto ret = Convert<FIDO2_UserVerificationRequirement>(UserVerificationRequirement::kRequired);
    EXPECT_EQ(ret, FIDO2_REQUIRED);
    ret = Convert<FIDO2_UserVerificationRequirement>(UserVerificationRequirement::kPreferred);
    EXPECT_EQ(ret, FIDO2_PREFERRED);
    ret = Convert<FIDO2_UserVerificationRequirement>(UserVerificationRequirement::kDiscouraged);
    EXPECT_EQ(ret, FIDO2_DISCOURAGED);
    ret = Convert<FIDO2_UserVerificationRequirement>(static_cast<UserVerificationRequirement>(3));
    EXPECT_EQ(ret, FIDO2_DISCOURAGED);
}

TEST_F(TypeConversionsTest, FIDO2_AuthenticatorAttachmentTest_001)
{
    auto ret = Convert<FIDO2_AuthenticatorAttachment>(AuthenticatorAttachment::kAny);
    EXPECT_EQ(ret, FIDO2_PLATFORM);
    ret = Convert<FIDO2_AuthenticatorAttachment>(AuthenticatorAttachment::kPlatform);
    EXPECT_EQ(ret, FIDO2_PLATFORM);
    ret = Convert<FIDO2_AuthenticatorAttachment>(AuthenticatorAttachment::kCrossPlatform);
    EXPECT_EQ(ret, FIDO2_CROSS_PLATFORM);
    ret = Convert<FIDO2_AuthenticatorAttachment>(static_cast<AuthenticatorAttachment>(3));
    EXPECT_EQ(ret, FIDO2_PLATFORM);
}

TEST_F(TypeConversionsTest, AuthenticatorAttachmentTest_001)
{
    auto ret = Convert<AuthenticatorAttachment>(FIDO2_PLATFORM);
    EXPECT_EQ(ret, AuthenticatorAttachment::kPlatform);
    ret = Convert<AuthenticatorAttachment>(FIDO2_CROSS_PLATFORM);
    EXPECT_EQ(ret, AuthenticatorAttachment::kCrossPlatform);
    ret = Convert<AuthenticatorAttachment>(static_cast<FIDO2_AuthenticatorAttachment>(2));
    EXPECT_EQ(ret, AuthenticatorAttachment::kAny);
}

TEST_F(TypeConversionsTest, PublicKeyCredentialRpEntityTest_001)
{
    PublicKeyCredentialRpEntity rp1("id", "name");
    auto ret = Convert<FIDO2_PublicKeyCredentialRpEntity>(rp1);
    EXPECT_NE(ret.name, nullptr);
    PublicKeyCredentialRpEntity rp2;
    ret = Convert<FIDO2_PublicKeyCredentialRpEntity>(rp2);
    EXPECT_EQ(ret.name, nullptr);
}

TEST_F(TypeConversionsTest, PublicKeyCredentialUserEntityTest_001)
{
    std::vector<uint8_t> id;
    PublicKeyCredentialUserEntity user1(id, "name", "display_name");
    auto ret = Convert<FIDO2_PublicKeyCredentialUserEntity>(user1);
    EXPECT_NE(ret.name, nullptr);
    PublicKeyCredentialUserEntity user2;
    ret = Convert<FIDO2_PublicKeyCredentialUserEntity>(user2);
    EXPECT_EQ(ret.name, nullptr);
}

TEST_F(TypeConversionsTest, AttestationConveyancePreferenceTest_001)
{
    auto ret = Convert<FIDO2_AttestationConveyancePreference>(AttestationConveyancePreference::kNone);
    EXPECT_EQ(ret, FIDO2_NONE);
    ret = Convert<FIDO2_AttestationConveyancePreference>(AttestationConveyancePreference::kIndirect);
    EXPECT_EQ(ret, FIDO2_INDIRECT);
    ret = Convert<FIDO2_AttestationConveyancePreference>(AttestationConveyancePreference::kDirect);
    EXPECT_EQ(ret, FIDO2_DIRECT);
    ret = Convert<FIDO2_AttestationConveyancePreference>(
            AttestationConveyancePreference::kEnterpriseIfRPListedOnAuthenticator);
    EXPECT_EQ(ret, FIDO2_ENTERPRISE);
    ret = Convert<FIDO2_AttestationConveyancePreference>(AttestationConveyancePreference::kEnterpriseApprovedByBrowser);
    EXPECT_EQ(ret, FIDO2_ENTERPRISE);
    ret = Convert<FIDO2_AttestationConveyancePreference>(static_cast<AttestationConveyancePreference>(5));
    EXPECT_EQ(ret, FIDO2_NONE);
}

TEST_F(TypeConversionsTest, Uint8BuffTest_001)
{
    uint8_t data[10] = {0};
    Uint8Buff buffer = {10, nullptr};
    std::vector<uint8_t> ret = Convert<std::vector<uint8_t>>(buffer);
    EXPECT_TRUE(ret.empty());
    buffer = {0, data};
    ret = Convert<std::vector<uint8_t>>(buffer);
    EXPECT_TRUE(ret.empty());
    buffer = {10, data};
    ret = Convert<std::vector<uint8_t>>(buffer);
    EXPECT_FALSE(ret.empty());
}

TEST_F(TypeConversionsTest, VectorUint8tTest_001)
{
    std::vector<uint8_t> data;
    std::string ret = Convert<std::string>(data);
    EXPECT_TRUE(ret.empty());
    data = {0x41, 0x42, 0x43};
    ret = Convert<std::string>(data);
    EXPECT_EQ(ret, "ABC");
}

TEST_F(TypeConversionsTest, CredentialHintTest_001)
{
    auto ret = Convert<FIDO2_PublicKeyCredentialHint>(CredentialHint::kSecurityKey);
    EXPECT_EQ(ret, FIDO2_SECURITY_KEY);
    ret = Convert<FIDO2_PublicKeyCredentialHint>(CredentialHint::kClientDevice);
    EXPECT_EQ(ret, FIDO2_CLIENT_DEVICE);
    ret = Convert<FIDO2_PublicKeyCredentialHint>(CredentialHint::kHintHybrid);
    EXPECT_EQ(ret, FIDO2_HINT_HYBRID);
    ret = Convert<FIDO2_PublicKeyCredentialHint>(static_cast<CredentialHint>(3));
    EXPECT_EQ(ret, FIDO2_HINT_HYBRID);
}

TEST_F(TypeConversionsTest, CredentialMediationRequirementTest_001)
{
    auto ret = Convert<FIDO2_CredentialMediationRequirement>(CredentialMediationRequirement::kSilent);
    EXPECT_EQ(ret, FIDO2_SILENT);
    ret = Convert<FIDO2_CredentialMediationRequirement>(CredentialMediationRequirement::kOptional);
    EXPECT_EQ(ret, FIDO2_OPTIONAL);
    ret = Convert<FIDO2_CredentialMediationRequirement>(CredentialMediationRequirement::kConditional);
    EXPECT_EQ(ret, FIDO2_CONDITIONAL);
    ret = Convert<FIDO2_CredentialMediationRequirement>(CredentialMediationRequirement::kRequired);
    EXPECT_EQ(ret, FIDO2_MEDIATION_REQUIRED);
    ret = Convert<FIDO2_CredentialMediationRequirement>(static_cast<CredentialMediationRequirement>(4));
    EXPECT_EQ(ret, FIDO2_OPTIONAL);
}

TEST_F(TypeConversionsTest, CharToStringTest_001)
{
    const char* c_str1 = nullptr;
    std::string ret = ToString(c_str1);
    EXPECT_TRUE(ret.empty());
    const char* c_str2 = "Hello World";
    ret = ToString(c_str2);
    EXPECT_EQ(ret, "Hello World");
}

TEST_F(TypeConversionsTest, TruncateStringTest_001)
{
    const char* c_str1 = nullptr;
    std::string_view ret = TruncateString(c_str1);
    EXPECT_EQ(ret, "<null>");
    const char* c_str2 = "Hello World";
    ret = TruncateString(c_str2);
    std::string_view val = "Hel";
    EXPECT_EQ(ret, val);
}

TEST_F(TypeConversionsTest, CredentialHintArrayToStringTest_001)
{
    FIDO2_PublicKeyCredentialHintArray credentialHintArray;
    credentialHintArray.hintNum = 2;
    FIDO2_PublicKeyCredentialHint credentialHints[2];
    credentialHints[0] = FIDO2_PublicKeyCredentialHint::FIDO2_SECURITY_KEY;
    credentialHints[1] = FIDO2_PublicKeyCredentialHint::FIDO2_CLIENT_DEVICE;
    credentialHintArray.hints = credentialHints;
    auto ret = Convert<std::string>(credentialHintArray);
    EXPECT_EQ(ret, "[0, 1]");
    credentialHintArray.hintNum = 0;
    ret = Convert<std::string>(credentialHintArray);
    EXPECT_EQ(ret, "[]");
}

TEST_F(TypeConversionsTest, TransportArrayToStringTest_001)
{
    FIDO2_AuthenticatorTransportArray transportArray;
    transportArray.transportNum = 2;
    FIDO2_AuthenticatorTransport transports[2];
    transports[0] = FIDO2_AuthenticatorTransport::FIDO2_NFC;
    transports[1] = FIDO2_AuthenticatorTransport::FIDO2_BLE;
    transportArray.transports = transports;
    auto ret = Convert<std::string>(transportArray);
    EXPECT_EQ(ret, "[1, 2]");
    transportArray.transportNum = 0;
    ret = Convert<std::string>(transportArray);
    EXPECT_EQ(ret, "[]");
}

TEST_F(TypeConversionsTest, CapabilityArrayToStringTest_001)
{
    FIDO2_CapabilityArray credentialHintArray;
    credentialHintArray.number = 2;
    FIDO2_Capability capabilitys[2];
    capabilitys[0] = {FIDO2_ClientCapability::FIDO2_HYBRID_TRANSPORT, true};
    capabilitys[1] = {FIDO2_ClientCapability::FIDO2_PASSKEY_PLATFORM_AUTHENTICATOR, false};
    credentialHintArray.capability = capabilitys;
    auto ret = Convert<std::string>(credentialHintArray);
    EXPECT_EQ(ret, "[{2:1}, {3:0}]");
    credentialHintArray.number = 0;
    ret = Convert<std::string>(credentialHintArray);
    EXPECT_EQ(ret, "[]");
}

TEST_F(TypeConversionsTest, ConvertHexStringToBytesTest_001)
{
    auto ret = ConvertHexStringToBytes("66666666");
    EXPECT_FALSE(ret.empty());
    ret = ConvertHexStringToBytes("(bytestring)1");
    EXPECT_TRUE(ret.empty());
    ret = ConvertHexStringToBytes("test..........");
    EXPECT_TRUE(ret.empty());
}

TEST_F(TypeConversionsTest, ResidentKeyRequirementTest_001)
{
    auto ret = Convert<const char*>(ResidentKeyRequirement::kDiscouraged);
    EXPECT_EQ(ret, "discouraged");
    ret = Convert<const char*>(ResidentKeyRequirement::kPreferred);
    EXPECT_EQ(ret, "preferred");
    ret = Convert<const char*>(ResidentKeyRequirement::kRequired);
    EXPECT_EQ(ret, "required");
    ret = Convert<const char*>(static_cast<ResidentKeyRequirement>(3));
    EXPECT_EQ(ret, "discouraged");
}

TEST_F(TypeConversionsTest, InitializeMakeCredentialTest_001)
{
    PublicKeyCredentialRpEntity rp;
    PublicKeyCredentialUserEntity user;
    PublicKeyCredentialParams::CredentialInfo info = {CredentialType::kPublicKey, 0};
    std::vector<PublicKeyCredentialParams::CredentialInfo> credential_params;
    credential_params.emplace_back(info);
    PublicKeyCredentialParams params(credential_params);
    CtapMakeCredentialRequest request("client_data_json", rp, user, params);
    CtapMakeCredentialRequestExtra extra;
    request.extra = extra;
    CredentialOptionsDataHolder data_holder(request);
    MakeCredentialOptions request_options;
    FIDO2_CredentialCreationOptions options;
    Initialize(data_holder, request, request_options, &options);
    EXPECT_TRUE(data_holder.descriptor_list.empty());
}

TEST_F(TypeConversionsTest, InitializeMakeCredentialTest_002)
{
    PublicKeyCredentialRpEntity rp;
    PublicKeyCredentialUserEntity user;
    PublicKeyCredentialParams::CredentialInfo info = {CredentialType::kPublicKey, 0};
    std::vector<PublicKeyCredentialParams::CredentialInfo> credential_params;
    credential_params.emplace_back(info);
    PublicKeyCredentialParams params(credential_params);
    CtapMakeCredentialRequest request("client_data_json", rp, user, params);
    std::vector<uint8_t> challenge;
    base::TimeDelta timeout = base::TimeDelta::Max();
    std::vector<CredentialHint> hints;
    hints.emplace_back(CredentialHint::kHintHybrid);
    CtapRequestExtraCommon common = {"origin", challenge, CredentialMediationRequirement::kSilent,
        timeout, hints, "extensions"};
    std::vector<std::string> attestation_formats;
    attestation_formats.emplace_back("format");
    CtapMakeCredentialRequestExtra extra = {common, attestation_formats};
    request.extra = extra;
    std::vector<PublicKeyCredentialDescriptor> exclude_list;
    PublicKeyCredentialDescriptor descriptor;
    exclude_list.emplace_back(descriptor);
    request.exclude_list = exclude_list;
    CredentialOptionsDataHolder data_holder(request);
    MakeCredentialOptions request_options;
    FIDO2_CredentialCreationOptions options;
    Initialize(data_holder, request, request_options, &options);
    EXPECT_FALSE(data_holder.descriptor_list.empty());
}

TEST_F(TypeConversionsTest, InitializeGetAssertionTest_001)
{
    CtapGetAssertionRequest request("in_rp_id", "in_client_data_json");
    CtapGetAssertionRequestExtra extra;
    request.extra = extra;
    CredentialOptionsDataHolder data_holder(request);
    CtapGetAssertionOptions request_options;
    FIDO2_CredentialRequestOptions options;
    Initialize(data_holder, request, request_options, &options);
    EXPECT_TRUE(data_holder.descriptor_list.empty());
}

TEST_F(TypeConversionsTest, InitializeGetAssertionTest_002)
{
    CtapGetAssertionRequest request("in_rp_id", "in_client_data_json");
    std::vector<uint8_t> challenge;
    base::TimeDelta timeout = base::TimeDelta::Max();
    std::vector<CredentialHint> hints;
    hints.emplace_back(CredentialHint::kHintHybrid);
    CtapRequestExtraCommon common = {"origin", challenge, CredentialMediationRequirement::kSilent,
        timeout, hints, "extensions"};
    CtapGetAssertionRequestExtra extra = {common};
    request.extra = extra;
    std::vector<PublicKeyCredentialDescriptor> allow_list;
    PublicKeyCredentialDescriptor descriptor;
    allow_list.emplace_back(descriptor);
    request.allow_list = allow_list;
    CredentialOptionsDataHolder data_holder(request);
    CtapGetAssertionOptions request_options;
    FIDO2_CredentialRequestOptions options;
    Initialize(data_holder, request, request_options, &options);
    EXPECT_FALSE(data_holder.descriptor_list.empty());
}
} // namespace base
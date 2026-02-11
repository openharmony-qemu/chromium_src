// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "device/fido/attestation_statement.h"
#include "device/fido/public_key.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/functional/bind.h"
#include "base/native_library.h"
#include "base/notreached.h"
#include "base/ranges/algorithm.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "base/task/task_traits.h"
#include "base/task/thread_pool.h"
#include "components/device_event_log/device_event_log.h"
#include "device/fido/authenticator_supported_options.h"
#include "device/fido/ctap_get_assertion_request.h"
#include "device/fido/ctap_make_credential_request.h"
#include "device/fido/discoverable_credential_metadata.h"
#include "device/fido/features.h"
#include "device/fido/fido_constants.h"
#include "device/fido/fido_request_handler_base.h"
#include "device/fido/fido_transport_protocol.h"
#include "device/fido/ohos/ohos_webauthn_api.h"
#include "device/fido/public_key_credential_descriptor.h"

#define private public
#include "arkweb/chromium_ext/device/fido/ohos/ohos_authenticator.h"
#undef private

using namespace base;
using namespace device;
using namespace testing;
namespace {
static bool loadNativeLibraryMockFlag_ = false;
class NativeLibraryMock {
public:
    static NativeLibraryMock& getInstance() {
        static NativeLibraryMock instance;
        return instance;
    }
    MOCK_METHOD(base::NativeLibrary, LoadNativeLibraryMock, (), ());
};
#ifdef __cplusplus
extern "C" {
#endif
base::NativeLibrary __real_LoadNativeLibrary();

base::NativeLibrary __wrap_LoadNativeLibrary() {
    if (loadNativeLibraryMockFlag_) {
        return NativeLibraryMock::getInstance().LoadNativeLibraryMock();
    } else {
        return __real_LoadNativeLibrary();
    }
}
#ifdef __cplusplus
}
#endif

constexpr std::array<uint8_t, 32> kRpIdHashTest = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
};

constexpr std::array<uint8_t, 16> kAaguidTest = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
};

constexpr std::array<uint8_t, 4> kSignCounterTest = {
    0x01, 0x01, 0x01, 0x01,
};

class OhosAuthenticatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
    void IsUserVerifyingPlatformAuthenticatorAvailableCallback(bool val) {}
    void GetClientCapabilitiesCallback(const std::vector<std::pair<std::string, bool>>& val) {}
    void MakeCredentialOnceCallback(MakeCredentialStatus val1,
        std::optional<AuthenticatorMakeCredentialResponse> val2) {}
    void GetAssertionOnceCallback(GetAssertionStatus val1,
        std::vector<AuthenticatorGetAssertionResponse> val2) {}
};

void OhosAuthenticatorTest::SetUpTestCase() {}

void OhosAuthenticatorTest::TearDownTestCase() {}

void OhosAuthenticatorTest::SetUp() {}

void OhosAuthenticatorTest::TearDown() {}

TEST_F(OhosAuthenticatorTest, OhosWebAuthnApiNoAvailable_001)
{
    loadNativeLibraryMockFlag_ = true;
    auto& mock = NativeLibraryMock::getInstance();
    EXPECT_CALL(mock, LoadNativeLibraryMock())
        .WillOnce(testing::Return(nullptr));
    OhosAuthenticator ohosAuthenticator;
    ohosAuthenticator.IsUserVerifyingPlatformAuthenticatorAvailable(
        BindOnce(&OhosAuthenticatorTest::IsUserVerifyingPlatformAuthenticatorAvailableCallback, Unretained(this)));
    ohosAuthenticator.GetClientCapabilities(
        BindOnce(&OhosAuthenticatorTest::GetClientCapabilitiesCallback, Unretained(this)));
    EXPECT_FALSE(OhosWebAuthnApi::Instance()->IsAvailable());
    loadNativeLibraryMockFlag_ = false;
}

TEST_F(OhosAuthenticatorTest, MakeCredentialDone_001)
{
    base::span<const uint8_t, kAaguidLength> aaguid = kAaguidTest;
    int32_t algorithm = 0;
    const uint8_t k32Bytes[32] = {1, 2, 3};
    std::optional<std::vector<uint8_t>> der_bytes;
    std::unique_ptr<PublicKey> public_key = std::make_unique<PublicKey>(algorithm,
        base::span<const uint8_t>(k32Bytes), der_bytes);
    AttestedCredentialData credential_data(aaguid, base::span<const uint8_t>(k32Bytes), std::move(public_key));
    std::optional<AttestedCredentialData> data = std::make_optional(std::move(credential_data));
    base::span<const uint8_t, kRpIdHashLength> rp_id_hash = kRpIdHashTest;
    uint8_t flags = 192;
    base::span<const uint8_t, kSignCounterLength> sign_counter = kSignCounterTest;
    cbor::Value::MapValue map;
    const cbor::Value key_foo("foo");
    map[cbor::Value("foo")] = cbor::Value("bar");
    std::optional<cbor::Value> extensions = std::make_optional(cbor::Value(map));
    AuthenticatorData authenticator_data(rp_id_hash, flags, sign_counter, std::move(data), std::move(extensions));
    std::unique_ptr<AttestationStatement> statement = std::make_unique<NoneAttestationStatement>();
    AttestationObject attestation_object(std::move(authenticator_data), std::move(statement));
    auto response = std::optional<AuthenticatorMakeCredentialResponse>(std::in_place,
        FidoTransportProtocol::kUsbHumanInterfaceDevice, std::move(attestation_object));
    auto result = std::make_pair(MakeCredentialStatus::kAuthenticatorResponseInvalid, std::move(response));
    OhosAuthenticator ohosAuthenticator;
    ohosAuthenticator.waiting_for_cancellation_ = false;
    ohosAuthenticator.MakeCredentialDone(
        BindOnce(&OhosAuthenticatorTest::MakeCredentialOnceCallback, Unretained(this)), std::move(result));
    result = std::make_pair(MakeCredentialStatus::kSuccess, std::move(response));
    ohosAuthenticator.MakeCredentialDone(
        BindOnce(&OhosAuthenticatorTest::MakeCredentialOnceCallback, Unretained(this)), std::move(result));
    ohosAuthenticator.waiting_for_cancellation_ = true;
    ohosAuthenticator.MakeCredentialDone(
        BindOnce(&OhosAuthenticatorTest::MakeCredentialOnceCallback, Unretained(this)), std::move(result));
    EXPECT_FALSE(ohosAuthenticator.waiting_for_cancellation_);
}

TEST_F(OhosAuthenticatorTest, GetAssertion_001)
{
    OhosAuthenticator ohosAuthenticator;
    CtapGetAssertionRequest request("in_rp_id", "in_client_data_json");
    CtapGetAssertionOptions request_options;
    ohosAuthenticator.is_pending_ = true;
    ohosAuthenticator.GetAssertion(request, request_options,
        BindOnce(&OhosAuthenticatorTest::GetAssertionOnceCallback, Unretained(this)));
    EXPECT_FALSE(ohosAuthenticator.waiting_for_cancellation_);
}

TEST_F(OhosAuthenticatorTest, GetAssertionDone_001)
{
    base::span<const uint8_t, kAaguidLength> aaguid = kAaguidTest;
    int32_t algorithm = 0;
    const uint8_t k32Bytes[32] = {1, 2, 3};
    std::optional<std::vector<uint8_t>> der_bytes;
    std::unique_ptr<PublicKey> public_key = std::make_unique<PublicKey>(algorithm,
        base::span<const uint8_t>(k32Bytes), der_bytes);
    AttestedCredentialData credential_data(aaguid, base::span<const uint8_t>(k32Bytes), std::move(public_key));
    std::optional<AttestedCredentialData> data = std::make_optional(std::move(credential_data));
    base::span<const uint8_t, kRpIdHashLength> rp_id_hash = kRpIdHashTest;
    uint8_t flags = 192;
    base::span<const uint8_t, kSignCounterLength> sign_counter = kSignCounterTest;
    cbor::Value::MapValue map;
    const cbor::Value key_foo("foo");
    map[cbor::Value("foo")] = cbor::Value("bar");
    std::optional<cbor::Value> extensions = std::make_optional(cbor::Value(map));
    AuthenticatorData authenticator_data(rp_id_hash, flags, sign_counter, std::move(data), std::move(extensions));
    std::vector<uint8_t> signature;
    auto response = std::optional<AuthenticatorGetAssertionResponse>(std::in_place,
        std::move(authenticator_data), signature, FidoTransportProtocol::kUsbHumanInterfaceDevice);
    auto result = std::make_pair(GetAssertionStatus::kAuthenticatorResponseInvalid, std::move(response));
    OhosAuthenticator ohosAuthenticator;
    ohosAuthenticator.waiting_for_cancellation_ = false;
    ohosAuthenticator.GetAssertionDone(
        BindOnce(&OhosAuthenticatorTest::GetAssertionOnceCallback, Unretained(this)), std::move(result));
    result = std::make_pair(GetAssertionStatus::kSuccess, std::move(response));
    ohosAuthenticator.GetAssertionDone(
        BindOnce(&OhosAuthenticatorTest::GetAssertionOnceCallback, Unretained(this)), std::move(result));
    ohosAuthenticator.waiting_for_cancellation_ = true;
    ohosAuthenticator.GetAssertionDone(
        BindOnce(&OhosAuthenticatorTest::GetAssertionOnceCallback, Unretained(this)), std::move(result));
    EXPECT_FALSE(ohosAuthenticator.waiting_for_cancellation_);
}

TEST_F(OhosAuthenticatorTest, Cancel_001)
{
    OhosAuthenticator ohosAuthenticator;
    ohosAuthenticator.is_pending_ = true;
    ohosAuthenticator.waiting_for_cancellation_ = true;
    ohosAuthenticator.Cancel();
    ohosAuthenticator.is_pending_ = false;
    ohosAuthenticator.Cancel();
    ohosAuthenticator.waiting_for_cancellation_ = false;
    ohosAuthenticator.Cancel();
    ohosAuthenticator.is_pending_ = true;
    ohosAuthenticator.Cancel();
    EXPECT_TRUE(ohosAuthenticator.waiting_for_cancellation_);
}

TEST_F(OhosAuthenticatorTest, GetId_001)
{
    OhosAuthenticator ohosAuthenticator;
    std::string id = ohosAuthenticator.GetId();
    EXPECT_FALSE(id.empty());
}

TEST_F(OhosAuthenticatorTest, Options_001)
{
    OhosAuthenticator ohosAuthenticator;
    const auto& options = ohosAuthenticator.Options();
    // Verify that options are accessible
    EXPECT_TRUE(options.supports_resident_key ||
                options.user_verification_availability != 
                device::AuthenticatorSupportedOptions::UserVerificationAvailability::kNotSupported);
}

TEST_F(OhosAuthenticatorTest, AuthenticatorTransport_001)
{
    OhosAuthenticator ohosAuthenticator;
    auto transport = ohosAuthenticator.AuthenticatorTransport();
    ASSERT_FALSE(transport.has_value());
}

TEST_F(OhosAuthenticatorTest, GetWeakPtr_001)
{
    OhosAuthenticator ohosAuthenticator;
    auto weak_ptr = ohosAuthenticator.GetWeakPtr();
    EXPECT_TRUE(weak_ptr.get() != nullptr);
}

TEST_F(OhosAuthenticatorTest, InitializeAuthenticator_001)
{
    OhosAuthenticator ohosAuthenticator;
    bool callback_called = false;
    auto callback = base::BindOnce([](bool* called) { *called = true; }, &callback_called);
    ohosAuthenticator.InitializeAuthenticator(std::move(callback));
    // The callback should be called synchronously or asynchronously
    // depending on implementation
}

TEST_F(OhosAuthenticatorTest, SignalUnknownCredential_001)
{
    std::vector<uint8_t> credential_id = {1, 2, 3, 4};
    std::string relying_party_id = "test.example.com";
    OhosAuthenticator::SignalUnknownCredential(credential_id, relying_party_id);
    SUCCEED();
}

TEST_F(OhosAuthenticatorTest, SignalAllAcceptedCredentials_001)
{
    std::string relying_party_id = "test.example.com";
    std::vector<uint8_t> user_id = {1, 2, 3, 4};
    std::vector<std::vector<uint8_t>> all_accepted_credential_ids = {
        {1, 2}, {3, 4}, {5, 6}
    };
    OhosAuthenticator::SignalAllAcceptedCredentials(
        relying_party_id, user_id, all_accepted_credential_ids);
    SUCCEED();
}

} // namespace
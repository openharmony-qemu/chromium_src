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

#include "arkweb/ohos_adapter_ndk/interfaces/mock/mock_ohos_adapter_helper.h"
#include "net/base/host_port_pair.h"
#include "net/base/hash_value.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#define private public
#include "net/http/arkweb_transport_security_state_ext.h"
#undef private

namespace net {

class MockCertManagerAdapter : public OHOS::NWeb::CertManagerAdapter {
 public:
  MOCK_METHOD(uint32_t, GetCertMaxSize, (), (override));
  MOCK_METHOD(uint32_t, GetAppCertMaxSize, (), (override));
  MOCK_METHOD(int32_t,
              GetSytemRootCertData,
              (uint32_t certCount, uint8_t* certData),
              (override));
  MOCK_METHOD(uint32_t, GetSytemRootCertSum, (), (override));
  MOCK_METHOD(int32_t,
              GetUserRootCertData,
              (uint32_t certCount, uint8_t* certData),
              (override));
  MOCK_METHOD(uint32_t, GetUserRootCertSum, (), (override));
  MOCK_METHOD(int32_t,
              GetAppCert,
              (uint8_t* uriData, uint8_t* certData, uint32_t* len),
              (override));
  MOCK_METHOD(int32_t,
              Sign,
              (const uint8_t* uri,
               const uint8_t* certData,
               uint32_t certDataLen,
               uint8_t* signData,
               uint32_t signDataLen),
              (override));
  MOCK_METHOD(int32_t,
              GetCertDataBySubject,
              (const char* subjectName, uint8_t* certData, int32_t certType),
              (override));
  MOCK_METHOD(bool,
              GetTrustAnchorsForHostName,
              (const std::string& hostname, std::vector<std::string>& certs),
              (override));
  MOCK_METHOD(bool,
              GetPinSetForHostName,
              (const std::string& hostname, std::vector<std::string>& pins),
              (override));
  MOCK_METHOD(int32_t,
              SignV2,
              (const uint8_t* uri,
               const uint8_t* certData,
               uint32_t certDataLen,
               uint8_t* signData,
               uint32_t* signDataLen,
               uint16_t algorithm),
              (override));
};

class ArkWebTransportSecurityStateExtTest : public ::testing::Test {
 protected:
  void SetUp() override {
    instance_ = new OHOS::NWeb::MockOhosAdapterHelper();
    OHOS::NWeb::OhosAdapterHelper::SetInstance(instance_);
  }

  void TearDown() override {
    OHOS::NWeb::OhosAdapterHelper::SetInstance(nullptr);
    delete instance_;
  }

  void SetupMockAdapter(std::unique_ptr<MockCertManagerAdapter> mock_adapter) {
    EXPECT_CALL(*instance_, GetRootCertDataAdapter())
        .WillOnce(testing::Return(std::move(mock_adapter)));
  }

  OHOS::NWeb::MockOhosAdapterHelper* instance_;
};

TEST_F(ArkWebTransportSecurityStateExtTest, CheckPublicKeyPinsOhos001) {
  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  EXPECT_CALL(*mock_adapter, GetPinSetForHostName("example.com", testing::_))
      .WillOnce(
          testing::DoAll(testing::SetArgReferee<1>(std::vector<std::string>()),
                         testing::Return(false)));
  SetupMockAdapter(std::move(mock_adapter));
  ArkWebTransportSecurityStateExt ext;
  HostPortPair host_port_pair("example.com", 443);
  HashValueVector public_key_hashes;
  auto result = ext.CheckPublicKeyPinsOhos(host_port_pair, public_key_hashes);
  EXPECT_EQ(result, TransportSecurityState::PKPStatus::OK);
}

TEST_F(ArkWebTransportSecurityStateExtTest, CheckPublicKeyPinsOhos002) {
  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  EXPECT_CALL(*mock_adapter, GetPinSetForHostName("example.com", testing::_))
      .WillOnce(
          testing::DoAll(testing::SetArgReferee<1>(std::vector<std::string>()),
                         testing::Return(true)));
  SetupMockAdapter(std::move(mock_adapter));
  ArkWebTransportSecurityStateExt ext;
  HostPortPair host_port_pair("example.com", 443);
  HashValueVector public_key_hashes;
  auto result = ext.CheckPublicKeyPinsOhos(host_port_pair, public_key_hashes);
  EXPECT_EQ(result, TransportSecurityState::PKPStatus::OK);
}

TEST_F(ArkWebTransportSecurityStateExtTest, CheckPublicKeyPinsOhos003) {
  ArkWebTransportSecurityStateExt ext;
  HostPortPair host_port_pair("example.com", 443);
  HashValue hash(HASH_VALUE_SHA256);
  crypto::SHA256HashString("test_key", hash.data(), crypto::kSHA256Length);
  HashValueVector public_key_hashes = {hash};
  std::string public_key_hash_string = hash.ToString();
  EXPECT_TRUE(public_key_hash_string.starts_with(kSha256Slash));
  std::string pin = "sha256//" + public_key_hash_string.substr(kSha256Slash.size());
  std::vector<std::string> pins = {pin};
  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  EXPECT_CALL(*mock_adapter, GetPinSetForHostName("example.com", testing::_))
      .WillOnce(testing::DoAll(testing::SetArgReferee<1>(pins), testing::Return(true)));
  SetupMockAdapter(std::move(mock_adapter));
  auto result = ext.CheckPublicKeyPinsOhos(host_port_pair, public_key_hashes);
  EXPECT_EQ(result, TransportSecurityState::PKPStatus::OK);
}

TEST_F(ArkWebTransportSecurityStateExtTest, CheckPublicKeyPinsOhos004) {
  ArkWebTransportSecurityStateExt ext;
  HostPortPair host_port_pair("example.com", 443);
  HashValue hash(HASH_VALUE_SHA256);
  crypto::SHA256HashString("test_key", hash.data(), crypto::kSHA256Length);
  HashValueVector public_key_hashes = {hash};
  std::string pin = "sha256//different_hash_value";
  std::vector<std::string> pins = {pin};
  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  EXPECT_CALL(*mock_adapter, GetPinSetForHostName("example.com", testing::_))
      .WillOnce(testing::DoAll(testing::SetArgReferee<1>(pins), testing::Return(true)));
  SetupMockAdapter(std::move(mock_adapter));
  auto result = ext.CheckPublicKeyPinsOhos(host_port_pair, public_key_hashes);
  EXPECT_EQ(result, TransportSecurityState::PKPStatus::VIOLATED);
}

TEST_F(ArkWebTransportSecurityStateExtTest, CheckPublicKeyPinsOhos005) {
  ArkWebTransportSecurityStateExt ext;
  HostPortPair host_port_pair("example.com", 443);
  HashValue hash(HASH_VALUE_SHA256);
  crypto::SHA256HashString("test_key", hash.data(), crypto::kSHA256Length);
  HashValueVector public_key_hashes = {hash};
  std::vector<std::string> pins = {"invalid_pin_format"};
  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  EXPECT_CALL(*mock_adapter, GetPinSetForHostName("example.com", testing::_))
      .WillOnce(testing::DoAll(testing::SetArgReferee<1>(pins), testing::Return(true)));
  SetupMockAdapter(std::move(mock_adapter));
  auto result = ext.CheckPublicKeyPinsOhos(host_port_pair, public_key_hashes);
  EXPECT_EQ(result, TransportSecurityState::PKPStatus::VIOLATED);
}

TEST_F(ArkWebTransportSecurityStateExtTest, CheckPublicKeyPinsOhos006) {
  ArkWebTransportSecurityStateExt ext;
  HostPortPair host_port_pair("example.com", 443);
  HashValue hash(HASH_VALUE_SHA256);
  crypto::SHA256HashString("test_key", hash.data(), crypto::kSHA256Length);
  HashValueVector public_key_hashes = {hash};
  std::string public_key_hash_string = hash.ToString();
  std::string public_key_hash_string_code = public_key_hash_string.substr(kSha256Slash.size());
  std::string valid_pin = "sha256//" + public_key_hash_string_code;
  std::vector<std::string> pins = {"sha256//invalid_hash", valid_pin, "another_invalid_pin"};
  auto mock_adapter = std::make_unique<MockCertManagerAdapter>();
  EXPECT_CALL(*mock_adapter, GetPinSetForHostName("example.com", testing::_))
      .WillOnce(testing::DoAll(testing::SetArgReferee<1>(pins), testing::Return(true)));
  SetupMockAdapter(std::move(mock_adapter));
  auto result = ext.CheckPublicKeyPinsOhos(host_port_pair, public_key_hashes);
  EXPECT_EQ(result, TransportSecurityState::PKPStatus::OK);
}

}  // namespace net
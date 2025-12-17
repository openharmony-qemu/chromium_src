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
#include <gtest/gtest.h>

#include "build/buildflag.h"
#include "crypto/symmetric_key.h"
#include "crypto/encryptor.h"
#include "base/logging.h"
#include "base/strings/string_number_conversions.h"
#include "chrome/test/base/testing_browser_process.h"
#include "base/strings/utf_string_conversions.h"
#include "base/metrics/histogram_functions.h"

#if BUILDFLAG(ARKWEB_ENCRYPT)
#include "arkweb/chromium_ext/components/os_crypt/sync/ohos_crypto.h"
#endif

namespace {
  constexpr char kObfuscationPrefixV10[] = "v10";
  constexpr char kMetricDecryptedWithEmptyKey[] =
    "OSCrypt.Linux.DecryptedWithEmptyKey";
  constexpr base::FilePath::CharType kNWebAssetHandleDir[] =
    FILE_PATH_LITERAL("migrate_bak");
  constexpr char kNewbAssetHandleAlias[] = "asset_data_key";
}

#define private public
#include "components/os_crypt/sync/os_crypt.h"
#undef private

#if BUILDFLAG(IS_ARKWEB)
#include "arkweb/chromium_ext/components/os_crypt/sync/os_crypt_linux_for_include.h"
#endif

class CryptLinuxForIncludeTest : public ::testing::Test {
  public:
  void SetUp() override {
    std::string keyHex1 = "000102030405060708090a0b0c0d0e0f";
    std::string keyHex2 = "1a1b1c1d1e1f20212223242526272829";
    std::string ivHex = "000102030405060708090a0b";
    base::HexStringToString(keyHex1, &keyString1);
    base::HexStringToString(keyHex2, &keyString2);
    base::HexStringToString(ivHex, &iv);
    oscryptImpl = std::make_unique<OSCryptImpl>();
  }

  void TearDown() override {
    oscryptImpl.reset();
  }

  std::unique_ptr<crypto::SymmetricKey> CreateTestKey(const std::string& key_data) {
    return crypto::SymmetricKey::Import(crypto::SymmetricKey::AES, key_data);
  }

  bool GetEncryptString(const std::string& encryText, std::string* rawCiphertext,
                        crypto::SymmetricKey* encrypKey, std::string& encryIv) {
    crypto::Encryptor encryptor;
    if (!encryptor.Init(encrypKey, crypto::Encryptor::GCM, encryIv)) {
        return false;
    }
    return encryptor.Encrypt(encryText, rawCiphertext);
  }

  std::string keyString1;
  std::string keyString2;
  std::string iv;
  std::unique_ptr<OSCryptImpl> oscryptImpl;
};

TEST_F(CryptLinuxForIncludeTest, TestDecryptWithIvForIncludeSuccess) {
  std::string plaintext;
  std::string testPlaintext = "testPlaintext";
  std::string rawCiphertext;
  bool result = false;
  std::unique_ptr<crypto::SymmetricKey> key1 = CreateTestKey(keyString1);
  std::unique_ptr<crypto::SymmetricKey> key2 = CreateTestKey(keyString2);

  EXPECT_TRUE(GetEncryptString(testPlaintext, &rawCiphertext, key1.get(), iv));
  bool returnValue = DecryptWithIvForInclude(rawCiphertext, key1.get(), key2.get(), &plaintext, iv, result);
  EXPECT_TRUE(returnValue);
  EXPECT_TRUE(result);
  EXPECT_EQ(testPlaintext, plaintext);
}

TEST_F(CryptLinuxForIncludeTest, TestDecryptWithIvForIncludeKeyOta) {
  std::string plaintext;
  std::string testPlaintext = "testPlaintextOta";
  std::string rawCiphertext;
  std::unique_ptr<crypto::SymmetricKey> key1 = CreateTestKey(keyString1);
  std::unique_ptr<crypto::SymmetricKey> key2 = CreateTestKey(keyString2);

  EXPECT_TRUE(GetEncryptString(testPlaintext, &rawCiphertext, key2.get(), iv));
  bool result = false;
  bool returnValue = DecryptWithIvForInclude(rawCiphertext,  key1.get(), key2.get(), &plaintext, iv, result);
  EXPECT_TRUE(returnValue);
  EXPECT_TRUE(result);
}

TEST_F(CryptLinuxForIncludeTest, TestDecryptWithIvForIncludeFail) {
  std::string plaintext;
  std::string testPlaintext = "testPlaintextError";
  std::string rawCiphertext;
  std::unique_ptr<crypto::SymmetricKey> key1 = CreateTestKey(keyString1);

  crypto::SymmetricKey* key2 = nullptr;
  EXPECT_TRUE(GetEncryptString(testPlaintext, &rawCiphertext, key1.get(), iv));
  std::string errorKey;
  std::string keyHexError = "100102030405060708090a0b0c1d1e1f";
  base::HexStringToString(keyHexError, &errorKey);
  std::unique_ptr<crypto::SymmetricKey> errorSKey = CreateTestKey(errorKey);
  bool result = true;
  bool returnValue = DecryptWithIvForInclude(rawCiphertext, errorSKey.get(), key2, &plaintext, iv, result);
  EXPECT_TRUE(returnValue);
  EXPECT_FALSE(result);
}

TEST_F(CryptLinuxForIncludeTest, TestDecryptWithIvForIncludeWrongkey) {
  std::string plaintext;
  std::string testPlaintext = "testPlaintextError";
  std::string rawCiphertext;
  std::unique_ptr<crypto::SymmetricKey> key1 = CreateTestKey(keyString1);
  std::unique_ptr<crypto::SymmetricKey> key2 = CreateTestKey(keyString2);

  std::string errorKey;
  std::string keyHexError = "100102030405060708090a0b0c1d1e1f";
  base::HexStringToString(keyHexError, &errorKey);
  std::unique_ptr<crypto::SymmetricKey> errorSKey = CreateTestKey(errorKey);
  EXPECT_TRUE(GetEncryptString(testPlaintext, &rawCiphertext, errorSKey.get(), iv));
  bool result = true;
  bool returnValue = DecryptWithIvForInclude(rawCiphertext, key1.get(), key2.get(), &plaintext, iv, result);
  EXPECT_FALSE(returnValue);
}

#if BUILDFLAG(ARKWEB_ENCRYPT)
TEST_F(CryptLinuxForIncludeTest, TestGetPasswordForOtaFail) {
  crypto::SymmetricKey* key = oscryptImpl->GetPasswordForOtaFail();
  EXPECT_EQ(key, nullptr);
  oscryptImpl->password_ota_cache_ = CreateTestKey(keyString1);
  crypto::SymmetricKey* key2 = oscryptImpl->GetPasswordForOtaFail();
  EXPECT_NE(key2, nullptr);
}
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
TEST_F(CryptLinuxForIncludeTest, TestDecryptStringForMigrateEmpty) {
  std::string ciphertext = "";
  std::string plaintext = "original";
  bool result = OSCrypt::DecryptStringForMigrate(ciphertext, &plaintext);
  EXPECT_TRUE(result);
  EXPECT_TRUE(plaintext.empty());
}

TEST_F(CryptLinuxForIncludeTest, TestDecryptStringForMigrateFalse) {
  std::string plaintext;
  std::string testPlaintext = "testPlaintext";
  std::string rawCiphertext;
  std::unique_ptr<crypto::SymmetricKey> encryptionKey = CreateTestKey(keyString1);
  EXPECT_TRUE(GetEncryptString(testPlaintext, &rawCiphertext, encryptionKey.get(), iv));
  bool result = OSCrypt::DecryptStringForMigrate(rawCiphertext, &plaintext);
  EXPECT_FALSE(result);
}

TEST_F(CryptLinuxForIncludeTest, TestDecryptStringForMigrateShortText) {
  std::string testPlaintext = "HelloTest";
  std::string ciphertext;
  std::string plaintext;
  std::unique_ptr<crypto::SymmetricKey> encryptionKey = CreateTestKey(keyString1);
  EXPECT_TRUE(GetEncryptString(testPlaintext, &ciphertext, encryptionKey.get(), iv));
  ciphertext.substr(14);
  bool result = OSCrypt::DecryptStringForMigrate(ciphertext, &plaintext);
  EXPECT_FALSE(result);
  EXPECT_TRUE(plaintext.empty());
}

TEST_F(CryptLinuxForIncludeTest, TestDecryptString16ForMigrate) {
  std::u16string plaintext;
  std::string testPlaintext = "testPlaintext";
  std::string rawCiphertext;
  std::unique_ptr<crypto::SymmetricKey> encryptionKey = CreateTestKey(keyString1);
  EXPECT_TRUE(GetEncryptString(testPlaintext, &rawCiphertext, encryptionKey.get(), iv));
  ASSERT_NO_FATAL_FAILURE(oscryptImpl->DecryptString16ForMigrate(rawCiphertext, &plaintext));
  base::FilePath cache_path;
  base::PathService::Get(base::DIR_CACHE, &cache_path);
  EXPECT_FALSE(cache_path.empty());
  ASSERT_NO_FATAL_FAILURE(oscryptImpl->DecryptString16ForMigrate(rawCiphertext, &plaintext));
  base::FilePath key_dir = cache_path.Append(FILE_PATH_LITERAL(kNWebAssetHandleDir));
  base::CreateDirectory(key_dir);
  EXPECT_TRUE(base::PathExists(key_dir));
  ASSERT_NO_FATAL_FAILURE(oscryptImpl->DecryptString16ForMigrate(rawCiphertext, &plaintext));
  base::FilePath key_file = key_dir.Append(
    FILE_PATH_LITERAL(crypto::ohos::get_asset_handle_file_256(kNewbAssetHandleAlias)));
  base::CreateDirectory(key_file);
  EXPECT_TRUE(base::PathExists(key_file));
  bool result = oscryptImpl->DecryptString16ForMigrate(rawCiphertext, &plaintext);
  EXPECT_FALSE(result);
  EXPECT_TRUE(plaintext.empty());
}

#endif

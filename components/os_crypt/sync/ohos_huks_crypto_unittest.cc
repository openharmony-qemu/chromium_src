// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/os_crypt/sync/ohos_huks_crypto.h"

#include <string>

#include "testing/gtest/include/gtest/gtest.h"

namespace crypto {

namespace ohos {

TEST(OhosKeyStoreUtilTest, TestEncryptDecrypt) {
  std::string plaintext;
  std::string result;
  std::string ciphertext;
  std::string alias = "chrome_huks_os_crypt_password_v1";

  // Test a simple string.
  plaintext = "hello";
  ASSERT_TRUE(
      OhosHuksCrypto::GetInstance().EncryptKey(alias, plaintext, &ciphertext));
  ASSERT_TRUE(
      OhosHuksCrypto::GetInstance().DecryptKey(alias, ciphertext, &result));
  EXPECT_EQ(plaintext, result);

  // Make sure it null terminates.
  plaintext.assign("hello", 3);
  ASSERT_TRUE(
      OhosHuksCrypto::GetInstance().EncryptKey(alias, plaintext, &ciphertext));
  ASSERT_TRUE(
      OhosHuksCrypto::GetInstance().DecryptKey(alias, ciphertext, &result));
  EXPECT_EQ(plaintext, "hel");

  // Test diff alias
  alias = "no_peanut";
  ASSERT_FALSE(
      OhosHuksCrypto::GetInstance().EncryptKey(alias, plaintext, &ciphertext));
  ASSERT_FALSE(
      OhosHuksCrypto::GetInstance().DecryptKey(alias, ciphertext, &result));
}

}  // namespace ohos

}  // namespace crypto

/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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

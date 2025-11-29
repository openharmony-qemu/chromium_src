// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/os_crypt/sync/os_crypt.h"

#include <memory>

#include "base/check.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/no_destructor.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/synchronization/lock.h"
#include "ohos_huks_crypto.h"

namespace {

// Alias for huks to generate key
static const std::string alias = "chrome_huks_os_crypt_password_v1";
}  // namespace

namespace OSCrypt {
bool EncryptString16(const std::u16string& plaintext, std::string* ciphertext) {
  return OSCryptImpl::GetInstance()->EncryptString16(plaintext, ciphertext);
}
bool DecryptString16(const std::string& ciphertext, std::u16string* plaintext) {
  return OSCryptImpl::GetInstance()->DecryptString16(ciphertext, plaintext);
}
bool EncryptString(const std::string& plaintext, std::string* ciphertext) {
  return OSCryptImpl::GetInstance()->EncryptString(plaintext, ciphertext);
}
bool DecryptString(const std::string& ciphertext, std::string* plaintext) {
  return OSCryptImpl::GetInstance()->DecryptString(ciphertext, plaintext);
}
std::string GetRawEncryptionKey() {
  return OSCryptImpl::GetInstance()->GetRawEncryptionKey();
}
void SetRawEncryptionKey(const std::string& key) {
  OSCryptImpl::GetInstance()->SetRawEncryptionKey(key);
}
bool IsEncryptionAvailable() {
  return OSCryptImpl::GetInstance()->IsEncryptionAvailable();
}
}  // namespace OSCrypt

OSCryptImpl* OSCryptImpl::GetInstance() {
  return base::Singleton<OSCryptImpl,
                         base::LeakySingletonTraits<OSCryptImpl>>::get();
}

OSCryptImpl::OSCryptImpl() = default;
OSCryptImpl::~OSCryptImpl() = default;

bool OSCryptImpl::EncryptString16(const std::u16string& plaintext,
                                  std::string* ciphertext) {
  return EncryptString(base::UTF16ToUTF8(plaintext), ciphertext);
}

bool OSCryptImpl::DecryptString16(const std::string& ciphertext,
                                  std::u16string* plaintext) {
  std::string utf8;
  if (!DecryptString(ciphertext, &utf8)) {
    return false;
  }

  *plaintext = base::UTF8ToUTF16(utf8);
  return true;
}

bool OSCryptImpl::EncryptString(const std::string& plaintext,
                                std::string* ciphertext) {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  CHECK(ciphertext);
  if (plaintext.empty()) {
    ciphertext->clear();
    return true;
  }

  return crypto::ohos::OhosHuksCrypto::GetInstance().EncryptKey(
      alias, plaintext, ciphertext);
}

bool OSCryptImpl::DecryptString(const std::string& ciphertext,
                                std::string* plaintext) {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  CHECK(plaintext);
  if (ciphertext.empty()) {
    plaintext->clear();
    return true;
  }

  return crypto::ohos::OhosHuksCrypto::GetInstance().DecryptKey(
      alias, ciphertext, plaintext);
}

// static
bool OSCryptImpl::IsEncryptionAvailable() {
  // In ohos, encryption and decryption services are provided by huks
  // and are available by default.
  return true;
}

// static
void OSCryptImpl::SetRawEncryptionKey(const std::string& raw_key) {
  DCHECK(raw_key.empty());
}

// static
std::string OSCryptImpl::GetRawEncryptionKey() {
  return "";
}

// static
base::Lock& OSCryptImpl::GetLock() {
  static base::NoDestructor<base::Lock> os_crypt_lock;
  return *os_crypt_lock;
}

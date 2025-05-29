// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "arkweb/chromium_ext/components/os_crypt/sync/ohos_crypto.h"
#include "base/check.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/metrics/histogram_functions.h"
#include "base/no_destructor.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/synchronization/lock.h"
#include "components/os_crypt/sync/key_storage_linux.h"
#include "components/os_crypt/sync/os_crypt.h"
#include "crypto/encryptor.h"
#include "ohos_huks_crypto.h"

namespace {

// Salt for Symmetric key derivation.
constexpr char kSalt[] = "saltysalt";

// Size of initialization vector for AES 128-bit.
constexpr size_t kIVBlockSizeAES128 = 16;

// Prefixes for cypher text returned by obfuscation version.  We prefix the
// ciphertext with this string so that future data migration can detect
// this and migrate to full encryption without data loss. kObfuscationPrefixV10
// means that the hardcoded password will be used. kObfuscationPrefixV11 means
// that a password is/will be stored using an OS-level library (e.g Libsecret).
// V11 will not be used if such a library is not available.
constexpr char kObfuscationPrefixV10[] = "v10";
constexpr char kObfuscationPrefixV11[] = "v11";

// The UMA metric name for whether the false was decryptable with an empty key.
constexpr char kMetricDecryptedWithEmptyKey[] =
    "OSCrypt.Linux.DecryptedWithEmptyKey";

// Size of initialization vectore for GCM
const size_t kIVSizeAESGCM = 12;

// Alias for huks to generate key
static const std::string alias = "chrome_huks_os_crypt_password_v1";

std::unique_ptr<crypto::SymmetricKey> GenerateEncryptionKey(
    const std::string& password) {
  const std::string salt(kSalt);

  std::unique_ptr<crypto::SymmetricKey> encryption_key(
      crypto::SymmetricKey::Import(
          crypto::SymmetricKey::AES,
          crypto::ohos::get_symmetric_key_256("nweb_data_key")));
  DCHECK(encryption_key);

  return encryption_key;
}

// Generates a newly allocated SymmetricKey object compatibility with ota.
// Ownership of the key is passed to the caller. Returns null key if a key
// generation error occurs.
std::unique_ptr<crypto::SymmetricKey> GenerateEncryptionKeyForOtaFail() {
  std::unique_ptr<crypto::SymmetricKey> encryption_key(
      crypto::SymmetricKey::Import(
          crypto::SymmetricKey::AES,
          crypto::ohos::get_symmetric_key_256_for_ota("nweb_data_key")));
  DCHECK(encryption_key);

  return encryption_key;
}

// Decrypt `ciphertext` using `encryption_key` and store the result in
// `encryption_key`.
bool DecryptWithIv(const std::string& ciphertext,
                   crypto::SymmetricKey* encryption_key,
                   std::string* plaintext,
                   std::string& iv) {
  crypto::Encryptor encryptor;
  if (!encryptor.Init(encryption_key, crypto::Encryptor::GCM, iv)) {
    return false;
  }

  return encryptor.Decrypt(ciphertext, plaintext);
}

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

  // If we are able to create a V11 key (i.e. a KeyStorage was available), then
  // we'll use it. If not, we'll use V10.
  crypto::SymmetricKey* encryption_key = GetPasswordV11(/*probe=*/false);
  std::string obfuscation_prefix = kObfuscationPrefixV11;
  if (!encryption_key) {
    encryption_key = GetPasswordV10();
  }

  if (!encryption_key) {
    return false;
  }

  std::string iv = crypto::ohos::get_iv(kIVSizeAESGCM);
  crypto::Encryptor encryptor;
  if (!encryptor.Init(encryption_key, crypto::Encryptor::GCM, iv)) {
    return false;
  }

  if (!encryptor.Encrypt(plaintext, ciphertext)) {
    return false;
  }

  ciphertext->insert(0, iv);

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

  // Check that the incoming ciphertext was encrypted and with what version.
  // Credit card numbers are current legacy unencrypted data, so false match
  // with prefix won't happen.
  crypto::SymmetricKey* encryption_key = nullptr;
  std::string obfuscation_prefix;
  if (base::StartsWith(ciphertext, kObfuscationPrefixV10,
                       base::CompareCase::SENSITIVE)) {
    encryption_key = GetPasswordV10();
    obfuscation_prefix = kObfuscationPrefixV10;
  } else if (base::StartsWith(ciphertext, kObfuscationPrefixV11,
                              base::CompareCase::SENSITIVE)) {
    encryption_key = GetPasswordV11(/*probe=*/false);
    obfuscation_prefix = kObfuscationPrefixV11;
  } else {
    // If the prefix is not found then we'll assume we're dealing with
    // old data saved as clear text and we'll return it directly.
    *plaintext = ciphertext;
    return true;
  }

  if (!encryption_key) {
    VLOG(1) << "Decryption failed: could not get the key";
    return false;
  }

  if (ciphertext.length() < (obfuscation_prefix.length() + kIVSizeAESGCM)) {
    return true;
  }
  std::string raw_ciphertext =
      ciphertext.substr(obfuscation_prefix.length() + kIVSizeAESGCM);
  std::string iv =
      ciphertext.substr(obfuscation_prefix.length(), kIVSizeAESGCM);

  if (DecryptWithIv(raw_ciphertext, encryption_key, plaintext, iv)) {
    return true;
  } else {
    // Retry use before second encrypted key to decrypt password
    crypto::SymmetricKey* encryption_key_ota = GetPasswordForOtaFail();
    if (!encryption_key_ota) {
      VLOG(1) << "Decryption failed: could not get the key in ota";
      return false;
    }
    if (DecryptWithIv(raw_ciphertext, encryption_key_ota, plaintext, iv)) {
      LOG(INFO) << "decryption success with ota compatible key";
      return true;
    }
  }

  auto empty_key = GenerateEncryptionKey(std::string());

  if (DecryptWithIv(raw_ciphertext, encryption_key, plaintext, iv)) {
    VLOG(1) << "Decryption succeeded after retrying with an empty key";
    base::UmaHistogramBoolean(kMetricDecryptedWithEmptyKey, true);
    return true;
  }

  VLOG(1) << "Decryption failed";
  base::UmaHistogramBoolean(kMetricDecryptedWithEmptyKey, false);

  return crypto::ohos::OhosHuksCrypto::GetInstance().DecryptKey(
      alias, ciphertext, plaintext);
}

void OSCryptImpl::SetConfig(std::unique_ptr<os_crypt::Config> config) {
  // Setting initialisation parameters makes no sense after initializing.
  DCHECK(!is_password_v11_cached_);
  config_ = std::move(config);
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

crypto::SymmetricKey* OSCryptImpl::GetPasswordForOtaFail() {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  if (!password_ota_cache_.get()) {
    password_ota_cache_ = GenerateEncryptionKeyForOtaFail();
  }
  return password_ota_cache_.get();
}

// Returns a cached string of "peanuts". Is thread-safe.
crypto::SymmetricKey* OSCryptImpl::GetPasswordV10() {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  if (!password_v10_cache_.get()) {
    password_v10_cache_ = GenerateEncryptionKey("peanuts");
  }
  return password_v10_cache_.get();
}

// Caches and returns the password from the KeyStorage or null if there is no
// service. Is thread-safe.
crypto::SymmetricKey* OSCryptImpl::GetPasswordV11(bool probe) {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  if (is_password_v11_cached_) {
    return password_v11_cache_.get();
  }

  std::unique_ptr<KeyStorageLinux> key_storage;
  if (storage_provider_factory_for_testing_) {
    key_storage = std::move(storage_provider_factory_for_testing_).Run();
  } else {
    CHECK(probe || config_);
    if (config_) {
      key_storage = KeyStorageLinux::CreateService(*config_);
      config_.reset();
    }
  }

  if (key_storage) {
    std::optional<std::string> key = key_storage->GetKey();
    if (key.has_value()) {
      password_v11_cache_ = GenerateEncryptionKey(*key);
    }
  }

  is_password_v11_cached_ = true;
  return password_v11_cache_.get();
}

// static
base::Lock& OSCryptImpl::GetLock() {
  static base::NoDestructor<base::Lock> os_crypt_lock;
  return *os_crypt_lock;
}

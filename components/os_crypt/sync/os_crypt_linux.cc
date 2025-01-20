// Copyright 2016 The Chromium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "components/os_crypt/sync/os_crypt.h"

#include <stddef.h>

#include <algorithm>
#include <iterator>
#include <memory>

#include "base/cxx17_backports.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/metrics/histogram_functions.h"
#include "base/no_destructor.h"
#include "base/strings/string_util.h"
#include "base/strings/utf_string_conversions.h"
#include "base/synchronization/lock.h"
#include "base/task/single_thread_task_runner.h"
#include "components/os_crypt/sync/key_storage_config_linux.h"
#include "components/os_crypt/sync/key_storage_linux.h"
#include "crypto/encryptor.h"
#include "crypto/symmetric_key.h"

#if defined(OHOS_EX_PASSWORD)
#include "base/feature_list.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"
#include "ohos_adapter_helper.h"
#include "chrome/browser/browser_process.h"
#include "cef/libcef/browser/prefs/browser_prefs.h"
#include "components/prefs/pref_service.h"
#include "base/strings/string_number_conversions.h"
#endif

#if defined(OHOS_ENCRYPT)
#include "ohos_crypto.h"
#endif

namespace {

// Salt for Symmetric key derivation.
constexpr char kSalt[] = "saltysalt";

#ifndef OHOS_ENCRYPT
// Key size required for 128 bit AES.
constexpr size_t kDerivedKeySizeInBits = 128;

// Constant for Symmetric key derivation.
constexpr size_t kEncryptionIterations = 1;

// Size of initialization vector for AES 128-bit.
constexpr size_t kIVBlockSizeAES128 = 16;
#endif

#if defined(OHOS_ENCRYPT)
// Size of initialization vectore for GCM
const size_t kIVSizeAESGCM = 12;

constexpr char kDdataKeyAlias[] = "nweb_data_key";
#endif

#if defined(OHOS_EX_PASSWORD)
constexpr base::FilePath::CharType kNWebAssetHandleDir[] =
    FILE_PATH_LITERAL("migrate");
constexpr char kNewbAssetHandleAlias[] = "";
#endif

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

// Generates a newly allocated SymmetricKey object based on a password.
// Ownership of the key is passed to the caller. Returns null key if a key
// generation error occurs.
std::unique_ptr<crypto::SymmetricKey> GenerateEncryptionKey(
    const std::string& password) {
  const std::string salt(kSalt);

#if defined(OHOS_ENCRYPT)
  std::unique_ptr<crypto::SymmetricKey> encryption_key(
      crypto::SymmetricKey::Import(
          crypto::SymmetricKey::AES,
          crypto::ohos::get_symmetric_key_256(kDdataKeyAlias)));
#else
  // Create an encryption key from our password and salt.
  std::unique_ptr<crypto::SymmetricKey> encryption_key(
      crypto::SymmetricKey::DeriveKeyFromPasswordUsingPbkdf2(
          crypto::SymmetricKey::AES, password, salt, kEncryptionIterations,
          kDerivedKeySizeInBits));
#endif
  DCHECK(encryption_key);

  return encryption_key;
}

#if defined(OHOS_EX_PASSWORD)
static std::string GetKeyFromAsset() {
  base::FilePath cache_path;
  base::PathService::Get(base::DIR_CACHE, &cache_path);
  if (cache_path.empty()) {
    return std::string();
  }
 
  base::FilePath key_dir =
      cache_path.Append(FILE_PATH_LITERAL(kNWebAssetHandleDir));
  if (!base::PathExists(key_dir)) {
    LOG(ERROR) << "[Autofill] Assethandle dir not exist, errorcode = 1.";
    g_browser_process->local_state()->SetBoolean(browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }
 
  base::FilePath key_file = key_dir.Append(
    FILE_PATH_LITERAL(crypto::ohos::get_asset_handle_file_256(kNewbAssetHandleAlias)));
  if (!base::PathExists(key_file)) {
    LOG(ERROR) << "[Autofill] Assethandle file not exist, errorcode = 1.";
    g_browser_process->local_state()->SetBoolean(browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }
 
  std::string assetHandle;
  bool res = base::ReadFileToString(key_file, &assetHandle);
  if (!res) {
    LOG(ERROR) << "[Autofill] read assethandle file failed, errorcode = 2.";
    g_browser_process->local_state()->SetBoolean(browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }
 
  if (assetHandle.empty()) {
    LOG(INFO) << "[Autofill] assethandle is empty, not need to migrate.";
    g_browser_process->local_state()->SetBoolean(browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }
 
  std::string local_key = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                          .GetKeystoreAdapterInstance().AssetQuery(assetHandle);
  if (local_key.empty()) {
    LOG(ERROR) << "[Autofill] get key from asset failed, errorcode = 2.";
    g_browser_process->local_state()->SetBoolean(browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }
  LOG(INFO) << "[Autofill] get key from asset success.";
  return local_key;
}
 
std::unique_ptr<crypto::SymmetricKey> GenerateEncryptionKeyForMigrate() {
  std::string asset_key = GetKeyFromAsset();
  if (asset_key.empty()) {
    return nullptr;
  }
 
  std::vector<uint8_t> key_byte_array;
 
  base::HexStringToBytes(asset_key, &key_byte_array);
 
  std::unique_ptr<crypto::SymmetricKey> encryption_key(
      crypto::SymmetricKey::Import(crypto::SymmetricKey::AES,
                                   std::string((std::string::value_type*)(key_byte_array.data()),
                                   key_byte_array.size())));
 
  DCHECK(encryption_key);
 
  return encryption_key;
}
#endif

// Decrypt `ciphertext` using `encryption_key` and store the result in
// `encryption_key`.
#if defined(OHOS_ENCRYPT)
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

#else
bool DecryptWith(const std::string& ciphertext,
                 crypto::SymmetricKey* encryption_key,
                 std::string* plaintext) {
  std::string iv(kIVBlockSizeAES128, ' ');
  crypto::Encryptor encryptor;
  if (!encryptor.Init(encryption_key, crypto::Encryptor::CBC, iv)) {
    return false;
  }

  return encryptor.Decrypt(ciphertext, plaintext);
}
#endif

}  // namespace

namespace OSCrypt {
void SetConfig(std::unique_ptr<os_crypt::Config> config) {
  OSCryptImpl::GetInstance()->SetConfig(std::move(config));
}
bool EncryptString16(const std::u16string& plaintext, std::string* ciphertext) {
  return OSCryptImpl::GetInstance()->EncryptString16(plaintext, ciphertext);
}
bool DecryptString16(const std::string& ciphertext, std::u16string* plaintext) {
  return OSCryptImpl::GetInstance()->DecryptString16(ciphertext, plaintext);
}
#if defined(OHOS_EX_PASSWORD)
bool DecryptString16ForMigrate(const std::string& ciphertext, std::u16string* plaintext) {
  return OSCryptImpl::GetInstance()->DecryptString16ForMigrate(ciphertext, plaintext);
}
#endif
bool EncryptString(const std::string& plaintext, std::string* ciphertext) {
  return OSCryptImpl::GetInstance()->EncryptString(plaintext, ciphertext);
}
bool DecryptString(const std::string& ciphertext, std::string* plaintext) {
  return OSCryptImpl::GetInstance()->DecryptString(ciphertext, plaintext);
}
#if defined(OHOS_EX_PASSWORD)
bool DecryptStringForMigrate(const std::string& ciphertext, std::string* plaintext) {
  return OSCryptImpl::GetInstance()->DecryptStringForMigrate(ciphertext, plaintext);
}
#endif
std::string GetRawEncryptionKey() {
  return OSCryptImpl::GetInstance()->GetRawEncryptionKey();
}
void SetRawEncryptionKey(const std::string& key) {
  OSCryptImpl::GetInstance()->SetRawEncryptionKey(key);
}
bool IsEncryptionAvailable() {
  return OSCryptImpl::GetInstance()->IsEncryptionAvailable();
}
#if !BUILDFLAG(IS_OHOS)
void UseMockKeyStorageForTesting(
    base::OnceCallback<std::unique_ptr<KeyStorageLinux>()>
        storage_provider_factory) {
  OSCryptImpl::GetInstance()->UseMockKeyStorageForTesting(
      std::move(storage_provider_factory));
}
void ClearCacheForTesting() {
  OSCryptImpl::GetInstance()->ClearCacheForTesting();
}
void SetEncryptionPasswordForTesting(const std::string& password) {
  OSCryptImpl::GetInstance()->SetEncryptionPasswordForTesting(password);
}
#endif
}  // namespace OSCrypt

OSCryptImpl* OSCryptImpl::GetInstance() {
  return base::Singleton<OSCryptImpl,
                         base::LeakySingletonTraits<OSCryptImpl>>::get();
}

OSCryptImpl::OSCryptImpl()
    : storage_provider_factory_(base::BindOnce(&OSCryptImpl::CreateKeyStorage,
                                               base::Unretained(this))) {}

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

#if defined(OHOS_EX_PASSWORD)
bool OSCryptImpl::DecryptString16ForMigrate(const std::string& ciphertext,
                                            std::u16string* plaintext) {
  std::string utf8;
  if (!DecryptStringForMigrate(ciphertext, &utf8)) {
    return false;
  }
 
  *plaintext = base::UTF8ToUTF16(utf8);
  return true;
}
#endif

bool OSCryptImpl::EncryptString(const std::string& plaintext,
                                std::string* ciphertext) {
  if (plaintext.empty()) {
    ciphertext->clear();
    return true;
  }

  // If we are able to create a V11 key (i.e. a KeyStorage was available), then
  // we'll use it. If not, we'll use V10.
  crypto::SymmetricKey* encryption_key = GetPasswordV11();
  std::string obfuscation_prefix = kObfuscationPrefixV11;
  if (!encryption_key) {
    encryption_key = GetPasswordV10();
    obfuscation_prefix = kObfuscationPrefixV10;
  }

  if (!encryption_key) {
    return false;
  }

#if defined(OHOS_ENCRYPT)
  std::string iv = crypto::ohos::get_iv(kIVSizeAESGCM);
  crypto::Encryptor encryptor;
  if (!encryptor.Init(encryption_key, crypto::Encryptor::GCM, iv)) {
#else
  std::string iv(kIVBlockSizeAES128, ' ');
  crypto::Encryptor encryptor;
  if (!encryptor.Init(encryption_key, crypto::Encryptor::CBC, iv)) {
#endif
    return false;
  }

  if (!encryptor.Encrypt(plaintext, ciphertext)) {
    return false;
  }
#if defined(OHOS_ENCRYPT)
  ciphertext->insert(0, iv);
#endif
  // Prefix the cipher text with version information.
  ciphertext->insert(0, obfuscation_prefix);
  return true;
}

bool OSCryptImpl::DecryptString(const std::string& ciphertext,
                                std::string* plaintext) {
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
    encryption_key = GetPasswordV11();
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

#if defined(OHOS_ENCRYPT)
  if (ciphertext.length() < (obfuscation_prefix.length() + kIVSizeAESGCM)) {
    return true;
  }
  std::string raw_ciphertext =
      ciphertext.substr(obfuscation_prefix.length() + kIVSizeAESGCM);
  std::string iv =
      ciphertext.substr(obfuscation_prefix.length(), kIVSizeAESGCM);
#else
  // Strip off the versioning prefix before decrypting.
  std::string raw_ciphertext = ciphertext.substr(obfuscation_prefix.length());
#endif

#if defined(OHOS_ENCRYPT)
  if (DecryptWithIv(raw_ciphertext, encryption_key, plaintext, iv)) {
#else
  if (DecryptWith(raw_ciphertext, encryption_key, plaintext)) {
#endif
    base::UmaHistogramBoolean(kMetricDecryptedWithEmptyKey, false);
    return true;
  }

  // Some clients have encrypted data with an empty key. See
  // crbug.com/1195256.
  auto empty_key = GenerateEncryptionKey(std::string());
#if defined(OHOS_ENCRYPT)
  if (DecryptWithIv(raw_ciphertext, encryption_key, plaintext, iv)) {
#else
  if (DecryptWith(raw_ciphertext, empty_key.get(), plaintext)) {
#endif
    VLOG(1) << "Decryption succeeded after retrying with an empty key";
    base::UmaHistogramBoolean(kMetricDecryptedWithEmptyKey, true);
    return true;
  }

  VLOG(1) << "Decryption failed";
  base::UmaHistogramBoolean(kMetricDecryptedWithEmptyKey, false);
  return false;
}

#if defined(OHOS_EX_PASSWORD)
bool OSCryptImpl::DecryptStringForMigrate(const std::string& ciphertext,
                                          std::string* plaintext) {
  if (ciphertext.empty()) {
    plaintext->clear();
    return true;
  }
 
  // the incoming ciphertext was encrypted and with V11 version.
  crypto::SymmetricKey* encryption_key = nullptr;
  std::string obfuscation_prefix;
  encryption_key = GetPasswordV10ForMigrate();
  obfuscation_prefix = kObfuscationPrefixV10;
 
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
  }
 
  VLOG(1) << "Decryption failed";
  base::UmaHistogramBoolean(kMetricDecryptedWithEmptyKey, false);
  return false;
}
#endif

void OSCryptImpl::SetConfig(std::unique_ptr<os_crypt::Config> config) {
  // Setting initialisation parameters makes no sense after initializing.
  DCHECK(!is_password_v11_cached_);
  config_ = std::move(config);
}

bool OSCryptImpl::IsEncryptionAvailable() {
  return GetPasswordV11();
}

void OSCryptImpl::SetRawEncryptionKey(const std::string& raw_key) {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  // Check if the v11 password is already cached. If it is, then data encrypted
  // with the old password might not be decryptable.
  DCHECK(!is_password_v11_cached_);
  // The config won't be used if this function is being called. Callers should
  // choose between setting a config and setting a raw encryption key.
  DCHECK(!config_);
  if (!raw_key.empty()) {
    password_v11_cache_ =
        crypto::SymmetricKey::Import(crypto::SymmetricKey::AES, raw_key);
  }
  // Always set |is_password_v11_cached_|, even if given an empty string.
  // Note that |raw_key| can be an empty string if real V11 encryption is not
  // available, and setting |is_password_v11_cached_| causes GetPasswordV11() to
  // correctly return nullptr in that case.
  is_password_v11_cached_ = true;
}

std::string OSCryptImpl::GetRawEncryptionKey() {
  if (crypto::SymmetricKey* key = GetPasswordV11()) {
    return key->key();
  }
  return std::string();
}

#if !BUILDFLAG(IS_OHOS)
void OSCryptImpl::ClearCacheForTesting() {
  password_v10_cache_.reset();
  password_v11_cache_.reset();
  is_password_v11_cached_ = false;
  config_.reset();
}

void OSCryptImpl::UseMockKeyStorageForTesting(
    base::OnceCallback<std::unique_ptr<KeyStorageLinux>()>
        storage_provider_factory) {
  if (storage_provider_factory) {
    storage_provider_factory_ = std::move(storage_provider_factory);
  } else {
    storage_provider_factory_ =
        base::BindOnce(&OSCryptImpl::CreateKeyStorage, base::Unretained(this));
  }
}
#endif

// Create the KeyStorage. Will be null if no service is found. A Config must be
// set before every call to this function.
std::unique_ptr<KeyStorageLinux> OSCryptImpl::CreateKeyStorage() {
  CHECK(config_);
  std::unique_ptr<KeyStorageLinux> key_storage =
      KeyStorageLinux::CreateService(*config_);
  config_.reset();
  return key_storage;
}

#if !BUILDFLAG(IS_OHOS)
void OSCryptImpl::SetEncryptionPasswordForTesting(const std::string& password) {
  ClearCacheForTesting();  // IN-TEST
  password_v11_cache_ = GenerateEncryptionKey(password);
  is_password_v11_cached_ = true;
}
#endif

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
crypto::SymmetricKey* OSCryptImpl::GetPasswordV11() {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  if (!is_password_v11_cached_) {
#if defined(OHOS_COOKIE)
    if (!config_) {
      return nullptr;
    }
#endif // defined(OHOS_COOKIE)
    std::unique_ptr<KeyStorageLinux> key_storage =
        std::move(storage_provider_factory_).Run();
    if (key_storage) {
      absl::optional<std::string> key = key_storage->GetKey();
      if (key.has_value()) {
        password_v11_cache_ = GenerateEncryptionKey(*key);
      }
    }
    is_password_v11_cached_ = true;
  }
  return password_v11_cache_.get();
}

#if defined(OHOS_EX_PASSWORD)
crypto::SymmetricKey* OSCryptImpl::GetPasswordV10ForMigrate() {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  if (!is_password_migrate_cached_) {
    password_migrate_cache_ = GenerateEncryptionKeyForMigrate();
    is_password_migrate_cached_ = true;
  }
  return password_migrate_cache_.get();
}
#endif

// static
base::Lock& OSCryptImpl::GetLock() {
  static base::NoDestructor<base::Lock> os_crypt_lock;
  return *os_crypt_lock;
}

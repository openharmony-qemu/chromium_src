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

#if BUILDFLAG(IS_ARKWEB)
#include "arkweb/chromium_ext/components/os_crypt/sync/os_crypt_linux_for_include.h"
#endif

namespace {

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
constexpr base::FilePath::CharType kNWebAssetHandleDir[] =
    FILE_PATH_LITERAL("migrate");
constexpr char kNewbAssetHandleAlias[] = "asset_data_key";
#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
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
    g_browser_process->local_state()->SetBoolean(
        browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }

  base::FilePath key_file = key_dir.Append(FILE_PATH_LITERAL(
      crypto::ohos::get_asset_handle_file_256(kNewbAssetHandleAlias)));
  if (!base::PathExists(key_file)) {
    LOG(ERROR) << "[Autofill] Assethandle file not exist, errorcode = 1.";
    g_browser_process->local_state()->SetBoolean(
        browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }

  std::string assetHandle;
  bool res = base::ReadFileToString(key_file, &assetHandle);
  if (!res) {
    LOG(ERROR) << "[Autofill] read assethandle file failed, errorcode = 2.";
    g_browser_process->local_state()->SetBoolean(
        browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }

  if (assetHandle.empty()) {
    LOG(INFO) << "[Autofill] assethandle is empty, not need to migrate.";
    g_browser_process->local_state()->SetBoolean(
        browser_prefs::kMigratePasswordsToPasswordVault, true);
    return std::string();
  }

  std::string local_key = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                              .GetKeystoreAdapterInstance()
                              .AssetQuery(assetHandle);
  if (local_key.empty()) {
    LOG(ERROR) << "[Autofill] get key from asset failed, errorcode = 2.";
    g_browser_process->local_state()->SetBoolean(
        browser_prefs::kMigratePasswordsToPasswordVault, true);
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
      crypto::SymmetricKey::Import(
          crypto::SymmetricKey::AES,
          std::string((std::string::value_type*)(key_byte_array.data()),
                      key_byte_array.size())));

  DCHECK(encryption_key);

  return encryption_key;
}
#endif

#if BUILDFLAG(ARKWEB_ENCRYPT)
// Generates a newly allocated SymmetricKey object compatibility with ota.
// Ownership of the key is passed to the caller. Returns null key if a key
// generation error occurs.
std::unique_ptr<crypto::SymmetricKey> GenerateEncryptionKeyForOtaFail() {
  std::unique_ptr<crypto::SymmetricKey> encryption_key(
      crypto::SymmetricKey::Import(
          crypto::SymmetricKey::AES,
          crypto::ohos::get_symmetric_key_256_for_ota(kDataKeyAlias)));
  DCHECK(encryption_key);

  return encryption_key;
}
#endif  // BUILDFLAG(ARKWEB_ENCRYPT)

#if BUILDFLAG(ARKWEB_ENCRYPT)
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
#endif

}  // namespace

namespace OSCrypt {
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
bool DecryptString16ForMigrate(const std::string& ciphertext,
                               std::u16string* plaintext) {
  return OSCryptImpl::GetInstance()->DecryptString16ForMigrate(ciphertext,
                                                               plaintext);
}
#endif
#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
bool DecryptStringForMigrate(const std::string& ciphertext,
                             std::string* plaintext) {
  return OSCryptImpl::GetInstance()->DecryptStringForMigrate(ciphertext,
                                                             plaintext);
}
#endif
}  // namespace OSCrypt

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
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

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
bool OSCryptImpl::DecryptStringForMigrate(const std::string& ciphertext,
                                          std::string* plaintext) {
  if (ciphertext.empty()) {
    plaintext->clear();
    return true;
  }

  // the incoming ciphertext was encrypted and with V10 version.
  crypto::SymmetricKey* encryption_key = GetPasswordV10ForMigrate();
  std::string obfuscation_prefix = kObfuscationPrefixV10;

  if (!encryption_key) {
    LOG(ERROR) << "[Autofill] Decryption failed: could not get the key";
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

  LOG(ERROR) << "[Autofill] Decryption failed";
  base::UmaHistogramBoolean(kMetricDecryptedWithEmptyKey, false);
  return false;
}
#endif

#if BUILDFLAG(ARKWEB_ENCRYPT)
crypto::SymmetricKey* OSCryptImpl::GetPasswordForOtaFail() {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  if (!password_ota_cache_.get()) {
    password_ota_cache_ = GenerateEncryptionKeyForOtaFail();
  }
  return password_ota_cache_.get();
}

#endif

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
crypto::SymmetricKey* OSCryptImpl::GetPasswordV10ForMigrate() {
  base::AutoLock auto_lock(OSCryptImpl::GetLock());
  if (!is_password_migrate_cached_) {
    password_migrate_cache_ = GenerateEncryptionKeyForMigrate();
    is_password_migrate_cached_ = true;
  }
  return password_migrate_cache_.get();
}
#endif

bool DecryptWithIvForInclude(const std::string& raw_ciphertext,
                             crypto::SymmetricKey* encryption_key,
                             crypto::SymmetricKey* encryption_key_ota,
                             std::string* plaintext,
                             std::string& iv,
                             bool& result) {
  if (DecryptWithIv(raw_ciphertext, encryption_key, plaintext, iv)) {
    result = true;
    return true;
  } else {
    // Retry use before second encrypted key to decrypt password
    if (!encryption_key_ota) {
      VLOG(1) << "Decryption failed: could not get the key in ota";
      result = false;
      return true;
    }
    if (DecryptWithIv(raw_ciphertext, encryption_key_ota, plaintext, iv)) {
      LOG(INFO) << "decryption success with ota compatible key";
      result = true;
      return true;
    }
  }
  return false;
}
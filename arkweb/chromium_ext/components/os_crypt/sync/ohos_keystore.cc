/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2023. All rights reserved.
 * Waterloo Cyber Security Center
 *
 * Creation Date: February 28th, 2018
 * Creator : Stefany Allaire (s84078948)
 * Location : Waterloo CSC
 */

#include "ohos_keystore.h"

#include <openssl/aes.h>
#include <openssl/conf.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include "base/feature_list.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/path_service.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace crypto {
namespace ohos {

const int COUNT_FOR_RETRY = 3;
const size_t KEY_LENGTH = 32;
constexpr base::FilePath::CharType kNWebKeyStoreDir[] =
    FILE_PATH_LITERAL("nwebks");
constexpr uint32_t IV_SIZE = 16;
constexpr char V10[] = "V10";
constexpr uint32_t V10_SIZE = 3;

namespace {
size_t GetEncryptSize() {
  return V10_SIZE + IV_SIZE + KEY_LENGTH;
}

bool ValidateKeyAndGetEncryptedData(const base::FilePath& key_file_path, std::string& encryptedData) {
  if (!base::PathExists(key_file_path)) {
    return false;
  }

  bool res = base::ReadFileToString(key_file_path, &encryptedData);
  if (!res) {
    PLOG(INFO) << "failed to read file";
    return false;
  }

  if (encryptedData.length() == GetEncryptSize() && encryptedData.compare(0, V10_SIZE, std::string(V10)) == 0) {
    return true;
  }

  if (encryptedData.length() == KEY_LENGTH) {
    return true;
  }

  LOG(ERROR) << "validate key fail encrypted data length: " << encryptedData.length();
  return false;
}
}

std::string GetKey(const std::string& alias) {
  base::FilePath cache_path;
  base::PathService::Get(base::DIR_CACHE, &cache_path);
  if (cache_path.empty()) {
    return std::string();
  }
  base::FilePath key_dir =
      cache_path.Append(FILE_PATH_LITERAL(kNWebKeyStoreDir));
  if (!base::PathExists(key_dir)) {
    if (!base::CreateDirectory(key_dir)) {
      LOG(ERROR) << "create directory failed: " << key_dir.value();
      return std::string();
    }
  }
  base::FilePath key_file = key_dir.Append(FILE_PATH_LITERAL(alias));

  std::string encryptedData;
  if (ValidateKeyAndGetEncryptedData(key_file, encryptedData)) {
    std::string local_key;
    for (int i = 0; i < COUNT_FOR_RETRY; i++) {
      local_key = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                      .GetKeystoreAdapterInstance()
                      .DecryptKey(alias, encryptedData);
      if (!local_key.empty()) {
        return local_key;
      }
    }
    return local_key;
  } else {
    base::File(key_file, base::File::FLAG_CREATE | base::File::FLAG_WRITE |
                             base::File::FLAG_READ);
    std::string local_key = GenerateLocalKey(KEY_LENGTH);

    for (int i = 0; i < COUNT_FOR_RETRY; i++) {
      encryptedData = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                          .GetKeystoreAdapterInstance()
                          .EncryptKey(alias, local_key);
      if (!encryptedData.empty()) {
        base::WriteFile(
            key_file,
            base::span<const uint8_t>(
                reinterpret_cast<const uint8_t*>(encryptedData.data()),
                encryptedData.size()));
        return local_key;
      }
    }
    return local_key;
  }
}

std::string GetKeyForOta(const std::string& alias) {
  base::FilePath cache_path;
  base::PathService::Get(base::DIR_CACHE, &cache_path);
  if (cache_path.empty()) {
    return std::string();
  }
  base::FilePath key_dir =
      cache_path.Append(FILE_PATH_LITERAL(kNWebKeyStoreDir));
  if (!base::PathExists(key_dir)) {
    LOG(ERROR) << "create directory failed: get key for ota "
               << key_dir.value();
    return std::string();
  }

  base::FilePath key_file = key_dir.Append(FILE_PATH_LITERAL(alias));

  if (base::PathExists(key_file)) {
    std::string encryptedData;
    bool res = base::ReadFileToString(key_file, &encryptedData);
    if (!res) {
      return std::string();
    }
    if (encryptedData.starts_with(V10) &&
        encryptedData.length() >= V10_SIZE + IV_SIZE) {
      encryptedData = encryptedData.substr(
          V10_SIZE + IV_SIZE, encryptedData.length() - V10_SIZE - IV_SIZE);
    }
    std::string local_key;
    for (int i = 0; i < COUNT_FOR_RETRY; i++) {
      local_key = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                      .GetKeystoreAdapterInstance()
                      .EncryptKey(alias, encryptedData);

      LOG(INFO) << "get key compatibility with ota upgrade";
      if (!local_key.empty()) {
        return local_key;
      }
    }
    return local_key;
  }
  return std::string();
}

std::string GenerateLocalKey(size_t sz) {
  std::string rn(sz, 0);
  if (RAND_bytes(reinterpret_cast<unsigned char*>(rn.data()), sz) != 1) {
    LOG(ERROR) << "Failed to generate random bytes";
    return std::string();
  }
  return rn;
}

}  // namespace ohos
}  // namespace crypto

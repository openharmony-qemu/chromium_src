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
#include <random>
#include "base/feature_list.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/guid.h"
#include "base/logging.h"
#include "base/memory/singleton.h"
#include "base/path_service.h"
#include "ohos_adapter_helper.h"

namespace crypto {
namespace ohos {

const int COUNT_FOR_RETRY = 3;
const size_t KEY_LENGTH = 32;
constexpr base::FilePath::CharType kNWebKeyStoreDir[] =
    FILE_PATH_LITERAL("nwebks");

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

  if (base::PathExists(key_file)) {
    std::string encryptedData;
    bool res = base::ReadFileToString(key_file, &encryptedData);
    if (!res) {
      return std::string();
    }
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
    base::File(key_file,
        base::File::FLAG_CREATE | base::File::FLAG_WRITE | base::File::FLAG_READ);
    std::string local_key = GenerateLocalKey(KEY_LENGTH);

    std::string encryptedData;
    for (int i = 0; i < COUNT_FOR_RETRY; i++) {
      encryptedData = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                          .GetKeystoreAdapterInstance()
                          .EncryptKey(alias, local_key);
      if (!encryptedData.empty()) {
        base::WriteFile(key_file, encryptedData.c_str(),
                        encryptedData.length());
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
    LOG(ERROR) << "create directory failed: get key for ota " << key_dir.value();
    return std::string();
  }

  base::FilePath key_file = key_dir.Append(FILE_PATH_LITERAL(alias));

  if (base::PathExists(key_file)) {
    std::string encryptedData;
    bool res = base::ReadFileToString(key_file, &encryptedData);
    if (!res) {
      return std::string();
    }
    std::string local_key;
    for (int i = 0; i < COUNT_FOR_RETRY; i++) {
      local_key = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                      .GetKeystoreAdapterInstance()
                      .EncryptKey(alias, encryptedData);

      LOG(INFO) << "get key compatibility with ota upgrade" ;
      if (!local_key.empty()) {
        return local_key;
      }
    }
    return local_key;
  }
    return std::string();
  }

std::string GenerateLocalKey(size_t sz) {
  std::random_device rd;
  std::mt19937 gen{rd()};
  std::uniform_int_distribution<> dis{0, 255};

  std::string rn(sz, 0);
  for (size_t i = 0; i < sz; ++i) {
    rn[i] = (std::string::value_type)dis(gen);
  }

  return rn;
}

}  // namespace ohos
}  // namespace crypto

/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 */

#ifndef OHOS_COMPONENTS_OS_CRYPT_OHOS_CRYPTO_H_
#define OHOS_COMPONENTS_OS_CRYPT_OHOS_CRYPTO_H_

#include <stddef.h>

#include <map>
#include <mutex>
#include <string>
#include "arkweb/ohos_nweb_ex/build/features/features.h"

namespace crypto {
namespace ohos {

// Constants
const size_t IV_SIZE = 16;

// Get symmetric key
std::string get_symmetric_key_256(const std::string& key_name);

std::string get_symmetric_key_256_for_ota(const std::string& key_name);

#if BUILDFLAG(ARKWEB_EXT_PASSWORD)
std::string get_asset_handle_file_256(const std::string& key_name);
#endif

// Get random IV for encryption
std::string get_iv(size_t sz = IV_SIZE);

}  // end namespace ohos
}  // end namespace crypto

#endif  // OHOS_COMPONENTS_OS_CRYPT_OHOS_CRYPTO_H_

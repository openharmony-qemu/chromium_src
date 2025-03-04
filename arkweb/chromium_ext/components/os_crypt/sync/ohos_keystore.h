/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2022. All rights reserved.
 * Waterloo Cyber Security Center
 *
 * Creation Date: February 28th, 2018
 * Creator : Stefany Allaire (s84078948)
 * Location : Waterloo CSC
 */

#ifndef COMPONENTS_OS_CRYPT_HUAWEI_KEYSTORE_H_
#define COMPONENTS_OS_CRYPT_HUAWEI_KEYSTORE_H_

#include <stdint.h>

#include <string>
#include <vector>

#include "base/component_export.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace crypto {
namespace ohos {

//
// |alias| is a string that get access to a 256Bits Key
// |key| will receive the key on success.
//
COMPONENT_EXPORT(OS_CRYPT)
std::string GetKey(const std::string& alias);

COMPONENT_EXPORT(OS_CRYPT)
std::string GetKeyForOta(const std::string& alias);

COMPONENT_EXPORT(OS_CRYPT)
std::string GenerateLocalKey(size_t key_size);

}  // namespace ohos
}  // namespace crypto

#endif  // COMPONENTS_OS_CRYPT_HUAWEI_KEYSTORE_H_

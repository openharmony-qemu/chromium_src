// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef COMPONENTS_OS_CRYPT_HUAWEI_KEYSTORE_H_
#define COMPONENTS_OS_CRYPT_HUAWEI_KEYSTORE_H_

#include <stdint.h>
#include <string>
#include <vector>

#include "base/component_export.h"
#include "base/strings/string_piece.h"

namespace crypto {
namespace ohos {

//
// |alias| is a string that get access to a 256Bits Key
// |key| will receive the key on success.
//
COMPONENT_EXPORT(OS_CRYPT)
std::string GetKey(const std::string& alias);

COMPONENT_EXPORT(OS_CRYPT)
std::string GenerateLocalKey(size_t key_size);

}  //namespace ohos
}  // namespace crypto

#endif  // COMPONENTS_OS_CRYPT_HUAWEI_KEYSTORE_H_

// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <string>

#include "build/build_config.h"
#include "net/base/platform_mime_util.h"
#include "ohos/adapter/mime_type/mime_type_adapter.h"

using namespace ohos::adapter;
namespace net {

bool PlatformMimeUtil::GetPlatformMimeTypeFromExtension(
    const base::FilePath::StringType& ext,
    std::string* result) const {
  std::vector<std::string> vec = MimeTypeOhos::GetMimeTypeByExtension(ext);
  if (vec.empty()) {
    return false;
  }
  *result = vec.front();
  return true;
}

bool PlatformMimeUtil::GetPlatformPreferredExtensionForMimeType(
    std::string_view mime_type,
    base::FilePath::StringType* ext) const {
  std::vector<std::string> vec =
      MimeTypeOhos::GetExtensionByMimeType(mime_type.data());
  if (vec.empty()) {
    return false;
  }
  *ext = vec.front();
  return true;
}

void PlatformMimeUtil::GetPlatformExtensionsForMimeType(
    std::string_view mime_type,
    std::unordered_set<base::FilePath::StringType>* extensions) const {
  std::vector<std::string> vec =
      MimeTypeOhos::GetExtensionByMimeType(mime_type.data());
  if (!vec.empty()) {
    for (const auto& str : vec) {
      extensions->insert(str);
    }
  }
}

}  // namespace net

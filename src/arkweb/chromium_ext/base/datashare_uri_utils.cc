// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/datashare_uri_utils.h"

#include <string>

#include "base/logging.h"
#include "base/strings/utf_string_conversions.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

namespace base {

File OpenDatashareUriForRead(const FilePath& datashare_uri) {
  std::string real_path = GetRealPath(datashare_uri);
  LOG(INFO) << "open datashare for read invoked, uri:" << datashare_uri.value();
  File file = File(FilePath(real_path), File::FLAG_OPEN | File::FLAG_READ);
  if (!file.IsValid()) {
    LOG(WARNING) << "open datashare for read file is not valid";
  }
  return file;
}
std::u16string GetFileDisplayName(const FilePath& datashare_uri) {
  std::string display_name = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                 .GetDatashareInstance()
                                 .GetFileDisplayName(datashare_uri.value());
  return base::UTF8ToUTF16(display_name);
}
std::string GetRealPath(const FilePath& datashare_uri) {
  return OHOS::NWeb::OhosAdapterHelper::GetInstance()
      .GetDatashareInstance()
      .GetRealPath(datashare_uri.value());
}
}  // namespace base

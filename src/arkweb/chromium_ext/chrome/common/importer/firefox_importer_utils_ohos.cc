// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/base_paths.h"
#include "base/files/file_path.h"
#include "chrome/common/importer/firefox_importer_utils.h"

base::FilePath GetProfilesINI() {
  // it is not supported on ohos platform
  return base::FilePath();
}

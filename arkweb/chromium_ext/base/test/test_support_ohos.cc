// Copyright (c) 2023 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TEST_TEST_SUPPORT_OHOS_H_
#define BASE_TEST_TEST_SUPPORT_OHOS_H_


#include "base/path_service.h"
#include "base/files/file_path.h"
#include "base/base_paths.h"

namespace {
// Provides the test path for paths overridden during tests.
bool GetTestProviderPath(int key, base::FilePath* result) {
  // On OHOS, such directory relate to arkui env, unittest cannot get. MOCK it.
  switch (key) {
    case base::DIR_TEMP:
      *result = base::FilePath("./tmp");
      return true;
    case base::DIR_CACHE:
      *result = base::FilePath("/data/local");
      return true;
    case base::DIR_USER_DESKTOP:
      *result = base::FilePath("./user_desktop");
      return true;
    case base::DIR_SRC_TEST_DATA_ROOT:
      *result = base::FilePath("./src");
      return true;
    case base::DIR_TEST_DATA:
      *result = base::FilePath("./test_data");
      return true;
    case base::DIR_EXE:
    case base::DIR_MODULE:
    case base::DIR_OHOS_APP_DATA:
      *result = base::FilePath(base::FilePath::kCurrentDirectory);
      return true;
    case base::DIR_GEN_TEST_DATA_ROOT:
      *result = base::FilePath("./test_gen_data");
      return true;
    default:
      return false;
  }
}

void InitPathProvider(int key) {
  base::FilePath path;
  // If failed to override the key, that means the way has not been registered.
  if (GetTestProviderPath(key, &path) &&
      !base::PathService::Override(key, path)) {
    base::PathService::RegisterProvider(&GetTestProviderPath, key, key + 1);
  }
}
}

namespace base {
void RegisterPathProviderForOhosTest() {
  InitPathProvider(DIR_TEMP);
  InitPathProvider(DIR_SRC_TEST_DATA_ROOT);
  InitPathProvider(DIR_TEST_DATA);
  InitPathProvider(DIR_EXE);
  InitPathProvider(DIR_OHOS_APP_DATA);
  InitPathProvider(DIR_GEN_TEST_DATA_ROOT);
  InitPathProvider(DIR_CACHE);
  InitPathProvider(DIR_USER_DESKTOP);
  InitPathProvider(DIR_SRC_TEST_DATA_ROOT);
  InitPathProvider(DIR_MODULE);
}

}  // namespace base
#endif  // BASE_TEST_TEST_SUPPORT_OHOS_H_

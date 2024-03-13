// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_BASE_PATHS_OHOS_H_
#define BASE_BASE_PATHS_OHOS_H_

namespace base {

enum {
  PATH_OHOS_START = 500,

  DIR_OHOS_APP_DATA,
  DIR_OHOS_EXTERNAL_STORAGE,
  DIR_OHOS_APP_INSTALLATION,
#if defined(OHOS_CRASHPAD)
  DIR_OHOS_CRASHPAD,
#endif
  PATH_OHOS_END
};

}  // namespace base

#endif  // BASE_BASE_PATHS_OHOS_H_

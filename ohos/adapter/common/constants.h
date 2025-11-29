// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_COMMON_CONSTANTS_H_
#define OHOS_ADAPTER_COMMON_CONSTANTS_H_

#ifndef DECLARE_NAPI_METHOD
#define DECLARE_NAPI_METHOD(name, func) \
  { name, 0, func, 0, 0, 0, napi_default, 0 }
#endif

enum ProcessType {
  kMainProcess = 0,
  kRenderProcess,
  kCrashpadHandler,
  kGpuProcess,
};

#endif  // OHOS_ADAPTER_COMMON_CONSTANTS_H_

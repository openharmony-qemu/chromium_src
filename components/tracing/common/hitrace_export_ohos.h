// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef COMPONENTS_TRACING_COMMON_HITRACE_EXPORT_OHOS_H_
#define COMPONENTS_TRACING_COMMON_HITRACE_EXPORT_OHOS_H_

#include "components/tracing/tracing_export.h"

namespace tracing {

class TRACING_EXPORT HitraceInterceptorController {
 public:
  /** register as an interceptor */
  static void EnableHitraceOnSetup();
};
}  // namespace tracing

#endif  // COMPONENTS_TRACING_COMMON_HITRACE_EXPORT_OHOS_H_
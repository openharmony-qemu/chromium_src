// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_TRACING_ADAPTER_H_
#define OHOS_ADAPTER_TRACING_ADAPTER_H_

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

#include "ohos/adapter/export.h"

namespace ohos::adapter::tracing {

using TraceControllerCallback = std::function<void(int32_t, std::string)>;

class ADAPTER_EXPORT_API TracingAdapter {
 public:
  static TracingAdapter& GetInstance();

  TracingAdapter(const TracingAdapter&) = delete;
  TracingAdapter& operator=(const TracingAdapter&) = delete;

  void RegisterTracingCallback(TraceControllerCallback callback);
  void UnregisterTracingCallback();

  // used for control adapter level trace status
  void EnableAdapterTrace();
  void DisableAdapterTrace();
  bool IsAdapterTraceEnabled();

 private:
  TracingAdapter() = default;
  ~TracingAdapter() = default;
};
}  // namespace ohos::adapter::tracing
#endif  // OHOS_ADAPTER_TRACING_ADAPTER_H_

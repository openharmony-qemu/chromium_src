/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
  TraceControllerCallback GetTracingCallback();

  // used for control adapter level trace status
  void EnableAdapterTrace();
  void DisableAdapterTrace();
  bool IsAdapterTraceEnabled();

 private:
  TracingAdapter() = default;
  ~TracingAdapter() = default;

  TraceControllerCallback callback_;
};
}  // namespace ohos::adapter::tracing
#endif  // OHOS_ADAPTER_TRACING_ADAPTER_H_

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

#ifndef OHOS_ADAPTER_AKI_HOOK_AKI_HOOK_H_
#define OHOS_ADAPTER_AKI_HOOK_AKI_HOOK_H_

#include <utility>
#include "aki/jsbind.h"
#include "ohos/adapter/common/logging.h"
#include "ohos/adapter/common/trace.h"
namespace ohos {
namespace adapter {
class AkiHookLogger {
 public:
  explicit AkiHookLogger(const std::string& func_name);

  ~AkiHookLogger();

 private:
  std::string function_name_;
};

class JSFunction {
 public:
  JSFunction(const aki::JSFunction* func, const std::string& function_name)
      : func_(func), function_name_(function_name) {}

  template <typename R, typename... P>
  R Invoke(P... args) const {
    TRACE_EVENT_0(function_name_);
    return func_->Invoke<R>(std::forward<P>(args)...);
  }

  template <typename R, typename... P>
  std::future<R> InvokeAsync(P... args) const {
    TRACE_EVENT_0(function_name_);
    return func_->InvokeAsync<R>(std::forward<P>(args)...);
  }

 private:
  const aki::JSFunction* func_;
  std::string function_name_;
};

const JSFunction* GetJSFunction(const std::string& func_name);

}  // namespace adapter
}  // namespace ohos
#endif  // OHOS_ADAPTER_AKI_HOOK_AKI_HOOK_H_

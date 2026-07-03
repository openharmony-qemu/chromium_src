/*
 * Copyright (c) 2023-2026 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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

#ifndef OHOS_ADAPTER_XCOMPONENT_ADAPTER_NATIVE_WINDOW_WRAPPER_H_
#define OHOS_ADAPTER_XCOMPONENT_ADAPTER_NATIVE_WINDOW_WRAPPER_H_

#include <memory>
#include <mutex>
#include <utility>

#include <ace/xcomponent/native_interface_xcomponent.h>

#include "ohos/adapter/export.h"

namespace ohos::adapter::xcomponent {

class ADAPTER_EXPORT_API NativeWindowWrapper {
public:
  explicit NativeWindowWrapper(OHNativeWindow* window);
  ~NativeWindowWrapper();

  NativeWindowWrapper(NativeWindowWrapper&& other) noexcept;
  NativeWindowWrapper& operator=(NativeWindowWrapper&& other) noexcept;

  NativeWindowWrapper(const NativeWindowWrapper&) = delete;
  NativeWindowWrapper& operator=(const NativeWindowWrapper&) = delete;

  OHNativeWindow* Get() const;
  bool IsValid() const;

private:
  OHNativeWindow* window_ = nullptr;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_ADAPTER_NATIVE_WINDOW_WRAPPER_H_
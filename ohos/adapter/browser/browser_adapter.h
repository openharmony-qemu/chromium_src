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

#ifndef OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_
#define OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_

#include <functional>
#include "ohos/adapter/export.h"

namespace ohos::adapter {

enum class BrowserCloseResponse {
  kUndetermined,
  kClosingContinue,
  kClosingInterrupt,
  kClosed,
  kCloseCancelled,
  kClosedAnyway,
};

class ADAPTER_EXPORT_API BrowserAdapter {
 public:
  static BrowserAdapter& GetInstance();
  using NewWindowCallback = std::function<void(const std::string&, bool)>;
  void StartNewWindow(const std::string& url, bool force_open);
  NewWindowCallback GetNewWindowCallback() { return new_window_callback_; }
  void RegisterNewWindowCallback(NewWindowCallback callback);
  using GetLastActiveCallback = std::function<uint32_t()>;
  void RegisterGetLastActiveCallback(GetLastActiveCallback callback);
  void UnRegisterGetLastActiveCallback();
  uint32_t GetLastActiveAcceleratedWidget();
  BrowserCloseResponse GetBrowserCloseResponse(int32_t id);
  void SetBrowserCloseResponse(int32_t id, BrowserCloseResponse response);

 private:
  BrowserAdapter() = default;
  ~BrowserAdapter() = default;
  NewWindowCallback new_window_callback_;
  GetLastActiveCallback get_last_active_callback_;
  std::mutex mutex_;
  std::unordered_map<int32_t, BrowserCloseResponse> close_responses_;
};
}

#endif // OHOS_ADAPTER_BROWSER_BROWSER_ADAPTER_H_

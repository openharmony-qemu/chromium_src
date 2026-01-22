/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#ifndef OHOS_ADAPTER_SCREEN_CAPTURE_ADAPTER_H_
#define OHOS_ADAPTER_SCREEN_CAPTURE_ADAPTER_H_

#include <multimedia/player_framework/native_avscreen_capture.h>
#include <napi/native_api.h>

#include "ohos/adapter/common/shared_library.h"
#include "ohos/adapter/export.h"

namespace ohos::adapter {

using CreateCaptureStrategyFunc = OH_AVScreenCapture_CaptureStrategy*();
using StrategyForCanvasFollowRotationFunc =
    OH_AVSCREEN_CAPTURE_ErrCode(OH_AVScreenCapture_CaptureStrategy* strategy,
                                bool value);
using SetCaptureStrategyFunc =
    OH_AVSCREEN_CAPTURE_ErrCode(struct OH_AVScreenCapture* capture,
                                OH_AVScreenCapture_CaptureStrategy* strategy);
using ReleaseCaptureStrategyFunc =
    OH_AVSCREEN_CAPTURE_ErrCode(OH_AVScreenCapture_CaptureStrategy* strategy);

using ShowCursorFunc =
    OH_AVSCREEN_CAPTURE_ErrCode(struct OH_AVScreenCapture* capture, bool value);

using OnCaptureContentChangedFunc =
    void(OH_AVScreenCapture* capture,
         OH_AVScreenCaptureContentChangedEvent event,
         OH_Rect* area,
         void* user_data);

using SetCaptureContentChangedFunc = OH_AVSCREEN_CAPTURE_ErrCode(
    struct OH_AVScreenCapture* capture,
    OnCaptureContentChangedFunc callback,
    void* user_data);

class ADAPTER_EXPORT_API ScreenCaptureAdapter {
 public:
  static ScreenCaptureAdapter& GetInstance();
  ScreenCaptureAdapter(const ScreenCaptureAdapter&) = delete;
  ScreenCaptureAdapter& operator=(const ScreenCaptureAdapter&) = delete;

  bool SetAutoRotation(OH_AVScreenCapture* screen_capture);

  OH_AVSCREEN_CAPTURE_ErrCode ShowCursor(OH_AVScreenCapture* screen_capture);

  OH_AVSCREEN_CAPTURE_ErrCode SetCaptureContentChanged(
      OH_AVScreenCapture* screen_capture,
      OnCaptureContentChangedFunc callback,
      void* user_data);

 private:
  bool LoadAllFunctions();
  common::SharedLibrary native_screen_capture_lib_;
  bool is_support_native_screen_capture_ = false;

  ScreenCaptureAdapter();
  ~ScreenCaptureAdapter();

  OH_AVScreenCapture_CaptureStrategy* CreateCaptureStrategy();
  OH_AVSCREEN_CAPTURE_ErrCode StrategyForCanvasFollowRotation(
      OH_AVScreenCapture_CaptureStrategy* strategy,
      bool value);
  OH_AVSCREEN_CAPTURE_ErrCode SetCaptureStrategy(
      struct OH_AVScreenCapture* capture,
      OH_AVScreenCapture_CaptureStrategy* strategy);
  OH_AVSCREEN_CAPTURE_ErrCode ReleaseCaptureStrategy(
      OH_AVScreenCapture_CaptureStrategy* strategy);

  CreateCaptureStrategyFunc* create_capture_strategy_func_ = nullptr;
  StrategyForCanvasFollowRotationFunc*
      strategy_for_canvas_follow_rotation_func_ = nullptr;
  SetCaptureStrategyFunc* set_capture_strategy_func_ = nullptr;
  ReleaseCaptureStrategyFunc* release_capture_strategy_func_ = nullptr;
  ShowCursorFunc* show_cursor_func_ = nullptr;
  SetCaptureContentChangedFunc* set_capture_content_changed_func_ = nullptr;
};

}  // namespace ohos::adapter
#endif  // OHOS_ADAPTER_SCREEN_CAPTURE_ADAPTER_H_

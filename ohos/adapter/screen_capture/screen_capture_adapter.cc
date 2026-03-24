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

#include "ohos/adapter/screen_capture/screen_capture_adapter.h"

#include "ohos/adapter/aki_hook/aki_hook.h"

namespace ohos::adapter {

ScreenCaptureAdapter& ScreenCaptureAdapter::GetInstance() {
  static ScreenCaptureAdapter instance;
  return instance;
}

ScreenCaptureAdapter::ScreenCaptureAdapter()
    : native_screen_capture_lib_("native_avscreen_capture") {
  if (!native_screen_capture_lib_.IsLoaded()) {
    LOGE("ScreenCaptureAdapter load libnative_avscreen_capture.so failed");
    return;
  }

  if (!LoadAllFunctions()) {
    LOGE("ScreenCaptureAdapter load functions failed");
    return;
  }

  is_support_native_screen_capture_ = true;
}

ScreenCaptureAdapter::~ScreenCaptureAdapter() {
  create_capture_strategy_func_ = nullptr;
  strategy_for_canvas_follow_rotation_func_ = nullptr;
  set_capture_strategy_func_ = nullptr;
  release_capture_strategy_func_ = nullptr;
}

bool ScreenCaptureAdapter::LoadAllFunctions() {
  if (!native_screen_capture_lib_.LoadFunction(
          &create_capture_strategy_func_,
          "OH_AVScreenCapture_CreateCaptureStrategy")) {
    LOGE("load function OH_AVScreenCapture_CreateCaptureStrategy failed");
    return false;
  }
  if (!native_screen_capture_lib_.LoadFunction(
          &strategy_for_canvas_follow_rotation_func_,
          "OH_AVScreenCapture_StrategyForCanvasFollowRotation")) {
    LOGE(
        "load function OH_AVScreenCapture_StrategyForCanvasFollowRotation "
        "failed");
    return false;
  }
  if (!native_screen_capture_lib_.LoadFunction(
          &set_capture_strategy_func_,
          "OH_AVScreenCapture_SetCaptureStrategy")) {
    LOGE("load function OH_AVScreenCapture_SetCaptureStrategy failed");
    return false;
  }
  if (!native_screen_capture_lib_.LoadFunction(
          &release_capture_strategy_func_,
          "OH_AVScreenCapture_ReleaseCaptureStrategy")) {
    LOGE("load function OH_AVScreenCapture_ReleaseCaptureStrategy failed");
    return false;
  }

  if (!native_screen_capture_lib_.LoadFunction(
          &show_cursor_func_, "OH_AVScreenCapture_ShowCursor")) {
    LOGE("load function OH_AVScreenCapture_ShowCursor failed");
    return false;
  }

  if (!native_screen_capture_lib_.LoadFunction(
          &set_capture_content_changed_func_,
          "OH_AVScreenCapture_SetCaptureContentChangedCallback")) {
    LOGE(
        "load function OH_AVScreenCapture_SetCaptureContentChangedCallback "
        "failed");
    return false;
  }

  return true;
}

bool ScreenCaptureAdapter::SetAutoRotation(OH_AVScreenCapture* screen_capture) {
  if (!is_support_native_screen_capture_) {
    LOGE("[ScreenCapture] not support SetAutoRotation");
    return false;
  }

  OH_AVScreenCapture_CaptureStrategy* strategy = CreateCaptureStrategy();
  if (!strategy) {
    LOGE("[ScreenCapture] CreateCaptureStrategy failed");
    return false;
  }
  OH_AVSCREEN_CAPTURE_ErrCode ret =
      StrategyForCanvasFollowRotation(strategy, true);
  if (ret != AV_SCREEN_CAPTURE_ERR_OK) {
    LOGE(
        "[ScreenCapture] StrategyForCanvasFollowRotation failed,result: "
        "%{public}d",
        ret);
    ReleaseCaptureStrategy(strategy);
    return false;
  }

  ret = SetCaptureStrategy(screen_capture, strategy);
  if (ret != AV_SCREEN_CAPTURE_ERR_OK) {
    LOGE("[ScreenCapture] SetCaptureStrategy failed,result: %{public}d", ret);
  }
  ret = ReleaseCaptureStrategy(strategy);
  if (ret != AV_SCREEN_CAPTURE_ERR_OK) {
    LOGE("[ScreenCapture] ReleaseCaptureStrategy failed,result: %{public}d",
         ret);
  }

  return true;
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
OH_AVScreenCapture_CaptureStrategy*
ScreenCaptureAdapter::CreateCaptureStrategy() {
  return create_capture_strategy_func_();
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
OH_AVSCREEN_CAPTURE_ErrCode
ScreenCaptureAdapter::StrategyForCanvasFollowRotation(
    OH_AVScreenCapture_CaptureStrategy* strategy,
    bool value) {
  return strategy_for_canvas_follow_rotation_func_(strategy, value);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
OH_AVSCREEN_CAPTURE_ErrCode ScreenCaptureAdapter::SetCaptureStrategy(
    struct OH_AVScreenCapture* capture,
    OH_AVScreenCapture_CaptureStrategy* strategy) {
  return set_capture_strategy_func_(capture, strategy);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
OH_AVSCREEN_CAPTURE_ErrCode ScreenCaptureAdapter::ReleaseCaptureStrategy(
    OH_AVScreenCapture_CaptureStrategy* strategy) {
  return release_capture_strategy_func_(strategy);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
OH_AVSCREEN_CAPTURE_ErrCode ScreenCaptureAdapter::ShowCursor(
    OH_AVScreenCapture* screen_capture) {
  if (!is_support_native_screen_capture_) {
    return AV_SCREEN_CAPTURE_ERR_INVALID_STATE;
  }
  return show_cursor_func_(screen_capture, true);
}

__attribute__((no_sanitize("cfi", "cfi-icall")))
OH_AVSCREEN_CAPTURE_ErrCode ScreenCaptureAdapter::SetCaptureContentChanged(
    OH_AVScreenCapture* screen_capture,
    OnCaptureContentChangedFunc callback,
    void* user_data) {
  if (!is_support_native_screen_capture_) {
    return AV_SCREEN_CAPTURE_ERR_INVALID_STATE;
  }
  return set_capture_content_changed_func_(screen_capture, callback, user_data);
}

}  // namespace ohos::adapter

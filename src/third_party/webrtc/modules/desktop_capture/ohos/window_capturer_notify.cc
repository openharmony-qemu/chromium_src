// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "modules/desktop_capture/ohos/window_capturer_notify.h"

#include "base/logging.h"
#include "base/memory/singleton.h"

namespace webrtc {

void OnStateChange(struct OH_AVScreenCapture* capture,
                   OH_AVScreenCaptureStateCode stateCode,
                   void* userData) {
  auto callback =
      WindowCapturerNotify::GetInstance()->GetEventCallBack(capture);
  if (callback == nullptr) {
    LOG(ERROR) << "Get capturer callback error,capture=" << capture;
    return;
  }
  callback->OnStateChange(stateCode, userData);
}

void OnError(OH_AVScreenCapture* capture, int32_t errorCode, void* userData) {
  auto callback =
      WindowCapturerNotify::GetInstance()->GetEventCallBack(capture);
  if (callback == nullptr) {
    LOG(ERROR) << "Get capturer callback error,capture=" << capture;
    return;
  }
  callback->OnError(errorCode, userData);
}

void OnBufferAvailable(OH_AVScreenCapture* capture,
                       OH_AVBuffer* buffer,
                       OH_AVScreenCaptureBufferType bufferType,
                       int64_t timestamp,
                       void* userData) {
  auto callback =
      WindowCapturerNotify::GetInstance()->GetEventCallBack(capture);
  if (callback == nullptr) {
    LOG(ERROR) << "Get capturer callback error,capture=" << capture;
    return;
  }
  callback->OnBufferAvailable(buffer, bufferType, timestamp, userData);
}

WindowCapturerNotify* WindowCapturerNotify::GetInstance() {
  return base::Singleton<WindowCapturerNotify>::get();
}

WindowCapturerNotify::WindowCapturerNotify() {}

WindowCapturerNotify::~WindowCapturerNotify() {
  callback_map_.clear();
}

CapturerEventCallBack* WindowCapturerNotify::GetEventCallBack(
    OH_AVScreenCapture* capture) {
  if (callback_map_.find(capture) != callback_map_.end()) {
    return callback_map_[capture].get();
  }

  return nullptr;
}

bool WindowCapturerNotify::RegisterEventCallback(
    OH_AVScreenCapture* capture,
    std::shared_ptr<CapturerEventCallBack> callback,
    void* userData) {
  if (callback_map_.count(capture)) {
    return true;
  }

  OH_AVSCREEN_CAPTURE_ErrCode errCode =
      OH_AVScreenCapture_SetErrorCallback(capture, OnError, userData);
  if (errCode != AV_SCREEN_CAPTURE_ERR_OK) {
    LOG(ERROR) << "screen capture set error callback failed, errCode:"
               << errCode;
    return false;
  }

  errCode =
      OH_AVScreenCapture_SetStateCallback(capture, OnStateChange, userData);
  if (errCode != AV_SCREEN_CAPTURE_ERR_OK) {
    LOG(ERROR) << "screen capture set state callback failed, errCode:"
               << errCode;
    return false;
  }

  errCode =
      OH_AVScreenCapture_SetDataCallback(capture, OnBufferAvailable, userData);
  if (errCode != AV_SCREEN_CAPTURE_ERR_OK) {
    LOG(ERROR) << "screen capture set data callback failed, errCode:"
               << errCode;
    return false;
  }
  callback_map_[capture] = std::move(callback);
  return true;
}

void WindowCapturerNotify::UnRegisterEventCallback(
    OH_AVScreenCapture* capture) {
  if (!callback_map_.count(capture)) {
    return;
  }
  callback_map_.erase(capture);
}

}  // namespace webrtc

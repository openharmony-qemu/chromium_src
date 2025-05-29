// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MODULES_DESKTOP_CAPTURE_OHOS_BASE_WINDOW_CAPTURER_H_
#define MODULES_DESKTOP_CAPTURE_OHOS_BASE_WINDOW_CAPTURER_H_

#include <memory>
#include <optional>

#include "absl/types/optional.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "third_party/webrtc/modules/desktop_capture/desktop_capture_options.h"
#include "third_party/webrtc/modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/ohos/desktop_capture_frame.h"
#include "modules/desktop_capture/ohos/native_frame.h"
#include "modules/desktop_capture/screen_capture_frame_queue.h"
#include "modules/desktop_capture/shared_desktop_frame.h"
#include "multimedia/player_framework/native_avscreen_capture.h"
#include "native_buffer/native_buffer.h"
#include "rtc_base/synchronization/mutex.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/screen_capture_adapter.h"
#include "base/memory/scoped_refptr.h"
#include "media/base/audio_capturer_source.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/base_screen_capture_read_callback.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/base_screen_capture_source_bridge.h"

namespace webrtc {
class BaseScreenCaptureReadCallback;

using namespace OHOS::NWeb;
using namespace media;

using OnReadDataCallback = base::RepeatingCallback<void(void)>;

class WindowCapturerReadCallback : public BaseScreenCaptureReadCallback {
 public:
  WindowCapturerReadCallback(const OnReadDataCallback& readDataCallback);

  ~WindowCapturerReadCallback();

  void OnReadData() override;

  void OnReadData(OHOS::NWeb::AudioCaptureSourceTypeAdapter type) override {}

 private:
  OnReadDataCallback readDataCallback_;
};

class BaseWindowCapturer : public DesktopCapturer {
 public:
  enum class CaptureSourceType : uint32_t {
    kScreen = 0b01,
    kWindow = 0b10,
    kAny = 0b11
  };

  enum class CursorMode : uint32_t {
    kHidden = 0b01,
    kEmbedded = 0b10,
    kMetadata = 0b100
  };

  struct PipeWireVersion {
    int major = 0;
    int minor = 0;
    int micro = 0;
  };

  explicit BaseWindowCapturer(CaptureSourceType source_type, bool is_picker_show, int nweb_id);
  ~BaseWindowCapturer() override;

  static std::unique_ptr<DesktopCapturer> CreateRawCapturer(
      const DesktopCaptureOptions& options,
      const CaptureSourceType& type);

  bool Init(const DesktopCaptureOptions& options);

  // DesktopCapturer interface.
  void Start(Callback* delegate) override;
  void CaptureFrame() override;
  bool GetSourceList(SourceList* sources) override;
  bool SelectSource(SourceId id) override;

  void SetSharedMemoryFactory(
      std::unique_ptr<SharedMemoryFactory> shared_memory_factory) override;

  // OHOSScreenCaptureCallback interface.
  void HandleStateChange(OH_AVScreenCaptureStateCode stateCode, void* userData);
  void HandleError(int32_t errorCode, void* userData);

  void HandleBuffer();

  // void HandleBuffer(OH_AVBuffer* buffer,
  //                   OH_AVScreenCaptureBufferType bufferType,
  //                   int64_t timestamp,
  //                   void* userData);

  void SetScreenCaptureState(
      const OHOS::NWeb::ScreenCaptureStateCodeAdapter& stateCode);

 private:
  static DesktopCapturer::Result HandleCaptureStateCode(
      const OHOS::NWeb::ScreenCaptureStateCodeAdapter& code);

  void DoCapture();
  void DoCaptureSync();
  bool EnsureFrameCaptured();
  void StopCapture();
  void SetState(bool isStart) { isStart_ = isStart; }

  webrtc::Mutex current_frame_lock_;

  std::unique_ptr<DesktopFrame> current_frame_;

  DesktopCaptureOptions options_;
  ScreenCaptureFrameQueue<DesktopCaptureFrame> queue_;

  OHOS::NWeb::ScreenCaptureStateCodeAdapter capture_state_code_ =
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID;

  Callback* callback_ = nullptr;

  CaptureSourceType capture_source_type_ =
      BaseWindowCapturer::CaptureSourceType::kScreen;

  bool portal_init_failed_ = false;

  std::unique_ptr<SharedMemoryFactory> shared_memory_factory_ = nullptr;

  std::unique_ptr<SharedMemoryFactory> factory_ = nullptr;

  bool isStart_ = false;

  SourceId selectSourceId_ = -1;

  std::shared_ptr<WindowCapturerReadCallback> WindowCapturerReadCallback_ = nullptr;

  OH_AVScreenCapture* screen_capture_ = nullptr;
  //ohos::adapter::OhosDisplay ohos_screen_;

  int nweb_id_ = 0;
  base::WeakPtrFactory<BaseWindowCapturer> weak_factory_{this};
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_OHOS_BASE_WINDOW_CAPTURER_H_

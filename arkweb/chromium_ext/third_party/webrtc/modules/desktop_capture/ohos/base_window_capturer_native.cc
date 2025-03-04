// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <limits>
#include <string>
#include <utility>

#include "absl/memory/memory.h"
#include "base/logging.h"
#include "base/memory/platform_shared_memory_region.h"
#include "base/memory/ptr_util.h"
#include "base/memory/read_only_shared_memory_region.h"
#include "base/task/single_thread_task_runner.h"
#include "base_window_capturer.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "modules/desktop_capture/ohos/capturer_event_callback.h"
#include "modules/desktop_capture/ohos/window_capturer_notify.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/string_encode.h"
#include "rtc_base/string_to_number.h"
#include "rtc_base/time_utils.h"
#include "system_wrappers/include/sleep.h"

namespace webrtc {
namespace {
constexpr int32_t kVideoFrameWidth = 1024;
constexpr int32_t kVideoFrameHeight = 1080;
constexpr int32_t kVideoBitrate = 2000000;
constexpr int32_t kVideoFrameRate = 30;

class SharedMemoryImpl : public webrtc::SharedMemory {
 public:
  static std::unique_ptr<SharedMemoryImpl> Create(size_t size, int id) {
    webrtc::SharedMemory::Handle handle = webrtc::SharedMemory::kInvalidHandle;
    base::MappedReadOnlyRegion region_mapping =
        base::ReadOnlySharedMemoryRegion::Create(size);
    base::ReadOnlySharedMemoryRegion read_only_region =
        std::move(region_mapping.region);
    base::WritableSharedMemoryMapping mapping =
        std::move(region_mapping.mapping);
    if (!mapping.IsValid()) {
      return nullptr;
    }
    // The SharedMemoryImpl ctor is private, so std::make_unique can't be
    // used.
    return base::WrapUnique(new SharedMemoryImpl(
        std::move(read_only_region), std::move(mapping), handle, id));
  }

  SharedMemoryImpl(const SharedMemoryImpl&) = delete;
  SharedMemoryImpl& operator=(const SharedMemoryImpl&) = delete;

  ~SharedMemoryImpl() override {}

  const base::ReadOnlySharedMemoryRegion& region() const { return region_; }

 private:
  SharedMemoryImpl(base::ReadOnlySharedMemoryRegion region,
                   base::WritableSharedMemoryMapping mapping,
                   webrtc::SharedMemory::Handle handle,
                   int id)
      : SharedMemory(mapping.memory(), mapping.size(), handle, id) {
    region_ = std::move(region);
    mapping_ = std::move(mapping);
  }

  base::ReadOnlySharedMemoryRegion region_;
  base::WritableSharedMemoryMapping mapping_;
};

class SharedMemoryFactoryImpl : public webrtc::SharedMemoryFactory {
 public:
  explicit SharedMemoryFactoryImpl() {}

  SharedMemoryFactoryImpl(const SharedMemoryFactoryImpl&) = delete;
  SharedMemoryFactoryImpl& operator=(const SharedMemoryFactoryImpl&) = delete;

  std::unique_ptr<webrtc::SharedMemory> CreateSharedMemory(
      size_t size) override {
    std::unique_ptr<SharedMemoryImpl> buffer =
        SharedMemoryImpl::Create(size, next_shared_buffer_id_);
    if (buffer) {
      next_shared_buffer_id_ += 2;
    }
    return std::move(buffer);
  }

 private:
  int next_shared_buffer_id_ = 1;
};
}  // namespace

class OHOSScreenCaptureCallback final : public webrtc::CapturerEventCallBack {
 public:
  explicit OHOSScreenCaptureCallback(base::WeakPtr<BaseWindowCapturer> capturer)
      : capturer_(capturer) {
    DCHECK(capturer_);
  }

  OHOSScreenCaptureCallback(const OHOSScreenCaptureCallback&) = delete;
  OHOSScreenCaptureCallback& operator=(const OHOSScreenCaptureCallback&) =
      delete;

  ~OHOSScreenCaptureCallback() = default;

  void OnStateChange(OH_AVScreenCaptureStateCode stateCode,
                     void* userData) override {
    capturer_->HandleStateChange(stateCode, userData);
  }

  void OnError(int32_t errorCode, void* userData) override {
    capturer_->HandleError(errorCode, userData);
  }

  void OnBufferAvailable(OH_AVBuffer* buffer,
                         OH_AVScreenCaptureBufferType bufferType,
                         int64_t timestamp,
                         void* userData) override {
    capturer_->HandleBuffer(buffer, bufferType, timestamp, userData);
  }

  void OnStateChange(
      OHOS::NWeb::ScreenCaptureStateCodeAdapter stateCode) override {
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&BaseWindowCapturer::SetScreenCaptureState,
                                  capturer_, stateCode));
  }

 private:
  base::WeakPtr<BaseWindowCapturer> capturer_;
};

void BaseWindowCapturer::SetSharedMemoryFactory(
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory) {
  shared_memory_factory_ = std::move(shared_memory_factory);
}

BaseWindowCapturer::BaseWindowCapturer(CaptureSourceType source_type)
    : capture_source_type_(source_type) {}

BaseWindowCapturer::~BaseWindowCapturer() {
  if (screen_capture_) {
    webrtc::WindowCapturerNotify::GetInstance()->UnRegisterEventCallback(
        screen_capture_);
    OH_AVScreenCapture_StopScreenCapture(screen_capture_);
    OH_AVScreenCapture_Release(screen_capture_);
    screen_capture_ = nullptr;
  }
  SetState(false);
  queue_.Reset();
}

bool BaseWindowCapturer::Init(const DesktopCaptureOptions& options) {
  options_ = options;

  ohos::adapter::ScreenAdapter::GetInstance().GetDefaultDisplay(ohos_screen_);
  int32_t videoFrameWidth =
      ohos_screen_.width > 0 ? ohos_screen_.width : kVideoFrameWidth;
  int32_t videoFrameHeight =
      ohos_screen_.height > 0 ? ohos_screen_.height : kVideoFrameHeight;

  screen_capture_ = OH_AVScreenCapture_Create();
  if (!screen_capture_) {
    LOG(ERROR) << "create screen capture adapter failed";
    return false;
  }

  OH_VideoCaptureInfo videoCapInfo = {
      .videoFrameWidth = videoFrameWidth,
      .videoFrameHeight = videoFrameHeight,
      .videoSource = OH_VideoSourceType::OH_VIDEO_SOURCE_SURFACE_RGBA};
  OH_VideoEncInfo videoEncInfo = {.videoCodec = OH_VideoCodecFormat::OH_H264,
                                  .videoBitrate = kVideoBitrate,
                                  .videoFrameRate = kVideoFrameRate};
  OH_VideoInfo videoInfo = {.videoCapInfo = videoCapInfo,
                            .videoEncInfo = videoEncInfo};
  OH_AVScreenCaptureConfig config = {
      .captureMode = OH_CaptureMode::OH_CAPTURE_HOME_SCREEN,
      .dataType = OH_DataType::OH_ORIGINAL_STREAM,
      .videoInfo = videoInfo};

  auto callback =
      std::make_shared<OHOSScreenCaptureCallback>(weak_factory_.GetWeakPtr());
  if (!webrtc::WindowCapturerNotify::GetInstance()->RegisterEventCallback(
          screen_capture_, callback, this)) {
    OH_AVScreenCapture_Release(screen_capture_);
    return false;
  }

  OH_AVSCREEN_CAPTURE_ErrCode errCode =
      OH_AVScreenCapture_Init(screen_capture_, config);
  if (errCode != AV_SCREEN_CAPTURE_ERR_OK) {
    LOG(ERROR) << "screen capture init failed, errCode:" << errCode;
    OH_AVScreenCapture_Release(screen_capture_);
    return false;
  }
  return true;
}

void BaseWindowCapturer::HandleStateChange(
    OH_AVScreenCaptureStateCode stateCode,
    void* userData) {
  switch (stateCode) {
    case OH_SCREEN_CAPTURE_STATE_STARTED:
      SetState(true);
      break;
    case OH_SCREEN_CAPTURE_STATE_CANCELED:
    case OH_SCREEN_CAPTURE_STATE_STOPPED_BY_USER:
    case OH_SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER:
      StopCapture();
      break;
    default:
      break;
  }
}

void BaseWindowCapturer::HandleError(int32_t errorCode, void* userData) {}

void BaseWindowCapturer::HandleBuffer(OH_AVBuffer* buffer,
                                      OH_AVScreenCaptureBufferType bufferType,
                                      int64_t timestamp,
                                      void* userData) {
  if (bufferType != OH_SCREEN_CAPTURE_BUFFERTYPE_VIDEO) {
    return;
  }

  webrtc::MutexLock lock(&current_frame_lock_);

  std::unique_ptr<NativeFrame> native_frame = NativeFrame::Create(buffer);
  if (!native_frame) {
    LOG(ERROR) << "acquire video buffer failed";
    StopCapture();
    return;
  }

  int32_t image_width = native_frame->image_width();
  int32_t image_height = native_frame->image_height();
  int32_t image_stride = native_frame->image_stride();

  queue_.MoveToNextFrame();
  if (queue_.current_frame() && queue_.current_frame()->frame() &&
      queue_.current_frame()->frame()->IsShared()) {
    LOG(WARNING) << "Overwriting frame that is still shared.";
  }

  DesktopSize image_size(image_width, image_height);
  if (!queue_.current_frame()) {
    queue_.ReplaceCurrentFrame(
        std::make_unique<DesktopCaptureFrame>(shared_memory_factory_.get()));
  }

  DesktopCaptureFrame* frame = queue_.current_frame();
  if (!frame->Prepare(image_size)) {
    LOG(ERROR) << "cannot create a new DesktopFrame";
    return;
  }
  frame->frame()->mutable_updated_region()->Clear();

  uint8_t* src_data = native_frame->GetBuffer();
  if (!src_data) {
    LOG(ERROR) << "get native buffer failed";
    return;
  }

  DesktopFrame* current_frame = frame->frame();
  current_frame->CopyPixelsFrom(src_data, image_stride,
                                DesktopRect::MakeSize(current_frame->size()));
  current_frame->mutable_updated_region()->SetRect(
      DesktopRect::MakeSize(current_frame->size()));

  return;
}

void BaseWindowCapturer::SetScreenCaptureState(
    const OHOS::NWeb::ScreenCaptureStateCodeAdapter& stateCode) {
  {
    webrtc::MutexLock lock(&current_frame_lock_);
    capture_state_code_ = stateCode;
  }
}

void BaseWindowCapturer::Start(Callback* callback) {
  RTC_DCHECK(!callback_);
  RTC_DCHECK(callback);
  (void)capture_source_type_;
  callback_ = callback;

  if (isStart_) {
    LOG(INFO) << "already started, no need to start again";
    return;
  }

  OH_AVSCREEN_CAPTURE_ErrCode errCode =
      OH_AVScreenCapture_StartScreenCapture(screen_capture_);
  if (errCode != AV_SCREEN_CAPTURE_ERR_OK) {
    LOG(ERROR) << "start capture failed, errCode:" << errCode;
    return;
  }
  SetState(true);
}

DesktopCapturer::Result BaseWindowCapturer::HandleCaptureStateCode(
    const OHOS::NWeb::ScreenCaptureStateCodeAdapter& code) {
  switch (code) {
    case OHOS::NWeb::ScreenCaptureStateCodeAdapter::
        SCREEN_CAPTURE_STATE_CANCELED:
    case OHOS::NWeb::ScreenCaptureStateCodeAdapter::
        SCREEN_CAPTURE_STATE_STOPPED_BY_USER:
    case OHOS::NWeb::ScreenCaptureStateCodeAdapter::
        SCREEN_CAPTURE_STATE_INTERRUPTED_BY_OTHER:
    case OHOS::NWeb::ScreenCaptureStateCodeAdapter::
        SCREEN_CAPTURE_STATE_STOPPED_BY_CALL:
      return DesktopCapturer::Result::ERROR_PERMANENT;
    case OHOS::NWeb::ScreenCaptureStateCodeAdapter::
        SCREEN_CAPTURE_STATE_ENTER_PRIVATE_SCENE:
      return DesktopCapturer::Result::ERROR_TEMPORARY;
    default:
      break;
  }
  return DesktopCapturer::Result::SUCCESS;
}

void BaseWindowCapturer::CaptureFrame() {
  if (portal_init_failed_) {
    callback_->OnCaptureResult(Result::ERROR_PERMANENT, nullptr);
    return;
  }

  std::unique_ptr<DesktopFrame> current_frame;
  OHOS::NWeb::ScreenCaptureStateCodeAdapter capture_state_code;
  {
    webrtc::MutexLock lock(&current_frame_lock_);
    capture_state_code = capture_state_code_;
    current_frame = std::move(current_frame_);
  }
  DesktopCapturer::Result result = HandleCaptureStateCode(capture_state_code);
  if (result != DesktopCapturer::Result::SUCCESS) {
    LOG(INFO) << "start capture interrupted or stopped";
    callback_->OnCaptureResult(result, nullptr);
    return;
  }

  if (!current_frame) {
    callback_->OnCaptureResult(Result::ERROR_TEMPORARY, nullptr);
    return;
  }
  callback_->OnCaptureResult(Result::SUCCESS, std::move(current_frame));
}

bool BaseWindowCapturer::GetSourceList(SourceList* sources) {
  switch (capture_source_type_) {
    case BaseWindowCapturer::CaptureSourceType::kScreen:
      RTC_DCHECK(sources->size() == 0);
      sources->push_back({ohos_screen_.id, ohos_screen_.name});
      return true;
    case BaseWindowCapturer::CaptureSourceType::kWindow:
      LOG(WARNING) << "capture window frame not supported";
      break;
    default:
      break;
  }

  return false;
}

bool BaseWindowCapturer::SelectSource(SourceId id) {
  selectSourceId_ = id;
  return true;
}

void BaseWindowCapturer::DoCapture() {
  if (!queue_.current_frame() || !queue_.current_frame()->frame()) {
    LOG(WARNING) << "no frames with a temporary error";
    callback_->OnCaptureResult(Result::ERROR_TEMPORARY, nullptr);
    return;
  }

  // Emit the current frame.
  std::unique_ptr<DesktopFrame> frame =
      queue_.current_frame()->frame()->Share();
  if (!frame) {
    LOG(ERROR) << "Failed to capture screen.";
    callback_->OnCaptureResult(Result::ERROR_PERMANENT, nullptr);
    return;
  }
  callback_->OnCaptureResult(Result::SUCCESS, std::move(frame));
}

bool BaseWindowCapturer::EnsureFrameCaptured() {
  // On a modern system, the FPS / monitor refresh rate is usually larger than
  // or equal to 60. So 17 milliseconds is enough to capture at least one frame.
  const int64_t ms_per_frame = 17;
  const int64_t timeout_ms = std::numeric_limits<int>::max();

  const int64_t start_ms = rtc::TimeMillis();
  while (isStart_) {
    if (queue_.current_frame() && queue_.current_frame()->frame()) {
      break;
    }

    if (rtc::TimeMillis() - start_ms > timeout_ms) {
      LOG(ERROR) << "Failed to capture frames within " << timeout_ms
                 << " milliseconds.";
      return false;
    }

    // Sleep `ms_per_frame` before attempting to capture the next frame to
    // ensure the video adapter has time to update the screen.
    webrtc::SleepMs(ms_per_frame);
  }
  return true;
}

void BaseWindowCapturer::DoCaptureSync() {
  if (!EnsureFrameCaptured()) {
    callback_->OnCaptureResult(Result::ERROR_PERMANENT, nullptr);
    return;
  }
  DoCapture();
}

void BaseWindowCapturer::StopCapture() {
  if (screen_capture_) {
    OH_AVScreenCapture_StopScreenCapture(screen_capture_);
  }

  SetState(false);
}

// static
std::unique_ptr<DesktopCapturer> BaseWindowCapturer::CreateRawCapturer(
    const DesktopCaptureOptions& options,
    const BaseWindowCapturer::CaptureSourceType& type) {
  std::unique_ptr<BaseWindowCapturer> capturer =
      std::make_unique<BaseWindowCapturer>(type);
  if (!capturer.get()->Init(options)) {
    return nullptr;
  }

  return std::move(capturer);
}

}  // namespace webrtc

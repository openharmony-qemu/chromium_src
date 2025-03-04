// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on base_capturer_pipewire.cc originally written by
// Copyright (c) 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "base_window_capturer.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <string>
#include <utility>
#include <vector>

#include "absl/memory/memory.h"
#include "audio_capture_info_adapter_impl.h"
#include "audio_enc_info_adapter_impl.h"
#include "audio_info_adapter_impl.h"
#include "base/logging.h"
#include "base/memory/platform_shared_memory_region.h"
#include "base/memory/ptr_util.h"
#include "base/memory/read_only_shared_memory_region.h"
#include "base/task/single_thread_task_runner.h"
#include "base/task/task_runner.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "recorder_info_adapter_impl.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/string_encode.h"
#include "rtc_base/string_to_number.h"
#include "rtc_base/time_utils.h"
#include "screen_capture_config_adapter_impl.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/webrtc/api/scoped_refptr.h"
#include "video_capture_info_adapter_impl.h"
#include "video_enc_info_adapter_impl.h"
#include "video_info_adapter_impl.h"

namespace webrtc {
namespace {
constexpr int32_t kAudioSampleRate = 16000;
constexpr int32_t kAudioChannels = 2;

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

class OHOSScreenCaptureCallback
    : public OHOS::NWeb::ScreenCaptureCallbackAdapter {
 public:
  OHOSScreenCaptureCallback(
      const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
      base::WeakPtr<BaseWindowCapturer> capturer)
      : task_runner_(task_runner), capturer_(capturer) {
    DCHECK(task_runner_.get());
    DCHECK(capturer_);
  }

  OHOSScreenCaptureCallback(const OHOSScreenCaptureCallback&) = delete;
  OHOSScreenCaptureCallback& operator=(const OHOSScreenCaptureCallback&) =
      delete;

  virtual ~OHOSScreenCaptureCallback() {}

  void OnError(int32_t errorCode) override {
    LOG(ERROR) << "OnError errorCode: " << errorCode;
    return;
  }

  void OnAudioBufferAvailable(
      bool isReady,
      OHOS::NWeb::AudioCaptureSourceTypeAdapter type) override {
    return;
  }

  void OnVideoBufferAvailable(bool isReady) override {
    if (!isReady) {
      LOG(ERROR) << "OnVideoBufferAvailable isReady is false";
      return;
    }
    task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&BaseWindowCapturer::HandleBuffer, capturer_));
  }

  void OnStateChange(
      OHOS::NWeb::ScreenCaptureStateCodeAdapter stateCode) override {
    task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&BaseWindowCapturer::SetScreenCaptureState,
                                  capturer_, stateCode));
  }

 private:
  scoped_refptr<base::SingleThreadTaskRunner> task_runner_;
  base::WeakPtr<BaseWindowCapturer> capturer_;
};

void BaseWindowCapturer::SetSharedMemoryFactory(
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory) {
  LOG(DEBUG) << "set shared memory factory";
  factory_ = std::move(shared_memory_factory);
}

BaseWindowCapturer::BaseWindowCapturer(CaptureSourceType source_type)
    : capture_source_type_(source_type) {
  portal_init_failed_ = true;
#if false
  task_runner_ = base::SingleThreadTaskRunner::GetCurrentDefault();
#endif
  task_runner_ = nullptr;
  if (!task_runner_) {
    LOG(ERROR) << "get task runner failed";
    return;
  }
  auto displayMgr =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateDisplayMgrAdapter();
  if (!displayMgr) {
    LOG(ERROR) << "create display manager Adapter failed";
    return;
  }
  auto display = displayMgr->GetDefaultDisplay();
  if (!display) {
    LOG(ERROR) << "display manager GetDefaultDisplay failed";
    return;
  }
  int32_t videoFrameWidth = display->GetWidth();
  int32_t videoFrameHeight = display->GetHeight();
  LOG(INFO) << "screen capture videoFrameWidth: " << videoFrameWidth
            << "; videoFrameHeight: " << videoFrameHeight;

  screen_capture_adapter_ =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateScreenCaptureAdapter();
  if (!screen_capture_adapter_) {
    LOG(ERROR) << "create screen capture adapter failed";
    return;
  }

  std::shared_ptr<OHOS::NWeb::AudioCaptureInfoAdapterImpl> micCapInfo =
      std::make_shared<OHOS::NWeb::AudioCaptureInfoAdapterImpl>();
  micCapInfo->SetAudioSampleRate(kAudioSampleRate);
  micCapInfo->SetAudioChannels(kAudioChannels);
  micCapInfo->SetAudioSource(
      OHOS::NWeb::AudioCaptureSourceTypeAdapter::SOURCE_DEFAULT);

  std::shared_ptr<OHOS::NWeb::AudioInfoAdapterImpl> audioInfo =
      std::make_shared<OHOS::NWeb::AudioInfoAdapterImpl>();
  audioInfo->SetMicCapInfo(micCapInfo);

  std::shared_ptr<OHOS::NWeb::VideoCaptureInfoAdapterImpl> videoCapInfo =
      std::make_shared<OHOS::NWeb::VideoCaptureInfoAdapterImpl>();
  videoCapInfo->SetVideoFrameWidth(videoFrameWidth);
  videoCapInfo->SetVideoFrameHeight(videoFrameHeight);
  videoCapInfo->SetVideoSourceType(
      OHOS::NWeb::VideoSourceTypeAdapter::VIDEO_SOURCE_SURFACE_RGBA);

  std::shared_ptr<OHOS::NWeb::VideoInfoAdapterImpl> videoInfo =
      std::make_shared<OHOS::NWeb::VideoInfoAdapterImpl>();
  videoInfo->SetVideoCapInfo(videoCapInfo);

  std::shared_ptr<OHOS::NWeb::ScreenCaptureConfigAdapterImpl> config =
      std::make_shared<OHOS::NWeb::ScreenCaptureConfigAdapterImpl>();
  config->SetCaptureMode(OHOS::NWeb::CaptureModeAdapter::CAPTURE_HOME_SCREEN);
  config->SetDataType(OHOS::NWeb::DataTypeAdapter::ORIGINAL_STREAM_DATA_TYPE);
  config->SetAudioInfo(audioInfo);
  config->SetVideoInfo(videoInfo);

  if (screen_capture_adapter_->Init(config) != 0) {
    screen_capture_adapter_ = nullptr;
    LOG(ERROR) << "screen capture init failed";
    return;
  }
  screen_capture_adapter_->SetMicrophoneEnable(false);
  auto callback = std::make_shared<OHOSScreenCaptureCallback>(
      task_runner_, weak_factory_.GetWeakPtr());
  if (screen_capture_adapter_->SetCaptureCallback(callback) != 0) {
    screen_capture_adapter_ = nullptr;
    LOG(ERROR) << "screen capture set capture callback failed";
    return;
  }
  LOG(INFO) << "BaseWindowCapturer init success";
  portal_init_failed_ = false;
}

BaseWindowCapturer::~BaseWindowCapturer() {
  LOG(DEBUG) << "BaseWindowCapturer::~BaseWindowCapturer";
  if (screen_capture_adapter_) {
    screen_capture_adapter_->StopCapture();
  }
}

void BaseWindowCapturer::HandleBuffer() {
  if (portal_init_failed_ || !screen_capture_adapter_) {
    LOG(ERROR) << "init failed";
    return;
  }

  std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> buffer =
      screen_capture_adapter_->AcquireVideoBuffer();
  if (!buffer) {
    LOG(ERROR) << "acquire video buffer failed";
    return;
  }

  int32_t format = buffer->GetFormat();
  if (format != OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGBA_8888) {
    LOG(ERROR) << "buffer format error";
    screen_capture_adapter_->ReleaseVideoBuffer();
    return;
  }
  int32_t width = buffer->GetWidth();
  int32_t height = buffer->GetHeight();
  int32_t stride = buffer->GetStride();
  uint32_t buffSize = buffer->GetSize();
  LOG(DEBUG) << "screen capture buffer, width: " << width
             << ", height: " << height << "; buffSize: " << buffSize
             << ", stride:" << stride;
  if (buffSize < height * stride) {
    LOG(ERROR) << "screen capture buff size error";
    screen_capture_adapter_->ReleaseVideoBuffer();
    return;
  }

  std::unique_ptr<DesktopFrame> current_frame;
  uint32_t frameStride = width * DesktopFrame::kBytesPerPixel;
  if (factory_) {
    uint32_t frame_size =
        static_cast<uint32_t>(width * height * DesktopFrame::kBytesPerPixel);
    current_frame = std::make_unique<webrtc::SharedMemoryDesktopFrame>(
        webrtc::DesktopSize(width, height), frameStride,
        factory_->CreateSharedMemory(frame_size).release());
  } else {
    current_frame =
        std::make_unique<BasicDesktopFrame>(DesktopSize(width, height));
  }

  char* pData = (char*)(current_frame->data());
  char* pSrcData = (char*)(buffer->GetVirAddr());
  if (!pData || !pSrcData) {
    LOG(ERROR) << "data or GetVirAddr failed";
    screen_capture_adapter_->ReleaseVideoBuffer();
    return;
  }
  for (int32_t i = 0; i < height; i++) {
    if (memcpy_s(pData, frameStride, pSrcData, frameStride) != EOK) {
      LOG(ERROR) << "data memcpy_s failed";
      screen_capture_adapter_->ReleaseVideoBuffer();
      return;
    }
    pData += frameStride;
    pSrcData += stride;
  }
  current_frame->mutable_updated_region()->SetRect(
      DesktopRect::MakeSize(current_frame->size()));

  {
    webrtc::MutexLock lock(&current_frame_lock_);
    current_frame_ = std::move(current_frame);
  }
  screen_capture_adapter_->ReleaseVideoBuffer();
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
  if (portal_init_failed_) {
    LOG(ERROR) << "init failed";
    return;
  }
  if (isStart_) {
    LOG(INFO) << "already started, no need to start again";
    return;
  }
  if (screen_capture_adapter_->StartCapture() != 0) {
    LOG(ERROR) << "start capture failed";
    return;
  }
  isStart_ = true;
  LOG(INFO) << "start capture success";
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
  RTC_DCHECK(sources->size() == 0);
  // List of available screens is already presented by the xdg-desktop-portal.
  // But we have to add an empty source as the code expects it.
  sources->push_back({0});
  return true;
}

bool BaseWindowCapturer::SelectSource(SourceId id) {
  // Screen selection is handled by the xdg-desktop-portal.
  LOG(INFO) << "select source id: " << id;
  selectSourceId_ = id;
  return true;
}

// static
std::unique_ptr<DesktopCapturer> BaseWindowCapturer::CreateRawCapturer(
    const DesktopCaptureOptions& options,
    const BaseWindowCapturer::CaptureSourceType& type) {
  return std::make_unique<BaseWindowCapturer>(type);
}

}  // namespace webrtc

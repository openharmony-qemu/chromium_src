// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on base_capturer_pipewire.cc originally written by
// Copyright (c) 2018 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "third_party/webrtc/modules/desktop_capture/ohos/base_window_capturer.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <string>
#include <utility>
#include <vector>

#include "absl/memory/memory.h"
#include "base/logging.h"
#include "base/memory/platform_shared_memory_region.h"
#include "base/memory/ptr_util.h"
#include "base/memory/read_only_shared_memory_region.h"
#include "base/task/single_thread_task_runner.h"
#include "media/audio/ohos/audio_dump.h"
#include "media/base/audio_bus.h"
#include "modules/desktop_capture/desktop_capture_options.h"
#include "modules/desktop_capture/desktop_capturer.h"
#include "rtc_base/checks.h"
#include "rtc_base/logging.h"
#include "rtc_base/string_encode.h"
#include "rtc_base/string_to_number.h"
#include "rtc_base/time_utils.h"
#include "third_party/bounds_checking_function/include/securec.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_buffer_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_capture_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_enc_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/audio_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/base_audio_capturer_source.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/base_screen_capture_source_bridge.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/recorder_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/screen_capture_config_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/video_capture_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/video_enc_info_adapter_impl.h"
#include "third_party/webrtc/modules/desktop_capture/ohos/video_info_adapter_impl.h"

namespace webrtc {
namespace {

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

WindowCapturerReadCallback::WindowCapturerReadCallback(
    const OnReadDataCallback& readDataCallback)
    : readDataCallback_(readDataCallback) {}

WindowCapturerReadCallback::~WindowCapturerReadCallback() {}

void WindowCapturerReadCallback::OnReadData() {
  if (!readDataCallback_.is_null()) {
    readDataCallback_.Run();
  }
}

void BaseWindowCapturer::SetSharedMemoryFactory(
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory) {
  LOG(DEBUG) << "set shared memory factory";
  factory_ = std::move(shared_memory_factory);
}

BaseWindowCapturer::BaseWindowCapturer(CaptureSourceType source_type, bool is_picker_show, int nweb_id,
                                       base::OnceCallback<void(uint64_t displayId)> callback)
    : capture_source_type_(source_type) {
  BaseScreenCaptureSource::GetInstance().SetDisplaySelectCallback(std::move(callback));
  portal_init_failed_ = true;

  LOG(INFO) << "BaseWindowCapturer, CreateBaseScreenCaptureSource: "
            << &BaseScreenCaptureSource::GetInstance();
  nweb_id_ = nweb_id;
  BaseScreenCaptureSource::GetInstance().SetScreenCapturePickerShow(is_picker_show);
  if (!BaseScreenCaptureSource::GetInstance().SetScreenCaptureConfig(nweb_id_)) {
      LOG(ERROR) << "BaseWindowCapturer init failed";
      return;
  }

  WindowCapturerReadCallback_ =
      std::make_shared<WindowCapturerReadCallback>(base::BindRepeating(
          &BaseWindowCapturer::HandleBuffer, weak_factory_.GetWeakPtr()));
  if (!WindowCapturerReadCallback_) {
    LOG(ERROR) << "window capturer read callback is nullptr";
    return;
  }
  BaseScreenCaptureSource::GetInstance().RegisterWindowCaptureCallback(WindowCapturerReadCallback_, nweb_id_);

  LOG(INFO) << "BaseWindowCapturer init success";
  portal_init_failed_ = false;
}

BaseWindowCapturer::~BaseWindowCapturer() {
  LOG(DEBUG) << "BaseWindowCapturer::~BaseWindowCapturer";
}

void BaseWindowCapturer::Stop()
{
  LOG(INFO) << "stop and release capture success";
  BaseScreenCaptureSource::GetInstance().StopCapture(nweb_id_);
  BaseScreenCaptureSource::GetInstance().ReleaseCapture(nweb_id_);
}

void BaseWindowCapturer::HandleBuffer() {
  if (portal_init_failed_ || !BaseScreenCaptureSource::GetInstance().ScreenCaptureAdapterIsExist(nweb_id_) ||
    BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id_] == nullptr) {
    LOG(ERROR) << "init failed";
    return;
  }

  std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> buffer =
      BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id_]->AcquireVideoBuffer();
  if (!buffer) {
    LOG(ERROR) << "acquire video buffer failed";
    return;
  }

  int32_t format = buffer->GetFormat();
  if (format != OHOS::NWeb::PixelFormatAdapter::PIXEL_FMT_RGBA_8888) {
    LOG(ERROR) << "buffer format error";
    BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id_]->ReleaseVideoBuffer();
    return;
  }
  int32_t width = buffer->GetWidth();
  int32_t height = buffer->GetHeight();
  int32_t stride = buffer->GetStride();
  uint32_t buffSize = buffer->GetSize();
  LOG(DEBUG) << "screen capture buffer, width: " << width
             << ", height: " << height << "; buffSize: " << buffSize
             << ", stride:" << stride;
  if (buffSize < static_cast<uint32_t>(height * stride)) {
    LOG(ERROR) << "screen capture buff size error";
    BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id_]->ReleaseVideoBuffer();
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
    BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id_]->ReleaseVideoBuffer();
    return;
  }
  for (int32_t i = 0; i < height; i++) {
    if (memcpy_s(pData, frameStride, pSrcData, frameStride) != EOK) {
      LOG(ERROR) << "data memcpy_s failed";
      BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id_]->ReleaseVideoBuffer();
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
  BaseScreenCaptureSource::GetInstance().screen_capture_adapter_map_[nweb_id_]->ReleaseVideoBuffer();
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
  if (BaseScreenCaptureSource::GetInstance().StartCapture(nweb_id_) != 0) {
    LOG(ERROR) << "start capture failed";
    return;
  }
  isStart_ = true;
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
  BaseScreenCaptureSource::GetInstance().SetScreenCaptureState(
      OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_STARTED,
      nweb_id_);
#endif
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
    capture_state_code = OHOS::NWeb::ScreenCaptureStateCodeAdapter::SCREEN_CAPTURE_STATE_INVLID;
    auto it = BaseScreenCaptureSource::GetInstance().capture_state_code_map_.find(nweb_id_);
    if (it != BaseScreenCaptureSource::GetInstance().capture_state_code_map_.end()) {
      capture_state_code = it->second;
    }
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
    const BaseWindowCapturer::CaptureSourceType& type,
    base::OnceCallback<void(uint64_t displayId)> callback) {
  return std::make_unique<BaseWindowCapturer>(type, options.get_picker_show(),
                                              options.get_nweb_id(),
                                              std::move(callback));
}

}  // namespace webrtc

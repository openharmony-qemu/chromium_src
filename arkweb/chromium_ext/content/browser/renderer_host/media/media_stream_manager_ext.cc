// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/content/browser/renderer_host/media/media_stream_manager_ext.h"
#include "content/browser/web_contents/web_contents_impl.h"

#include "content/browser/renderer_host/media/audio_input_device_manager.h"
namespace content {

MediaStreamManagerExt::MediaStreamManagerExt(media::AudioSystem* audio_system)
  : MediaStreamManager(audio_system) {
}

MediaStreamManagerExt::MediaStreamManagerExt(
    media::AudioSystem* audio_system,
    std::unique_ptr<VideoCaptureProvider> video_capture_provider)
  : MediaStreamManager(audio_system, std::move(video_capture_provider)) {
}

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
// static
MediaStreamManagerExt::ScreenCaptureCallback
    MediaStreamManagerExt::screen_capture_callback_;
// static
void MediaStreamManagerExt::SetScreenCaptureDelegateCallback(
    ScreenCaptureCallback callback) {
  if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    auto io_task_runner = GetIOThreadTaskRunner({});
    if (!io_task_runner) {
      LOG(ERROR) << "SetScreenCaptureDelegateCallback io_task_runner is nullptr";
      return;
    }
    io_task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&MediaStreamManagerExt::SetScreenCaptureDelegateCallback,
                       std::move(callback)));
    return;
  }

  screen_capture_callback_ = std::move(callback);
}

void MediaStreamManagerExt::StopScreenCapture(int32_t nweb_id,
                                              const std::string& session_id) {
  if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    auto io_task_runner = GetIOThreadTaskRunner({});
    if (!io_task_runner) {
      LOG(ERROR) << "StopScreenCapture io_task_runner is nullptr";
      return;
    }
    io_task_runner->PostTask(
      FROM_HERE,
      base::BindOnce(&MediaStreamManagerExt::StopScreenCapture,
                     weak_factory_.GetWeakPtr(), nweb_id, session_id));
      return;
  }
  LOG(INFO) << "MediaStreamManagerExt::StopScreenCapture, nweb_id=" << nweb_id;
  if (!video_capture_manager_) {
    LOG(ERROR) << "videoCaptureManager null";
    return;
  }

  std::lock_guard<std::mutex> lock(nweb_id_mutex_);
  auto nweb_id_it = nweb_id_maps_.find(session_id);
  if (nweb_id_it == nweb_id_maps_.end()) {
    return;
  } else {
    if (nweb_id_it->second != nweb_id) {
      return;
    }
  }
  video_capture_manager_->AsVideoCaptureManagerExt()->StopScreenCapture(session_id);
}

// LCOV_EXCL_START
void MediaStreamManagerExt::SetScreenCapturePickerShow() {
  if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    auto io_task_runner = GetIOThreadTaskRunner({});
    if (!io_task_runner) {
      LOG(ERROR) << "SetScreenCapturePickerShow io_task_runner is nullptr";
      return;
    }
    io_task_runner->PostTask(
      FROM_HERE,
      base::BindOnce(&MediaStreamManagerExt::SetScreenCapturePickerShow,
                     weak_factory_.GetWeakPtr()));
      return;
  }
  LOG(INFO) << "MediaStreamManagerExt::SetScreenCapturePickerShow";
  if (!video_capture_manager_) {
    LOG(ERROR) << "videoCaptureManager null";
    return;
  }

  video_capture_manager_->AsVideoCaptureManagerExt()->SetScreenCapturePickerShow();
}

void MediaStreamManagerExt::DisableSessionReuse() {
  if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    auto io_task_runner = GetIOThreadTaskRunner({});
    if (!io_task_runner) {
      LOG(ERROR) << "DisableSessionReuse io_task_runner is nullptr";
      return;
    }
    io_task_runner->PostTask(
      FROM_HERE,
      base::BindOnce(&MediaStreamManagerExt::DisableSessionReuse,
                     weak_factory_.GetWeakPtr()));
      return;
  }
  LOG(INFO) << "MediaStreamManagerExt::DisableSessionReuse";
  if (!video_capture_manager_) {
    LOG(ERROR) << "videoCaptureManager null";
    return;
  }

  video_capture_manager_->AsVideoCaptureManagerExt()->DisableSessionReuse();
}
// LCOV_EXCL_STOP

void MediaStreamManagerExt::SendScreenCaptureState(const std::string& session_id,
                                                   int32_t state) {
  std::lock_guard<std::mutex> lock(nweb_id_mutex_);
  auto nweb_id_it = nweb_id_maps_.find(session_id);
  if (nweb_id_it == nweb_id_maps_.end()) {
    SessionIdState session_id_state;
    session_id_state.session_id = session_id;
    session_id_state.state = static_cast<ScreenCaptureState>(state);
    session_id_state_.push_back(session_id_state);
    return;
  }
  MediaStreamManagerExt::SendScreenCaptureStateToNative(nweb_id_it->second,
                                                        session_id, state);
}

// static
void MediaStreamManagerExt::SendScreenCaptureStateToNative(
    int32_t nweb_id,
    const std::string& session_id,
    int32_t state) {
  if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
    auto ui_task_runner = GetUIThreadTaskRunner({});
    if (!ui_task_runner) {
      LOG(ERROR) << "SendScreenCaptureStateToNative ui_task_runner is nullptr";
      return;
    }
    ui_task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&MediaStreamManagerExt::SendScreenCaptureStateToNative,
                       nweb_id, session_id, state));
    return;
  }

  if (!screen_capture_callback_.is_null()) {
    screen_capture_callback_.Run(nweb_id, session_id.c_str(), state);
  }
}

void MediaStreamManagerExt::OnScreenCaptureOpened(const std::string& session_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  SendScreenCaptureState(session_id, SCREEN_CAPTURE_OPENED);
}

void MediaStreamManagerExt::RemoveNWebIdBySession(const base::UnguessableToken& capture_session_id) {
  std::lock_guard<std::mutex> lock(nweb_id_mutex_);
  auto nweb_id_it = nweb_id_maps_.find(capture_session_id.ToString());
  if (nweb_id_it == nweb_id_maps_.end()) {
    return;
  }
  nweb_id_maps_.erase(nweb_id_it);
}

void MediaStreamManagerExt::AddNWebIdBySession(int32_t nweb_id, const base::UnguessableToken& session_id) {
  std::lock_guard<std::mutex> lock(nweb_id_mutex_);
  std::string session_id_str = session_id.ToString();
  auto nweb_id_it = nweb_id_maps_.find(session_id_str);
  if (nweb_id_it == nweb_id_maps_.end()) {
    for (auto state_it = session_id_state_.begin();
        state_it != session_id_state_.end();) {
      if (state_it->session_id == session_id_str) {
        MediaStreamManagerExt::SendScreenCaptureStateToNative(
            nweb_id, state_it->session_id, state_it->state);
        state_it = session_id_state_.erase(state_it);
      } else {
        state_it++;
      }
    }
  }
  nweb_id_maps_[session_id_str] = nweb_id;
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

#if BUILDFLAG(ARKWEB_WEBRTC)
int MediaStreamManagerExt::GetNWebIdMatchStreamType(GlobalRenderFrameHostId host_id) {
  if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
    int result = 0;
    base::WaitableEvent event;
    auto ui_task_runner = GetUIThreadTaskRunner({});
    if (!ui_task_runner) {
      LOG(ERROR) << "GetNWebIdMatchStreamType ui task runner is nullptr";
      return 0;
    }
    ui_task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(
            [](base::SafeRef<MediaStreamManagerExt> manager, GlobalRenderFrameHostId host_id,
               base::WaitableEvent* out_event, int* out_result) {
              *out_result = manager->GetNWebIdMatchStreamType(host_id);
              out_event->Signal();
            },
            weak_factory_.GetSafeRef(), host_id, &event, &result));
    event.Wait();
    return result;
  }
  auto* web_contents = static_cast<WebContentsImpl*>(WebContentsImpl::FromRenderFrameHostID(host_id));
  return web_contents ? web_contents->GetNWebId() : 0;
}

void MediaStreamManagerExt::CloseAudioCapture(int32_t nweb_id) {
  if (!audio_input_device_manager()) {
    LOG(ERROR) << "audio_input_device_manager() is nullptr.";
    return;
  }
  auto io_task_runner = GetIOThreadTaskRunner({});
  auto& audio_nweb_id_map = audio_input_device_manager()->GetNWebIdMap();
  for (auto& audio : audio_nweb_id_map) {
    if (audio.second == nweb_id) {
      blink::mojom::MediaStreamType type =
          audio_input_device_manager()->GetDeviceType(audio.first);
      if (!BrowserThread::CurrentlyOn(BrowserThread::IO) && io_task_runner) {
        io_task_runner->PostTask(
            FROM_HERE,
            base::BindOnce(&MediaStreamManager::StopDevice,
                           weak_factory_.GetSafeRef(), type, audio.first));
        continue;
      }
      StopDevice(type, audio.first);
    }
  }
}
#endif
}
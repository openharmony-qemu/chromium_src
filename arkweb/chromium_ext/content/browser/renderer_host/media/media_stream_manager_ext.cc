// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/chromium_ext/content/browser/renderer_host/media/media_stream_manager_ext.h"
#include "content/browser/web_contents/web_contents_impl.h"

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
    GetIOThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&MediaStreamManagerExt::SetScreenCaptureDelegateCallback,
                       std::move(callback)));
    return;
  }

  screen_capture_callback_ = std::move(callback);
}

void MediaStreamManagerExt::StopScreenCapture(int32_t nweb_id,
                                              const std::string& session_id) {
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

void MediaStreamManagerExt::SetScreenCapturePickerShow() {
  if (!video_capture_manager_) {
    LOG(ERROR) << "videoCaptureManager null";
    return;
  }

  video_capture_manager_->AsVideoCaptureManagerExt()->SetScreenCapturePickerShow();
}

void MediaStreamManagerExt::DisableSessionReuse() {
  if (!video_capture_manager_) {
    LOG(ERROR) << "videoCaptureManager null";
    return;
  }

  video_capture_manager_->AsVideoCaptureManagerExt()->DisableSessionReuse();
}

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
    GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE,
        base::BindOnce(&MediaStreamManagerExt::SendScreenCaptureStateToNative,
                       nweb_id, session_id, state));
    return;
  }

  if (!screen_capture_callback_.is_null()) {
    screen_capture_callback_.Run(nweb_id, session_id.c_str(), state);
  }
}

void MediaStreamManagerExt::PopSessionIdState(int32_t nweb_id,
                                              const std::string& session_id) {
  for (auto state_it = session_id_state_.begin();
       state_it != session_id_state_.end();) {
    if (state_it->session_id == session_id) {
      MediaStreamManagerExt::SendScreenCaptureStateToNative(
          nweb_id, state_it->session_id, state_it->state);
      state_it = session_id_state_.erase(state_it);
    } else {
      state_it++;
    }
  }
}

void MediaStreamManagerExt::OnScreenCaptureOpened(const std::string& session_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  SendScreenCaptureState(session_id, SCREEN_CAPTURE_OPENED);
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)

}
// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/content/browser/renderer_host/media/video_capture_manager_ext.h"
#include "content/browser/renderer_host/media/video_capture_manager.h"
#include "content/browser/renderer_host/media/video_capture_controller.h"
#include "content/public/browser/browser_thread.h"

namespace content {
VideoCaptureManagerExt::VideoCaptureManagerExt(
    std::unique_ptr<VideoCaptureProvider> video_capture_provider,
    base::RepeatingCallback<void(const std::string&)> emit_log_message_cb)
  : VideoCaptureManager(std::move(video_capture_provider), std::move(emit_log_message_cb)) {
}

#if BUILDFLAG(ARKWEB_WEBRTC)
void VideoCaptureManagerExt::StartCamera(int nWebId) const {
  if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    auto io_task_runner = GetIOThreadTaskRunner({});
    if (!io_task_runner) {
      LOG(ERROR) << "StartCamera io_task_runner is nullptr";
      return;
    }
    io_task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&VideoCaptureManagerExt::StartCamera,
                       weak_factory_.GetWeakPtr(), nWebId));
      return;
  }
  LOG(INFO) << "VideoCaptureManagerExt::StartCamera, nWebId = " << nWebId;
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  std::lock_guard<std::mutex> lock(NWebIdMutex_);
  for (const auto& it : nWebId_) {
    if (it.second == nWebId) {
      auto videoCaptureController = LookupControllerBySessionId(it.first);
      if (videoCaptureController == nullptr ||
          videoCaptureController->stream_type() !=
              blink::mojom::MediaStreamType::DEVICE_VIDEO_CAPTURE) {
        continue;
      }
      videoCaptureController->ResumeClientBySessionId(it.first);
    }
  }
}

void VideoCaptureManagerExt::StopCamera(int nWebId) const {
  if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    auto io_task_runner = GetIOThreadTaskRunner({});
    if (!io_task_runner) {
      LOG(ERROR) << "StopCamera io_task_runner is nullptr";
      return;
    }
    io_task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&VideoCaptureManagerExt::StopCamera,
                       weak_factory_.GetWeakPtr(), nWebId));
      return;
  }
  LOG(INFO) << "VideoCaptureManagerExt::StopCamera, nWebId = " << nWebId;
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  std::lock_guard<std::mutex> lock(NWebIdMutex_);
  for (const auto& it : nWebId_) {
    if (it.second == nWebId) {
      auto videoCaptureController = LookupControllerBySessionId(it.first);
      if (videoCaptureController == nullptr ||
          videoCaptureController->stream_type() !=
              blink::mojom::MediaStreamType::DEVICE_VIDEO_CAPTURE) {
        continue;
      }
      videoCaptureController->PauseClientBySessionId(it.first);
    }
  }
}

void VideoCaptureManagerExt::CloseCamera(int nWebId) const {
  if (!BrowserThread::CurrentlyOn(BrowserThread::IO)) {
    auto io_task_runner = GetIOThreadTaskRunner({});
    if (!io_task_runner) {
      LOG(ERROR) << "CloseCamera io_task_runner is nullptr";
      return;
    }
    io_task_runner->PostTask(
        FROM_HERE,
        base::BindOnce(&VideoCaptureManagerExt::CloseCamera,
                       weak_factory_.GetWeakPtr(), nWebId));
      return;
  }
  LOG(INFO) << "VideoCaptureManagerExt::CloseCamera, nWebId = " << nWebId;
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  std::lock_guard<std::mutex> lock(NWebIdMutex_);
  for (const auto& it : nWebId_) {
    if (it.second == nWebId) {
      auto videoCaptureController = LookupControllerBySessionId(it.first);
      if (videoCaptureController == nullptr ||
          videoCaptureController->stream_type() !=
              blink::mojom::MediaStreamType::DEVICE_VIDEO_CAPTURE) {
        continue;
      }
      videoCaptureController->StopSession(it.first);
    }
  }
}

void VideoCaptureManagerExt::BindSessionIdToNWebId(
    media::VideoCaptureSessionId sessionId,
    int nWebId) {
  std::lock_guard<std::mutex> lock(NWebIdMutex_);
  nWebId_[sessionId] = nWebId;
}
#endif  // BUILDFLAG(ARKWEB_WEBRTC)

#if BUILDFLAG(ARKWEB_EX_SCREEN_CAPTURE)
void VideoCaptureManagerExt::StopScreenCapture(const std::string& session_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  std::optional<base::Token> token = base::Token::FromString(session_id);
  if (!token.has_value()) {
    return;
  }

  std::optional<base::UnguessableToken> unguessable_token =
      base::UnguessableToken::Deserialize(token->high(), token->low());
  auto session_it = sessions_.find(unguessable_token.value());
  if (session_it == sessions_.end()) {
    return;
  }

  auto videoCaptureController =
      LookupControllerBySessionId(unguessable_token.value());
  if (videoCaptureController != nullptr) {
    videoCaptureController->StopSession(unguessable_token.value());
  }
}

void VideoCaptureManagerExt::SetScreenCapturePickerShow() {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  is_picker_show_ = true;
}

void VideoCaptureManagerExt::DisableSessionReuse() {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  is_session_reuse_ = false;
}

void VideoCaptureManagerExt::ScreenCaptureOpened(const std::string& session_id) {
  // Notify listener asynchronously.
  base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
      FROM_HERE, base::BindOnce(&VideoCaptureManagerExt::OnScreenCaptureOpened,
                                weak_factory_.GetWeakPtr(), session_id));
}

void VideoCaptureManagerExt::OnScreenCaptureOpened(const std::string& session_id) {
  DCHECK_CURRENTLY_ON(BrowserThread::IO);
  for (auto& listener : listeners_) {
    listener.OnScreenCaptureOpened(session_id);
  }
}
#endif  // defined(ARKWEB_EX_SCREEN_CAPTURE)
}

// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/audio/ohos/ohos_audio_focus_controller.h"
#include "base/logging.h"
#include "base/synchronization/waitable_event.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"

namespace media {
void OHOSAudioFocusController::OnResume(const AudioParameters& parameters) {
  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(parameters.render_process_id(),
                                       parameters.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "OnResume RenderFrameHost not found for PID: "
               << parameters.render_process_id() 
               << ", FrameID: " << parameters.render_frame_id();
  }
  auto webContent = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContent) {
    LOG(ERROR) << "OHOSAudioFocusController OnResume get webContent failed.";
    return;
  }
  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContent);
  if (!mediaSession) {
    LOG(ERROR) << "OHOSAudioFocusController OnResume get mediaSession failed.";
    return;
  }
  mediaSession->Resume(content::MediaSession::SuspendType::kSystem);
}

void OHOSAudioFocusController::OnSuspend(const AudioParameters& parameters) {
  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(parameters.render_process_id(),
                                       parameters.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "OnSuspend RenderFrameHost not found for PID: "
               << parameters.render_process_id() 
               << ", FrameID: " << parameters.render_frame_id();
  }
  auto webContent = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContent) {
    LOG(ERROR) << "OHOSAudioFocusController OnSuspend get webContent failed.";
    return;
  }
  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContent);
  if (!mediaSession) {
    LOG(ERROR) << "OHOSAudioFocusController OnSuspend get mediaSession failed.";
    return;
  }
  mediaSession->Suspend(content::MediaSession::SuspendType::kSystem);
}

MediaContentType OHOSAudioFocusController::GetMediaContentType(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return GetContentTypeOnUIThread(parameters);
  }

  MediaContentType result;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const AudioParameters& params,
                        MediaContentType* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = GetContentTypeOnUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;
}

MediaContentType OHOSAudioFocusController::GetContentTypeOnUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "GetContentTypeOnUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(ERROR) << "GetContentTypeOnUIThread WebContents not associated with RenderFrameHost";
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(ERROR) << "GetContentTypeOnUIThread MediaSession not initialized for WebContents";
  }

  return mediaSession->getMediaContentType();
}

bool OHOSAudioFocusController::IsActive(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckActiveOnUIThread(parameters);
  }

  bool result = false;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        bool* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckActiveOnUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;
}

bool OHOSAudioFocusController::CheckActiveOnUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckActiveOnUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return false;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(ERROR) << "CheckActiveOnUIThread WebContents not found for RenderFrameHost";
    return false;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(ERROR) << "CheckActiveOnUIThread MediaSession not available for WebContents";
    return false;
  }

  return mediaSession->IsActive();
}

bool OHOSAudioFocusController::HasOnlyOneShotPlayersPublic(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckOneShotPlayersOnUIThread(parameters);
  }

  bool result = false;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        bool* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckOneShotPlayersOnUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;    
}

bool OHOSAudioFocusController::CheckOneShotPlayersOnUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckOneShotPlayersOnUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return false;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(ERROR) << "CheckOneShotPlayersOnUIThread WebContents not found for RenderFrameHost";
    return false;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(ERROR) << "CheckOneShotPlayersOnUIThread MediaSession not available for WebContents";
    return false;
  }

  return mediaSession->HasOnlyOneShotPlayersPublic();
}

content::MediaSessionImpl::NWebMediaSessionState
    OHOSAudioFocusController::GetSessionState(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckGetSessionStateOnUIThread(parameters);
  }

  content::MediaSessionImpl::NWebMediaSessionState
      result = content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        content::MediaSessionImpl::NWebMediaSessionState* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckGetSessionStateOnUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;    
}

content::MediaSessionImpl::NWebMediaSessionState
    OHOSAudioFocusController::CheckGetSessionStateOnUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckGetSessionStateOnUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(INFO) << "CheckGetSessionStateOnUIThread WebContents not found for RenderFrameHost";
    return content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(INFO) << "CheckGetSessionStateOnUIThread MediaSession not available for WebContents";
    return content::MediaSessionImpl::NWebMediaSessionState::NOINITIAL;
  }

  return mediaSession->GetSessionState();
}

bool OHOSAudioFocusController::GetPlayingState(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckGetPlayingStateOnUIThread(parameters);
  }

  bool result = false;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        bool* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckGetPlayingStateOnUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;    
}

bool OHOSAudioFocusController::CheckGetPlayingStateOnUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckGetPlayingStateOnUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return false;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(ERROR) << "CheckGetPlayingStateOnUIThread WebContents not found for RenderFrameHost";
    return false;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(ERROR) << "CheckGetPlayingStateOnUIThread MediaSession not available for WebContents";
    return false;
  }

  return mediaSession->GetPlayingState();
}

bool OHOSAudioFocusController::GetMuteState(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckGetMuteStateOnUIThread(parameters);
  }

  bool result = false;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        bool* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckGetMuteStateOnUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;    
}

bool OHOSAudioFocusController::CheckGetMuteStateOnUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckGetMuteStateOnUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return false;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(INFO) << "WebContents not found for RenderFrameHost";
    return false;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(INFO) << "MediaSession not available for WebContents";
    return false;
  }

  return mediaSession->GetMuteState();
}

bool OHOSAudioFocusController::GetMediaSession(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckGetMuteStateOnUIThread(parameters);
  }

  bool result = false;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        bool* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckGetMuteStateOnUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;    
}

bool OHOSAudioFocusController::CheckGetMediaSessionUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckGetMediaSessionUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return false;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(ERROR) << "CheckGetMediaSessionUIThread WebContents not found for RenderFrameHost";
    return false;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(ERROR) << "CheckGetMediaSessionUIThread MediaSession not available for WebContents";
    return false;
  }

  return true;
}

void OHOSAudioFocusController::OneShotMediaPlayerStopped(const AudioParameters& parameters) {
  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(parameters.render_process_id(),
                                       parameters.render_frame_id());
  auto webContent = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContent) {
    LOG(ERROR) << "OneShotMediaPlayerStopped OnSuspend get webContent failed.";
    return;
  }
  webContent->OneShotMediaPlayerStopped();
}

bool OHOSAudioFocusController::GetAudioExclusive(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckGetAudioExclusiveUIThread(parameters);
  }

  bool result = false;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        bool* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckGetAudioExclusiveUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;    
}

bool OHOSAudioFocusController::CheckGetAudioExclusiveUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckGetAudioExclusiveUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return false;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(ERROR) << "CheckGetAudioExclusiveUIThread WebContents not found for RenderFrameHost";
    return false;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(ERROR) << "CheckGetAudioExclusiveUIThread MediaSession not available for WebContents";
    return false;
  }

  return mediaSession->audioExclusive_;
}

int OHOSAudioFocusController::GetAudioResumeInterval(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckGetAudioResumeIntervalUIThread(parameters);
  }

  int result = 0;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        int* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckGetAudioResumeIntervalUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;    
}

int OHOSAudioFocusController::CheckGetAudioResumeIntervalUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckGetAudioResumeIntervalUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return 0;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(ERROR) << "CheckGetAudioResumeIntervalUIThread WebContents not found for RenderFrameHost";
    return 0;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(ERROR) << "CheckGetAudioResumeIntervalUIThread MediaSession not available for WebContents";
    return 0;
  }

  return mediaSession->audioResumeInterval_;
}

bool OHOSAudioFocusController::IsSuspended(const AudioParameters& parameters) {
  if (content::BrowserThread::CurrentlyOn(content::BrowserThread::UI)) {
    return CheckIsSuspendedUIThread(parameters);
  }

  bool result = false;
  base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                            base::WaitableEvent::InitialState::NOT_SIGNALED);

  content::GetUIThreadTaskRunner({})->PostTask(
      FROM_HERE,
      base::BindOnce([](const media::AudioParameters& params,
                        bool* out_result,
                        base::WaitableEvent* out_event) {
        *out_result = CheckIsSuspendedUIThread(params);
        out_event->Signal();
      }, parameters, &result, &event));

  event.Wait();
  return result;    
}

bool OHOSAudioFocusController::CheckIsSuspendedUIThread(const AudioParameters& params) {
  DCHECK_CURRENTLY_ON(content::BrowserThread::UI);

  content::RenderFrameHost* renderFrameHost =
      content::RenderFrameHost::FromID(params.render_process_id(),
                                       params.render_frame_id());
  if (!renderFrameHost) {
    LOG(ERROR) << "CheckIsSuspendedUIThread RenderFrameHost not found for PID: "
               << params.render_process_id() 
               << ", FrameID: " << params.render_frame_id();
    return false;
  }

  content::WebContents* webContents = content::WebContents::FromRenderFrameHost(renderFrameHost);
  if (!webContents) {
    LOG(ERROR) << "CheckIsSuspendedUIThread WebContents not found for RenderFrameHost";
    return false;
  }

  content::MediaSessionImpl* mediaSession = content::MediaSessionImpl::Get(webContents);
  if (!mediaSession) {
    LOG(ERROR) << "CheckIsSuspendedUIThread MediaSession not available for WebContents";
    return false;
  }

  return mediaSession->IsSuspended();
}

} // media
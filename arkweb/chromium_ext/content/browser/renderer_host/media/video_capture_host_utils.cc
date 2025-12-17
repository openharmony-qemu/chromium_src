/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "arkweb/chromium_ext/content/browser/renderer_host/media/video_capture_host_utils.h"
#include "content/public/browser/render_process_host.h"
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
#include "content/public/browser/browser_thread.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif
#if BUILDFLAG(ARKWEB_WEBRTC)
#include "base/synchronization/waitable_event.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/render_frame_host.h"
#endif

namespace content {

VideoCaptureHostUtils::VideoCaptureHostUtils(VideoCaptureHost* impl) {
    this->videoCaptureHost = impl;
}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
void VideoCaptureHostUtils::SetRenderFrameHostId(GlobalRenderFrameHostId render_frame_host_id) {
    render_frame_host_id_ = render_frame_host_id;
}

// LCOV_EXCL_START
void VideoCaptureHostUtils::ReportStartScreenCaptureBind(int child_id) {
    if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
        GetUIThreadTaskRunner({})->PostTask(FROM_HERE,
        base::BindOnce(&VideoCaptureHostUtils::ReportStartScreenCaptureBind, child_id));
        return;
    }
    RenderProcessHost* host =
        RenderProcessHost::FromID(child_id);
    if (host) {
        LOG(INFO) << __func__
                << " start screen capture, pid: " << host->GetProcess().Pid();
        OHOS::NWeb::ResSchedClientAdapter::ReportScreenCapture(
            OHOS::NWeb::ResSchedStatusAdapter::SCREEN_CAPTURE_START,
            host->GetProcess().Pid());
    }
}

void VideoCaptureHostUtils::ReportStopScreenCaptureBind(int child_id) {
    if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
        GetUIThreadTaskRunner({})->PostTask(FROM_HERE,
        base::BindOnce(&VideoCaptureHostUtils::ReportStopScreenCaptureBind, child_id));
        return;
    }
    RenderProcessHost* host =
        RenderProcessHost::FromID(child_id);
    if (host) {
        LOG(INFO) << __func__
                << " stop screen capture, pid: " << host->GetProcess().Pid();
        OHOS::NWeb::ResSchedClientAdapter::ReportScreenCapture(
            OHOS::NWeb::ResSchedStatusAdapter::SCREEN_CAPTURE_STOP,
            host->GetProcess().Pid());
    }
}

void VideoCaptureHostUtils::ReportStartScreenCapture() {
    ReportStartScreenCaptureBind(render_frame_host_id_.child_id);
}

void VideoCaptureHostUtils::ReportStopScreenCapture() {
    ReportStopScreenCaptureBind(render_frame_host_id_.child_id);
}
// LCOV_EXCL_STOP
#endif

#if BUILDFLAG(ARKWEB_WEBRTC)
void VideoCaptureHostUtils::OnCameraCaptureStateChangedBind(CameraCaptureState camera_state,
                                                            CameraCaptureState new_state,
                                                            GlobalRenderFrameHostId render_frame_host_id) {
    if (camera_state == new_state) {
        return;
    }

    if (!BrowserThread::CurrentlyOn(BrowserThread::UI)) {
        base::WaitableEvent event(base::WaitableEvent::ResetPolicy::AUTOMATIC,
                                  base::WaitableEvent::InitialState::NOT_SIGNALED);
        auto ui_task_runner = GetUIThreadTaskRunner({});
        if (!ui_task_runner) {
            LOG(ERROR) << "VideoCaptureHostUtils::OnCameraCaptureStateChangedBind "
                          "ui_task_runner is nullptr";
            return;
        }
        ui_task_runner->PostTask(
            FROM_HERE,
            base::BindOnce([](CameraCaptureState camera_state,
                              CameraCaptureState new_state,
                              GlobalRenderFrameHostId render_frame_host_id,
                              base::WaitableEvent* out_event) {
                VideoCaptureHostUtils::OnCameraCaptureStateChangedBind(camera_state, new_state, render_frame_host_id);
                out_event->Signal();
            }, camera_state, new_state, render_frame_host_id, &event));
        event.Wait();
        return;
    }

    RenderFrameHost* host = RenderFrameHost::FromID(render_frame_host_id);
    if (!host) {
        LOG(ERROR) << "host is null.";
        return;
    }
    content::WebContents* webContent =
        content::WebContents::FromRenderFrameHost(host);
    if (webContent) {
        webContent->OnCameraCaptureStateChanged(static_cast<int>(camera_state),
                                                static_cast<int>(new_state));
        LOG(INFO) << "VideoCaptureHostUtils::OnCameraCaptureStateChangedBind in "
                     "BrowserThread::UI";
    }
}

void VideoCaptureHostUtils::OnCameraCaptureStateChanged(CameraCaptureState new_state) {
    VideoCaptureHostUtils::OnCameraCaptureStateChangedBind(camera_state_, new_state, render_frame_host_id_);
    camera_state_ = new_state;
}

void VideoCaptureHostUtils::OnCameraCaptureStarted(
    const VideoCaptureControllerID& controller_id) {
    if(!videoCaptureHost){
        LOG(ERROR) << "videoCaptureHost is null.";
        return;
    }
    if (videoCaptureHost->controllers_.find(controller_id) ==
        videoCaptureHost->controllers_.end()) {
        LOG(INFO) <<" videoCaptureHost->controllers_ not found. controller_id:" << controller_id.ToString();   
        return;
    }
    auto it = videoCaptureHost->controllers_.find(controller_id);
    if (it->second->stream_type() ==
        blink::mojom::MediaStreamType::DEVICE_VIDEO_CAPTURE) {
        OnCameraCaptureStateChanged(CameraCaptureState::ACTIVE);
    }
}
#endif
}
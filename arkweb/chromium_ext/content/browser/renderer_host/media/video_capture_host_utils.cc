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
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

namespace content {

VideoCaptureHostUtils::VideoCaptureHostUtils(VideoCaptureHost* impl) {
    this->videoCaptureHost = impl;
}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
void VideoCaptureHostUtils::SetRenderFrameHostId(GlobalRenderFrameHostId render_frame_host_id) {
    render_frame_host_id_ = render_frame_host_id;
}

void VideoCaptureHostUtils::ReportStartScreenCapture() {
    RenderProcessHost* host =
        RenderProcessHost::FromID(render_frame_host_id_.child_id);
    if (host) {
        LOG(INFO) << __func__
                << " start screen capture, pid: " << host->GetProcess().Pid();
        OHOS::NWeb::ResSchedClientAdapter::ReportScreenCapture(
            OHOS::NWeb::ResSchedStatusAdapter::SCREEN_CAPTURE_START,
            host->GetProcess().Pid());
    }
}

void VideoCaptureHostUtils::ReportStopScreenCapture() {
    RenderProcessHost* host =
        RenderProcessHost::FromID(render_frame_host_id_.child_id);
    if (host) {
        LOG(INFO) << __func__
                << " stop screen capture, pid: " << host->GetProcess().Pid();
        OHOS::NWeb::ResSchedClientAdapter::ReportScreenCapture(
            OHOS::NWeb::ResSchedStatusAdapter::SCREEN_CAPTURE_STOP,
            host->GetProcess().Pid());
    }
}
#endif
}
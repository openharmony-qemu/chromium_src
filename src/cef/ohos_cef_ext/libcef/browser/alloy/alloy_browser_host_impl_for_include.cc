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

#if BUILDFLAG(ARKWEB_OCCLUDED_OPT)
void AlloyBrowserHostImpl::SetEnableHalfFrameRate(bool enabled) {
  LOG(DEBUG) << "SetEnableHalfFrameRate:" << enabled;
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&AlloyBrowserHostImpl::SetEnableHalfFrameRate,
                                 this, enabled));
    return;
  }

  if (web_contents() == nullptr) {
    return;
  }

  auto rvh = web_contents()->GetRenderViewHost();
  if (rvh && rvh->GetWidget()) {
    ArkWebRenderWidgetHostViewOSRExt* view =
        static_cast<ArkWebRenderWidgetHostViewOSRExt*>(
            rvh->GetWidget()->GetView());
    if (view) {
      view->SetEnableHalfFrameRate(enabled);
    }
  }
}
#endif

#if BUILDFLAG(ARKWEB_PIP)
void AlloyBrowserHostImpl::OnPip(int status,
                                 int delegate_id,
                                 int child_id,
                                 int frame_routing_id,
                                 int width,
                                 int height) {
  platform_delegate_->OnPip(status, delegate_id, child_id,
                            frame_routing_id, width, height);
}

void AlloyBrowserHostImpl::OnPipEvent(int event) {
  if (platform_delegate_) {
    platform_delegate_->OnPipEvent(event);
  }
}

void AlloyBrowserHostImpl::SetPipNativeWindow(int delegate_id,
                                              int child_id,
                                              int frame_routing_id,
                                              cef_native_window_t window) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&AlloyBrowserHostImpl::SetPipNativeWindow,
                                 this, delegate_id, child_id,
                                 frame_routing_id, window));
    return;
  }
  if (!window) {
    LOG(INFO) << "Set Pip native window is null";
    return;
  }
  if (platform_delegate_) {
    platform_delegate_->SetPipNativeWindow(delegate_id, child_id,
                                           frame_routing_id, window);
  }

}

void AlloyBrowserHostImpl::SendPipEvent(int delegate_id,
                                        int child_id,
                                        int frame_routing_id,
                                        int event) {
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&AlloyBrowserHostImpl::SendPipEvent,
                                 this, delegate_id, child_id,
                                 frame_routing_id, event));
    return;
  }

  if (platform_delegate_) {
    platform_delegate_->SendPipEvent(delegate_id, child_id,
                                     frame_routing_id, event);
  }
}

void AlloyBrowserHostImpl::SetPipActive(bool active) { 
  LOG(DEBUG) << "SetPipActive:" << active;
  if (!CEF_CURRENTLY_ON_UIT()) {
    CEF_POST_TASK(CEF_UIT,
                  base::BindOnce(&AlloyBrowserHostImpl::SetPipActive,
                                 this, active));
    return;
  }

  if (web_contents() == nullptr) {
    return;
  }

  auto rvh = web_contents()->GetRenderViewHost();
  if (rvh && rvh->GetWidget()) {
    ArkWebRenderWidgetHostViewOSRExt* view =
        static_cast<ArkWebRenderWidgetHostViewOSRExt*>(
            rvh->GetWidget()->GetView());
    if (view) {
      view->SetPipActive(active);
    }
  }
}
#endif

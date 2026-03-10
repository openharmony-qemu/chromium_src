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
 
// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_AI)
void RenderFrameImpl::CloseImageOverlaySelection() {
  if (GetFrameHost()) {
    GetFrameHost()->CloseImageOverlaySelection();
  }
}
#endif  // BUILDFLAG(ARKWEB_AI)
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_SAME_LAYER)
blink::WebNativeBridge* RenderFrameImpl::CreateWebNativeBridge(
    blink::WebNativeClient* client) {
    return media_factory_.CreateWebNativeBridge(client, GetLocalRootWebFrameWidget()->GetFrameSinkId(),
        agent_scheduling_group_->agent_group_scheduler().CompositorTaskRunner());
}

float RenderFrameImpl::DeviceScaleFactor() {
  return media_factory_.GetDeviceScaleFactor();
}
#endif

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
bool RenderFrameImpl::IsOffscreen() {
  return is_offscreen_;
}
#endif

#if BUILDFLAG(ARKWEB_PASSWORD_AUTOFILL)
gfx::RectF RenderFrameImpl::ElementBoundsInWindow(
    const blink::WebElement& element) {
  return gfx::RectF(GetLocalRootWebFrameWidget()->BlinkSpaceToEnclosedDIPs(
      element.BoundsInWidget()));
}
#endif

#if BUILDFLAG(ARKWEB_MULTI_WINDOW)
bool RenderFrameImpl::GetNewWindowWebView(
    const GURL& target_url,
    blink::WebNavigationPolicy policy,
    bool allow_popup,
    const blink::WebWindowFeatures& features) {
  mojom::CreateNewWindowStatus status = mojom::CreateNewWindowStatus::kBlocked;
  auto* frame_host = GetFrameHost();
  if (!frame_host) {
    return false;
  }
  if (!frame_host->GetCreateNewWindow(
          target_url, NavigationPolicyToDisposition(policy), allow_popup,
          ConvertWebWindowFeaturesToMojoWindowFeatures(features), &status) ||
      status != mojom::CreateNewWindowStatus::kSuccess) {
    return false;
  }
  return true;
}
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_DRAG_DROP)
void RenderFrameImpl::ClearContextMenu() {
  // It does not postTask here because contextmenu popup windows should be
  // dismissed before drag start.
  GetFrameHost()->OnClearContextMenu();
}
#endif  // BUILDFLAG(ARKWEB_DRAG_DROP)

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void RenderFrameImpl::SetZoomLevel(float magnify_delta,
                                   const gfx::Point& anchor) {
  auto web_frame_widget = GetLocalRootWebFrameWidget();
  if (!web_frame_widget) {
    return;
  }
  web_frame_widget->SetZoomLevel(magnify_delta, anchor);
}

void RenderFrameImpl::SetOverscrollMode(int mode) {
  auto web_frame_widget = GetLocalRootWebFrameWidget();
  if (!web_frame_widget) {
    return;
  }
  web_frame_widget->SetOverscrollMode(mode);
}

bool RenderFrameImpl::IsElementExist(std::string xPath) {
  auto web_frame_widget = GetLocalRootWebFrameWidget();
  if (!web_frame_widget) {
    return false;
  }
  return web_frame_widget->IsElementExist(xPath);
}

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
gfx::Vector2dF RenderFrameImpl::GetOverScrollOffset() {
  gfx::Vector2dF overscroll_offset;
  overscroll_offset.set_x(0.0f);
  overscroll_offset.set_y(0.0f);
  auto web_frame_widget = GetLocalRootWebFrameWidget();
  if (!web_frame_widget) {
    return overscroll_offset;
  }
  return web_frame_widget->GetOverScrollOffset();
}
#endif
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
#if BUILDFLAG(ARKWEB_MENU)
void RenderFrameImpl::MouseSelectMenuShow(bool show) {
  if (GetFrameHost()) {
    GetFrameHost()->MouseSelectMenuShow(show);
  }
}

void RenderFrameImpl::ChangeVisibilityOfQuickMenu() {
  if (GetFrameHost()) {
    GetFrameHost()->ChangeVisibilityOfQuickMenu();
  }
}

void RenderFrameImpl::HideQuickMenu() {
  if (GetFrameHost()) {
    GetFrameHost()->HideQuickMenu();
  }
}
#endif

#if BUILDFLAG(ARKWEB_PDF)
void RenderFrameImpl::OnPdfScrollAtBottom(const std::string& url) {
  if (GetFrameHost()) {
    GetFrameHost()->OnPdfScrollAtBottom(url);
  }
}

void RenderFrameImpl::OnPdfLoadEvent(int32_t result, const std::string& url) {
  if (GetFrameHost()) {
    GetFrameHost()->OnPdfLoadEvent(result, url);
  }
}

void RenderFrameImpl::SetIsPDF(bool is_pdf) {
  is_pdf_ = is_pdf;
}

bool RenderFrameImpl::IsPDF() {
  return is_pdf_;
}

#endif  // BUILDFLAG(ARKWEB_PDF)

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
bool RenderFrameImpl::IsVideoLoadOptimizationEnabled(const std::string& url) {
  if (auto* thread = RenderThreadImpl::current()) {
    return thread->IsVideoLoadOptimizationEnabled(url);
  }
  return false;
}

int RenderFrameImpl::GetVideoPreloadTimeDefault() const {
  if (auto* thread = RenderThreadImpl::current()) {
    return thread->GetVideoPreloadTimeDefault();
  }
  return INT_MAX;
}

int RenderFrameImpl::GetVideoMinCacheTimeDefault() const {
  if (auto* thread = RenderThreadImpl::current()) {
    return thread->GetVideoMinCacheTimeDefault();
  }
  return INT_MAX;
}

int RenderFrameImpl::GetVideoMaxCacheTimeDefault() const {
  if (auto* thread = RenderThreadImpl::current()) {
    return thread->GetVideoMaxCacheTimeDefault();
  }
  return INT_MAX;
}

int RenderFrameImpl::GetVideoMoovSizeDefault() const {
  if (auto* thread = RenderThreadImpl::current()) {
    return thread->GetVideoMoovSizeDefault();
  }
  return INT_MAX;
}

int RenderFrameImpl::GetVideoBitrateDefault() const {
  if (auto* thread = RenderThreadImpl::current()) {
    return thread->GetVideoBitrateDefault();
  }
  return INT_MAX;
}

bool RenderFrameImpl::SetNewsFeedPageFitted() {
  return media_factory_.SetNewsFeedPageFitted();
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_JS_ON_DOCUMENT_END)
void RenderFrameImpl::OnDocumentEndReady() {
  if (GetFrameHost()) {
    GetFrameHost()->OnDocumentEndReady();
  }
}
#endif

// LCOV_EXCL_STOP

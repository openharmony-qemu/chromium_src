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

#ifndef CC_TREES_LAYER_TREE_HOST_IMPL_H_
#error "must be in include form CC_TREES_LAYER_TREE_HOST_IMPL_H_"
#endif

namespace cc {

namespace {

#if BUILDFLAG(ARKWEB_FLING) && BUILDFLAG(ARKWEB_SLIDE)
static bool g_frameIsScrolling = false;
#endif
}  // namespace

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
void LayerTreeHostImpl::SetDrawRectState(bool isNeedDrawRect) {
  isNeedDrawRect_ = isNeedDrawRect;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void LayerTreeHostImpl::HandleScrollUpdateForInternalBeginFrame(const viz::BeginFrameArgs& args) {
  if (input_delegate_) {
    input_delegate_->HandleScrollUpdateForInternalBeginFrame(args);
  }
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void LayerTreeHostImpl::OnLayerRectUpdate(int id, const gfx::Rect& rect) {
  client_->OnLayerRectUpdate(id, rect);
}

void LayerTreeHostImpl::OnLayerRectVisibilityChange(int id, bool visibility) {
  client_->OnLayerRectVisibilityChange(id, visibility);
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void LayerTreeHostImpl::OnLayerBoundsUpdate(int id, const gfx::Rect& bounds) {
  client_->OnLayerBoundsUpdate(id, bounds);
}
#endif // ARKWEB_VIDEO_ASSISTANT

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
void LayerTreeHostImpl::SetupScrollBy() {
  if (!input_delegate_) {
    return;
  }

  gfx::Vector2dF scroll_delta(
      0.f, browser_controls_offset_manager_->ContentTopOffset());
#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
  LOG_FEEDBACK(INFO) << "AnimateBrowserControlsMock  delta.y"
                     << scroll_delta.y();
#endif
  if (scroll_delta.IsZero()) {
    return;
  }

  // This counter-scrolls the page to keep the appearance of the page content
  // being fixed while the browser controls animate.
  viewport().ScrollBy(scroll_delta,
                      /*viewport_point=*/gfx::Point(),
                      /*is_wheel_scroll=*/false,
                      /*affect_browser_controls=*/false,
                      /*scroll_outer_viewport=*/true);
  client_->SetNeedsCommitOnImplThread();
  client_->RenewTreePriority();
}
#endif

#if BUILDFLAG(ARKWEB_THROTTLE_FRAME)
base::TimeDelta LayerTreeHostImpl::ThrottleFrameEnd() {
  if (throttleFrameStarted_) {
    if (layer_tree_frame_sink_) {
      layer_tree_frame_sink_->UpdateThrottleMode(false);
    }
    throttleFrameStarted_ = false;
    return viz::BeginFrameArgs::MinInterval();
  }
  return frame_rate_estimator_.GetPreferredInterval();
}

void LayerTreeHostImpl::ThrottleFrameStart() {
  if (!throttleFrameStarted_) {
    layer_tree_frame_sink_->UpdateThrottleMode(true);
    throttleFrameStarted_ = true;
  }
}

bool LayerTreeHostImpl::IsThrottleEnable() {
  static const bool feature_allowed = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                      .GetSystemPropertiesInstance()
                                      .GetBoolParameter("web.throttle.enabled", false)
  return feature_allowed && frame_rate_estimator_.GetBeginFrameThrottleMode();
}
#endif
// LCOV_EXCL_STOP

} // namespace cc
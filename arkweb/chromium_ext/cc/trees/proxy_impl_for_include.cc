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

#if BUILDFLAG(ARKWEB_WEBGL)
void ProxyImpl::SetDeferInvalidationForFastMainFrameFromImpl(
    bool defer_invalidation_for_fast_main_frame) {
  DCHECK(IsImplThread());
  scheduler_->GetSchedulerUtils()->SetDeferInvalidationForFastMainFrame(
      defer_invalidation_for_fast_main_frame);
}
#endif

#if BUILDFLAG(ARKWEB_PINCH_SMOOTH)
void ProxyImpl::SetPinchSmoothModeOnImpl(bool isEnable) {
  DCHECK(IsImplThread());
  pinch_smooth_ = isEnable;
}
#endif

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void ProxyImpl::HandleScrollUpdateForInternalBeginFrame(
    const viz::BeginFrameArgs& args) {
  DCHECK(IsImplThread());
  host_impl_->HandleScrollUpdateForInternalBeginFrame(args);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)

#if BUILDFLAG(ARKWEB_SAME_LAYER)
void ProxyImpl::OnLayerRectUpdate(int id, const gfx::Rect& rect) {
  DCHECK(IsImplThread());
  MainThreadTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&ProxyMain::OnLayerRectUpdate,
                                proxy_main_weak_ptr_, id, rect));
}

void ProxyImpl::OnLayerRectVisibilityChange(int id, bool visibility) {
  DCHECK(IsImplThread());
  MainThreadTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&ProxyMain::OnLayerRectVisibilityChange,
                                proxy_main_weak_ptr_, id, visibility));
}
#endif

#if BUILDFLAG(ARKWEB_VIDEO_ASSISTANT)
void ProxyImpl::OnLayerBoundsUpdate(int id, const gfx::Rect& bounds) {
  DCHECK(IsImplThread());
  MainThreadTaskRunner()->PostTask(
      FROM_HERE, base::BindOnce(&ProxyMain::OnLayerBoundsUpdate,
                                proxy_main_weak_ptr_, id, bounds));
}
#endif  // ARKWEB_VIDEO_ASSISTANT
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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/display_cutout_client_imp_utils.h"

#include "third_party/blink/renderer/core/css/style_engine.h"
#include "third_party/blink/renderer/core/frame/display_cutout_client_impl.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/page/page_utils.h"

#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
#include "third_party/blink/renderer/core/page/page.h"
#include "ui/gfx/geometry/insets.h"
#endif

namespace blink {

DisplayCutoutClientImplUtils::DisplayCutoutClientImplUtils(
    DisplayCutoutClientImpl* impl)
    : impl_(impl) {}
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
void DisplayCutoutClientImplUtils::UpdateSafeArea(LocalFrame* frame) {
  if (!frame || !frame->GetDocument() || !frame->GetPage()) {
    return;
  }

  DocumentStyleEnvironmentVariables& vars =
      frame->GetDocument()->GetStyleEngine().EnsureEnvironmentVariables();
  gfx::Insets safe_area = frame->GetPage()->page_utils()->SafeAreaScaled();
  LOG(DEBUG) << __func__ << " " << safe_area.ToString();

  vars.SetVariable(UADefinedVariable::kSafeAreaInsetTop,
                   StyleEnvironmentVariables::FormatPx(safe_area.top()));
  vars.SetVariable(UADefinedVariable::kSafeAreaInsetLeft,
                   StyleEnvironmentVariables::FormatPx(safe_area.left()));
  vars.SetVariable(UADefinedVariable::kSafeAreaInsetBottom,
                   StyleEnvironmentVariables::FormatPx(safe_area.bottom()));
  vars.SetVariable(UADefinedVariable::kSafeAreaInsetRight,
                   StyleEnvironmentVariables::FormatPx(safe_area.right()));
}
#endif

void DisplayCutoutClientImplUtils::SetSafeArea(const gfx::Insets& safe_area) {
#if BUILDFLAG(ARKWEB_DISPLAY_CUTOUT)
  if (impl_->frame_->GetPage()) {
    impl_->frame_->GetPage()->page_utils()->SetSafeArea(safe_area);
  }
  DisplayCutoutClientImplUtils::UpdateSafeArea(impl_->frame_);
#else
  DocumentStyleEnvironmentVariables& vars = impl_->frame_->GetDocument()
                                                ->GetStyleEngine()
                                                .EnsureEnvironmentVariables();

  vars.SetVariable(UADefinedVariable::kSafeAreaInsetTop,
                   StyleEnvironmentVariables::FormatPx(safe_area.top()));
  vars.SetVariable(UADefinedVariable::kSafeAreaInsetLeft,
                   StyleEnvironmentVariables::FormatPx(safe_area.left()));
  vars.SetVariable(UADefinedVariable::kSafeAreaInsetBottom,
                   StyleEnvironmentVariables::FormatPx(safe_area.bottom()));
  vars.SetVariable(UADefinedVariable::kSafeAreaInsetRight,
                   StyleEnvironmentVariables::FormatPx(safe_area.right()));
#endif
}
}  // namespace blink
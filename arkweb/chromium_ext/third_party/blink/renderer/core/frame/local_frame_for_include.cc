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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_for_include.h"

#include "arkweb/build/features/features.h"
#include "third_party/blink/renderer/core/page/page_utils.h"
#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
#include "base/memory/safe_ref.h"
#endif
#if BUILDFLAG(IS_ARKWEB)
#include "arkweb/chromium_ext/third_party/blink/renderer/core/editing/frame_selection_ext.h"
#include "base/ohos/sys_info_utils_ext.h"
#endif

namespace blink {
  
// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
void LocalFrame::NotifyContextMenuWillShow() {
  GetLocalFrameHostRemote().NotifyContextMenuWillShow();
}
#endif
// LCOV_EXCL_STOP

bool LocalFrameUtil::SetLayoutAndTextZoomFactorsPage(
    LocalFrame* LocalFrameObj,
    float& layout_zoom_factor,
    float& text_zoom_factor,
    Page* page) {
  if (!page) {
    return false;
  }

  text_zoom_factor = page->GetSettings().GetTextZoomFactor();
  if (LocalFrameObj->layout_zoom_factor_ == layout_zoom_factor &&
      LocalFrameObj->text_zoom_factor_ == text_zoom_factor) {
    return false;
  }
  return true;
}

void LocalFrameUtil::SetLayoutAndTextZoomFactorsExt(
    LocalFrame* LocalFrameObj,
    float& layout_zoom_factor,
    float& text_zoom_factor,
    bool& layout_zoom_changed,
    Page* page) {
  if (base::ohos::IsTabletDevice()) {
    float zoom_factor_for_device_scale =
        page->GetChromeClient().ZoomFactorForViewportLayout();
    zoom_factor_for_device_scale =
        zoom_factor_for_device_scale ? zoom_factor_for_device_scale : 1;
    float current_zoom_factor =
        layout_zoom_factor / zoom_factor_for_device_scale;
    if (layout_zoom_changed && current_zoom_factor > 1.0f &&
        !LocalFrameObj->scale_limits_min_changed_) {
      LocalFrameObj->scale_limits_min_changed_ = true;
      page->page_utils()->ResetPageScaleConstraints(false);
      LocalFrameObj->scale_limits_max_changed_ = false;
    }
    if (layout_zoom_changed && current_zoom_factor <= 1.0f &&
        !LocalFrameObj->scale_limits_max_changed_) {
      LocalFrameObj->scale_limits_max_changed_ = true;
      page->page_utils()->ResetPageScaleConstraints(true);
      LocalFrameObj->scale_limits_min_changed_ = false;
    }
  }
}

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_ADBLOCK)
void LocalFrame::DidSubresourceFiltered() {
  Client()->DispatchDidSubresourceFiltered();
}

bool LocalFrame::GetGlobalAdblockEnabled() {
  return Client()->GetGlobalAdblockEnabled();
}

void LocalFrame::SetAdBlockEnableForSite(bool site_adblock_enabled) {
  LOG(WARNING) << "[Adblock] Set adblock enable for site: "
               << site_adblock_enabled;
  adblock_enabled_for_site_ = site_adblock_enabled;
}

void LocalFrame::SetHasElemHideTypeOption(bool has_elemhide_type_option) {
  has_elemhide_type_option_ = has_elemhide_type_option;
}

void LocalFrame::SetHasDocumentTypeOption(bool has_document_type_option) {
  has_document_type_option_ = has_document_type_option;
}

void LocalFrame::SetHasGenericHideTypeOption(bool has_generichide_type_option) {
  has_generichide_type_option_ = has_generichide_type_option;
}
#endif
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_GET_SCROLL_OFFSET)
void LocalFrame::OnOverScrollOffsetChanged(float offset_x, float offset_y) {
  if (!IsMainThread()) {
    if (!GetFrameScheduler()) {
      LOG(ERROR) << "LocalFrame::OnOverScrollOffsetChanged GetFrameScheduler "
                    "is nullptr";
      return;
    }
    auto task_runner = GetTaskRunner(TaskType::kInternalDefault);
    if (!task_runner) {
      LOG(ERROR)
          << "LocalFrame::OnOverScrollOffsetChanged GetTaskRunner is nullptr";
      return;
    }
    task_runner->PostTask(
        FROM_HERE,
        WTF::BindOnce(&LocalFrame::OnOverScrollOffsetChanged,
                      weak_local_frame_.GetSafeRef(), offset_x, offset_y));
  } else {
    GetLocalFrameHostRemote().OnOverScrollOffsetChanged(offset_x, offset_y);
  }
}
#endif
}  // namespace blink

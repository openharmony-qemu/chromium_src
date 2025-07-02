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

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
#include <multimedia/player_framework/native_avscreen_capture.h>
#include <multimedia/player_framework/native_avscreen_capture_base.h>
#include "content/browser/renderer_host/media/video_capture_manager.h"
#endif
#include "base/logging.h"
#include "chrome/browser/extensions/extension_tab_util.h"
#include "extensions/browser/extension_action.h"
#include "extensions/common/extension.h"

namespace extensions {

#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
bool DesktopCaptureChooseDesktopMediaFunctionBase::ExecuteOhos(
    api::desktop_capture::DesktopCaptureSourceType source_type,
    std::vector<DesktopMediaList::Type>& media_types,
    bool has_screen) {
  if (source_type != api::desktop_capture::DesktopCaptureSourceType::kWindow &&
      source_type != api::desktop_capture::DesktopCaptureSourceType::kTab) {
    return false;
  }
  if (!has_screen) {
    media_types.push_back(DesktopMediaList::Type::kScreen);
    has_screen = true;
  }
  return true;
}

int GetNwebId(
    const content::GlobalRenderFrameHostId& render_frame_host_id) {
  content::RenderFrameHost* render_frame_host =
      content::RenderFrameHost::FromID(render_frame_host_id);
  if (render_frame_host == nullptr) {
    return -1;
  }
  content::WebContents* web_contents =
      content::WebContents::FromRenderFrameHost(render_frame_host);
  if (web_contents == nullptr) {
    return -1;
  }
  return web_contents->GetNWebId();
}

#endif // ARKWEB_ARKWEB_EXTENSIONS

}

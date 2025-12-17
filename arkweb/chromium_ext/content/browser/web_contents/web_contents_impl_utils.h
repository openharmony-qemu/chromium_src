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

#ifndef CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IMPL_UTILS_H_
#define CONTENT_BROWSER_WEB_CONTENTS_WEB_CONTENTS_IMPL_UTILS_H_

#include <string>

#include "arkweb/build/features/features.h"
#include "base/memory/raw_ptr.h"
#include "content/browser/site_instance_impl.h"
#include "content/public/browser/web_contents.h"
#include "content/browser/renderer_host/frame_tree_node.h"

#if BUILDFLAG(ARKWEB_PDF)
#include "base/ohos/ltpo/include/sliding_observer.h"
#include "components/pdf/common/pdf_util.h"
#endif

namespace content {
class WebContentsImpl;

class WebContentsImplUtils {
 public:
  static bool is_pdf_static;
  raw_ptr<WebContentsImpl> webContentsImpl;
  WebContentsImplUtils(WebContentsImpl* impl);

#if BUILDFLAG(ARKWEB_I18N)
  void UpdateRenderAcceptLanguageIfNeed(const std::string& old_accept_language);
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  void renderProcessShareInit(const WebContents::CreateParams& params,
                              scoped_refptr<SiteInstanceImpl> site_instance);
#endif

#if BUILDFLAG(ARKWEB_EXT_TOPCONTROLS)
  void UpdateMainFrameLoadingControlsState(FrameTreeNode* frame_tree_node, bool should_show_loading_ui);
#endif

#if BUILDFLAG(ARKWEB_USERAGENT) || BUILDFLAG(ARKWEB_EXT_UA)
  void UpdateUserAgentOverride(const blink::UserAgentOverride& ua_override);
#endif

#if BUILDFLAG(ARKWEB_PDF)
  void JudgeIsPdfPageVisibilityChanged(Visibility visibility);
#endif

#if BUILDFLAG(ARKWEB_OFFLINE_WEB_EVICT_BACK_BUFFERS)
  void EvictFrameBackBuffersWhenNWebWasHidden();
#endif
};
}  // namespace content
#endif

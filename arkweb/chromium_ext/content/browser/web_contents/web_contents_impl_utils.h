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
#include "content/browser/site_instance_impl.h"
#include "content/public/browser/web_contents.h"

namespace content {
class WebContentsImpl;

class WebContentsImplUtils {
 public:
  WebContentsImpl* webContentsImpl;
  WebContentsImplUtils(WebContentsImpl* impl);

#if BUILDFLAG(ARKWEB_I18N)
  void UpdateRenderAcceptLanguageIfNeed(const std::string& old_accept_language);
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  void renderProcessShareInit(const WebContents::CreateParams& params,
                              scoped_refptr<SiteInstanceImpl> site_instance);
#endif
};

}  // namespace content
#endif

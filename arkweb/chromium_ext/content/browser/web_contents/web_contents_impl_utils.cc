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

#include "arkweb/chromium_ext/content/browser/web_contents/web_contents_impl_utils.h"

#include "content/browser/web_contents/web_contents_impl.h"

#if BUILDFLAG(ARKWEB_I18N)
#include "base/ohos/locale_utils.h"
#include "ui/base/ui_base_switches.h"
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
#include "content/browser/renderer_host/render_process_host_impl.h"
#endif

namespace content {

WebContentsImplUtils::WebContentsImplUtils(WebContentsImpl* impl) {
  this->webContentsImpl = impl;
}

#if BUILDFLAG(ARKWEB_I18N)
void WebContentsImplUtils::UpdateRenderAcceptLanguageIfNeed(
    const std::string& old_accept_language) {
  const base::CommandLine& command_line =
      *base::CommandLine::ForCurrentProcess();
  if (!command_line.HasSwitch(::switches::kLang)) {
    return;
  }
  std::string lang = command_line.GetSwitchValueASCII(::switches::kLang);
  std::regex pattern("-");
  std::smatch match;
  std::string current_accept_language;
  if (std::regex_search(lang, match, pattern)) {
    std::string region = match.suffix();
    current_accept_language = base::ohos::ComputeLanguageByRegion(region);
  } else {
    current_accept_language = base::ohos::ComputeLanguageByRegion("");
  }
  if (current_accept_language != "" &&
      current_accept_language != old_accept_language) {
    webContentsImpl->renderer_preferences_.accept_languages =
        current_accept_language;
  }
}
#endif  // #if BUILDFLAG(ARKWEB_I18N)

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
void WebContentsImplUtils::renderProcessShareInit(
    const WebContents::CreateParams& params,
    scoped_refptr<SiteInstanceImpl> site_instance) {
  if (!params.shared_render_process_token.empty()) {
    webContentsImpl->shared_render_process_token_ =
        params.shared_render_process_token;
    RenderProcessHost* render_process =
        RenderProcessHostImpl::GetProcessForSharedToken(
            webContentsImpl->shared_render_process_token_);
    if (render_process) {
      site_instance->ReuseExistingProcessIfPossible(render_process);
    } else {
      RenderProcessHostImpl::RegisteProcessForSharedToken(
          webContentsImpl->shared_render_process_token_,
          site_instance->GetProcess());
    }
  }
}
#endif
}  // namespace content

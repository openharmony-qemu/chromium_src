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

#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_ext.h"

#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_utils.h"

#if BUILDFLAG(IS_ARKWEB)
#include "base/ohos/nweb_engine_event_logger.h"
#endif

namespace content {

ArkwebRenderProcessHostImplExt::ArkwebRenderProcessHostImplExt(
    BrowserContext* browser_context,
    StoragePartitionImpl* storage_partition_impl,
    int flags)
    : RenderProcessHostImpl(browser_context, storage_partition_impl, flags) {}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
bool ArkwebRenderProcessHostImplExt::IsProcessBackgrounded() {
  return priority_.is_background();
}

const base::TimeTicks& ArkwebRenderProcessHostImplExt::ProcessBackgroundTime() {
  return priority_.background_time;
}
#endif  // ARKWEB_RENDER_PROCESS_MODE

#if BUILDFLAG(ARKWEB_THEME_FONT)
void ArkwebRenderProcessHostImplExt::OnThemeFontChange() {
  if (auto* theme_font = ArkwebRenderProcessHostImplUtils::EnsureThemeFont()) {
    ArkwebRenderProcessHostImplUtils::UpdateThemeFontFile(
        this, theme_font->font_file.Duplicate());
  } else {
    ArkwebRenderProcessHostImplUtils::UpdateThemeFontFile(this, base::File());
  }
}
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
void ArkwebRenderProcessHostImplExt::dumpCurrentJavaScriptStackInMainThread(
    base::OnceCallback<void(const std::string&)> dump_callback) {
  child_process_->dumpCurrentJavaScriptStackInMainThread(base::BindOnce(
      [](base::OnceCallback<void(const std::string&)> callback,
         const std::string& stack) { std::move(callback).Run(stack); },
      std::move(dump_callback)));
}

void ArkwebRenderProcessHostImplExt::InvokeRenderCrashDump() {
  child_process_->InvokeRenderCrashDump();
}

#if BUILDFLAG(IS_ARKWEB)
void ArkwebRenderProcessHostImplExt::ReportEngineEvent(const std::string& module,
                                                       const std::string& resource,
                                                       const std::string& error_code,
                                                       const std::string& error_msg) {
  base::ohos::ReportEngineEvent(module, resource, error_code, error_msg);
}
#endif
#endif

}  // namespace content

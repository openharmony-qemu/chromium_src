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

#if BUILDFLAG(ARKWEB_THEME_FONT)
#include "third_party/blink/renderer/platform/fonts/font_cache.h"
#include "third_party/skia/include/core/SkFontMgr.h"
#endif  // ARKWEB_THEME_FONT
 
#if BUILDFLAG(ARKWEB_I18N)
#include "ui/base/resource/resource_bundle.h"
#endif
 
#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "arkweb/chromium_ext/content/renderer/media/ohos/native_texture_factory.h"
#endif
 
#if BUILDFLAG(ARKWEB_HTML_SELECT)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#endif

namespace content {

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void InitializeWebKitExt(scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_) {
  if (!compositor_task_runner_) {
    LOG(WARNING) << "compositor task runner is nullptr";
  } else {
    compositor_task_runner_->PostTask(FROM_HERE,
      base::BindOnce(&ChildProcess::ReportCompositorKeyThread, base::Unretained(ChildProcess::current()), true));
  }
}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
scoped_refptr<NativeTextureFactory> RenderThreadImpl::GetNativeTexureFactory() {
  DCHECK(IsMainThread());
  if (!native_texture_factory_ || native_texture_factory_->IsLost()) {
    scoped_refptr<gpu::GpuChannelHost> channel = EstablishGpuChannelSync();
    if (!channel) {
      native_texture_factory_ = nullptr;
      return nullptr;
    }
    native_texture_factory_ = NativeTextureFactory::Create(std::move(channel));
  }
  return native_texture_factory_;
}
#endif

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
void RenderThreadImpl::SetDrawMode(int mode, base::PassKey<AgentSchedulingGroup>) {
  DCHECK(blink_platform_impl_);
  blink_platform_impl_->SetDrawMode(mode);
}
#endif

#if BUILDFLAG(ARKWEB_THEME_FONT)
void RenderThreadImpl::UpdateThemeFontFile(base::File theme_font) {
  blink::FontCache::Get().Invalidate();
  skia::DefaultFontMgr().get()->InvalidateThemeFont(theme_font.GetPlatformFile());
  blink::FontCache::Get().InvalidateSystemFontFamily();
}
#endif
 
#if BUILDFLAG(ARKWEB_I18N)
void RenderThreadImpl::NotifyLocaleChanged(const std::string& locale) {
  if (!ui::ResourceBundle::HasSharedInstance() ||
      !ui::ResourceBundle::LocaleDataPakExists(locale)) {
    LOG(ERROR) << "render thread update locale failed";
    return;
  }
  std::string origin_locale =
      ui::ResourceBundle::GetSharedInstance().GetLoadedLocaleForTesting();
  if (origin_locale == locale) {
    LOG(WARNING) << "render thread no need to update locale";
    return;
  }
  std::string result =
      ui::ResourceBundle::GetSharedInstance().ReloadLocaleResources(locale);
  if (result.empty()) {
    LOG(ERROR) << "CefFrameImpl update locale failed";
  }
}
#endif

}  // namespace content
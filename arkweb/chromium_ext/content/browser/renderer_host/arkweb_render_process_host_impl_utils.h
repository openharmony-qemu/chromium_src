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

#ifndef ARKWEB_RENDER_PROCESS_HOST_IMPL_UTILS_H_
#define ARKWEB_RENDER_PROCESS_HOST_IMPL_UTILS_H_

#include "arkweb/build/features/features.h"

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_ext.h"
#endif

#if BUILDFLAG(ARKWEB_THEME_FONT)
#include "base/files/file.h"
#include "base/json/json_reader.h"
#endif

#ifdef BUILDFLAG(ARKWEB_INCOGNITO_MODE)
#include "base/ohos/sys_info_utils_ext.h"
#endif

#include "base/memory/raw_ptr.h"
#include "content/public/browser/render_process_host.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

// Stores the maximum number of renderer processes the content module can
// create. Only applies if it is set to a non-zero value.
extern size_t g_max_renderer_count_override;

namespace device {
namespace mojom {
class ScreenOrientation;
}
}  // namespace device

namespace content {

class RenderProcessHostImpl;
class ArkwebRenderProcessHostImplExt;
class DelayedRenderKiller;

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
typedef std::map<std::string, RenderProcessHost*>
    SharedProcessTokenToProcessMap;
#endif

namespace {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
inline RenderProcessMode g_render_process_mode =
    RenderProcessMode::DEFAULT_MODE;
static constexpr char kExtensionScheme[] = "chrome-extension";
static constexpr char kArkwebExtensionScheme[] = "arkweb-extension";
#endif
}  // namespace

class ArkwebRenderProcessHostImplUtils {
 public:
  friend class RenderProcessHostImpl;
  friend class ArkwebRenderProcessHostImplExt;
  friend class DelayedRenderKiller;
  explicit ArkwebRenderProcessHostImplUtils(
      RenderProcessHostImpl* render_process_host_impl);
  ~ArkwebRenderProcessHostImplUtils();

  ArkwebRenderProcessHostImplUtils(const ArkwebRenderProcessHostImplUtils&) =
      delete;
  ArkwebRenderProcessHostImplUtils& operator=(
      const ArkwebRenderProcessHostImplUtils&) = delete;

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
  static RenderProcessHost* GetProcessForSharedToken(
      const std::string& shared_render_process_token);

  static void RegisteProcessForSharedToken(
      const std::string& shared_render_process_token,
      RenderProcessHost* renderProcessHost);
  static void RemoveFromSharedRenderProcessMap(
      RenderProcessHost* renderProcessHost);
#endif

  static size_t GetMaxRendererProcessCountEx();

  static bool IsSuitableHostForArkweb(RenderProcessHost* host,
                                      const SiteInfo& site_info);

  static size_t GetProcessCountForLimitArkweb(size_t process_count_to_ignore);

  static void Refresh();

  static void GetProcessHostForSiteInstanceArkweb(
      RenderProcessHost* render_process_host,
      const SiteInfo& site_info,
      BrowserContext* browser_context,
      size_t count,
      SiteInstanceImpl* site_instance);

#if BUILDFLAG(ARKWEB_I18N)
  static void NotifyLocaleChanged(RenderProcessHostImpl* host,
                                  const std::string& update_locale);
#endif

#if BUILDFLAG(ARKWEB_THEME_FONT)
  static ThemeFont* EnsureThemeFont();
  static bool IsThemeFontValid();
  static void UpdateThemeFontFile(RenderProcessHostImpl* host,
                                  std::vector<base::File> font_files);
#endif

#if BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)
 void AddHostUIThreadInterface(service_manager::BinderRegistry* registry);
#endif
#if BUILDFLAG(ARKWEB_CRASHPAD)
  void AddDFXToUIThreadInterface(service_manager::BinderRegistry* registry);
#endif
#if BUILDFLAG(ARKWEB_TEST)
 public:
#else
 private:
#endif
  const raw_ptr<RenderProcessHostImpl> render_process_host_impl_;
#if BUILDFLAG(ARKWEB_THEME_FONT)
  static std::unique_ptr<ThemeFont> g_theme_font_;
#endif
  static RenderProcessHost* GetExistingBackgroundProcessHost();
  static void ReportKeyThreadEx(int32_t status,
                                int32_t process_id,
                                int32_t thread_id,
                                int32_t role);
  static void ReportKeyThreadIdsEx(int32_t status,
                                   int32_t process_id,
                                   const std::vector<int32_t>& thread_ids,
                                   int32_t role);
#if BUILDFLAG(ARKWEB_DFX_TRACING)
  static void ReportHisyevent(int64_t block_time, const std::string& mode);
#endif
};

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
class DelayedRenderKiller {
  public:
    static DelayedRenderKiller* GetInstance() {
      static DelayedRenderKiller* inst_ = new DelayedRenderKiller();
      return inst_;
    }
    ~DelayedRenderKiller() = delete;
    void StartTimer();
    bool NeedDebug();
 
#if BUILDFLAG(ARKWEB_TEST)
  public:
#else
  private:
#endif
    DelayedRenderKiller()
      : timer_(std::make_unique<base::RepeatingTimer>()) {
    }
    DelayedRenderKiller(const DelayedRenderKiller& i) = delete;
    DelayedRenderKiller& operator= (const DelayedRenderKiller& i) = delete;
 
    void TryKillRender();
 
    std::unique_ptr<base::RepeatingTimer> timer_;
    int32_t rep_ = 0;
    const int32_t MAX_REP = 15;
    const int32_t SLEEP_TIME = 3;
};
#endif
}  // namespace content

#endif  // ARKWEB_RENDER_PROCESS_HOST_IMPL_UTILS_H_

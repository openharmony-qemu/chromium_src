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
#include "arkweb/chromium_ext/content/browser/renderer_host/arkweb_render_process_host_impl_utils.h"

#include "content/browser/renderer_host/render_frame_host_impl.h"
#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/browser/site_info.h"
#include "content/browser/site_instance_impl.h"
#include "content/browser/webui/web_ui_controller_factory_registry.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/device_service.h"
#include "media/mojo/mojom/interface_factory.mojom.h"
#include "media/mojo/mojom/video_encoder_metrics_provider.mojom.h"
#include "services/device/public/mojom/power_monitor.mojom.h"
#include "services/device/public/mojom/screen_orientation.mojom.h"
#include "services/device/public/mojom/time_zone_monitor.mojom.h"
#include "services/metrics/public/cpp/ukm_recorder.h"
#include "services/metrics/public/mojom/ukm_interface.mojom.h"
#if BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)
#include "arkweb/chromium_ext/services/device/public/mojom/res_sched_report.mojom.h"
#include "arkweb/chromium_ext/services/device/public/mojom/sysprop_render_observer.mojom.h"
#endif  // BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)

// VLOG additional statements in Fuchsia release builds.
#if BUILDFLAG(IS_FUCHSIA)
#define MAYBEVLOG VLOG
#else
#define MAYBEVLOG DVLOG
#endif

namespace content {
namespace {

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
// the global list of all renderer processes
SharedProcessTokenToProcessMap& GetAllSharedProcessHosts() {
  static base::NoDestructor<SharedProcessTokenToProcessMap>
      s_all_shared_process_hosts;
  return *s_all_shared_process_hosts;
}
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
constexpr int kSingleRenderProcessCount = 1;
constexpr int kMinMultipleRenderProcessCount = 5;
#endif

}  // namespace

ArkwebRenderProcessHostImplUtils::ArkwebRenderProcessHostImplUtils(
    RenderProcessHostImpl* render_process_host_impl)
    : render_process_host_impl_(render_process_host_impl) {
  DCHECK(render_process_host_impl);
}

ArkwebRenderProcessHostImplUtils::~ArkwebRenderProcessHostImplUtils() = default;

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_SHARE)
RenderProcessHost* ArkwebRenderProcessHostImplUtils::GetProcessForSharedToken(
    const std::string& shared_render_process_token) {
  SharedProcessTokenToProcessMap& processes = GetAllSharedProcessHosts();
  auto process = processes.find(shared_render_process_token);
  if (process == processes.end()) {
    return nullptr;
  }
  return process->second;
}

void ArkwebRenderProcessHostImplUtils::RegisteProcessForSharedToken(
    const std::string& shared_render_process_token,
    RenderProcessHost* renderProcessHost) {
  GetAllSharedProcessHosts().emplace(shared_render_process_token,
                                     renderProcessHost);
}

void ArkwebRenderProcessHostImplUtils::RemoveFromSharedRenderProcessMap(
    RenderProcessHost* renderProcessHost) {
  SharedProcessTokenToProcessMap& processes = GetAllSharedProcessHosts();
  if (processes.empty()) {
    return;
  }
  auto iter = processes.begin();
  for (; iter != processes.end(); ++iter) {
    if (iter->second == renderProcessHost) {
      processes.erase(iter);
      break;
    }
  }
}
#endif

size_t ArkwebRenderProcessHostImplUtils::GetMaxRendererProcessCountEx() {
  if (RenderProcessHost::render_process_mode() ==
      RenderProcessMode::SINGLE_MODE) {
    return kSingleRenderProcessCount;
  }

  if (g_max_renderer_count_override) {
    return std::max(kMinMultipleRenderProcessCount,
                    (int)(g_max_renderer_count_override * 0.9));
  }

  size_t client_override =
      GetContentClient()->browser()->GetMaxRendererProcessCountOverride();
  if (client_override) {
    return client_override * 0.9;
  }

  static size_t max_count = 0;
  if (!max_count) {
    static constexpr size_t kEstimatedWebContentsMemoryUsage =
#if defined(ARCH_CPU_64_BITS)
        85;  // In MB
#else
        60;  // In MB
#endif
    max_count = base::SysInfo::AmountOfPhysicalMemoryMB() / 2;
    max_count /= kEstimatedWebContentsMemoryUsage;

    static constexpr size_t kMinRendererProcessCount = 3;
    static const size_t kMaxRendererProcessCount =
        RenderProcessHostImpl::GetPlatformMaxRendererProcessCount();
    DCHECK_LE(kMinRendererProcessCount, kMaxRendererProcessCount);

    max_count = std::clamp(max_count, kMinRendererProcessCount,
                           kMaxRendererProcessCount);
    MAYBEVLOG(1) << __func__ << ": Calculated max " << max_count;
  }

  return max_count * 0.9;
}

bool ArkwebRenderProcessHostImplUtils::IsSuitableHostForArkweb(
    RenderProcessHost* host,
    const SiteInfo& site_info) {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  if (site_info.is_pdf()) {
    return host->special_render_numbers_[RenderProcessHost::RenderType::kPdf] >
           0;
  }
  if (site_info.site_url().SchemeIs(kExtensionScheme)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
      || site_info.site_url().SchemeIs(kArkwebExtensionScheme)
#endif
  ) {
    return host->special_render_numbers_
               [RenderProcessHost::RenderType::kExtension] > 0;
  }
#endif  // ARKWEB_RENDER_PROCESS_MODE
  return false;
}

size_t ArkwebRenderProcessHostImplUtils::GetProcessCountForLimitArkweb(
    size_t process_count_to_ignore) {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  RenderProcessHost::iterator it = RenderProcessHost::AllHostsIterator();
  size_t count = 0;
  while (!it.IsAtEnd()) {
    RenderProcessHostImpl* host =
        static_cast<RenderProcessHostImpl*>(it.GetCurrentValue());

    if (!host->is_dead()) {
      count++;
    }
    it.Advance();
  }
  LOG(DEBUG) << "RenderProcessHostImpl::GetProcessCount count: " << count
             << ", process_count_to_ignore:" << process_count_to_ignore;
  return count - process_count_to_ignore;
#endif
}

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
class DelayedRenderKiller {
  public:
    static DelayedRenderKiller* GetInstance() {
      static DelayedRenderKiller inst_;
      return &inst_;
    }
    ~DelayedRenderKiller() = default;
    void StartTimer() {
      if (!timer_.IsRunning()) {
        rep_ = 0;
        timer_.Start(FROM_HERE, base::Seconds(SLEEP_TIME),
          base::BindRepeating(&DelayedRenderKiller::TryKillRender, base::Unretained(this)));
      }
    }
    bool NeedDebug()
    {
      return rep_ >= MAX_REP;
    }

  private:
    DelayedRenderKiller() = default;
    DelayedRenderKiller(const DelayedRenderKiller& i) = delete;
    DelayedRenderKiller& operator= (const DelayedRenderKiller& i) = delete;

    void TryKillRender() {
      LOG(INFO) << "DelayedRenderKiller start timer";
      rep_++;
      if (rep_ > MAX_REP) {
        LOG(INFO) << "DelayedRenderKiller up to limit";
        timer_.Stop();
        return;
      }
      size_t count = RenderProcessHostImpl::GetProcessCountForLimit();
      if (RenderProcessHost::render_process_mode() !=
            RenderProcessMode::SINGLE_MODE &&
        (count > RenderProcessHostImpl::GetMaxRendererProcessCount())) {
        // Kill the idel render process.
        RenderProcessHostImpl* render_host = static_cast<RenderProcessHostImpl*>(
            ArkwebRenderProcessHostImplUtils::GetExistingBackgroundProcessHost());
        if (render_host) {
          render_host->FastShutdownIfPossible(1u, true);
          LOG(INFO) << "Successfully tried to fast shutdown idle render process with handle: "
                    << render_host->GetProcess().Handle();
        }
      }

      count = RenderProcessHostImpl::GetProcessCountForLimit();
      if (count <= RenderProcessHostImpl::GetMaxRendererProcessCount()) {
        timer_.Stop();
        LOG(INFO) << "DelayedRenderKiller stop timer";
      }
    }

    base::RepeatingTimer timer_;
    int32_t rep_ = 0;
    const int32_t MAX_REP = 15;
    const int32_t SLEEP_TIME = 3;
};

// static
RenderProcessHost*
ArkwebRenderProcessHostImplUtils::GetExistingBackgroundProcessHost() {
  // First figure out which existing renderers we can use.
  RenderProcessHost* longest_background_host;
  base::TimeDelta longest_duration;
  base::TimeTicks current_time = base::TimeTicks::Now();

  for (RenderProcessHost::iterator iter(RenderProcessHost::AllHostsIterator());
       !iter.IsAtEnd(); iter.Advance()) {
    // The spare RenderProcessHost will have been considered by this point.
    // Ensure it is not added to the collection of suitable renderers.
    if (iter.GetCurrentValue()->IsSpare()) {
      continue;
    }

    if (DelayedRenderKiller::GetInstance()->NeedDebug()) {
      LOG(INFO) << "isBackground " << iter.GetCurrentValue()->IsProcessBackgrounded()
                << " AreAllRefCountsZero " << static_cast<RenderProcessHostImpl*>
                (iter.GetCurrentValue())->AreAllRefCountsZero()
                << " ActiveView " << iter.GetCurrentValue()->GetActiveViewCount();
    }

    if (iter.GetCurrentValue()->IsProcessBackgrounded() &&
        static_cast<RenderProcessHostImpl*>(iter.GetCurrentValue())
            ->AreAllRefCountsZero() &&
        iter.GetCurrentValue()->GetActiveViewCount() <= 1) {
      base::TimeDelta background_duration =
          current_time - iter.GetCurrentValue()->ProcessBackgroundTime();
      if (background_duration >= longest_duration) {
        longest_background_host = iter.GetCurrentValue();
        longest_duration = background_duration;
      }
    }
  }

  // Now pick a longest time in background renderer.
  if (longest_background_host) {
    LOG(INFO) << ": Found one background render process host "
              << longest_background_host->GetProcess().Handle();
    return longest_background_host;
  }

  LOG(WARNING) << ": It has no background render process host to shutdown.";
  return nullptr;
}

// static
void RenderProcessHost::SetRenderProcessMode(RenderProcessMode mode) {
  g_render_process_mode = mode;
  LOG(INFO) << "SetRenderProcessMode g_render_process_mode:"
            << (int)g_render_process_mode;
}

// static
RenderProcessMode RenderProcessHost::render_process_mode() {
  if (g_render_process_mode == RenderProcessMode::DEFAULT_MODE) {
    return (base::ohos::IsPcDevice() || base::ohos::IsTabletDevice())
               ? RenderProcessMode::MULTIPLE_MODE
               : RenderProcessMode::SINGLE_MODE;
  }
  return g_render_process_mode;
}
#endif  // ARKWEB_RENDER_PROCESS_MODE

// static
#if BUILDFLAG(ARKWEB_OOP_GPU_PROCESS)
void ArkwebRenderProcessHostImplUtils::Refresh() {
  RenderProcessHost::iterator it = RenderProcessHost::AllHostsIterator();
  if (it.IsAtEnd()) {
    return;
  }
  do {
    RenderProcessHostImpl* host =
        static_cast<RenderProcessHostImpl*>(it.GetCurrentValue());
    if (!host) {
      return;
    }
    auto temp_set = host->render_frame_host_id_set_;

    for (auto rfh_id : temp_set) {
      auto rfh = RenderFrameHostImpl::FromID(rfh_id);
      if (rfh && rfh->IsActive()) {
        rfh->Reload();
      }
    }
  } while (0);
}
#endif

void ArkwebRenderProcessHostImplUtils::GetProcessHostForSiteInstanceArkweb(
    RenderProcessHost* render_process_host,
    const SiteInfo& site_info,
    BrowserContext* browser_context,
    size_t count,
    SiteInstanceImpl* site_instance) {
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  if (site_info.site_url().SchemeIs(kExtensionScheme)
#if BUILDFLAG(ARKWEB_ARKWEB_EXTENSIONS)
      || site_info.site_url().SchemeIs(kArkwebExtensionScheme)
#endif
  ) {
    render_process_host
        ->special_render_numbers_[RenderProcessHost::RenderType::kExtension]++;
    int extensionCount = render_process_host->special_render_numbers_
                             [RenderProcessHost::RenderType::kExtension];
  }
  if (site_info.is_pdf()) {
    render_process_host
        ->special_render_numbers_[RenderProcessHost::RenderType::kPdf]++;
    int pdfCount =
        render_process_host
            ->special_render_numbers_[RenderProcessHost::RenderType::kPdf];
  }
  if (WebUIControllerFactoryRegistry::GetInstance()->UseWebUIForURL(
          browser_context, site_info.site_url())) {
    render_process_host
        ->special_render_numbers_[RenderProcessHost::RenderType::kWebUI]++;
    int webuiCount =
        render_process_host
            ->special_render_numbers_[RenderProcessHost::RenderType::kWebUI];
  }

  if (RenderProcessHost::render_process_mode() !=
          RenderProcessMode::SINGLE_MODE &&
      (count >= RenderProcessHostImpl::GetMaxRendererProcessCount())) {
    // Kill the idel render process.
    RenderProcessHostImpl* render_host = static_cast<RenderProcessHostImpl*>(
        ArkwebRenderProcessHostImplUtils::GetExistingBackgroundProcessHost());
    if (render_host) {
      render_host->FastShutdownIfPossible(1u, true);
      LOG(INFO) << "Successfully tried to fast shutdown idle render process "
                   "with handle: "
                << render_host->GetProcess().Handle();
    }

    if (RenderProcessHostImpl::GetProcessCountForLimit() >
      RenderProcessHostImpl::GetMaxRendererProcessCount()) {
      DelayedRenderKiller::GetInstance()->StartTimer();
    }
  }
#endif  // ARKWEB_RENDER_PROCESS_MODE
}

// static
#if BUILDFLAG(ARKWEB_I18N)
void ArkwebRenderProcessHostImplUtils::NotifyLocaleChanged(
    RenderProcessHostImpl* host,
    const std::string& update_locale) {
  host->GetRendererInterface()->NotifyLocaleChanged(update_locale);
}

// static
void RenderProcessHost::OnLocaleChangedToRenderer(
    const std::string& update_locale) {
  iterator iter(AllHostsIterator());
  while (!iter.IsAtEnd()) {
    RenderProcessHostImpl* host =
        static_cast<RenderProcessHostImpl*>(iter.GetCurrentValue());
    if (host && host->IsInitializedAndNotDead()) {
      ArkwebRenderProcessHostImplUtils::NotifyLocaleChanged(host,
                                                            update_locale);
    }
    iter.Advance();
  }
}
#endif

#if BUILDFLAG(ARKWEB_THEME_FONT)
const base::FilePath::CharType kAppThemePathA[] =
    FILE_PATH_LITERAL("/data/themes/a/app");
const base::FilePath::CharType kAppThemePathB[] =
    FILE_PATH_LITERAL("/data/themes/b/app");
const base::FilePath::CharType kAppThemeFontsDirName[] =
    FILE_PATH_LITERAL("fonts");
const base::FilePath::CharType kAppThemeFlagFileName[] =
    FILE_PATH_LITERAL("flag");
const base::FilePath::CharType kAppThemeFontsManifest[] =
    FILE_PATH_LITERAL("manifest.json");

std::unique_ptr<ThemeFont> ArkwebRenderProcessHostImplUtils::g_theme_font_ =
    nullptr;

// static
bool ArkwebRenderProcessHostImplUtils::IsThemeFontValid() {
  if (!g_theme_font_ || !base::PathExists(g_theme_font_->flag_path) ||
      !base::PathExists(g_theme_font_->manifest_path) ||
      !base::PathExists(g_theme_font_->font_path) ||
      !g_theme_font_->font_file.IsValid()) {
    return false;
  }

  return true;
}

// static
ThemeFont* ArkwebRenderProcessHostImplUtils::EnsureThemeFont() {
  if (IsThemeFontValid()) {
    return g_theme_font_.get();
  }

  g_theme_font_.reset();

  base::FilePath theme_path(kAppThemePathA);
  base::FilePath theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  if (!base::PathExists(
          base::FilePath(kAppThemePathA).Append(kAppThemeFlagFileName))) {
    if (!base::PathExists(
            base::FilePath(kAppThemePathB).Append(kAppThemeFlagFileName))) {
      LOG(DEBUG) << "[themefont] flag file occurs error";
      return nullptr;
    }
    theme_path = base::FilePath(base::FilePath(kAppThemePathB));
    theme_font_path = theme_path.Append(kAppThemeFontsDirName);
  }

  std::string input_json;
  base::FilePath manifest_path(theme_font_path.Append(kAppThemeFontsManifest));
  if (!base::PathExists(manifest_path) ||
      !base::ReadFileToString(manifest_path, &input_json) ||
      input_json.empty()) {
    LOG(DEBUG) << "[themefont] manifest file occurs error";
    return nullptr;
  }

  auto parsed_json = base::JSONReader::ReadAndReturnValueWithError(input_json);
  if (!parsed_json.has_value() || !parsed_json->is_dict()) {
    LOG(ERROR) << "[themefont] manifest file occurs error:"
               << parsed_json.error().message;
    return nullptr;
  }
  // Two example for the manifest.json:
  // {"id":"0","origin":"online","ttfFileSrc":"/absolute/path/themefont.ttf"}
  // {"id":"1","origin":"preset","ttfFileSrc":"/absolute/path/default.ttf"}
  const base::Value::Dict& dict = parsed_json->GetDict();
  const std::string* origin = dict.FindString("origin");
  if (!origin || origin->empty()) {
    LOG(ERROR) << "[themefont] manifest file has no origin tag";
    return nullptr;
  }
  if (*origin != std::string("online")) {
    LOG(DEBUG) << "[themefont] manifest file's origin tag is not online";
    return nullptr;
  }

  const std::string* absolte_font_path = dict.FindString("ttfFileSrc");
  if (!absolte_font_path || absolte_font_path->empty()) {
    LOG(ERROR) << "[themefont] manifest file has no ttfFileSrc tag";
    return nullptr;
  }

  base::FilePath font_path =
      theme_font_path.Append(base::FilePath(*absolte_font_path).BaseName());
  if (!base::PathExists(font_path)) {
    LOG(ERROR) << "[themefont] font file not exist:" << font_path.value();
    return nullptr;
  }

  base::File font_file(font_path,
                       base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!font_file.IsValid()) {
    LOG(ERROR) << "[themefont] font file not valid";
    return nullptr;
  }

  base::FilePath flag_path = theme_path.Append(kAppThemeFlagFileName);
  g_theme_font_ = std::make_unique<ThemeFont>();
  g_theme_font_->flag_path = flag_path;
  g_theme_font_->manifest_path = manifest_path;
  g_theme_font_->font_path = font_path;
  g_theme_font_->font_file = std::move(font_file);

  LOG(INFO) << "[themefont] valid font:" << font_path;

  return g_theme_font_.get();
}

void ArkwebRenderProcessHostImplUtils::UpdateThemeFontFile(
    RenderProcessHostImpl* host,
    base::File font_file) {
  host->GetRendererInterface()->UpdateThemeFontFile(std::move(font_file));
}
#endif

void ArkwebRenderProcessHostImplUtils::ReportKeyThreadEx(int32_t status,
                                                         int32_t process_id,
                                                         int32_t thread_id,
                                                         int32_t role) {
  using namespace OHOS::NWeb;
  LOG(INFO) << "TEST0324 ReportKeyThread pid = " << process_id
            << ", tid = " << thread_id << ", role = " << role;
  ResSchedClientAdapter::ReportKeyThread(
      static_cast<ResSchedStatusAdapter>(status), process_id, thread_id,
      static_cast<ResSchedRoleAdapter>(role));
}

void ArkwebRenderProcessHostImplUtils::ReportKeyThreadIdsEx(
    int32_t status,
    int32_t process_id,
    const std::vector<int32_t>& thread_ids,
    int32_t role) {
  using namespace OHOS::NWeb;
  for (auto thread_id : thread_ids) {
    ResSchedClientAdapter::ReportKeyThread(
        static_cast<ResSchedStatusAdapter>(status), process_id, thread_id,
        static_cast<ResSchedRoleAdapter>(role));
  }
}

void ArkwebRenderProcessHostImplUtils::AddHostUIThreadInterface(
    service_manager::BinderRegistry* registry) {
  render_process_host_impl_->AddUIThreadInterface(
      registry,
      base::BindRepeating(
          [](mojo::PendingReceiver<device::mojom::ResSchedReport> receiver) {
            GetDeviceService().BindResSchedReport(std::move(receiver));
          }));

  render_process_host_impl_->AddUIThreadInterface(
      registry,
      base::BindRepeating(
          [](mojo::PendingReceiver<device::mojom::SysPropRenderObserver>
                 receiver) {
            GetDeviceService().BindSysPropRenderObserver(
                std::move(receiver));
          }));
}

}  // namespace content

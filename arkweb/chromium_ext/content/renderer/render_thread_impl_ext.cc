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

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
#include "base/base_switches.h"
#include "content/renderer/logger_report.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
#include "third_party/blink/renderer/platform/weborigin/kurl.h"
#include "third_party/blink/renderer/platform/weborigin/security_origin.h"
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

namespace {

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
const base::TimeDelta kMaxIPCLoginInterval = base::Milliseconds(5);
base::TimeTicks g_last_renderer_log_sent_time[logging::LOGGING_MAX];
bool g_has_renderer_log_dropped[logging::LOGGING_MAX] = {false};

// Report renderer log on main thread because ReportRendererLog only can
// be called on MainThread, we cann't get the RenderThreadImpl when
// RenderThreadImpl::current() called from other threads due to it's a
// pointer in tls.
void ReportRendererLogOnMainThread(int policy, const std::string& msg) {
  if (policy <= 0 || policy >= logging::LOGGING_MAX) {
    return;
  }
  const base::TimeTicks now = base::TimeTicks::Now();
  if ((now - g_last_renderer_log_sent_time[policy]) < kMaxIPCLoginInterval) {
    if (!g_has_renderer_log_dropped[policy]) {
      g_has_renderer_log_dropped[policy] = true;
      std::string drop_info_msg;
      drop_info_msg.append(
          "Renderer ipc logging is too frequently, some subsequent logs will "
          "be dropped. ");
      drop_info_msg.append(msg);
      content::EventLog::ReportRendererLog(policy, drop_info_msg);
    }
  } else {
    g_has_renderer_log_dropped[policy] = false;
    content::EventLog::ReportRendererLog(policy, msg);
    g_last_renderer_log_sent_time[policy] = base::TimeTicks::Now();
  }
}

// Handle the FATAL message before crash.
bool HandleFatalMessageForCrashpad(int severity,
                                   const char* file,
                                   int line,
                                   size_t message_start,
                                   const std::string& string) {
  // Only handle FATAL.
  if (severity != logging::LOGGING_FATAL) {
    return false;
  }

  // In case of an out-of-memory condition, this code could be reentered when
  // constructing and storing the key. Using a static is not thread-safe, but if
  // multiple threads are in the process of a fatal crash at the same time, this
  // should work.
  static bool guarded = false;
  if (guarded) {
    return false;
  }
  base::AutoReset<bool> guard(&guarded, true);

  // Only log last path component.  This matches logging.cc.
  if (file) {
    const char* slash = strrchr(file, '/');
    if (slash) {
      file = slash + 1;
    }
  }

  CHECK_LE(message_start, string.size());

  // Rather than including the code to force the crash here, allow the caller to
  // do it.
  return false;
}

bool RenderProcessLogMessageHandler(int severity,
                                    const char* file,
                                    int line,
                                    size_t message_start,
                                    const std::string& str) {
  logging::LogPriority priority = logging::LOGGING_VERBOSE;
  switch (severity) {
    case logging::LOGGING_FEEDBACK:
      priority = logging::LOGGING_FEEDBACK;
      break;
    case logging::LOGGING_URL:
      priority = logging::LOGGING_URL;
      break;
    default:
      return false;
  }

  HandleFatalMessageForCrashpad(severity, file, line, message_start, str);

#if DCHECK_IS_ON()
  // For unittests.
  //  Some threads are not allowed to access 'g_main_task_runner.Get()'.
  //  See 'void AssertSingletonAllowed()' in file
  //  'base/threading/thread_restrictions.cc'.
#else
  if (severity >= logging::LOGGING_WARNING &&
      severity != logging::LOGGING_FATAL) {
    content::RenderThreadImpl::DeprecatedGetMainTaskRunner()->PostTask(
        FROM_HERE,
        base::BindOnce(ReportRendererLogOnMainThread, priority, str));
    return true;
  }
#endif
  return false;
}
#endif  // ARKWEB_LOGGER_REPORT

}  // anonymous namespace

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

// LCOV_EXCL_START
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
void RenderThreadImpl::UpdateThemeFontFile(const std::vector<base::File> theme_fonts) {
  std::vector<int> fds(theme_fonts.size());
  std::transform(theme_fonts.begin(), theme_fonts.end(), fds.begin(),
      [] (const base::File& f) { return f.GetPlatformFile(); });
  blink::FontCache::Get().Invalidate();
  skia::DefaultFontMgr().get()->InvalidateThemeFont(fds);
  blink::FontCache::Get().InvalidateSystemFontFamily();
}
#endif
// LCOV_EXCL_STOP

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

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void RenderThreadImpl::SetBlanklessDumpInfo(uint32_t nweb_id, uint64_t blankless_key,
                                            uint64_t frame_sink_id, int32_t lcp_time, int64_t pref_hash) {
  if (GetGpuChannel() && GetGpuChannel()->gpu_channel_host_utils()) {
    GetGpuChannel()->gpu_channel_host_utils()->SetBlanklessDumpInfo(nweb_id,
                                                                    blankless_key,
                                                                    frame_sink_id,
                                                                    lcp_time,
                                                                    pref_hash);
  }
}
#endif

#if BUILDFLAG(ARKWEB_LOGGER_REPORT)
void RenderThreadImpl::OnChannelListenError() {
  if ((*base::CommandLine::ForCurrentProcess())
          .HasSwitch(switches::kEnableLoggerReport)) {
    if (logging::GetLogMessageHandler()) {
      logging::SetLogMessageHandler(nullptr);
      LOG_FEEDBACK(INFO) << "remove log message handler for "
                         << base::GetCurrentProcId();
    }
  }

  ChildThreadImpl::OnChannelListenError();
}

void RenderThreadImpl::OnChannelConnected(int32_t peer_pid) {
  if ((*base::CommandLine::ForCurrentProcess())
          .HasSwitch(switches::kEnableLoggerReport)) {
    if (!logging::GetLogMessageHandler()) {
      logging::SetLogMessageHandler(RenderProcessLogMessageHandler);
    } else {
      LOG_FEEDBACK(INFO)
          << "maybe you runs in single process mode, log message handler "
             "had been setted by other";
    }
  }

  ChildThreadImpl::OnChannelConnected(peer_pid);
}
#endif
// LCOV_EXCL_STOP


#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
void RenderThreadImpl::UpdateVideoLoadOptimizationConfigData(
    const bool enable,
    const int preload_video_time,
    const int min_cache_time,
    const int max_cache_time,
    const int moov_size,
    const int bit_rate,
    const std::vector<std::string>& support_domains) {
  LOG(INFO) << "VideoOpt: global cloudControl enable:" << enable
            << ", preload_video_time:" << preload_video_time
            << ", min_cache_time:" << min_cache_time
            << ", max_cache_time:" << max_cache_time
            << ", moov_size:" << moov_size
            << ", bit_rate:" << bit_rate;
  std::lock_guard<std::mutex> cloudConfigMutexLock(cloud_control_config_mutex);
  video_load_opt_enable_ = enable;
  preload_video_time_ = preload_video_time;
  min_cache_time_ = min_cache_time;
  max_cache_time_ = max_cache_time;
  moov_size_ = moov_size;
  bit_rate_ = bit_rate;
  support_domains_ = support_domains;
}

bool RenderThreadImpl::IsVideoLoadOptimizationEnabled(const std::string& url) const {
  std::lock_guard<std::mutex> cloudConfigMutexLock(cloud_control_config_mutex);
  if (!video_load_opt_enable_) {
    LOG(INFO) << "VideoOpt: cloudControl VLO GlobalEnabled is false";
    return false;
  }
  if (!IsVideoLoadOptSupportDomainMatch(url)) {
    LOG(INFO) << "VideoOpt: cloudControl VLO LocalEnabled is false";
    return false;
  }
  return true;
}

int RenderThreadImpl::GetVideoPreloadTimeDefault() const {
  return preload_video_time_;
}

int RenderThreadImpl::GetVideoMinCacheTimeDefault() const {
  return min_cache_time_;
}

int RenderThreadImpl::GetVideoMaxCacheTimeDefault() const {
  return max_cache_time_;
}

int RenderThreadImpl::GetVideoMoovSizeDefault() const {
  return moov_size_;
}

int RenderThreadImpl::GetVideoBitrateDefault() const {
  return bit_rate_;
}

bool RenderThreadImpl::IsVideoLoadOptSupportDomainMatch(const std::string& url) const {
  // Exact match
  auto exact_iter = std::find(support_domains_.begin(), support_domains_.end(), url);
  if (exact_iter != support_domains_.end()) {
    LOG(INFO) << "VideoOpt: IsVideoLoadOptSupportDomain exact match url";
    return true;
  }

  WTF::String url_string(url.c_str());
  const blink::KURL kurl(url_string);
  for (auto& domain : support_domains_) {
    WTF::String url_domain_string(domain.c_str());
    const blink::KURL domain_kurl(url_domain_string);
    if (blink::SecurityOrigin::AreSameOrigin(kurl, domain_kurl)) {
      LOG(INFO) << "VideoOpt: IsVideoLoadOptSupportDomain regular expression match url";
      return true;
    }
  }

  return false;
}

#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_TEST)
  const base::TimeDelta& GetMaxIPCLoginInterval() {
    return kMaxIPCLoginInterval;
  }
  
  base::TimeTicks& GetLastRendererLogSentTime(int policy) {
    return g_last_renderer_log_sent_time[policy];
  }
  
  void SetLastRendererLogSentTime(int policy, const base::TimeTicks& time) {
    g_last_renderer_log_sent_time[policy] = time;
  }
  
  bool& GetHasRendererLogDropped(int policy) {
    return g_has_renderer_log_dropped[policy];
  }

  void SetHasRendererLogDropped(int policy, bool value) {
    g_has_renderer_log_dropped[policy] = value;
  }

  void CallReportRendererLogOnMainThread(int policy, const std::string& msg) {
    ReportRendererLogOnMainThread(policy, msg);
  }
  
  bool CallHandleFatalMessageForCrashpad(int severity,
                                               const char* file,
                                               int line,
                                               size_t message_start,
                                               const std::string& string) {
    return HandleFatalMessageForCrashpad(severity, file, line, message_start, string);
  }
  
  bool CallRenderProcessLogMessageHandler(int severity,
                                                const char* file,
                                                int line,
                                                size_t message_start,
                                                const std::string& str) {
    return RenderProcessLogMessageHandler(severity, file, line, message_start, str);
  }
#endif

}  // namespace content

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

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "arkweb/chromium_ext/third_party/blink/public/mojom/dom_distiller/reader_mode_config.mojom.h"
#include "arkweb/ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config_utils.h"

using blink::mojom::ContentFeatureIndex;
using blink::mojom::ContentTemplateIndex;
using blink::mojom::DetailTemplateIndex;
using blink::mojom::ReaderModeConfig;
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
#include "arkweb/ohos_nweb/src/sysevent/event_reporter.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
#include "base/functional/bind.h"
#include "base/task/single_thread_task_runner.h"
#endif

namespace content {

#if BUILDFLAG(ARKWEB_READER_MODE)
constexpr int kDetailTemplateCatalogIndex =
    static_cast<int>(DetailTemplateIndex::CHAPTERS);
constexpr int kContentTemplateChapterIndex =
    static_cast<int>(ContentTemplateIndex::PAGER);
constexpr int kFeaturePrevIndex = static_cast<int>(ContentFeatureIndex::PREV);
constexpr int kFeatureNextIndex = static_cast<int>(ContentFeatureIndex::NEXT);
constexpr int kFeatureCatalogIndex =
    static_cast<int>(ContentFeatureIndex::CATALOG);
#endif

// LCOV_EXCL_START
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

void ArkwebRenderProcessHostImplExt::RenderProcessChannelConnectCheck() {
  LOG(INFO) << "RenderProcessHost: " << GetProcess().Handle()
            << " check channel connect status";
  if (is_dead() || !GetProcess().Handle()) {
    LOG(ERROR) << "RenderProcessHost channel connect host: " << GetProcess().Handle()
               << " is dead";
    return;
  }
  if (!IsReady()) {
    LOG(WARNING) << "RenderProcessHost channel connect timeout(6s), terminate process: "
                 << GetProcess().Handle();
#if BUILDFLAG(ARKWEB_RENDER_PROCESS_STARTUP) && !defined(COMPONENT_BUILD)
    ReportRenderProcessTerminate(false, GetProcess().Handle(),
      std::string("TERMINATION_CHANNEL_CONNECT_FAILED"), 0);
#endif
    internal::ChildProcessLauncherHelper::TerminateProcess(GetProcess(), 0);
  }
}

void ArkwebRenderProcessHostImplExt::StartChannelConnectedCheckTask(ArkwebRenderProcessHostImplExt* host) {
  if (host->channel_connected_check_callback_.callback().is_null()) {
    LOG(WARNING) << "RenderProcessHost: " << host->GetProcess().Handle()
                 << " Wait 6-second to monitoring channel connection status.";
    host->channel_connected_check_callback_.Reset(
      base::BindOnce(&ArkwebRenderProcessHostImplExt::RenderProcessChannelConnectCheck,
                     host->instance_weak_factory_.GetWeakPtr()));
    /* If the render process and the main process are killed before establishing IPC,
     * the main process cannot detect the death of the render process. This would cause
     * RenderProcessHostImpl to mistakenly assume that it still holds the connection,
     * leading to incorrect reuse. Therefore, a 6-second check is set to verify whether
     * the IPC channel has been established; if not, the process is terminated and an
     * onRenderProcessExited callback is reported.
     */
    GetUIThreadTaskRunner({})->PostDelayedTask(
      FROM_HERE, host->channel_connected_check_callback_.callback(), base::Seconds(6));
  } else if (host->channel_connected_check_callback_.IsCancelled()) {
    LOG(ERROR) << "RenderProcessHost: " << host->GetProcess().Handle()
               << " channel connect check is cancelled.";
  }
}

void ArkwebRenderProcessHostImplExt::CancelChannelConnectedCheckTask(ArkwebRenderProcessHostImplExt* host) {
  if (host->channel_connected_check_callback_.callback().is_null() ||
      host->channel_connected_check_callback_.IsCancelled()) {
    return;
  }
  LOG(INFO) << "RenderProcessHost: " << host->GetProcess().Handle()
            << " cancel channel connected check task";
  host->channel_connected_check_callback_.Cancel();
}
#endif  // ARKWEB_RENDER_PROCESS_MODE

#if BUILDFLAG(ARKWEB_THEME_FONT)
void ArkwebRenderProcessHostImplExt::OnThemeFontChange() {
  if (auto* theme_font = ArkwebRenderProcessHostImplUtils::EnsureThemeFont()) {
    std::vector<base::File> font_files(theme_font->font_files.size());
    std::transform(theme_font->font_files.begin(), theme_font->font_files.end(), font_files.begin(),
          [] (const base::File& f) { return f.Duplicate(); });
    ArkwebRenderProcessHostImplUtils::UpdateThemeFontFile(
        this, std::move(font_files));
  } else {
    ArkwebRenderProcessHostImplUtils::UpdateThemeFontFile(this, std::vector<base::File>());
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

std::string GetProcessName() {
  std::ifstream input_file("/proc/self/cmdline");
  if (!input_file.is_open()) {
    LOG(ERROR) << "Error: Could not open /proc/self/cmdline";
    return "";
  }

  std::string processName = "";
  if (!std::getline(input_file, processName)) {
    LOG(ERROR) << "Error: Failed to read process name from /proc/self/cmdline";
  }
  return processName;
}

void OnUidRetrieved(int32_t pid, int32_t uid) {
#if !defined(COMPONENT_BUILD)
  ReportRenderJsFreeze(
    pid,
    OHOS::NWeb::OhosAdapterHelper::GetInstance().GetSystemPropertiesInstance().GetBundleName(),
    GetProcessName() + ":render",
    "render unresponsive",
    uid
  );
#endif
}

void ArkwebRenderProcessHostImplExt::ReportRenderUnresponsive(int32_t pid) {
  child_process_->GetUid(base::BindOnce(&OnUidRetrieved, pid));
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
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_READER_MODE)
void ArkwebRenderProcessHostImplExt::UpdateCloudControlReaderModeConfigData(
    const nweb_ex::BrowserReaderModeConfigData* data) {
  content::RenderProcessHost::iterator it =
      content::RenderProcessHost::AllHostsIterator();
  while (!it.IsAtEnd()) {
    content::RenderProcessHost* host = it.GetCurrentValue();
    if (host && host->IsInitializedAndNotDead()) {
      host->UpdateReaderModeConfig(data);
    }
    it.Advance();
  }
}

void ArkwebRenderProcessHostImplExt::UpdateReaderModeConfig(
    const nweb_ex::BrowserReaderModeConfigData* reader_mode_config_data) {
  if (reader_mode_config_data == nullptr) {
    LOG(WARNING) << "UpdateReaderModeConfig param reader_mode_config_data is null";
    return;
  }
  auto* renderer_interface = GetRendererInterface();
  if (!renderer_interface) {
    LOG(WARNING) << "UpdateReaderModeConfig interface is null";
    return;
  }

  auto config = blink::mojom::ReaderModeConfig::New();
  config->reader_mode_enabled = reader_mode_config_data->enabled;

  // 详情模板
  for (const auto& item : reader_mode_config_data->detail_config.templates) {
    if (item.templateConfig.size() > kDetailTemplateCatalogIndex) {
      config->detail_templates.emplace(item.templateId, item.templateConfig);
    } else {
      LOG(WARNING) << "UpdateReaderModeConfig detail template must 3 items[info, intro, chapter]";
    }
  }
  // 正文模板
  for (const auto& item :
       reader_mode_config_data->content_config.template_match) {
    if (item.template_config.size() > kContentTemplateChapterIndex) {
      config->content_templates.emplace(item.template_id, item.template_config);
    } else {
      LOG(WARNING) << "UpdateReaderModeConfig content template must 2 items[body, chapter]";
    }
  }
  // 正文特征
  if (reader_mode_config_data->content_config.feature_match.size() >
      kFeatureCatalogIndex) {
    const std::vector<std::vector<std::string>>& features =
        reader_mode_config_data->content_config.feature_match;
    config->content_features.emplace(blink::mojom::ContentFeatureIndex::PREV, features[kFeaturePrevIndex]);
    config->content_features.emplace(blink::mojom::ContentFeatureIndex::NEXT, features[kFeatureNextIndex]);
    config->content_features.emplace(blink::mojom::ContentFeatureIndex::CATALOG, features[kFeatureCatalogIndex]);
  } else {
    LOG(WARNING) << "UpdateReaderModeConfig features must 3 items[pre, next, catalog]";
  }
  // 正文元数据
  const nweb_ex::BrowserReaderModeContentMetaDataConfig& metaData = reader_mode_config_data->content_config.meta_data;
  config->must_have_catalog = metaData.must_have_catalog;
  config->must_have_prev_and_next = metaData.must_have_prev_and_next;
  config->minimum_content_length = metaData.minimum_content_length < 0 ? 0 : metaData.minimum_content_length;

  LOG(INFO) << "RenderProcessHostImpl::UpdateReaderModeConfig config enable " << config->reader_mode_enabled
            << " must_have_catalog:" << config->must_have_catalog
            << " must_have_prev_and_next:" << config->must_have_prev_and_next
            << " minimum_content_length:" << config->minimum_content_length
            << " detail_templates size:" << config->detail_templates.size()
            << " content_templates size:" << config->content_templates.size()
            << " content_features size:" << config->content_features.size();

  renderer_interface->UpdateReaderModeConfig(std::move(config));
}
#endif  // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
void ArkwebRenderProcessHostImplExt::UpdateVideoLoadOptimizationConfigData(
    nweb_ex::AlloyVideoLoadOptimizationData& data) {
  content::RenderProcessHost::iterator it =
      content::RenderProcessHost::AllHostsIterator();
  while (!it.IsAtEnd()) {
    content::RenderProcessHost* host = it.GetCurrentValue();
    if (host && host->IsInitializedAndNotDead()) {
      host->UpdateVideoLoadOptimizationConfig(data);
    }
    it.Advance();
  }
}

void ArkwebRenderProcessHostImplExt::UpdateVideoLoadOptimizationConfig(
    nweb_ex::AlloyVideoLoadOptimizationData& data) {
  auto* renderer_interface = GetRendererInterface();
  if (!renderer_interface) {
    LOG(WARNING) << "UpdateVideoLoadOptimizationConfig interface is null";
    return;
  }
  LOG(INFO) << "VideoOpt: UpdateVideoLoadOptimizationConfig";
  renderer_interface->UpdateVideoLoadOptimizationConfigData(data.use_video_load_optimization_,
      data.preload_video_time_, data.min_cache_time_,
      data.max_cache_time_, data.moov_size_, data.bit_rate_, data.support_domains_);
}
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

}  // namespace content

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

#ifndef ARKWEB_RENDER_PROCESS_HOST_IMPL_EXT_H_
#define ARKWEB_RENDER_PROCESS_HOST_IMPL_EXT_H_

#include "content/browser/renderer_host/render_process_host_impl.h"
#include "content/common/child_process.mojom.h"
#include "content/common/content_export.h"
#include "content/public/browser/browser_context.h"

#if BUILDFLAG(ARKWEB_READER_MODE)
#include "arkweb/chromium_ext/base/arkweb_report_statistics.h"
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config_utils.h"
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_browser_reader_mode_config.h"
#endif

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
#include "ohos_nweb_ex/overrides/cef/libcef/browser/alloy/alloy_video_load_optimization_config.h"
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
#include "base/functional/callback.h"
#endif

namespace content {

class RenderProcessHostImpl;

class CONTENT_EXPORT ArkwebRenderProcessHostImplExt : public RenderProcessHostImpl {
 public:
  ArkwebRenderProcessHostImplExt(BrowserContext* browser_context,
                                 StoragePartitionImpl* storage_partition_impl,
                                 int flags);

  ArkwebRenderProcessHostImplExt* AsArkwebRenderProcessHostImplExt() override {
    return this;
  }

#if BUILDFLAG(ARKWEB_RENDER_PROCESS_MODE)
  bool IsProcessBackgrounded() override;

  const base::TimeTicks& ProcessBackgroundTime() override;

  void RenderProcessChannelConnectCheck();
  static void StartChannelConnectedCheckTask(ArkwebRenderProcessHostImplExt* host);
  static void CancelChannelConnectedCheckTask(ArkwebRenderProcessHostImplExt* host);

  base::CancelableOnceCallback<void()> channel_connected_check_callback_;
#endif

#if BUILDFLAG(ARKWEB_THEME_FONT)
  void OnThemeFontChange() override;
#endif

#if BUILDFLAG(ARKWEB_RENDERER_ANR_DUMP)
  void dumpCurrentJavaScriptStackInMainThread(
      base::OnceCallback<void(const std::string&)> dump_callback) override;

  void InvokeRenderCrashDump() override;

  void ReportRenderUnresponsive(int32_t pid) override;
#endif

#if BUILDFLAG(IS_ARKWEB)
  void ReportEngineEvent(const std::string& module,
                         const std::string& resource,
                         const std::string& error_code,
                         const std::string& error_msg) override;
#endif

#if BUILDFLAG(ARKWEB_READER_MODE)
  static void UpdateCloudControlReaderModeConfigData(
      const nweb_ex::BrowserReaderModeConfigData* data);
  void UpdateReaderModeConfig(
      const nweb_ex::BrowserReaderModeConfigData* data) override;
  void SetMojomReaderModeConfigV1(
      blink::mojom::ReaderModeConfig* config,
      const nweb_ex::BrowserReaderModeConfigData* config_data);
  void SetMojomReaderModeConfigV2(
      blink::mojom::ReaderModeConfig* config,
      const nweb_ex::BrowserReaderModeConfigData* config_data);
  void ReportDistillableResult(
      const std::string& event_type,
      const std::string& value) override;
#endif  // ARKWEB_READER_MODE

#if BUILDFLAG(ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION)
  static void UpdateVideoLoadOptimizationConfigData(nweb_ex::AlloyVideoLoadOptimizationData& data);
  void UpdateVideoLoadOptimizationConfig(nweb_ex::AlloyVideoLoadOptimizationData& data) override;
#endif // ARKWEB_EXT_VIDEO_LOAD_OPTIMIZATION

 private:
  base::WeakPtrFactory<ArkwebRenderProcessHostImplExt> instance_weak_factory_{this};
};

}  // namespace content

#endif  // ARKWEB_RENDER_PROCESS_HOST_IMPL_EXT_H_

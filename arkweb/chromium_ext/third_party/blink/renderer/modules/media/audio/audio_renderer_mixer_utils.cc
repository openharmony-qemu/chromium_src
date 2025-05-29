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

#include "arkweb/chromium_ext/base/process/process_handle_posix_ex.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/modules/media/audio/audio_renderer_mixer_utils.h"
#include "third_party/blink/renderer/modules/media/audio/audio_renderer_mixer.h"
#include "base/process/process_handle.h"
#include "base/threading/platform_thread.h"

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#endif

#if BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)
#include "base/command_line.h"
#include "content/public/common/content_switches.h"
#include "third_party/blink/renderer/core/render_mojom/render_mojom_client.h"
#endif  // BUILDFLAG(ARKWEB_RENDER_REMOVE_BINDER)

namespace blink {

AudioRendererMixerUtils::AudioRendererMixerUtils(AudioRendererMixer* impl) {
    this->audioRendererMixer = impl;
}

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void AudioRendererMixerUtils::AudioRendererMixerShareInit(int media_tid) {
  // Here add a condition, when it is called by render, transfer the data to browser by mojom first
  // as render can execute script from outside.
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      switches::kProcessType);
  if (type == switches::kRendererProcess) {
    blink::ResSchedReportClient report_client(base::GetCurrentRealPid());
    report_client.SendAudioData(0, base::GetCurrentRealPid(), media_tid);
  } else {
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_START,
        base::GetCurrentRealPid(), media_tid);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void AudioRendererMixerUtils::AudioRendererMixerShareDestroy(int audio_output_tid, int media_tid) {
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      switches::kProcessType);
  if (type == switches::kRendererProcess) {
    blink::ResSchedReportClient report_client(base::GetCurrentRealPid());
    report_client.SendAudioData(1, base::GetCurrentRealPid(), audio_output_tid);
    report_client.SendAudioData(1, base::GetCurrentRealPid(), media_tid);
  } else {
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_STOP,
        base::GetCurrentRealPid(), audio_output_tid);
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_STOP,
        base::GetCurrentRealPid(), media_tid);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void AudioRendererMixerUtils::AddMixerInputShareInit(int audio_output_tid, int media_tid) {
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      switches::kProcessType);
  if (type == switches::kRendererProcess) {
    blink::ResSchedReportClient report_client(base::GetCurrentRealPid());
    report_client.SendAudioData(1, base::GetCurrentRealPid(), audio_output_tid);
    report_client.SendAudioData(1, base::GetCurrentRealPid(), media_tid);
  } else {
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_STOP,
        base::GetCurrentRealPid(), audio_output_tid);
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_STOP,
        base::GetCurrentRealPid(), media_tid);
  }
}
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
void AudioRendererMixerUtils::AddMixerInputShareRender(int audio_output_tid, int media_tid) {
  auto type = base::CommandLine::ForCurrentProcess()->GetSwitchValueASCII(
      switches::kProcessType);
  if (type == switches::kRendererProcess) {
    blink::ResSchedReportClient report_client(base::GetCurrentRealPid());
    report_client.SendAudioData(1, base::GetCurrentRealPid(), audio_output_tid);
    report_client.SendAudioData(1, base::GetCurrentRealPid(), media_tid);
  } else {
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_STOP,
        base::GetCurrentRealPid(), audio_output_tid);
    OHOS::NWeb::ResSchedClientAdapter::ReportAudioData(
        OHOS::NWeb::ResSchedStatusAdapter::AUDIO_STATUS_STOP,
        base::GetCurrentRealPid(), media_tid);
  }
}
#endif

}
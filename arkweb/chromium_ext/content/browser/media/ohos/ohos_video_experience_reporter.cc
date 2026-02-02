/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "content/browser/media/ohos/ohos_video_experience_reporter.h"

#include <sstream>
#include <string>
#include "base/logging.h"
#include "base/json/json_writer.h"
#include "base/values.h"
#include "url/gurl.h"
#include "arkweb/chromium_ext/url/ohos/log_utils.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "arkweb/chromium_ext/base/arkweb_report_statistics.h"

namespace content {

namespace {

constexpr char kVideoExperienceReportEventType[] = "video_play_statistic";
constexpr char kStartUsedTime[] = "start_used_time";
constexpr char kTotalFreezeTime[] = "total_freeze_time";
constexpr char kTotalPlayedTime[] = "total_played_time";
constexpr char kVideoFreezeCnt[] = "video_freeze_cnt";

constexpr char kErrorCode[] = "error_code";
constexpr char kErrorMsg[] = "error_msg";
constexpr char kDomain[] = "domain";
constexpr char kUrl[] = "url";
constexpr char kVideoSrc[] = "video_src";

constexpr char kPipelineStatus[] = "pipeline_status";
constexpr char kMimeType[] = "mime_type";
constexpr char kNetErrorCode[] = "net_error_code";
constexpr char kVideoDuration[] = "video_duration";

constexpr char kVideoWidth[] = "video_width";
constexpr char kVideoHeight[] = "video_height";
constexpr char kVideoPlayer[] = "video_player";

} // namespace

// static
void OHOSVideoExperienceReporter::CreateForFrameHost(
    content::RenderFrameHost* rfh,
    mojo::PendingReceiver<blink::mojom::VideoExperienceReporter> receiver) {
  if (rfh) {
    mojo::MakeSelfOwnedReceiver(
        std::unique_ptr<OHOSVideoExperienceReporter>(new OHOSVideoExperienceReporter(rfh)),
        std::move(receiver));
  }
}

OHOSVideoExperienceReporter::OHOSVideoExperienceReporter(
    content::RenderFrameHost* rfh)
    : web_contents_(content::WebContents::FromRenderFrameHost(rfh)) {
  Observe(web_contents_.get());
}

OHOSVideoExperienceReporter::~OHOSVideoExperienceReporter() = default;

void OHOSVideoExperienceReporter::ReportVideoExperienceToBI(
    blink::mojom::VideoExpParamsPtr params) {
  if (!params) {
    return;
  }

  base::Value::Dict record;
  record.Set(kStartUsedTime, base::NumberToString(params->start_used_time));
  record.Set(kTotalFreezeTime, base::NumberToString(params->total_freeze_time));
  record.Set(kTotalPlayedTime, base::NumberToString(params->total_played_time));
  record.Set(kVideoFreezeCnt, base::NumberToString(params->video_freeze_cnt));

  record.Set(kErrorCode, base::NumberToString(params->error_code));
  record.Set(kErrorMsg, params->error_msg);
  record.Set(kUrl, url::LogUtils::ConvertUrlParamWithMask(params->page_url));
  record.Set(kVideoSrc, url::LogUtils::ConvertUrlParamWithMask(params->video_src));
  GURL urlDomain(params->page_url);
  record.Set(kDomain, urlDomain.host());

  record.Set(kPipelineStatus, params->pipeline_status);
  record.Set(kMimeType, params->mime_type);
  record.Set(kNetErrorCode, base::NumberToString(params->net_error_code));
  record.Set(kVideoDuration, base::NumberToString(params->video_duration));

  record.Set(kVideoWidth, base::NumberToString(params->video_width));
  record.Set(kVideoHeight, base::NumberToString(params->video_height));
  record.Set(kVideoPlayer, params->video_player);

  auto json = base::WriteJson(record);
  if (json) {
    base::ohos::OperationStatistics::Statistics(
        base::ohos::REGION_CHINA, base::ohos::PLATFORM_OPERATION_ANALYSIS,
        base::ohos::OperationStatistics::GROUP_BECE,
        kVideoExperienceReportEventType, 
        base::ohos::OperationStatistics::DEFAULT_DATA_VERSION, json.value(), 
        true, false, true, base::ohos::REPORT_DAILY);
  }
}

void OHOSVideoExperienceReporter::BindChannel(
  mojo::PendingReceiver<blink::mojom::VideoExperienceReporter> pendingReceive) {
  receiver_.Bind(std::move(pendingReceive));
}

void OHOSVideoExperienceReporter::WebContentsDestroyed() {
  web_contents_ = nullptr;
}

} // namespace content
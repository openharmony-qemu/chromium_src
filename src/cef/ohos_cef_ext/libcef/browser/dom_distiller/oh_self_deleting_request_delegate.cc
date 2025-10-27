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

#include "libcef/browser/dom_distiller/oh_self_deleting_request_delegate.h"

#include <utility>

#include "base/json/json_writer.h"
#include "base/strings/strcat.h"
#include "build/build_config.h"
#include "components/dom_distiller/core/task_tracker.h"
#include "components/dom_distiller/core/viewer.h"
#include "libcef/browser/dom_distiller/oh_distiller_utils.h"

namespace oh_dom_distiller {

OhSelfDeletingRequestDelegate::OhSelfDeletingRequestDelegate(
    DistillResultCallback callback,
    GURL url,
    std::string guid)
    : distill_result_callback_(std::move(callback)),
      request_url_(url),
      guid_(guid) {}

OhSelfDeletingRequestDelegate::~OhSelfDeletingRequestDelegate() = default;

void OhSelfDeletingRequestDelegate::OnArticleReady(
    const DistilledArticleProto* article_proto) {
  if (distill_result_callback_.is_null()) {
    LOG(ERROR) << __func__ << " [Distiller] DistillResult, callback is null";
    return;
  }
  LOG(INFO) << __func__ << " [Distiller]";
  auto json_data =
      FormatDistilledArticleProtoToJsonData(article_proto, GetRequestURL());

  std::move(distill_result_callback_).Run(*json_data);

  // No need to hold on to the ViewerHandle now that distillation is complete.
  viewer_handle_.reset();
}

void OhSelfDeletingRequestDelegate::OnArticleUpdated(
    ArticleDistillationUpdate article_update) {}

void OhSelfDeletingRequestDelegate::OnArticleAborted() {
  if (!distill_result_callback_.is_null()) {
    base::Value::Dict json;
    json.Set(kResultCode, kDistillCancelledErrorCode);
    json.Set(kResultMessage, kErrorMsgDistillCancelled);
    std::string json_data;
    base::JSONWriter::Write(std::move(json), &json_data);
    LOG(INFO) << __func__ << " [Distiller]";
    std::move(distill_result_callback_).Run(json_data);
  }
  viewer_handle_.reset();
}

void OhSelfDeletingRequestDelegate::TakeViewerHandle(
    std::unique_ptr<ViewerHandle> viewer_handle) {
  viewer_handle_ = std::move(viewer_handle);
}

} // namespace oh_dom_distiller
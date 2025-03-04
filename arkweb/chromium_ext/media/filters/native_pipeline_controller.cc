/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "media/filters/native_pipeline_controller.h"

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"

namespace media {

NativePipelineController::NativePipelineController(
    std::unique_ptr<NativePipeline> native_pipeline)
    : native_pipeline_(std::move(native_pipeline)) {
  DCHECK(native_pipeline_);
}

NativePipelineController::~NativePipelineController() {
  DCHECK(thread_checker_.CalledOnValidThread());
}

void NativePipelineController::Start(NativePipeline::Client* client,
                                     CreateTextureCB create_texture_cb,
                                     DestroyTextureCB destroy_texture_cb) {
  DCHECK(thread_checker_.CalledOnValidThread());

  native_pipeline_->Start(client, std::move(create_texture_cb),
                          std::move(destroy_texture_cb));
}

void NativePipelineController::Stop() {
  native_pipeline_->Stop();
  if (native_pipeline_) {
    native_pipeline_.reset();
  }
}

}  // namespace media
                     
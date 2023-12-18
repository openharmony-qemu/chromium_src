/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
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
  DCHECK_EQ(state_, State::STOPPED);

  state_ = State::STARTING;

  native_pipeline_->Start(client, std::move(create_texture_cb),
                          std::move(destroy_texture_cb));
}

void NativePipelineController::Stop() {
  if (state_ == State::STOPPED)
    return;

  state_ = State::STOPPED;

  // native_pipeline_->Stop();
}

}  // namespace media
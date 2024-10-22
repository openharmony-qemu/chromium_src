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

#ifndef MEDIA_FILTERS_NATIVE_PIPELINE_CONTROLLER_H_
#define MEDIA_FILTERS_NATIVE_PIPELINE_CONTROLLER_H_

#include "base/functional/callback.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/base/media_export.h"
#include "media/base/native_pipeline.h"

namespace media {

class MEDIA_EXPORT NativePipelineController {
 public:
  NativePipelineController(std::unique_ptr<NativePipeline> native_pipeline);

  NativePipelineController(const NativePipelineController&) = delete;
  NativePipelineController& operator=(const NativePipelineController&) = delete;

  ~NativePipelineController();

  void Start(NativePipeline::Client* client,
             CreateTextureCB create_texture_cb,
             DestroyTextureCB destroy_texture_cb);

  void Stop();

 private:
  // The NativePipeline we are managing state for.
  std::unique_ptr<NativePipeline> native_pipeline_;

  base::ThreadChecker thread_checker_;
  base::WeakPtrFactory<NativePipelineController> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_FILTERS_NATIVE_PIPELINE_CONTROLLER_H_
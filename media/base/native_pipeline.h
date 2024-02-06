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

#ifndef MEDIA_BASE_NATIVE_PIPELINE_H_
#define MEDIA_BASE_NATIVE_PIPELINE_H_

#include <memory>

#include "base/memory/raw_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/base/media_export.h"
#include "media/base/pipeline.h"
#include "media/base/renderer.h"
#include "media/base/renderer_factory_selector.h"
#include "third_party/abseil-cpp/absl/types/optional.h"

namespace base {
class SingleThreadTaskRunner;
}

namespace media {

// Callbacks used for Renderer creation. When the RendererType is nullopt, the
// current base one will be created.
using CreateRendererCB = base::RepeatingCallback<std::unique_ptr<Renderer>(
    absl::optional<RendererType>)>;
using RendererCreatedCB = base::OnceCallback<void(std::unique_ptr<Renderer>)>;
using AsyncCreateRendererCB =
    base::RepeatingCallback<void(absl::optional<RendererType>,
                                 RendererCreatedCB)>;

class MEDIA_EXPORT NativePipeline {
 public:
  class Client {
   public:
    virtual void OnSetLayer() = 0;
  };

  virtual ~NativePipeline() {}

  // Build a pipeline to using the given |demuxer| to construct a filter chain,
  // executing |seek_cb| when the initial seek has completed. Methods on
  // PipelineClient may be called up until Stop() has completed. It is an error
  // to call this method after the pipeline has already started.
  //
  // If a |start_type| is specified which allows suspension, pipeline startup
  // will halt after metadata has been retrieved and the pipeline will be in a
  // suspended state.
  virtual void Start(Client* client,
                     CreateTextureCB create_texture_cb,
                     DestroyTextureCB destroy_texture_cb) = 0;

  // Returns true if the pipeline has been started via Start().  If IsRunning()
  // returns true, it is expected that Stop() will be called before destroying
  // the pipeline.
  virtual bool IsRunning() const = 0;
  virtual void Stop() = 0;
};

}  // namespace media

#endif  // MEDIA_BASE_NATIVE_PIPELINE_H_

// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on pipeline_impl.h originally written by
// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 
 
#ifndef MEDIA_BASE_NATIVE_PIPELINE_IMPL_H_
#define MEDIA_BASE_NATIVE_PIPELINE_IMPL_H_

#include <memory>

#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/base/media_export.h"
#include "media/base/native_pipeline.h"
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

class MEDIA_EXPORT NativePipelineImpl : public NativePipeline {
 public:
  // Constructs a pipeline that will execute media tasks on |media_task_runner|.
  // |create_renderer_cb|: to create renderers when starting and resuming.
  NativePipelineImpl(
      scoped_refptr<base::SequencedTaskRunner> media_task_runner,
      scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
      CreateRendererCB create_renderer_cb);

  NativePipelineImpl(const NativePipelineImpl&) = delete;
  NativePipelineImpl& operator=(const NativePipelineImpl&) = delete;

  ~NativePipelineImpl() override;

  // NativePipeline implementation.
  void Start(Client* client,
             CreateTextureCB create_texture_cb,
             DestroyTextureCB destroy_texture_cb) override;

  bool IsRunning() const override;

  void Stop() override;

 private:
  class RendererWrapper;

  void OnSetLayer();

  // Create a Renderer asynchronously. Must be called on the main task runner
  // and the callback will be called on the main task runner as well.
  void AsyncCreateRenderer(absl::optional<RendererType> renderer_type,
                           RendererCreatedCB renderer_created_cb);

  // Parameters passed in the constructor.
  const scoped_refptr<base::SequencedTaskRunner> media_task_runner_;
  CreateRendererCB create_renderer_cb_;

  // Pipeline client. Valid only while the pipeline is running.
  raw_ptr<Client> client_;

  // RendererWrapper instance that runs on the media thread.
  std::unique_ptr<RendererWrapper> renderer_wrapper_;

  base::ThreadChecker thread_checker_;
  base::WeakPtrFactory<NativePipelineImpl> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_BASE_NATIVE_PIPELINE_IMPL_H_

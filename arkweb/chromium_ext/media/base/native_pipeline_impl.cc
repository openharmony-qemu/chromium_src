// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on pipeline_impl.cc originally written by
// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 
 
#include "media/base/native_pipeline_impl.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/logging.h"
#include "base/task/bind_post_task.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/renderer.h"
#include "media/base/renderer_client.h"
#include "media/base/serial_runner.h"

namespace media {

// A wrapper of Renderer that runs on the |media_task_runner|.
// |default_renderer| in Start() and Resume() helps avoid a round trip to the
// render main task runner for Renderer creation in most cases which could add
// latency to start-to-play time.
class NativePipelineImpl::RendererWrapper {
 public:
  RendererWrapper(scoped_refptr<base::SequencedTaskRunner> media_task_runner,
                  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner);

  RendererWrapper(const RendererWrapper&) = delete;
  RendererWrapper& operator=(const RendererWrapper&) = delete;

  virtual ~RendererWrapper();

  void Start(CreateTextureCB create_texture_cb,
             DestroyTextureCB destroy_texture_cb,
             base::WeakPtr<NativePipelineImpl> weak_pipeline);
  void Stop();

 private:
  // State transition tasks.
  void Complete(PipelineStatus status);

  void CreateRenderer(PipelineStatusCallback done_cb);
  void OnRendererCreated(PipelineStatusCallback done_cb,
                         std::unique_ptr<Renderer> renderer);
  void InitializeRenderer(PipelineStatusCallback done_cb);
  void DestroyRenderer();
  void ReportMetadata();

  // Uses |default_renderer_| as the Renderer or asynchronously creates a new
  // one by calling back to NativePipelineImpl. Fires |done_cb| with the result.
  void CreateRendererInternal(PipelineStatusCallback done_cb);

  const scoped_refptr<base::SequencedTaskRunner> media_task_runner_;
  const scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;

  // A weak pointer to NativePipelineImpl. Must only use on the main task
  // runner.
  base::WeakPtr<NativePipelineImpl> weak_pipeline_;

  // Optional default renderer to be used during Start() and Resume(). If not
  // available, or if a different Renderer is needed,
  // NativePipelineImpl::AsyncCreateRenderer() will be called to create a new
  // one.
  std::unique_ptr<Renderer> default_renderer_;

  // Lock used to serialize |shared_state_|.
  // TODO(crbug.com/893739): Add GUARDED_BY annotations.
  mutable base::Lock shared_state_lock_;

  std::unique_ptr<Renderer> shared_state_renderer_;

  std::unique_ptr<SerialRunner> pending_callbacks_;

  // Callback to store the |done_cb| when CreateRenderer() needs to wait for a
  // CDM to be set. Should only be set in kStarting or kResuming states.
  PipelineStatusCallback create_renderer_done_cb_;

  // Called from non-media threads when an error occurs.
  PipelineStatusCB error_cb_;
  CreateTextureCB create_texture_cb_;
  DestroyTextureCB destroy_texture_cb_;

  base::WeakPtrFactory<RendererWrapper> weak_factory_{this};
};

NativePipelineImpl::RendererWrapper::RendererWrapper(
    scoped_refptr<base::SequencedTaskRunner> media_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner)
    : media_task_runner_(std::move(media_task_runner)),
      main_task_runner_(std::move(main_task_runner)) {}

NativePipelineImpl::RendererWrapper::~RendererWrapper() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
}

// Note that the usage of base::Unretained() with the renderers is considered
// safe as they are owned by |pending_callbacks_| and share the same lifetime.
//
// That being said, deleting the renderers while keeping |pending_callbacks_|
// running on the media thread would result in crashes.

void NativePipelineImpl::RendererWrapper::Start(
    CreateTextureCB create_texture_cb,
    DestroyTextureCB destroy_texture_cb,
    base::WeakPtr<NativePipelineImpl> weak_pipeline) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  weak_pipeline_ = weak_pipeline;

  create_texture_cb_ = std::move(create_texture_cb);
  destroy_texture_cb_ = std::move(destroy_texture_cb);

  // Queue asynchronous actions required to start.
  DCHECK(!pending_callbacks_);
  SerialRunner::Queue fns;

  // Report the metadata to client to set VideoLayer. If starting without a
  // renderer we'll complete initialization at this point.
  fns.Push(base::BindOnce(&RendererWrapper::ReportMetadata,
                          weak_factory_.GetWeakPtr()));

  // Create renderer.
  fns.Push(base::BindOnce(&RendererWrapper::CreateRenderer,
                          weak_factory_.GetWeakPtr()));

  // Initialize renderer.
  fns.Push(base::BindOnce(&RendererWrapper::InitializeRenderer,
                          weak_factory_.GetWeakPtr()));

  // Run tasks.
  pending_callbacks_ = SerialRunner::Run(
      std::move(fns),
      base::BindOnce(&RendererWrapper::Complete, weak_factory_.GetWeakPtr()));
}

void NativePipelineImpl::RendererWrapper::Stop() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  pending_callbacks_.reset();
  weak_factory_.InvalidateWeakPtrs();

  DestroyRenderer();
}

bool NativePipelineImpl::IsRunning() const {
  DCHECK(thread_checker_.CalledOnValidThread());
  return !!client_;
}

void NativePipelineImpl::RendererWrapper::CreateRendererInternal(
    PipelineStatusCallback done_cb) {
  if (!default_renderer_) {
    // Create the Renderer asynchronously on the main task runner. Use
    // BindToCurrentLoop to call OnRendererCreated() on the media task runner.
    auto renderer_created_cb = base::BindPostTaskToCurrentDefault(
        base::BindOnce(&RendererWrapper::OnRendererCreated,
                       weak_factory_.GetWeakPtr(), std::move(done_cb)));

    absl::optional<RendererType> renderer_type;
    main_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&NativePipelineImpl::AsyncCreateRenderer, weak_pipeline_,
                       renderer_type, std::move(renderer_created_cb)));
    return;
  }

  // Just use the default one.
  OnRendererCreated(std::move(done_cb), std::move(default_renderer_));
}

void NativePipelineImpl::RendererWrapper::Complete(PipelineStatus status) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  DCHECK(pending_callbacks_);
  pending_callbacks_.reset();
  DestroyRenderer();
}

void NativePipelineImpl::RendererWrapper::CreateRenderer(
    PipelineStatusCallback done_cb) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  CreateRendererInternal(std::move(done_cb));
}

void NativePipelineImpl::RendererWrapper::OnRendererCreated(
    PipelineStatusCallback done_cb,
    std::unique_ptr<Renderer> renderer) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  if (!renderer) {
    std::move(done_cb).Run(PIPELINE_ERROR_INITIALIZATION_FAILED);
    return;
  }
  {
    base::AutoLock auto_lock(shared_state_lock_);
    DCHECK(!shared_state_renderer_);
    shared_state_renderer_ = std::move(renderer);
  }
  std::move(done_cb).Run(PIPELINE_OK);
}

void NativePipelineImpl::RendererWrapper::InitializeRenderer(
    PipelineStatusCallback done_cb) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  shared_state_renderer_->Initialize(std::move(create_texture_cb_),
                                     std::move(destroy_texture_cb_));
}

void NativePipelineImpl::RendererWrapper::DestroyRenderer() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  // Destroy the renderer outside the lock scope to avoid holding the lock
  // while renderer is being destroyed (in case Renderer destructor is costly).
  std::unique_ptr<Renderer> renderer;
  {
    base::AutoLock auto_lock(shared_state_lock_);
    renderer.swap(shared_state_renderer_);
  }
}

void NativePipelineImpl::RendererWrapper::ReportMetadata() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&NativePipelineImpl::OnSetLayer, weak_pipeline_));
}

NativePipelineImpl::NativePipelineImpl(
    scoped_refptr<base::SequencedTaskRunner> media_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    CreateRendererCB create_renderer_cb)
    : media_task_runner_(media_task_runner),
      create_renderer_cb_(create_renderer_cb),
      client_(nullptr) {
  DCHECK(create_renderer_cb_);

  renderer_wrapper_ = std::make_unique<RendererWrapper>(
      media_task_runner_, std::move(main_task_runner));
}

NativePipelineImpl::~NativePipelineImpl() {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(!client_) << "Stop() must complete before destroying object";
  DCHECK(!weak_factory_.HasWeakPtrs())
      << "Stop() should have invalidated all weak pointers";

  // RendererWrapper is deleted on the media thread.
  media_task_runner_->DeleteSoon(FROM_HERE, renderer_wrapper_.release());
}

void NativePipelineImpl::Start(Client* client,
                               CreateTextureCB create_texture_cb,
                               DestroyTextureCB destroy_texture_cb) {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(client);

  DCHECK(!client_);
  client_ = client;

  media_task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&RendererWrapper::Start,
                                base::Unretained(renderer_wrapper_.get()),
                                std::move(create_texture_cb),
                                std::move(destroy_texture_cb),
                                weak_factory_.GetWeakPtr()));
}

void NativePipelineImpl::Stop() {
  DCHECK(thread_checker_.CalledOnValidThread());

  if (media_task_runner_->RunsTasksInCurrentSequence()) {
    renderer_wrapper_->Stop();
  } else {
    media_task_runner_->PostTask(
        FROM_HERE, base::BindOnce(&RendererWrapper::Stop,
                                  base::Unretained(renderer_wrapper_.get())));
  }

  client_ = nullptr;

  // Invalidate self weak pointers effectively canceling all pending
  // notifications in the message queue.
  weak_factory_.InvalidateWeakPtrs();
}

void NativePipelineImpl::AsyncCreateRenderer(
    absl::optional<RendererType> renderer_type,
    RendererCreatedCB renderer_created_cb) {
  DCHECK(thread_checker_.CalledOnValidThread());
  std::move(renderer_created_cb).Run(create_renderer_cb_.Run(renderer_type));
}

void NativePipelineImpl::OnSetLayer() {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(IsRunning());

  DCHECK(client_);
  client_->OnSetLayer();
}

}  // namespace media
                     
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 */

#include "media/base/native_pipeline_impl.h"

#include <algorithm>
#include <memory>
#include <utility>

#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/functional/callback_helpers.h"
#include "base/memory/raw_ptr.h"
#include "base/metrics/histogram_macros.h"
#include "base/synchronization/lock.h"
#include "base/synchronization/waitable_event.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "media/base/renderer.h"
#include "media/base/renderer_client.h"
#include "media/base/serial_runner.h"
#include "media/base/text_renderer.h"
#include "base/task/bind_post_task.h"

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
             std::unique_ptr<Renderer> default_renderer,
             base::WeakPtr<NativePipelineImpl> weak_pipeline);

 private:
  // State transition tasks.
  void SetState(State next_state);
  void Complete(base::TimeDelta seek_time, PipelineStatus status);

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

  // Current state of the pipeline.
  State state_;

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
      main_task_runner_(std::move(main_task_runner)),
      state_(kCreated) {}

NativePipelineImpl::RendererWrapper::~RendererWrapper() {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(state_ == kCreated || state_ == kStopped);
}

// Note that the usage of base::Unretained() with the renderers is considered
// safe as they are owned by |pending_callbacks_| and share the same lifetime.
//
// That being said, deleting the renderers while keeping |pending_callbacks_|
// running on the media thread would result in crashes.

void NativePipelineImpl::RendererWrapper::Start(
    CreateTextureCB create_texture_cb,
    DestroyTextureCB destroy_texture_cb,
    std::unique_ptr<Renderer> default_renderer,
    base::WeakPtr<NativePipelineImpl> weak_pipeline) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(state_ == kCreated || state_ == kStopped)
      << "Received start in unexpected state: " << state_;

  default_renderer_ = std::move(default_renderer);
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
      base::BindOnce(&RendererWrapper::Complete, weak_factory_.GetWeakPtr(),
                     base::TimeDelta()));
}

bool NativePipelineImpl::IsRunning() const {
  DCHECK(thread_checker_.CalledOnValidThread());
  return !!client_;
}

bool NativePipelineImpl::IsSuspended() const {
  DCHECK(thread_checker_.CalledOnValidThread());
  return is_suspended_;
}

void NativePipelineImpl::RendererWrapper::CreateRendererInternal(
    PipelineStatusCallback done_cb) {
  DCHECK(state_ == kStarting || state_ == kResuming);

  absl::optional<RendererType> renderer_type;

  // TODO(xhwang): During Resume(), the |default_renderer_| might already match
  // the |renderer_type|, in which case we shouldn't need to create a new one.
  if (!default_renderer_ || renderer_type) {
    // Create the Renderer asynchronously on the main task runner. Use
    // BindToCurrentLoop to call OnRendererCreated() on the media task runner.
    auto renderer_created_cb = base::BindPostTaskToCurrentDefault(
        base::BindOnce(&RendererWrapper::OnRendererCreated,
                       weak_factory_.GetWeakPtr(), std::move(done_cb)));
    main_task_runner_->PostTask(
        FROM_HERE,
        base::BindOnce(&NativePipelineImpl::AsyncCreateRenderer, weak_pipeline_,
                       renderer_type, std::move(renderer_created_cb)));
    return;
  }

  // Just use the default one.
  OnRendererCreated(std::move(done_cb), std::move(default_renderer_));
}

void NativePipelineImpl::RendererWrapper::SetState(State next_state) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());

  state_ = next_state;
}

void NativePipelineImpl::RendererWrapper::Complete(base::TimeDelta seek_time,
                                                   PipelineStatus status) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(state_ == kStarting || state_ == kResuming);

  DCHECK(pending_callbacks_);
  pending_callbacks_.reset();
}

void NativePipelineImpl::RendererWrapper::CreateRenderer(
    PipelineStatusCallback done_cb) {
  DCHECK(media_task_runner_->RunsTasksInCurrentSequence());
  DCHECK(state_ == kStarting || state_ == kResuming);

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

  // Abort pending render initialization tasks and suspend the pipeline.
  pending_callbacks_.reset();
  DestroyRenderer();
  SetState(kSuspended);
  main_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&NativePipelineImpl::OnSeekDone, weak_pipeline_, true));
}

NativePipelineImpl::NativePipelineImpl(
    scoped_refptr<base::SequencedTaskRunner> media_task_runner,
    scoped_refptr<base::SingleThreadTaskRunner> main_task_runner,
    CreateRendererCB create_renderer_cb)
    : media_task_runner_(media_task_runner),
      create_renderer_cb_(create_renderer_cb),
      client_(nullptr),
      is_suspended_(false) {
  DCHECK(create_renderer_cb_);

  renderer_wrapper_ = std::make_unique<RendererWrapper>(
      media_task_runner_, std::move(main_task_runner));
}

NativePipelineImpl::~NativePipelineImpl() {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(!client_) << "Stop() must complete before destroying object";
  DCHECK(!suspend_cb_);
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

  std::unique_ptr<Renderer> default_renderer =
      create_renderer_cb_.Run(absl::nullopt);

  media_task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(
          &RendererWrapper::Start, base::Unretained(renderer_wrapper_.get()),
          std::move(create_texture_cb), std::move(destroy_texture_cb),
          std::move(default_renderer), weak_factory_.GetWeakPtr()));
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

void NativePipelineImpl::OnSeekDone(bool is_suspended) {
  DCHECK(thread_checker_.CalledOnValidThread());
  DCHECK(IsRunning());

  is_suspended_ = is_suspended;
}

}  // namespace media

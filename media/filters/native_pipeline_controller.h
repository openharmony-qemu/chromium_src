/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
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
  enum class State {
    STOPPED,
    STARTING,
    PLAYING,
    PLAYING_OR_SUSPENDED,
    SEEKING,
    SWITCHING_TRACKS,
    SUSPENDING,
    SUSPENDED,
    RESUMING,
  };

  using SeekedCB = base::RepeatingCallback<void(bool time_updated)>;
  using SuspendedCB = base::RepeatingClosure;
  using BeforeResumeCB = base::RepeatingClosure;
  using ResumedCB = base::RepeatingClosure;
  using CdmAttachedCB = base::OnceCallback<void(bool)>;

  // Construct a PipelineController wrapping |pipeline_|.
  // The callbacks are:
  //   - |seeked_cb| is called upon reaching a stable state if a seek occurred.
  //   - |suspended_cb| is called immediately after suspending.
  //   - |before_resume_cb| is called immediately before resuming.
  //   - |resumed_cb| is called immediately after resuming.
  //   - |error_cb| is called if any operation on |pipeline_| does not result
  //     in PIPELINE_OK or its error callback is called.
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

  // Tracks the current state of |pipeline_|.
  State state_ = State::STOPPED;

  base::ThreadChecker thread_checker_;
  base::WeakPtrFactory<NativePipelineController> weak_factory_{this};
};

}  // namespace media

#endif  // MEDIA_FILTERS_NATIVE_PIPELINE_CONTROLLER_H_
/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
 */

#ifndef MEDIA_BASE_NATIVE_TEXTURE_WRAPPER_H_
#define MEDIA_BASE_NATIVE_TEXTURE_WRAPPER_H_

#include "base/task/single_thread_task_runner.h"
#include "base/unguessable_token.h"
#include "media/base/video_frame.h"

namespace media {

using CreateSurfaceTextureCB = base::OnceCallback<void(int)>;
using DestroyTextureCB = base::OnceCallback<void()>;

class MEDIA_EXPORT NativeTextureWrapper {
 public:
  using StreamTextureWrapperInitCB = base::OnceCallback<void(bool)>;

  NativeTextureWrapper() = default;

  virtual void Initialize(
      const base::RepeatingClosure& received_frame_cb,
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
      StreamTextureWrapperInitCB init_cb,
      CreateSurfaceTextureCB create_surface_cb,
      DestroyTextureCB destroy_surface_cb) = 0;

  // Called whenever the video's natural size changes.
  // See NativeTextureWrapperImpl.
  virtual void UpdateTextureSize(const gfx::Size& natural_size) = 0;

  // Returns the latest frame.
  // See NativeTextureWrapperImpl.
  virtual scoped_refptr<VideoFrame> GetCurrentFrame() = 0;

  // Clears the |received_frame_cb| passed in Initialize().
  // Should be safe to call from any thread.
  virtual void ClearReceivedFrameCBOnAnyThread() = 0;

  struct Deleter {
    inline void operator()(NativeTextureWrapper* ptr) const { ptr->Destroy(); }
  };

 protected:
  virtual ~NativeTextureWrapper() {}

  // Safely destroys the NativeTextureWrapper.
  // See NativeTextureWrapperImpl.
  virtual void Destroy() = 0;
};

typedef std::unique_ptr<NativeTextureWrapper, NativeTextureWrapper::Deleter>
    ScopedNativeTextureWrapper;

}  // namespace media

#endif  // MEDIA_BASE_NATIVE_TEXTURE_WRAPPER_H_

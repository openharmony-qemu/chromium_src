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
  virtual void ClearCBOnAnyThread() = 0;

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

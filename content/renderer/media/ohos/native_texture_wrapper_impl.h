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

#ifndef CONTENT_RENDERER_MEDIA_OHOS_NATIVE_TEXTURE_WRAPPER_IMPL_H_
#define CONTENT_RENDERER_MEDIA_OHOS_NATIVE_TEXTURE_WRAPPER_IMPL_H_

#include "base/task/single_thread_task_runner.h"
#include "content/common/content_export.h"
#include "content/renderer/media/ohos/native_texture_factory.h"
#include "gpu/command_buffer/common/mailbox.h"
#include "media/base/native_texture_wrapper.h"
#include "media/base/video_frame.h"
#include "ui/gl/ohos/native_buffer_utils.h"

namespace content {

class CONTENT_EXPORT NativeTextureWrapperImpl
    : public media::NativeTextureWrapper {
 public:
  // |enable_texture_copy| controls the VideoFrameMetadata::COPY_REQUIRED flag,
  // making sure it is correctly set on |current_frame_|, for webview scenarios.
  static media::ScopedNativeTextureWrapper Create(
      bool enable_texture_copy,
      gl::ohos::TextureOwnerMode texture_owner_mode,
      scoped_refptr<NativeTextureFactory> factory,
      scoped_refptr<base::SingleThreadTaskRunner> main_task_runner);

  NativeTextureWrapperImpl(const NativeTextureWrapperImpl&) = delete;
  NativeTextureWrapperImpl& operator=(const NativeTextureWrapperImpl&) = delete;

  void Initialize(
      const base::RepeatingClosure& received_frame_cb,
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
      StreamTextureWrapperInitCB init_cb,
      CreateSurfaceTextureCB create_surface_cb,
      DestroyTextureCB destroy_surface_cb) override;

  // Should be called when the Video size changes.
  // Can be called from any thread, but runs on |main_task_runner_|.
  void UpdateTextureSize(const gfx::Size& rotated_visible_size) override;

  // Returns the latest frame.
  // N.B: We create a single VideoFrame at initialization time (and update it
  // in UpdateTextureSize()), and repeatedly return it here. The underlying
  // texture's changes are signalled via |client|'s DidReceiveFrame() callback.
  scoped_refptr<media::VideoFrame> GetCurrentFrame() override;

  // Clears the |received_frame_cb| passed in Initialize().
  // Should be safe to call from any thread.
  void ClearCBOnAnyThread() override;
 private:
  NativeTextureWrapperImpl(
      bool enable_texture_copy,
      gl::ohos::TextureOwnerMode texture_owner_mode,
      scoped_refptr<NativeTextureFactory> factory,
      scoped_refptr<base::SingleThreadTaskRunner> main_task_runner);
  ~NativeTextureWrapperImpl() override;

  // Destroys |this| safely on |main_task_runner_|.
  void Destroy() override;

  void InitializeOnMainThread(const base::RepeatingClosure& received_frame_cb,
                              StreamTextureWrapperInitCB init_cb,
                              CreateSurfaceTextureCB create_texture_cb,
                              DestroyTextureCB destroy_texture_cb);

  void CreateVideoFrame(const gpu::Mailbox& mailbox,
                        const gfx::Size& coded_size,
                        const gfx::Rect& visible_rect,
                        const absl::optional<gpu::VulkanYCbCrInfo>& ycbcr_info);

  void SetCurrentFrameInternal(scoped_refptr<media::VideoFrame> video_frame);

  bool enable_texture_copy_;
  gl::ohos::TextureOwnerMode texture_owner_mode_;

  // Object for calling back the compositor thread to repaint the video when a
  // frame is available. It should be bound to |compositor_task_runner_|.
  ScopedNativeTextureProxy native_texture_proxy_;

  // Visible size of the video with rotation applied.
  gfx::Size rotated_visible_size_;

  scoped_refptr<NativeTextureFactory> factory_;

  base::Lock current_frame_lock_;
  scoped_refptr<media::VideoFrame> current_frame_;

  scoped_refptr<base::SingleThreadTaskRunner> main_task_runner_;
  scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;

  base::WeakPtrFactory<NativeTextureWrapperImpl> weak_factory_{this};
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_NATIVE_TEXTURE_WRAPPER_IMPL_H_

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

#ifndef CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_H_
#define CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "media/base/native_texture_wrapper.h"
#include "media/base/video_renderer_sink.h"
#include "media/mojo/clients/mojo_renderer_wrapper.h"
#include "ui/gfx/geometry/rect.h"

namespace content {

class NativeRenderClient : public media::MojoRendererWrapper {
 public:
  NativeRenderClient();
  NativeRenderClient(
      scoped_refptr<base::SequencedTaskRunner> media_task_runner,
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
      media::ScopedNativeTextureWrapper native_texture_wrapper,
      media::VideoRendererSink* sink);
  ~NativeRenderClient() override;

  // media::Renderer implementation (inherited from media::MojoRendererWrapper).
  // We override normal initialization to set up |stream_texture_wrapper_|.
  void Initialize(media::CreateTextureCB create_texture_cb,
                  media::DestroyTextureCB destroy_texture_cb) override;

  void OnSurfaceCreated(int native_embed_id);
  void OnSurfaceDestroy();

  void OnSizeChange(const gfx::Rect& rect);

  // Called on |compositor_task_runner_| whenever |stream_texture_wrapper_| has
  // a new frame.
  void OnFrameAvailable();

  media::RendererType GetRendererType() override {
    return media::RendererType::kNative;
  }

 private:
  scoped_refptr<base::SequencedTaskRunner> media_task_runner_;

  // Owns the StreamTexture whose surface is used by NativeRenderer.
  // Provides the VideoFrames to |sink_|.
  media::ScopedNativeTextureWrapper native_texture_wrapper_;

  raw_ptr<media::VideoRendererSink> sink_;

  // Used by |native_texture_wrapper_| to signal OnFrameAvailable() and to send
  // VideoFrames to |sink_| on the right thread.
  scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;

  media::CreateTextureCB create_texture_cb_;
  media::DestroyTextureCB destroy_texture_cb_;

  // NOTE: Weak pointers must be invalidated before all other member variables.
  base::WeakPtrFactory<NativeRenderClient> weak_factory_{this};
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_H_

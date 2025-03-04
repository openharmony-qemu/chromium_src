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

#ifndef CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_FACTORY_H_
#define CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_FACTORY_H_

#include <memory>

#include "base/functional/callback.h"
#include "base/task/single_thread_task_runner.h"
#include "content/common/content_export.h"
#include "media/base/native_texture_wrapper.h"
#include "media/base/renderer_factory.h"
#include "media/mojo/clients/mojo_renderer_factory.h"

namespace media {
class MojoRendererFactory;
}

namespace content {

class CONTENT_EXPORT NativeRendererClientFactory
    : public media::RendererFactory {
 public:
  using GetNativeTextureWrapperCB =
      base::RepeatingCallback<media::ScopedNativeTextureWrapper()>;

  NativeRendererClientFactory(
      scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
      const GetNativeTextureWrapperCB& get_stream_texture_wrapper_cb);
  ~NativeRendererClientFactory() override;

  std::unique_ptr<media::Renderer> CreateRenderer(
      const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
      const scoped_refptr<base::TaskRunner>& worker_task_runner,
      media::AudioRendererSink* audio_renderer_sink,
      media::VideoRendererSink* video_renderer_sink,
      media::RequestOverlayInfoCB request_surface_cb,
      const gfx::ColorSpace& target_color_space) override;

  media::MediaResource::Type GetRequiredMediaResourceType() override;

 private:
  GetNativeTextureWrapperCB get_native_texture_wrapper_cb_;

  scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner_;
};

}  // namespace content

#endif  // CONTENT_RENDERER_MEDIA_OHOS_NATIVE_RENDERER_CLIENT_FACTORY_H_

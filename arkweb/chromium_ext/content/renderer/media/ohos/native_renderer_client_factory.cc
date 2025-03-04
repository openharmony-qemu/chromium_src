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

#include "content/renderer/media/ohos/native_renderer_client_factory.h"

#include "content/renderer/media/ohos/native_renderer_client.h"
#include "media/mojo/clients/mojo_renderer.h"

namespace content {

NativeRendererClientFactory::NativeRendererClientFactory(
    scoped_refptr<base::SingleThreadTaskRunner> compositor_task_runner,
    const GetNativeTextureWrapperCB& get_native_texture_wrapper_cb)
    : get_native_texture_wrapper_cb_(get_native_texture_wrapper_cb),
      compositor_task_runner_(std::move(compositor_task_runner)) {}

NativeRendererClientFactory::~NativeRendererClientFactory() {}

std::unique_ptr<media::Renderer> NativeRendererClientFactory::CreateRenderer(
    const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
    const scoped_refptr<base::TaskRunner>& worker_task_runner,
    media::AudioRendererSink* audio_renderer_sink,
    media::VideoRendererSink* video_renderer_sink,
    media::RequestOverlayInfoCB request_overlay_info_cb,
    const gfx::ColorSpace& target_color_space) {
  media::ScopedNativeTextureWrapper native_texture_wrapper =
      get_native_texture_wrapper_cb_.Run();

  return std::make_unique<NativeRenderClient>(
      media_task_runner, compositor_task_runner_,
      std::move(native_texture_wrapper), video_renderer_sink);
}

media::MediaResource::Type
NativeRendererClientFactory::GetRequiredMediaResourceType() {
  return media::MediaResource::Type::KUrl;
}

}  // namespace content
                       
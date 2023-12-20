/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2022-2023. All rights reserved.
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

  return std::make_unique<NativeRenderClient>(compositor_task_runner_,
                                              std::move(native_texture_wrapper),
                                              video_renderer_sink);
}

media::MediaResource::Type
NativeRendererClientFactory::GetRequiredMediaResourceType() {
  return media::MediaResource::Type::URL;
}

}  // namespace content
/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(IS_ANDROID) || BUILDFLAG(ARKWEB_MEDIA)
std::unique_ptr<MojoRenderer> MojoRendererFactory::CreateMediaPlayerRenderer(
    mojo::PendingReceiver<mojom::MediaPlayerRendererExtension>
        renderer_extension_receiver,
    mojo::PendingRemote<mojom::MediaPlayerRendererClientExtension>
        client_extension_remote,
    const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
    VideoRendererSink* video_renderer_sink) {
  DCHECK(interface_factory_);
  mojo::PendingRemote<mojom::Renderer> renderer_remote;

  interface_factory_->CreateMediaPlayerRenderer(
      std::move(client_extension_remote),
      renderer_remote.InitWithNewPipeAndPassReceiver(),
      std::move(renderer_extension_receiver));

  return std::make_unique<MojoRenderer>(media_task_runner, nullptr,
                                        video_renderer_sink,
                                        std::move(renderer_remote));
}
#endif  // BUILDFLAG(IS_ANDROID) || BUILDFLAG(ARKWEB_MEDIA)

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
std::unique_ptr<MojoRenderer> MojoRendererFactory::CreateCustomMediaPlayerRenderer(
    mojo::PendingReceiver<mojom::MediaPlayerRendererExtension>
        renderer_extension_receiver,
    mojo::PendingRemote<mojom::CustomMediaPlayerRendererClientExtension>
        client_extension_remote,
    const scoped_refptr<base::SequencedTaskRunner>& media_task_runner,
    VideoRendererSink* video_renderer_sink,
    int player_id) {
  DCHECK(interface_factory_);
  mojo::PendingRemote<mojom::Renderer> renderer_remote;

  interface_factory_->CreateCustomMediaPlayerRenderer(
      std::move(client_extension_remote),
      renderer_remote.InitWithNewPipeAndPassReceiver(),
      std::move(renderer_extension_receiver),
      player_id);

  return std::make_unique<MojoRenderer>(media_task_runner, nullptr,
                                        video_renderer_sink,
                                        std::move(renderer_remote));
}
#endif // ARKWEB_CUSTOM_VIDEO_PLAYER
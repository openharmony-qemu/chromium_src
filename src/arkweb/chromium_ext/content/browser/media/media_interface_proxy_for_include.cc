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

#if BUILDFLAG(ARKWEB_MEDIA)
#include "content/browser/media/ohos/ohos_media_player_renderer.h"
#include "media/mojo/services/mojo_renderer_service.h"  // nogncheck
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#include "content/browser/media/ohos/ohos_custom_media_player_renderer.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

namespace content {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void MediaInterfaceProxy::CreateCustomMediaPlayerRenderer(
    mojo::PendingRemote<media::mojom::CustomMediaPlayerRendererClientExtension>
        client_extension_remote,
    mojo::PendingReceiver<media::mojom::Renderer> receiver,
    mojo::PendingReceiver<media::mojom::MediaPlayerRendererExtension>
        renderer_extension_receiver,
    int player_id) {
  DCHECK(thread_checker_.CalledOnValidThread());

  media::MojoRendererService::Create(
      nullptr,
      std::make_unique<OHOSCustomMediaPlayerRenderer>(
          render_frame_host().GetProcess()->GetID(),
          render_frame_host().GetRoutingID(), player_id,
          WebContents::FromRenderFrameHost(&render_frame_host()),
          std::move(renderer_extension_receiver),
          std::move(client_extension_remote)),
      std::move(receiver));
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER
}  // namespace content
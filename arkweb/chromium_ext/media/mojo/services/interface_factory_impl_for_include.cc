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

namespace media {

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
void InterfaceFactoryImpl::CreateCustomMediaPlayerRenderer(
    mojo::PendingRemote<mojom::CustomMediaPlayerRendererClientExtension>
        client_extension_ptr,
    mojo::PendingReceiver<mojom::Renderer> receiver,
    mojo::PendingReceiver<mojom::MediaPlayerRendererExtension>
        renderer_extension_receiver,
    int player_id) {
  NOTREACHED();
}
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

}  // namespace media

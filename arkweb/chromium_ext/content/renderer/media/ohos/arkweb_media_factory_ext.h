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

#ifndef CONTENT_RENDERER_MEDIA_OHOS_ARKWEB_MEDIA_FACTORY_EXT_H_
#define CONTENT_RENDERER_MEDIA_OHOS_ARKWEB_MEDIA_FACTORY_EXT_H_

#include "content/renderer/media/media_factory.h"
#include "content/renderer/render_thread_impl.h"
#include <memory>

#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "build/buildflag.h"
#include "build/chromecast_buildflags.h"
#include "components/viz/common/surfaces/surface_id.h"
#include "media/base/key_system_info.h"
#include "media/base/key_systems.h"
#include "media/base/key_systems_support_registration.h"
#include "media/base/media_player_logging_id.h"
#include "media/base/renderer_factory_selector.h"
#include "media/base/routing_token_callback.h"
#include "media/media_buildflags.h"
#include "media/mojo/buildflags.h"
#include "media/mojo/clients/mojo_renderer_factory.h"
#include "media/mojo/mojom/interface_factory.mojom.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "third_party/blink/public/platform/web_media_player_source.h"
#include "third_party/blink/public/platform/web_set_sink_id_callbacks.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/web/web_media_inspector.h"

#if BUILDFLAG(ENABLE_MEDIA_REMOTING)
// Needed by remoting sender.
#include "media/mojo/mojom/remoting.mojom.h"  // nogncheck
#endif  // BUILDFLAG(ENABLE_MEDIA_REMOTING)


namespace content {
class MediaFactory;
class RendererBlinkPlatformImpl;

class ArkwebMediaFactoryExt : public MediaFactory {
public:
  ArkwebMediaFactoryExt(RenderFrameImpl* render_frame,
    media::RequestRoutingTokenCallback request_routing_token_cb);

  content::ArkwebMediaFactoryExt* AsArkwebMediaFactoryExt() override {
    return this;
  }
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  blink::WebNativeBridge* CreateWebNativeBridge(blink::WebNativeClient* client,
    viz::FrameSinkId parent_frame_sink_id, scoped_refptr<base::SingleThreadTaskRunner>
        main_thread_compositor_task_runner);
  media::RendererWebNativeDelegate* GetWebNativeDelegate();
#endif

private:
  raw_ptr<RenderFrameImpl> render_frame_;
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  media::RendererWebNativeDelegate* web_native_delegate_ = nullptr;
#endif

};
}

#endif
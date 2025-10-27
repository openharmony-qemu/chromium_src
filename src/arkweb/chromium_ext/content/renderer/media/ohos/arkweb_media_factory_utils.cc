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
#include "arkweb_media_factory_utils.h"

#include <memory>
#include <string>
#include <tuple>
#include <utility>

#include "base/command_line.h"
#include "base/logging.h"
#include "base/functional/bind.h"
#include "base/functional/callback.h"
#include "base/metrics/field_trial_params.h"
#include "base/metrics/histogram_functions.h"
#include "base/strings/string_number_conversions.h"
#include "base/task/bind_post_task.h"
#include "base/task/sequenced_task_runner.h"
#include "base/task/single_thread_task_runner.h"
#include "build/build_config.h"
#include "build/buildflag.h"
#include "cc/trees/layer_tree_settings.h"
#include "components/viz/common/features.h"
#include "content/public/common/content_client.h"
#include "content/public/common/content_switches.h"
#include "content/public/renderer/content_renderer_client.h"
#include "content/public/renderer/key_system_support.h"
#include "content/public/renderer/render_frame_media_playback_options.h"
#include "content/renderer/media/batching_media_log.h"
#include "content/renderer/media/inspector_media_event_handler.h"
#include "content/renderer/media/media_interface_factory.h"
#include "content/renderer/media/render_media_event_handler.h"
#include "content/renderer/media/renderer_web_media_player_delegate.h"
#include "content/renderer/render_frame_impl.h"
#include "content/renderer/render_thread_impl.h"
#include "gpu/ipc/client/client_shared_image_interface.h"
#include "media/base/cdm_factory.h"
#include "media/base/decoder_factory.h"
#include "media/base/demuxer.h"
#include "media/base/key_systems_impl.h"
#include "media/base/media_switches.h"
#include "media/base/renderer_factory_selector.h"
#include "media/media_buildflags.h"
#include "media/mojo/buildflags.h"
#include "media/mojo/mojom/key_system_support.mojom.h"
#include "media/renderers/decrypting_renderer_factory.h"
#include "media/renderers/default_decoder_factory.h"
#include "media/renderers/renderer_impl_factory.h"
#include "media/video/gpu_video_accelerator_factories.h"
#include "mojo/public/cpp/bindings/pending_remote.h"
#include "services/service_manager/public/cpp/connect.h"
#include "services/viz/public/cpp/gpu/context_provider_command_buffer.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_registry.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/media/key_system_config_selector.h"
#include "third_party/blink/public/platform/media/video_frame_compositor.h"
#include "third_party/blink/public/platform/media/web_encrypted_media_client_impl.h"
#include "third_party/blink/public/platform/media/web_media_player_builder.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/public/platform/web_media_player_client.h"
#include "third_party/blink/public/platform/web_surface_layer_bridge.h"
#include "third_party/blink/public/platform/web_video_frame_submitter.h"
#include "third_party/blink/public/web/blink.h"
#include "third_party/blink/public/web/modules/media/audio/audio_device_factory.h"
#include "third_party/blink/public/web/modules/mediastream/media_stream_video_source.h"
#include "third_party/blink/public/web/modules/mediastream/web_media_player_ms.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_view.h"
#include "url/origin.h"

#if BUILDFLAG(ENABLE_CAST_RECEIVER)
#include "components/cast_streaming/common/public/cast_streaming_url.h"  // nogncheck
#include "components/cast_streaming/common/public/features.h"  // nogncheck
#include "components/cast_streaming/renderer/public/resource_provider.h"  // nogncheck
#include "components/cast_streaming/renderer/public/wrapping_renderer_factory_selector.h"  // nogncheck
#endif

#if BUILDFLAG(ENABLE_CAST_RENDERER)
#include "content/renderer/media/cast_renderer_client_factory.h"
#endif

#if BUILDFLAG(IS_FUCHSIA)
#include "media/cdm/fuchsia/fuchsia_cdm_factory.h"
#include "media/fuchsia/video/fuchsia_decoder_factory.h"
#include "media/mojo/clients/mojo_fuchsia_cdm_provider.h"
#elif BUILDFLAG(ENABLE_MOJO_CDM)
#include "media/mojo/clients/mojo_cdm_factory.h"  // nogncheck
#else
#include "media/cdm/default_cdm_factory.h"
#endif

#if BUILDFLAG(IS_FUCHSIA) && BUILDFLAG(ENABLE_MOJO_CDM)
#error "MojoCdm should be disabled for Fuchsia."
#endif

#if BUILDFLAG(ENABLE_MOJO_AUDIO_DECODER) || BUILDFLAG(ENABLE_MOJO_VIDEO_DECODER)
#include "media/mojo/clients/mojo_decoder_factory.h"  // nogncheck
#endif


#if BUILDFLAG(ENABLE_CAST_AUDIO_RENDERER)
#include "content/renderer/media/cast_renderer_factory.h"
#endif  // BUILDFLAG(ENABLE_CAST_AUDIO_RENDERER)

#if BUILDFLAG(IS_CASTOS)
// Enable remoting receiver
#include "media/base/remoting_constants.h"             // nogncheck
#include "media/remoting/receiver_controller.h"        // nogncheck
#include "media/remoting/remoting_renderer_factory.h"  // nogncheck
#endif  // BUILDFLAG(IS_CASTOS)


#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "arkweb/chromium_ext/content/renderer/media/ohos/native_renderer_client_factory.h"
#include "arkweb/chromium_ext/content/renderer/media/ohos/native_texture_wrapper_impl.h"
#include "arkweb/chromium_ext/content/renderer/media/renderer_web_native_delegate.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/platform/web_native_bridge_impl.h"
#include "base/system/sys_info.h"
#endif

#if BUILDFLAG(ARKWEB_MEDIA)
#include "content/renderer/media/ohos/ohos_media_player_renderer_client_factory.h"
#endif

#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
#if !BUILDFLAG(ARKWEB_SAME_LAYER)
#include "arkweb/chromium_ext/base/ohos/sys_info_utils_ext.h"
#include "base/system/sys_info.h"
#endif
#include "content/renderer/media/ohos/ohos_custom_media_player_renderer_client_factory.h"
#include "gpu/config/gpu_finch_features.h"
#endif  // ARKWEB_CUSTOM_VIDEO_PLAYER

namespace content {

ArkwebMediaFactoryUtils::ArkwebMediaFactoryUtils(MediaFactory* media_factory)
    : media_factory_(media_factory) {}

void ArkwebMediaFactoryUtils::AddOhosAndCustomMediaFactory(media::RendererFactorySelector* factory_selector,
    RenderThreadImpl* render_thread) {
#if BUILDFLAG(ARKWEB_MEDIA)
  gl::ohos::TextureOwnerMode texture_owner_mode = features::IsUsingVulkan()
      ? gl::ohos::TextureOwnerMode::kHwVideoZeroCopyNativeBuffer
      : gl::ohos::TextureOwnerMode::kNativeImageTexture;
  auto ohos_media_player_factory =
      std::make_unique<OHOSMediaPlayerRendererClientFactory>(
        render_thread->compositor_task_runner(), media_factory_->CreateMojoRendererFactory(),
        base::BindRepeating(
            &NativeTextureWrapperImpl::Create,
            true,
            texture_owner_mode,
            render_thread->GetNativeTexureFactory(),
            media_factory_->render_frame_->GetTaskRunner(blink::TaskType::kInternalMedia)));
  factory_selector->AddFactory(media::RendererType::kOHOSMediaPlayer,
                               std::move(ohos_media_player_factory));
#endif
#if BUILDFLAG(ARKWEB_CUSTOM_VIDEO_PLAYER)
  texture_owner_mode = base::ohos::IsEmulator() || base::SysInfo::IsLowEndDevice()
          ? gl::ohos::TextureOwnerMode::kNativeImageTexture
          : gl::ohos::TextureOwnerMode::kHwVideoZeroCopyNativeBuffer;
  auto ohos_custom_media_player_factory =
      std::make_unique<OHOSCustomMediaPlayerRendererClientFactory>(
          render_thread->compositor_task_runner(), media_factory_->CreateMojoRendererFactory(),
          base::BindRepeating(
              &NativeTextureWrapperImpl::Create,
              features::IsUsingVulkan() || base::ohos::IsEmulator() ||
                  base::SysInfo::IsLowEndDevice(),
              texture_owner_mode, render_thread->GetNativeTexureFactory(),
              media_factory_->render_frame_->GetTaskRunner(blink::TaskType::kInternalMedia)));
  factory_selector->AddFactory(media::RendererType::kOHOSCustomMediaPlayer,
                               std::move(ohos_custom_media_player_factory));
#endif
}
}
// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "third_party/blink/renderer/core/frame/local_frame_mojo_handler.h"
#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/arkweb_local_frame_mojo_handler_ext.h"

#include "arkweb/build/features/features.h"
#include "base/metrics/histogram_functions.h"
#include "base/numerics/safe_conversions.h"
#include "base/time/time.h"
#include "build/build_config.h"
#include "cc/input/browser_controls_offset_tags_info.h"
#include "services/network/public/cpp/url_loader_completion_status.h"
#include "services/network/public/mojom/url_response_head.mojom.h"
#include "third_party/blink/public/common/associated_interfaces/associated_interface_provider.h"
#include "third_party/blink/public/common/chrome_debug_urls.h"
#include "third_party/blink/public/common/features.h"
#include "third_party/blink/public/common/frame/frame_owner_element_type.h"
#include "third_party/blink/public/common/page_state/page_state.h"
#include "third_party/blink/public/mojom/devtools/console_message.mojom-blink-forward.h"
#include "third_party/blink/public/mojom/devtools/inspector_issue.mojom-blink.h"
#include "third_party/blink/public/mojom/frame/frame_owner_properties.mojom-blink.h"
#include "third_party/blink/public/mojom/frame/media_player_action.mojom-blink.h"
#include "third_party/blink/public/mojom/opengraph/metadata.mojom-blink.h"
#include "third_party/blink/public/mojom/timing/resource_timing.mojom-blink-forward.h"
#include "third_party/blink/public/platform/browser_interface_broker_proxy.h"
#include "third_party/blink/public/platform/interface_registry.h"
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/public/web/web_plugin.h"
#include "third_party/blink/renderer/bindings/core/v8/script_controller.h"
#include "third_party/blink/renderer/bindings/core/v8/script_evaluation_result.h"
#include "third_party/blink/renderer/bindings/core/v8/script_function.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_fullscreen_options.h"
#include "third_party/blink/renderer/core/dom/element_traversal.h"
#include "third_party/blink/renderer/core/dom/focus_params.h"
#include "third_party/blink/renderer/core/dom/ignore_opens_during_unload_count_incrementer.h"
#include "third_party/blink/renderer/core/editing/editing_utilities.h"
#include "third_party/blink/renderer/core/editing/frame_selection.h"
#include "third_party/blink/renderer/core/editing/surrounding_text.h"
#include "third_party/blink/renderer/core/exported/web_plugin_container_impl.h"
#include "third_party/blink/renderer/core/frame/csp/content_security_policy.h"
#include "third_party/blink/renderer/core/frame/frame_console.h"
#include "third_party/blink/renderer/core/frame/intervention.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/local_frame_client.h"
#include "third_party/blink/renderer/core/frame/pausable_script_executor.h"
#include "third_party/blink/renderer/core/frame/remote_frame_owner.h"
#include "third_party/blink/renderer/core/frame/reporting_context.h"
#include "third_party/blink/renderer/core/frame/savable_resources.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/fullscreen/fullscreen.h"
#include "third_party/blink/renderer/core/html/html_element.h"
#include "third_party/blink/renderer/core/html/html_embed_element.h"
#include "third_party/blink/renderer/core/html/html_link_element.h"
#include "third_party/blink/renderer/core/html/html_meta_element.h"
#include "third_party/blink/renderer/core/html/html_object_element.h"
#include "third_party/blink/renderer/core/html/media/html_video_element.h"
#include "third_party/blink/renderer/core/input/event_handler.h"
#include "third_party/blink/renderer/core/inspector/console_message.h"
#include "third_party/blink/renderer/core/inspector/main_thread_debugger.h"
#include "third_party/blink/renderer/core/layout/layout_embedded_content.h"
#include "third_party/blink/renderer/core/loader/mixed_content_checker.h"
#include "third_party/blink/renderer/core/messaging/message_port.h"
#include "third_party/blink/renderer/core/navigation_api/navigation_api.h"
#include "third_party/blink/renderer/core/page/chrome_client.h"
#include "third_party/blink/renderer/core/page/focus_controller.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing.h"
#include "third_party/blink/renderer/core/script/classic_script.h"
#include "third_party/blink/renderer/core/timing/dom_window_performance.h"
#include "third_party/blink/renderer/core/view_transition/page_swap_event.h"
#include "third_party/blink/renderer/core/view_transition/view_transition_supplement.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_timing_utils.h"
#include "third_party/blink/renderer/platform/widget/frame_widget.h"
#if BUILDFLAG(ARKWEB_PRECOMPILE)
#include "arkweb/chromium_ext/third_party/blink/renderer/core/v8/v8_code_cache_utils.h"
#endif

#if BUILDFLAG(IS_MAC)
#include "base/apple/foundation_util.h"
#include "third_party/blink/renderer/core/editing/substring_util.h"
#include "third_party/blink/renderer/platform/fonts/mac/attributed_string_type_converter.h"
#include "ui/base/mojom/attributed_string.mojom-blink.h"
#endif

#if BUILDFLAG(IS_ARKWEB)
#include <sys/mman.h>

#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#endif

namespace blink {

ArkWebLocalFrameMojoHandlerExt::ArkWebLocalFrameMojoHandlerExt(blink::LocalFrame& frame)
    : LocalFrameMojoHandler(frame) {}

#if BUILDFLAG(IS_ARKWEB)
void ArkWebLocalFrameMojoHandlerExt::JavaScriptExecuteRequestExt(
    mojo::ScopedHandle handle_fd,
    const uint64_t scriptLength,
    bool wants_result,
    JavaScriptExecuteRequestCallback callback) {
  MojoPlatformHandle platform_handle;
  platform_handle.struct_size = sizeof(platform_handle);
  MojoUnwrapPlatformHandle(handle_fd.release().value(), nullptr,
                           &platform_handle);
  int fd = static_cast<int>(platform_handle.value);

  auto flowbufferAdapter =
      OHOS::NWeb::OhosAdapterHelper::GetInstance().CreateFlowbufferAdapter();
  if (!flowbufferAdapter) {
    LOG(ERROR) << "create flowbuffer adapter failed";
    return;
  }
  char* ashmem = static_cast<char*>(flowbufferAdapter->CreateAshmemWithFd(
      fd, static_cast<size_t>(scriptLength + 1), PROT_READ));
  if (!ashmem) {
    return;
  }
  String javascript = String::FromUTF8(ashmem);
  JavaScriptExecuteRequest(javascript, wants_result, std::move(callback));
  close(fd);
}
#endif

#if BUILDFLAG(ARKWEB_PRECOMPILE)
void ArkWebLocalFrameMojoHandlerExt::GenerateCodeCache(
    const WTF::String& url,
    const WTF::String& script,
    mojom::blink::CacheOptionsPtr cache_options,
    GenerateCodeCacheCallback callback) {
  ScriptState* script_state = ToScriptStateForMainWorld(frame_);
  V8CodeCacheUtils::CacheOptions options(cache_options->response_headers,
                                    cache_options->is_module,
                                    cache_options->is_top_level);
  V8CodeCacheUtils::CacheError err =
      V8CodeCacheUtils::GenerateCodeCache(script_state, url, script, options);
  std::move(callback).Run(static_cast<int32_t>(err));
}
#endif

#if BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)
void ArkWebLocalFrameMojoHandlerExt::GetImageFromCache(
    const WTF::String& url,
    LocalFrameMojoHandler::GetImageFromCacheCallback callback) {
  blink::Document* document = frame_->GetDocument();
  if (document == nullptr) {
    LOG(ERROR) << "getImageFromCache: document nullptr";
    std::move(callback).Run(0, base::ReadOnlySharedMemoryRegion());
    return;
  }

  DocumentLoader* loader = document->Loader();
  if (loader == nullptr) {
    LOG(ERROR) << "getImageFromCache: loader nullptr";
    std::move(callback).Run(0, base::ReadOnlySharedMemoryRegion());
    return;
  }

  scoped_refptr<const SharedBuffer> resource_buffer =
#if BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)
      loader->AsArkWebDocumentLoaderExt()->OnGetImageFromCache(url);
#else
      loader->OnGetImageFromCache(url);
#endif
  if (resource_buffer == nullptr) {
    LOG(ERROR) << "getImageFromCache: Get resource buffer null";
    std::move(callback).Run(0, base::ReadOnlySharedMemoryRegion());
    return;
  }

  base::WritableSharedMemoryRegion region =
      base::WritableSharedMemoryRegion::Create(resource_buffer->size());
  if (!region.IsValid()) {
    LOG(ERROR) << "getImageFromCache: WritableSharedMemoryRegion create failed";
    std::move(callback).Run(0, base::ReadOnlySharedMemoryRegion());
    return;
  }

  base::WritableSharedMemoryMapping mapping = region.Map();
  if (!mapping.IsValid()) {
    LOG(ERROR) << "getImageFromCache: WritableSharedMemoryRegion map failed";
    std::move(callback).Run(0, base::ReadOnlySharedMemoryRegion());
    return;
  }

  bool finished = resource_buffer->GetBytes(mapping.GetMemoryAsSpan<uint8_t>());
  if (!finished) {
    LOG(ERROR)
        << "getImageFromCache: Get resource bytes unfinished, buffer size "
        << resource_buffer->size();
    std::move(callback).Run(0, base::ReadOnlySharedMemoryRegion());
    return;
  }

  base::ReadOnlySharedMemoryRegion ro_region =
      base::WritableSharedMemoryRegion::ConvertToReadOnly(std::move(region));
  std::move(callback).Run(static_cast<uint32_t>(resource_buffer->size()),
                          std::move(ro_region));
  return;
}
#endif  // BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)
}  // namespace blink

// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ARKWEB_LOCAL_FRAME_MOJO_HANDLER_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ARKWEB_LOCAL_FRAME_MOJO_HANDLER_EXT_H_

#include "arkweb/build/features/features.h"
#include "build/build_config.h"
#if BUILDFLAG(IS_ARKWEB_EXT)
#include "arkweb/ohos_nweb_ex/build/features/features.h"
#endif
#include "build/build_config.h"
#include "cc/input/browser_controls_offset_tags_info.h"
#include "third_party/blink/public/common/tokens/tokens.h"
#include "third_party/blink/public/mojom/confidence_level.mojom-blink.h"
#include "third_party/blink/public/mojom/device_posture/device_posture_provider.mojom-blink.h"
#include "third_party/blink/public/mojom/frame/back_forward_cache_controller.mojom-blink.h"
#include "third_party/blink/public/mojom/frame/frame.mojom-blink.h"
#include "third_party/blink/public/mojom/frame/fullscreen.mojom-blink-forward.h"
#include "third_party/blink/public/mojom/frame/lifecycle.mojom-blink-forward.h"
#include "third_party/blink/public/mojom/media/fullscreen_video_element.mojom-blink.h"
#include "third_party/blink/public/mojom/reporting/reporting.mojom-blink.h"
#include "third_party/blink/public/mojom/timing/resource_timing.mojom-blink-forward.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"
#include "third_party/blink/renderer/platform/heap/persistent.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_associated_receiver.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_associated_remote.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_receiver.h"
#include "third_party/blink/renderer/platform/mojo/heap_mojo_remote.h"
#include "third_party/blink/renderer/platform/weborigin/kurl.h"

#if BUILDFLAG(IS_MAC)
#include "third_party/blink/public/mojom/input/text_input_host.mojom-blink.h"
#endif

namespace blink {

class LocalFrame;

class ArkWebLocalFrameMojoHandlerExt : public LocalFrameMojoHandler {
 public:
  explicit ArkWebLocalFrameMojoHandlerExt(blink::LocalFrame& frame);

 private:
#if BUILDFLAG(IS_ARKWEB)
  void JavaScriptExecuteRequestExt(
      mojo::ScopedHandle handle_fd,
      const uint64_t scriptLength,
      bool wants_result,
      JavaScriptExecuteRequestCallback callback) final;
#endif

#if BUILDFLAG(ARKWEB_MENU) || BUILDFLAG(IS_ARKWEB_EXT)
  void GetImageFromCache(const WTF::String& url,
                         GetImageFromCacheCallback callback) final;
#endif

#if BUILDFLAG(ARKWEB_PRECOMPILE)
  void GenerateCodeCache(const WTF::String& url,
                         const WTF::String& script,
                         mojom::blink::CacheOptionsPtr cache_options,
                         GenerateCodeCacheCallback callback) final;
#endif
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ARKWEB_LOCAL_FRAME_MOJO_HANDLER_EXT_H_

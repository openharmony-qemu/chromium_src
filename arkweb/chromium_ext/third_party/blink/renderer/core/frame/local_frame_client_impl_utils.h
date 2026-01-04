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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_CLIENT_IMPL_UTILS_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_CLIENT_IMPL_UTILS_H_


#include "third_party/blink/renderer/core/frame/local_frame_client_impl.h"
#if BUILDFLAG(ARKWEB_SAME_LAYER)
#include "third_party/blink/renderer/core/loader/native_loader.h"
#endif

namespace blink {
class LocalFrameClientImpl;

class LocalFrameClientImplUtils {
 public:
  LocalFrameClientImplUtils(LocalFrameClientImpl* impl);

#if BUILDFLAG(ARKWEB_JSPROXY)
  void RunScriptsAtHeadElementAvailable();
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
  std::unique_ptr<WebNativeBridge> CreateWebNativeBridge(NativeLoader&, WebNativeClient*);
  float GetDeviceScaleFactor(NativeLoader&);
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
  void DispatchDidSubresourceFiltered();
  bool GetGlobalAdblockEnabled();
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
  void NotifyLcpForBlankless();
#endif

 private:
  raw_ptr<LocalFrameClientImpl> frame_client_impl_;
};
}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_CLIENT_IMPL_UTILS_H_

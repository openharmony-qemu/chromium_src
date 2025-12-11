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

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_CORE_INITIALIZER_EXT_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_CORE_INITIALIZER_EXT_H_


#include "arkweb/build/features/features.h"

#ifdef BUILDFLAG(ARKWEB_SAME_LAYER)
#include "third_party/blink/renderer/core/loader/native_loader.h"
#endif

namespace blink {

class WebLocalFrameClient;
#if BUILDFLAG(ARKWEB_SAME_LAYER)
class WebNativeBridge;
class WebNativeClient;
class NativeLoader;
#endif

class CoreInitializerExt {
public:
#if BUILDFLAG(ARKWEB_SAME_LAYER)
  virtual std::unique_ptr<WebNativeBridge> CreateWebNativeBridge(
      WebLocalFrameClient* web_frame_client,
      NativeLoader& html_native_element,
      WebNativeClient*) const = 0;

  virtual float GetDeviceScaleFactor(WebLocalFrameClient* web_frame_client) = 0;
#endif
};
}
#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_LOCAL_FRAME_CLIENT_EXT_H_
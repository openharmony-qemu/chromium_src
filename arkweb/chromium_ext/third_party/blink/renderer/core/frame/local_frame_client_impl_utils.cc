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
#include "arkweb/chromium_ext/third_party/blink/renderer/core/frame/local_frame_client_impl_utils.h"
#include "third_party/blink/renderer/core/core_initializer.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "base/logging.h"

namespace blink {
LocalFrameClientImplUtils::LocalFrameClientImplUtils(LocalFrameClientImpl* impl)
    : frame_client_impl_(impl){}

#if BUILDFLAG(ARKWEB_JSPROXY)
void LocalFrameClientImplUtils::RunScriptsAtHeadElementAvailable() {
  if (frame_client_impl_->web_frame_ && frame_client_impl_->web_frame_->Client()) {
    frame_client_impl_->web_frame_->Client()->AsWebLocalFrameClientExt()->RunScriptsAtHeadReady();
  }
  // The callback might have deleted the frame, do not use |this|!
}
#endif

#if BUILDFLAG(ARKWEB_SAME_LAYER)
std::unique_ptr<WebNativeBridge> LocalFrameClientImplUtils::CreateWebNativeBridge(
    NativeLoader& native_loader, WebNativeClient* client) {
  WebLocalFrameImpl* web_frame =
      WebLocalFrameImpl::FromFrame(native_loader.CurrentFrame());

  if (!web_frame || !web_frame->Client()) {
    return nullptr;
  }
  return CoreInitializer::GetInstance().CreateWebNativeBridge(
      web_frame->Client(), native_loader, client);
}
#endif

#if BUILDFLAG(ARKWEB_ADBLOCK)
void LocalFrameClientImplUtils::DispatchDidSubresourceFiltered() {
  frame_client_impl_->web_frame_->DidSubresourceFiltered();
}

bool LocalFrameClientImplUtils::GetGlobalAdblockEnabled() {
  return frame_client_impl_->web_frame_->GetGlobalAdblockEnabled();
}
#endif

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void LocalFrameClientImplUtils::NotifyLcpForBlankless() {
  if (frame_client_impl_->web_frame_ && frame_client_impl_->web_frame_->Client() &&
      frame_client_impl_->web_frame_->Client()->AsWebLocalFrameClientExt()) {
    frame_client_impl_->web_frame_->Client()->AsWebLocalFrameClientExt()->NotifyLcpForBlankless();
  }
}
#endif
} // namespace blink
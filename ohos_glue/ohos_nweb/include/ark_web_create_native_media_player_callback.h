/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef ARK_WEB_CREATE_NATIVE_MEDIA_PLAYER_CALLBACK_H_
#define ARK_WEB_CREATE_NATIVE_MEDIA_PLAYER_CALLBACK_H_
#pragma once

#include "ohos_nweb/include/ark_web_media_info.h"
#include "ohos_nweb/include/ark_web_native_media_player_bridge.h"
#include "ohos_nweb/include/ark_web_native_media_player_handler.h"

namespace OHOS::ArkWeb {

/*--ark web(source=library)--*/
class ArkWebCreateNativeMediaPlayerCallback
    : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebNativeMediaPlayerBridge>
  OnCreate(ArkWebRefPtr<ArkWebNativeMediaPlayerHandler> handler,
           ArkWebRefPtr<ArkWebMediaInfo> mediaInfo) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_CREATE_NATIVE_MEDIA_PLAYER_CALLBACK_H_

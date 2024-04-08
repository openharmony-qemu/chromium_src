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

#ifndef ARK_WEB_MEDIA_INFO_H_
#define ARK_WEB_MEDIA_INFO_H_
#pragma once

#include "ohos_nweb/include/ark_web_media_source_info_vector.h"
#include "ohos_nweb/include/ark_web_native_media_player_surface_info.h"

namespace OHOS::ArkWeb {

/*--ark web(source=web core)--*/
class ArkWebMediaInfo : public virtual ArkWebBaseRefCounted {
public:
  /*--ark web()--*/
  virtual int GetPreload() = 0;

  /*--ark web()--*/
  virtual bool GetIsMuted() = 0;

  /*--ark web()--*/
  virtual ArkWebString GetEmbedId() = 0;

  /*--ark web()--*/
  virtual ArkWebString GetPosterUrl() = 0;

  /*--ark web()--*/
  virtual int GetMediaType() = 0;

  /*--ark web()--*/
  virtual bool GetIsControlsShown() = 0;

  /*--ark web()--*/
  virtual ArkWebStringVector GetControls() = 0;

  /*--ark web()--*/
  virtual ArkWebStringMap GetHeaders() = 0;

  /*--ark web()--*/
  virtual ArkWebStringMap GetAttributes() = 0;

  /*--ark web()--*/
  virtual ArkWebMediaSourceInfoVector GetSourceInfos() = 0;

  /*--ark web()--*/
  virtual ArkWebRefPtr<ArkWebNativeMediaPlayerSurfaceInfo> GetSurfaceInfo() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_MEDIA_INFO_H_

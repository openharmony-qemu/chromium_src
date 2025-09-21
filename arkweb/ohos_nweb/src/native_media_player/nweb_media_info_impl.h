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

#ifndef OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_MEDIA_INFO_IMPL_H_
#define OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_MEDIA_INFO_IMPL_H_

#include "cef/ohos_cef_ext/include/cef_custom_media_info.h"
#include "ohos_nweb/include/nweb_native_media_player.h"

namespace OHOS::NWeb {

class NWebMediaInfoImpl : public NWebMediaInfo {
 public:
  NWebMediaInfoImpl(const CefCustomMediaInfo& media_info);
  virtual ~NWebMediaInfoImpl() override;

  Preload GetPreload() override;
  bool GetIsMuted() override;
  std::string GetEmbedId() override;
  std::string GetPosterUrl() override;
  MediaType GetMediaType() override;
  bool GetIsControlsShown() override;
  std::vector<std::string> GetControls() override;
  std::map<std::string, std::string> GetHeaders() override;
  std::map<std::string, std::string> GetAttributes() override;
  std::vector<std::shared_ptr<NWebMediaSourceInfo>> GetSourceInfos() override;
  std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> GetSurfaceInfo() override;

 private:
  CefCustomMediaInfo media_info_;
};

}  // namespace OHOS::NWeb

#endif  // OHOS_NWEB_SRC_NATIVE_MEDIA_PLAYER_NWEB_MEDIA_INFO_IMPL_H_

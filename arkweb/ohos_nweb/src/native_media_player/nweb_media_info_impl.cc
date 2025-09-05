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

#include "ohos_nweb/src/native_media_player/nweb_media_info_impl.h"

namespace OHOS::NWeb {

namespace {

class NWebMediaSourceInfoImpl : public NWebMediaSourceInfo {
 public:
  NWebMediaSourceInfoImpl(const CefMediaSourceInfo& info) : info_(info) {}
  ~NWebMediaSourceInfoImpl() override = default;

  SourceType GetType() override {
    return static_cast<SourceType>(info_.source_type);
  }
  std::string GetFormat() override { return info_.media_format; }
  std::string GetSource() override { return info_.media_source; }

 private:
  CefMediaSourceInfo info_;
};

class NWebNativeMediaPlayerSurfaceInfoImpl
    : public NWebNativeMediaPlayerSurfaceInfo {
 public:
  NWebNativeMediaPlayerSurfaceInfoImpl(
      const CefNativeMediaPlayerSurfaceInfo& info)
      : info_(info) {}
  ~NWebNativeMediaPlayerSurfaceInfoImpl() override = default;

  std::string GetId() override { return info_.id; }
  double GetX() override { return info_.x; }
  double GetY() override { return info_.y; }
  double GetWidth() override { return info_.width; }
  double GetHeight() override { return info_.height; }

 private:
  CefNativeMediaPlayerSurfaceInfo info_;
};

}  // namespace

NWebMediaInfoImpl::NWebMediaInfoImpl(const CefCustomMediaInfo& media_info)
    : media_info_(media_info) {}

NWebMediaInfoImpl::~NWebMediaInfoImpl() = default;

Preload NWebMediaInfoImpl::GetPreload() {
  return static_cast<Preload>(media_info_.preload);
}
bool NWebMediaInfoImpl::GetIsMuted() {
  return media_info_.muted;
}
std::string NWebMediaInfoImpl::GetEmbedId() {
  return media_info_.embed_id;
}
std::string NWebMediaInfoImpl::GetPosterUrl() {
  return media_info_.poster_url;
}
MediaType NWebMediaInfoImpl::GetMediaType() {
  return static_cast<MediaType>(media_info_.media_type);
}
bool NWebMediaInfoImpl::GetIsControlsShown() {
  return media_info_.controls;
}
std::vector<std::string> NWebMediaInfoImpl::GetControls() {
  return media_info_.controlslist;
}
std::map<std::string, std::string> NWebMediaInfoImpl::GetHeaders() {
  return media_info_.https_headers;
}
std::map<std::string, std::string> NWebMediaInfoImpl::GetAttributes() {
  return media_info_.attributes;
}
std::vector<std::shared_ptr<NWebMediaSourceInfo>>
NWebMediaInfoImpl::GetSourceInfos() {
  std::vector<std::shared_ptr<NWebMediaSourceInfo>> infos;
  infos.reserve(media_info_.media_src_list.size());
  for (const auto& media_src : media_info_.media_src_list) {
    infos.emplace_back(new NWebMediaSourceInfoImpl(media_src));
  }
  return infos;
}
std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo>
NWebMediaInfoImpl::GetSurfaceInfo() {
  std::shared_ptr<NWebNativeMediaPlayerSurfaceInfo> source_info(
      new NWebNativeMediaPlayerSurfaceInfoImpl(media_info_.surface_info));
  return source_info;
}

}  // namespace OHOS::NWeb

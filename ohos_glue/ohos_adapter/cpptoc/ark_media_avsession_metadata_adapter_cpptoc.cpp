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

#include "ohos_adapter/cpptoc/ark_media_avsession_metadata_adapter_cpptoc.h"
#include "base/cpptoc/ark_web_cpptoc_macros.h"

namespace OHOS::ArkWeb {

namespace {

void ARK_WEB_CALLBACK ark_media_avsession_metadata_adapter_set_title(
    struct _ark_media_avsession_metadata_adapter_t *self,
    const ArkWebString *title) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(title, );

  // Execute
  ArkMediaAVSessionMetadataAdapterCppToC::Get(self)->SetTitle(*title);
}

ArkWebString ARK_WEB_CALLBACK ark_media_avsession_metadata_adapter_get_title(
    struct _ark_media_avsession_metadata_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkMediaAVSessionMetadataAdapterCppToC::Get(self)->GetTitle();
}

void ARK_WEB_CALLBACK ark_media_avsession_metadata_adapter_set_artist(
    struct _ark_media_avsession_metadata_adapter_t *self,
    const ArkWebString *artist) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(artist, );

  // Execute
  ArkMediaAVSessionMetadataAdapterCppToC::Get(self)->SetArtist(*artist);
}

ArkWebString ARK_WEB_CALLBACK ark_media_avsession_metadata_adapter_get_artist(
    struct _ark_media_avsession_metadata_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkMediaAVSessionMetadataAdapterCppToC::Get(self)->GetArtist();
}

void ARK_WEB_CALLBACK ark_media_avsession_metadata_adapter_set_album(
    struct _ark_media_avsession_metadata_adapter_t *self,
    const ArkWebString *album) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, );

  ARK_WEB_CPPTOC_CHECK_PARAM(album, );

  // Execute
  ArkMediaAVSessionMetadataAdapterCppToC::Get(self)->SetAlbum(*album);
}

ArkWebString ARK_WEB_CALLBACK ark_media_avsession_metadata_adapter_get_album(
    struct _ark_media_avsession_metadata_adapter_t *self) {
  ARK_WEB_CPPTOC_DV_LOG("capi struct is %{public}ld", (long)self);

  ARK_WEB_CPPTOC_CHECK_PARAM(self, ark_web_string_default);

  // Execute
  return ArkMediaAVSessionMetadataAdapterCppToC::Get(self)->GetAlbum();
}

} // namespace

ArkMediaAVSessionMetadataAdapterCppToC::
    ArkMediaAVSessionMetadataAdapterCppToC() {
  GetStruct()->set_title = ark_media_avsession_metadata_adapter_set_title;
  GetStruct()->get_title = ark_media_avsession_metadata_adapter_get_title;
  GetStruct()->set_artist = ark_media_avsession_metadata_adapter_set_artist;
  GetStruct()->get_artist = ark_media_avsession_metadata_adapter_get_artist;
  GetStruct()->set_album = ark_media_avsession_metadata_adapter_set_album;
  GetStruct()->get_album = ark_media_avsession_metadata_adapter_get_album;
}

ArkMediaAVSessionMetadataAdapterCppToC::
    ~ArkMediaAVSessionMetadataAdapterCppToC() {
}

template <>
ArkWebBridgeType ArkWebCppToCRefCounted<
    ArkMediaAVSessionMetadataAdapterCppToC, ArkMediaAVSessionMetadataAdapter,
    ark_media_avsession_metadata_adapter_t>::kBridgeType =
    ARK_MEDIA_AVSESSION_METADATA_ADAPTER;

} // namespace OHOS::ArkWeb

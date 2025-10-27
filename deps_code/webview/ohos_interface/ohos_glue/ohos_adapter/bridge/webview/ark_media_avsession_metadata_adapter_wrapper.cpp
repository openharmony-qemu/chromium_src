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

#include "ohos_adapter/bridge/ark_media_avsession_metadata_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkMediaAVSessionMetadataAdapterWrapper::ArkMediaAVSessionMetadataAdapterWrapper(
    ArkWebRefPtr<ArkMediaAVSessionMetadataAdapter> ref)
    : ctocpp_(ref)
{}

void ArkMediaAVSessionMetadataAdapterWrapper::SetTitle(const std::string& title)
{
    ArkWebString str = ArkWebStringClassToStruct(title);
    ctocpp_->SetTitle(str);
    ArkWebStringStructRelease(str);
}

std::string ArkMediaAVSessionMetadataAdapterWrapper::GetTitle()
{
    ArkWebString title = ctocpp_->GetTitle();
    std::string result = ArkWebStringStructToClass(title);
    ArkWebStringStructRelease(title);
    return result;
}

void ArkMediaAVSessionMetadataAdapterWrapper::SetArtist(const std::string& artist)
{
    ArkWebString str = ArkWebStringClassToStruct(artist);
    ctocpp_->SetArtist(str);
    ArkWebStringStructRelease(str);
}

std::string ArkMediaAVSessionMetadataAdapterWrapper::GetArtist()
{
    ArkWebString artist = ctocpp_->GetArtist();
    std::string result = ArkWebStringStructToClass(artist);
    ArkWebStringStructRelease(artist);
    return result;
}

void ArkMediaAVSessionMetadataAdapterWrapper::SetAlbum(const std::string& album)
{
    ArkWebString str = ArkWebStringClassToStruct(album);
    ctocpp_->SetAlbum(str);
    ArkWebStringStructRelease(str);
}

std::string ArkMediaAVSessionMetadataAdapterWrapper::GetAlbum()
{
    ArkWebString album = ctocpp_->GetAlbum();
    std::string result = ArkWebStringStructToClass(album);
    ArkWebStringStructRelease(album);
    return result;
}

void ArkMediaAVSessionMetadataAdapterWrapper::SetImageUrl(const std::string& imageUrl)
{
    ArkWebString str = ArkWebStringClassToStruct(imageUrl);
    ctocpp_->SetImageUrl(str);
    ArkWebStringStructRelease(str);
}

std::string ArkMediaAVSessionMetadataAdapterWrapper::GetImageUrl()
{
    ArkWebString imageUrl = ctocpp_->GetImageUrl();
    std::string result = ArkWebStringStructToClass(imageUrl);
    ArkWebStringStructRelease(imageUrl);
    return result;
}
} // namespace OHOS::ArkWeb

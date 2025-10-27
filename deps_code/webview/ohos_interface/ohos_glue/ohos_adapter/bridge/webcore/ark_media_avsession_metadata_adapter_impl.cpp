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

#include "ohos_adapter/bridge/ark_media_avsession_metadata_adapter_impl.h"

namespace OHOS::ArkWeb {

ArkMediaAVSessionMetadataAdapterImpl::ArkMediaAVSessionMetadataAdapterImpl(
    std::shared_ptr<OHOS::NWeb::MediaAVSessionMetadataAdapter> ref)
    : real_(ref)
{}

void ArkMediaAVSessionMetadataAdapterImpl::SetTitle(const ArkWebString& title)
{
    real_->SetTitle(ArkWebStringStructToClass(title));
}

ArkWebString ArkMediaAVSessionMetadataAdapterImpl::GetTitle()
{
    std::string str = real_->GetTitle();
    return ArkWebStringClassToStruct(str);
}

void ArkMediaAVSessionMetadataAdapterImpl::SetArtist(const ArkWebString& artist)
{
    real_->SetArtist(ArkWebStringStructToClass(artist));
}

ArkWebString ArkMediaAVSessionMetadataAdapterImpl::GetArtist()
{
    std::string str = real_->GetArtist();
    return ArkWebStringClassToStruct(str);
}

void ArkMediaAVSessionMetadataAdapterImpl::SetAlbum(const ArkWebString& album)
{
    real_->SetAlbum(ArkWebStringStructToClass(album));
}

ArkWebString ArkMediaAVSessionMetadataAdapterImpl::GetAlbum()
{
    std::string str = real_->GetAlbum();
    return ArkWebStringClassToStruct(str);
}

void ArkMediaAVSessionMetadataAdapterImpl::SetImageUrl(const ArkWebString& imageUrl)
{
    real_->SetImageUrl(ArkWebStringStructToClass(imageUrl));
}

ArkWebString ArkMediaAVSessionMetadataAdapterImpl::GetImageUrl()
{
    std::string str = real_->GetImageUrl();
    return ArkWebStringClassToStruct(str);
}
} // namespace OHOS::ArkWeb

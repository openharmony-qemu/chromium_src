/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ohos_adapter/bridge/ark_paste_data_record_adapter_impl.h"

#include "ohos_adapter/bridge/ark_clip_board_image_data_adapter_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebRefPtr<ArkPasteDataRecordAdapter> ArkPasteDataRecordAdapter::NewRecord(const ArkWebString& mimeType)
{
    std::shared_ptr<NWeb::PasteDataRecordAdapter> adapter =
        NWeb::PasteDataRecordAdapter::NewRecord(ArkWebStringStructToClass(mimeType));
    return new ArkPasteDataRecordAdapterImpl(adapter);
}

ArkWebRefPtr<ArkPasteDataRecordAdapter> ArkPasteDataRecordAdapter::NewRecord(
    const ArkWebString& mimeType, void* htmlText, void* plainText)
{
    std::shared_ptr<std::string>* html = static_cast<std::shared_ptr<std::string>*>(htmlText);
    std::shared_ptr<std::string>* plain = static_cast<std::shared_ptr<std::string>*>(plainText);
    std::shared_ptr<NWeb::PasteDataRecordAdapter> adapter =
        NWeb::PasteDataRecordAdapter::NewRecord(ArkWebStringStructToClass(mimeType), *html, *plain);
    return new ArkPasteDataRecordAdapterImpl(adapter);
}

ArkPasteDataRecordAdapterImpl::ArkPasteDataRecordAdapterImpl(std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter> ref)
    : real_(ref)
{}

bool ArkPasteDataRecordAdapterImpl::SetHtmlText(void* htmlText)
{
    std::shared_ptr<std::string>* temp = static_cast<std::shared_ptr<std::string>*>(htmlText);
    return real_->SetHtmlText(*temp);
}

bool ArkPasteDataRecordAdapterImpl::SetHtmlTextV2(const ArkWebString& htmlText)
{
    std::shared_ptr<std::string> temp = std::make_shared<std::string>(ArkWebStringStructToClass(htmlText));
    return real_->SetHtmlText(temp);
}

bool ArkPasteDataRecordAdapterImpl::SetPlainText(void* plainText)
{
    std::shared_ptr<std::string>* temp = static_cast<std::shared_ptr<std::string>*>(plainText);
    return real_->SetPlainText(*temp);
}

bool ArkPasteDataRecordAdapterImpl::SetPlainTextV2(const ArkWebString& plainText)
{
    std::shared_ptr<std::string> temp = std::make_shared<std::string>(ArkWebStringStructToClass(plainText));
    return real_->SetPlainText(temp);
}

bool ArkPasteDataRecordAdapterImpl::SetImgData(ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData)
{
    if (CHECK_REF_PTR_IS_NULL(imageData)) {
        return real_->SetImgData(nullptr);
    }

    return real_->SetImgData(std::make_shared<ArkClipBoardImageDataAdapterWrapper>(imageData));
}

ArkWebString ArkPasteDataRecordAdapterImpl::GetMimeType()
{
    return ArkWebStringClassToStruct(real_->GetMimeType());
}

void ArkPasteDataRecordAdapterImpl::GetHtmlText(void* data)
{
    std::shared_ptr<std::string> str = real_->GetHtmlText();
    std::shared_ptr<std::string>* html = static_cast<std::shared_ptr<std::string>*>(data);
    *html = str;
}

bool ArkPasteDataRecordAdapterImpl::GetHtmlTextV2(ArkWebString& data)
{
    std::shared_ptr<std::string> str = real_->GetHtmlText();
    if (str) {
        data = ArkWebStringClassToStruct(*str);
        return true;
    }
    return false;
}

void ArkPasteDataRecordAdapterImpl::GetPlainText(void* data)
{
    std::shared_ptr<std::string> str = real_->GetPlainText();
    std::shared_ptr<std::string>* text = static_cast<std::shared_ptr<std::string>*>(data);
    *text = str;
}

bool ArkPasteDataRecordAdapterImpl::GetPlainTextV2(ArkWebString& data)
{
    std::shared_ptr<std::string> str = real_->GetPlainText();
    if (str) {
        data = ArkWebStringClassToStruct(*str);
        return true;
    }
    return false;
}

bool ArkPasteDataRecordAdapterImpl::GetImgData(ArkWebRefPtr<ArkClipBoardImageDataAdapter> imageData)
{
    if (CHECK_REF_PTR_IS_NULL(imageData)) {
        return real_->GetImgData(nullptr);
    }

    return real_->GetImgData(std::make_shared<ArkClipBoardImageDataAdapterWrapper>(imageData));
}

bool ArkPasteDataRecordAdapterImpl::SetUri(const ArkWebString& uriString)
{
    return real_->SetUri(ArkWebStringStructToClass(uriString));
}

bool ArkPasteDataRecordAdapterImpl::SetCustomData(void* data)
{
    NWeb::PasteCustomData* temp = static_cast<NWeb::PasteCustomData*>(data);
    return real_->SetCustomData(*temp);
}

bool ArkPasteDataRecordAdapterImpl::SetCustomDataV2(ArkWebUInt8VectorMap& data)
{
    NWeb::PasteCustomData temp = ArkWebUInt8VectorMapStructToClass(data);
    return real_->SetCustomData(temp);
}

void ArkPasteDataRecordAdapterImpl::GetUri(void* data)
{
    std::shared_ptr<std::string> str = real_->GetUri();
    std::shared_ptr<std::string>* uri = static_cast<std::shared_ptr<std::string>*>(data);
    *uri = str;
}

bool ArkPasteDataRecordAdapterImpl::GetUriV2(ArkWebString& data)
{
    std::shared_ptr<std::string> str = real_->GetUri();
    if (str) {
        data = ArkWebStringClassToStruct(*str);
        return true;
    }
    return false;
}

void ArkPasteDataRecordAdapterImpl::GetCustomData(void* data)
{
    std::shared_ptr<NWeb::PasteCustomData> custom = real_->GetCustomData();
    std::shared_ptr<NWeb::PasteCustomData>* temp = static_cast<std::shared_ptr<NWeb::PasteCustomData>*>(data);
    *temp = custom;
}

bool ArkPasteDataRecordAdapterImpl::GetCustomDataV2(ArkWebUInt8VectorMap& data)
{
    std::shared_ptr<NWeb::PasteCustomData> custom = real_->GetCustomData();
    if (custom) {
        data = ArkWebUInt8VectorMapClassToStruct(*custom);
        return true;
    }
    return false;
}

} // namespace OHOS::ArkWeb

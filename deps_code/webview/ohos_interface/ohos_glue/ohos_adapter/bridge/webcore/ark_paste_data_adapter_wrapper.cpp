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

#include "ohos_adapter/bridge/ark_paste_data_adapter_wrapper.h"

#include "ohos_adapter/bridge/ark_paste_data_record_adapter_wrapper.h"
#include "ohos_adapter/ctocpp/ark_paste_record_vector_ctocpp.h"

namespace OHOS::ArkWeb {

ArkPasteDataAdapterWrapper::ArkPasteDataAdapterWrapper(ArkWebRefPtr<ArkPasteDataAdapter> ref) : ctocpp_(ref) {}

void ArkPasteDataAdapterWrapper::AddHtmlRecord(const std::string& html)
{
    ArkWebString str = ArkWebStringClassToStruct(html);
    ctocpp_->AddHtmlRecord(str);
    ArkWebStringStructRelease(str);
}

void ArkPasteDataAdapterWrapper::AddTextRecord(const std::string& text)
{
    ArkWebString str = ArkWebStringClassToStruct(text);
    ctocpp_->AddTextRecord(str);
    ArkWebStringStructRelease(str);
}

std::vector<std::string> ArkPasteDataAdapterWrapper::GetMimeTypes()
{
    ArkWebStringVector vec = ctocpp_->GetMimeTypes();
    std::vector<std::string> result = ArkWebStringVectorStructToClass(vec);
    ArkWebStringVectorStructRelease(vec);
    return result;
}

std::shared_ptr<std::string> ArkPasteDataAdapterWrapper::GetPrimaryHtml()
{
    std::shared_ptr<std::string> result;
    ctocpp_->GetPrimaryHtml((void*)&result);
    return result;
}

std::shared_ptr<std::string> ArkPasteDataAdapterWrapper::GetPrimaryText()
{
    std::shared_ptr<std::string> result;
    ctocpp_->GetPrimaryText((void*)&result);
    return result;
}

std::shared_ptr<std::string> ArkPasteDataAdapterWrapper::GetPrimaryMimeType()
{
    std::shared_ptr<std::string> result;
    ctocpp_->GetPrimaryMimeType((void*)&result);
    return result;
}

std::shared_ptr<NWeb::PasteDataRecordAdapter> ArkPasteDataAdapterWrapper::GetRecordAt(std::size_t index)
{
    ArkWebRefPtr<ArkPasteDataRecordAdapter> temp = ctocpp_->GetRecordAt(index);
    return std::make_shared<ArkPasteDataRecordAdapterWrapper>(temp);
}

std::size_t ArkPasteDataAdapterWrapper::GetRecordCount()
{
    return ctocpp_->GetRecordCount();
}

NWeb::PasteRecordVector ArkPasteDataAdapterWrapper::AllRecords()
{
    ArkPasteRecordVector ark_vector = ctocpp_->AllRecords();
    NWeb::PasteRecordVector result = ArkPasteRecordVectorStructToClass(ark_vector);
    ArkPasteRecordVectorStructRelease(ark_vector);
    return result;
}

} // namespace OHOS::ArkWeb

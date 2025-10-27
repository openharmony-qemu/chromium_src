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

#include "ohos_adapter/bridge/ark_paste_data_adapter_impl.h"

#include "ohos_adapter/bridge/ark_paste_data_record_adapter_impl.h"
#include "ohos_adapter/cpptoc/ark_paste_record_vector_cpptoc.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkPasteDataAdapterImpl::ArkPasteDataAdapterImpl(std::shared_ptr<OHOS::NWeb::PasteDataAdapter> ref) : real_(ref) {}

void ArkPasteDataAdapterImpl::AddHtmlRecord(const ArkWebString& html)
{
    real_->AddHtmlRecord(ArkWebStringStructToClass(html));
}

void ArkPasteDataAdapterImpl::AddTextRecord(const ArkWebString& text)
{
    real_->AddTextRecord(ArkWebStringStructToClass(text));
}

ArkWebStringVector ArkPasteDataAdapterImpl::GetMimeTypes()
{
    return ArkWebStringVectorClassToStruct(real_->GetMimeTypes());
}

void ArkPasteDataAdapterImpl::GetPrimaryHtml(void* data)
{
    std::shared_ptr<std::string> str = real_->GetPrimaryHtml();
    std::shared_ptr<std::string>* html = static_cast<std::shared_ptr<std::string>*>(data);
    *html = str;
}

void ArkPasteDataAdapterImpl::GetPrimaryText(void* data)
{
    std::shared_ptr<std::string> str = real_->GetPrimaryText();
    std::shared_ptr<std::string>* text = static_cast<std::shared_ptr<std::string>*>(data);
    *text = str;
}

void ArkPasteDataAdapterImpl::GetPrimaryMimeType(void* data)
{
    std::shared_ptr<std::string> str = real_->GetPrimaryMimeType();
    std::shared_ptr<std::string>* mimeType = static_cast<std::shared_ptr<std::string>*>(data);
    *mimeType = str;
}

ArkWebRefPtr<ArkPasteDataRecordAdapter> ArkPasteDataAdapterImpl::GetRecordAt(size_t index)
{
    return new ArkPasteDataRecordAdapterImpl(real_->GetRecordAt(index));
}

size_t ArkPasteDataAdapterImpl::GetRecordCount()
{
    return real_->GetRecordCount();
}

ArkPasteRecordVector ArkPasteDataAdapterImpl::AllRecords()
{
    NWeb::PasteRecordVector temp = real_->AllRecords();
    return ArkPasteRecordVectorClassToStruct(temp);
}

} // namespace OHOS::ArkWeb

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

#include "ohos_nweb/bridge/ark_web_url_resource_response_impl.h"

#include "ohos_nweb/bridge/ark_web_resource_ready_callback_wrapper.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebUrlResourceResponseImpl::ArkWebUrlResourceResponseImpl(
    std::shared_ptr<OHOS::NWeb::NWebUrlResourceResponse> nweb_url_resource_response)
    : nweb_url_resource_response_(nweb_url_resource_response)
{}

ArkWebString ArkWebUrlResourceResponseImpl::ResponseData()
{
    return ArkWebStringClassToStruct(nweb_url_resource_response_->ResponseData());
}

void ArkWebUrlResourceResponseImpl::PutResponseData(const ArkWebString& input_stream)
{
    nweb_url_resource_response_->PutResponseData(ArkWebStringStructToClass(input_stream));
}

ArkWebString ArkWebUrlResourceResponseImpl::ResponseStatus()
{
    return ArkWebStringClassToStruct(nweb_url_resource_response_->ResponseStatus());
}

const ArkWebStringMap ArkWebUrlResourceResponseImpl::ResponseHeaders()
{
    return ArkWebStringMapClassToStruct(nweb_url_resource_response_->ResponseHeaders());
}

void ArkWebUrlResourceResponseImpl::PutResponseHeaders(const ArkWebStringMap& response_headers)
{
    nweb_url_resource_response_->PutResponseHeaders(ArkWebStringMapStructToClass(response_headers));
}

int32_t ArkWebUrlResourceResponseImpl::ResponseDataType()
{
    return static_cast<int32_t>(nweb_url_resource_response_->ResponseDataType());
}

ArkWebString ArkWebUrlResourceResponseImpl::ResponseMimeType()
{
    return ArkWebStringClassToStruct(nweb_url_resource_response_->ResponseMimeType());
}

void ArkWebUrlResourceResponseImpl::PutResponseMimeType(const ArkWebString& mime_type)
{
    nweb_url_resource_response_->PutResponseMimeType(ArkWebStringStructToClass(mime_type));
}

ArkWebString ArkWebUrlResourceResponseImpl::ResponseEncoding()
{
    return ArkWebStringClassToStruct(nweb_url_resource_response_->ResponseEncoding());
}

void ArkWebUrlResourceResponseImpl::PutResponseEncoding(const ArkWebString& encoding)
{
    nweb_url_resource_response_->PutResponseEncoding(ArkWebStringStructToClass(encoding));
}

bool ArkWebUrlResourceResponseImpl::ResponseDataStatus()
{
    return nweb_url_resource_response_->ResponseDataStatus();
}

void ArkWebUrlResourceResponseImpl::PutResponseDataStatus(bool is_data_ready)
{
    nweb_url_resource_response_->PutResponseDataStatus(is_data_ready);
}

int ArkWebUrlResourceResponseImpl::ResponseStatusCode()
{
    return nweb_url_resource_response_->ResponseStatusCode();
}

ArkWebString ArkWebUrlResourceResponseImpl::ResponseResourceUrl()
{
    return ArkWebStringClassToStruct(nweb_url_resource_response_->ResponseResourceUrl());
}

void ArkWebUrlResourceResponseImpl::PutResponseResourceUrl(const ArkWebString& url)
{
    nweb_url_resource_response_->PutResponseResourceUrl(ArkWebStringStructToClass(url));
}

int ArkWebUrlResourceResponseImpl::ResponseFileHandle()
{
    return nweb_url_resource_response_->ResponseFileHandle();
}

void ArkWebUrlResourceResponseImpl::PutResponseFileHandle(int fd)
{
    nweb_url_resource_response_->PutResponseFileHandle(fd);
}

bool ArkWebUrlResourceResponseImpl::ResponseIsFileHandle()
{
    return nweb_url_resource_response_->ResponseIsFileHandle();
}

void ArkWebUrlResourceResponseImpl::PutResponseStateAndStatuscode(int status_code, const ArkWebString& reason_phrase)
{
    nweb_url_resource_response_->PutResponseStateAndStatuscode(status_code, ArkWebStringStructToClass(reason_phrase));
}

void ArkWebUrlResourceResponseImpl::PutResponseReadyCallback(ArkWebRefPtr<ArkWebResourceReadyCallback> callback)
{
    if (CHECK_REF_PTR_IS_NULL(callback)) {
        nweb_url_resource_response_->PutResponseReadyCallback(nullptr);
        return;
    }

    nweb_url_resource_response_->PutResponseReadyCallback(
        std::make_shared<ArkWebResourceReadyCallbackWrapper>(callback));
}

void ArkWebUrlResourceResponseImpl::PutResponseDataBuffer(char* buffer, size_t bufferSize)
{
    nweb_url_resource_response_->PutResponseDataBuffer(buffer, bufferSize);
}

char* ArkWebUrlResourceResponseImpl::GetResponseDataBuffer()
{
    return nweb_url_resource_response_->GetResponseDataBuffer();
}

size_t ArkWebUrlResourceResponseImpl::GetResponseDataBufferSize()
{
    return nweb_url_resource_response_->GetResponseDataBufferSize();
}

} // namespace OHOS::ArkWeb

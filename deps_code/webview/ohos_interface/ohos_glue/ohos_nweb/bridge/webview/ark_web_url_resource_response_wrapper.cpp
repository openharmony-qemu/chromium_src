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

#include "ohos_nweb/bridge/ark_web_url_resource_response_wrapper.h"

#include "ohos_nweb/bridge/ark_web_resource_ready_callback_impl.h"

#include "base/bridge/ark_web_bridge_macros.h"

namespace OHOS::ArkWeb {

ArkWebUrlResourceResponseWrapper::ArkWebUrlResourceResponseWrapper(
    ArkWebRefPtr<ArkWebUrlResourceResponse> ark_web_url_resource_response)
    : ark_web_url_resource_response_(ark_web_url_resource_response)
{}

std::string ArkWebUrlResourceResponseWrapper::ResponseData()
{
    ArkWebString stData = ark_web_url_resource_response_->ResponseData();

    std::string objData = ArkWebStringStructToClass(stData);
    ArkWebStringStructRelease(stData);
    return objData;
}

void ArkWebUrlResourceResponseWrapper::PutResponseData(const std::string& input_stream)
{
    ArkWebString stData = ArkWebStringClassToStruct(input_stream);

    ark_web_url_resource_response_->PutResponseData(stData);

    ArkWebStringStructRelease(stData);
}

std::string ArkWebUrlResourceResponseWrapper::ResponseStatus()
{
    ArkWebString stStatus = ark_web_url_resource_response_->ResponseStatus();

    std::string objStatus = ArkWebStringStructToClass(stStatus);
    ArkWebStringStructRelease(stStatus);
    return objStatus;
}

std::map<std::string, std::string> ArkWebUrlResourceResponseWrapper::ResponseHeaders()
{
    ArkWebStringMap stHeaders = ark_web_url_resource_response_->ResponseHeaders();

    std::map<std::string, std::string> objHeaders = ArkWebStringMapStructToClass(stHeaders);
    ArkWebStringMapStructRelease(stHeaders);
    return objHeaders;
}

void ArkWebUrlResourceResponseWrapper::PutResponseHeaders(const std::map<std::string, std::string>& response_headers)
{
    ArkWebStringMap stHeaders = ArkWebStringMapClassToStruct(response_headers);

    ark_web_url_resource_response_->PutResponseHeaders(stHeaders);

    ArkWebStringMapStructRelease(stHeaders);
}

ArkWebResponseDataType ArkWebUrlResourceResponseWrapper::ResponseDataType()
{
    return static_cast<ArkWebResponseDataType>(ark_web_url_resource_response_->ResponseDataType());
}

std::string ArkWebUrlResourceResponseWrapper::ResponseMimeType()
{
    ArkWebString stMimeType = ark_web_url_resource_response_->ResponseMimeType();

    std::string objMimeType = ArkWebStringStructToClass(stMimeType);
    ArkWebStringStructRelease(stMimeType);
    return objMimeType;
}

void ArkWebUrlResourceResponseWrapper::PutResponseMimeType(const std::string& mime_type)
{
    ArkWebString stMimeType = ArkWebStringClassToStruct(mime_type);

    ark_web_url_resource_response_->PutResponseMimeType(stMimeType);

    ArkWebStringStructRelease(stMimeType);
}

std::string ArkWebUrlResourceResponseWrapper::ResponseEncoding()
{
    ArkWebString stEncoding = ark_web_url_resource_response_->ResponseEncoding();

    std::string objEncoding = ArkWebStringStructToClass(stEncoding);
    ArkWebStringStructRelease(stEncoding);
    return objEncoding;
}

void ArkWebUrlResourceResponseWrapper::PutResponseEncoding(const std::string& encoding)
{
    ArkWebString stEncoding = ArkWebStringClassToStruct(encoding);

    ark_web_url_resource_response_->PutResponseEncoding(stEncoding);

    ArkWebStringStructRelease(stEncoding);
}

bool ArkWebUrlResourceResponseWrapper::ResponseDataStatus()
{
    return ark_web_url_resource_response_->ResponseDataStatus();
}

void ArkWebUrlResourceResponseWrapper::PutResponseDataStatus(bool is_data_ready)
{
    ark_web_url_resource_response_->PutResponseDataStatus(is_data_ready);
}

int ArkWebUrlResourceResponseWrapper::ResponseStatusCode()
{
    return ark_web_url_resource_response_->ResponseStatusCode();
}

std::string ArkWebUrlResourceResponseWrapper::ResponseResourceUrl()
{
    ArkWebString stUrl = ark_web_url_resource_response_->ResponseResourceUrl();

    std::string objUrl = ArkWebStringStructToClass(stUrl);
    ArkWebStringStructRelease(stUrl);
    return objUrl;
}

void ArkWebUrlResourceResponseWrapper::PutResponseResourceUrl(const std::string& url)
{
    ArkWebString stUrl = ArkWebStringClassToStruct(url);

    ark_web_url_resource_response_->PutResponseResourceUrl(stUrl);

    ArkWebStringStructRelease(stUrl);
}

int ArkWebUrlResourceResponseWrapper::ResponseFileHandle()
{
    return ark_web_url_resource_response_->ResponseFileHandle();
}

void ArkWebUrlResourceResponseWrapper::PutResponseFileHandle(int fd)
{
    ark_web_url_resource_response_->PutResponseFileHandle(fd);
}

bool ArkWebUrlResourceResponseWrapper::ResponseIsFileHandle()
{
    return ark_web_url_resource_response_->ResponseIsFileHandle();
}

void ArkWebUrlResourceResponseWrapper::PutResponseStateAndStatuscode(int status_code, const std::string& reason_phrase)
{
    ArkWebString stReasonPhrase = ArkWebStringClassToStruct(reason_phrase);

    ark_web_url_resource_response_->PutResponseStateAndStatuscode(status_code, stReasonPhrase);

    ArkWebStringStructRelease(stReasonPhrase);
}

void ArkWebUrlResourceResponseWrapper::PutResponseReadyCallback(
    std::shared_ptr<OHOS::NWeb::NWebResourceReadyCallback> callback)
{
    if (CHECK_SHARED_PTR_IS_NULL(callback)) {
        ark_web_url_resource_response_->PutResponseReadyCallback(nullptr);
        return;
    }

    ark_web_url_resource_response_->PutResponseReadyCallback(new ArkWebResourceReadyCallbackImpl(callback));
}

void ArkWebUrlResourceResponseWrapper::PutResponseDataBuffer(char* buffer, size_t bufferSize)
{
    ark_web_url_resource_response_->PutResponseDataBuffer(buffer, bufferSize);
}

char* ArkWebUrlResourceResponseWrapper::GetResponseDataBuffer()
{
    return ark_web_url_resource_response_->GetResponseDataBuffer();
}

size_t ArkWebUrlResourceResponseWrapper::GetResponseDataBufferSize()
{
    return ark_web_url_resource_response_->GetResponseDataBufferSize();
}

} // namespace OHOS::ArkWeb

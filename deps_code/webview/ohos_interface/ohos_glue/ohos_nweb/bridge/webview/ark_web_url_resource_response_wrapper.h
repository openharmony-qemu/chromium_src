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

#ifndef ARK_WEB_URL_RESOURCE_RESPONSE_WRAPPER_H_
#define ARK_WEB_URL_RESOURCE_RESPONSE_WRAPPER_H_
#pragma once

#include "include/nweb_url_resource_response.h"
#include "ohos_nweb/include/ark_web_url_resource_response.h"

namespace OHOS::ArkWeb {

using ArkWebResponseDataType = OHOS::NWeb::NWebResponseDataType;

class ArkWebUrlResourceResponseWrapper : public OHOS::NWeb::NWebUrlResourceResponse {
public:
    ArkWebUrlResourceResponseWrapper(ArkWebRefPtr<ArkWebUrlResourceResponse> ark_web_url_resource_response);
    ~ArkWebUrlResourceResponseWrapper() = default;

    /**
     * @brief get input stream
     *
     * @return inputstream string
     */
    std::string ResponseData() override;

    /**
     * @brief set input stream
     *
     * @param input_stream set inputstream for example: fread(buf, 1, sizeof(buf),
     *        file)
     */
    void PutResponseData(const std::string& input_stream) override;

    /**
     * @brief Get ReasonPhrase
     *
     * @return errorcode reason
     */
    std::string ResponseStatus() override;

    /**
     * @brief Get ResponseHeaders
     *
     * @return response headers
     */
    std::map<std::string, std::string> ResponseHeaders() override;

    /**
     * @brief Set ResponseHeaders
     *
     * @param response_headers response header
     */
    void PutResponseHeaders(const std::map<std::string, std::string>& response_headers) override;

    ArkWebResponseDataType ResponseDataType() override;

    /**
     * @brief Get mimetype
     *
     * @return mimetype The resource response's MIME type
     */
    std::string ResponseMimeType() override;

    /**
     * @brief Construct a resource response with the given parameters.
     *
     * @param mime_type mime_type{ "text/html" }
     */
    void PutResponseMimeType(const std::string& mime_type) override;

    /**
     * @brief get encoding
     *
     * @return encoding the resource response's encoding
     */
    std::string ResponseEncoding() override;

    /**
     * @brief Construct a resource response with the given parameters.
     *
     * @param encoding encoding { "utf-8" }
     */
    void PutResponseEncoding(const std::string& encoding) override;

    bool ResponseDataStatus() override;

    void PutResponseDataStatus(bool is_data_ready) override;

    /**
     * @brief Get status code
     *
     * @return status code
     */
    int ResponseStatusCode() override;

    std::string ResponseResourceUrl() override;

    void PutResponseResourceUrl(const std::string& url) override;

    int ResponseFileHandle() override;

    void PutResponseFileHandle(int fd) override;

    bool ResponseIsFileHandle() override;

    void PutResponseStateAndStatuscode(int status_code, const std::string& reason_phrase) override;

    void PutResponseReadyCallback(std::shared_ptr<OHOS::NWeb::NWebResourceReadyCallback> callback) override;

    void PutResponseDataBuffer(char* buffer, size_t bufferSize) override;

    char* GetResponseDataBuffer() override;

    size_t GetResponseDataBufferSize() override;

private:
    ArkWebRefPtr<ArkWebUrlResourceResponse> ark_web_url_resource_response_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_URL_RESOURCE_RESPONSE_WRAPPER_H_

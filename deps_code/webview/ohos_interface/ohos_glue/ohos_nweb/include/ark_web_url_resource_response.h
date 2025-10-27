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

#ifndef ARK_WEB_URL_RESOURCE_RESPONSE_H_
#define ARK_WEB_URL_RESOURCE_RESPONSE_H_
#pragma once

#include "ohos_nweb/include/ark_web_resource_ready_callback.h"

#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebUrlResourceResponse : public virtual ArkWebBaseRefCounted {
public:
    /**
     * @brief get input stream
     *
     * @return inputstream string
     */
    /*--ark web()--*/
    virtual ArkWebString ResponseData() = 0;

    /**
     * @brief set input stream
     *
     * @param input_stream set inputstream for example: fread(buf, 1, sizeof(buf),
     *        file)
     */
    /*--ark web()--*/
    virtual void PutResponseData(const ArkWebString& input_stream) = 0;

    /**
     * @brief Get ReasonPhrase
     *
     * @return errorcode reason
     */
    /*--ark web()--*/
    virtual ArkWebString ResponseStatus() = 0;

    /**
     * @brief Get ResponseHeaders
     *
     * @return response headers
     */
    /*--ark web()--*/
    virtual const ArkWebStringMap ResponseHeaders() = 0;

    /**
     * @brief Set ResponseHeaders
     *
     * @param response_headers response header
     */
    /*--ark web()--*/
    virtual void PutResponseHeaders(const ArkWebStringMap& response_headers) = 0;

    /*--ark web()--*/
    virtual int32_t ResponseDataType() = 0;

    /**
     * @brief Get mimetype
     *
     * @return mimetype The resource response's MIME type
     */
    /*--ark web()--*/
    virtual ArkWebString ResponseMimeType() = 0;

    /**
     * @brief Construct a resource response with the given parameters.
     *
     * @param mime_type mime_type{ "text/html" }
     */
    /*--ark web()--*/
    virtual void PutResponseMimeType(const ArkWebString& mime_type) = 0;

    /**
     * @brief get encoding
     *
     * @return encoding the resource response's encoding
     */
    /*--ark web()--*/
    virtual ArkWebString ResponseEncoding() = 0;

    /**
     * @brief Construct a resource response with the given parameters.
     *
     * @param encoding encoding { "utf-8" }
     */
    /*--ark web()--*/
    virtual void PutResponseEncoding(const ArkWebString& encoding) = 0;

    /*--ark web()--*/
    virtual bool ResponseDataStatus() = 0;

    /*--ark web()--*/
    virtual void PutResponseDataStatus(bool is_data_ready) = 0;

    /**
     * @brief Get status code
     *
     * @return status code
     */
    /*--ark web()--*/
    virtual int ResponseStatusCode() = 0;

    /*--ark web()--*/
    virtual ArkWebString ResponseResourceUrl() = 0;

    /*--ark web()--*/
    virtual void PutResponseResourceUrl(const ArkWebString& url) = 0;

    /*--ark web()--*/
    virtual int ResponseFileHandle() = 0;

    /*--ark web()--*/
    virtual void PutResponseFileHandle(int fd) = 0;

    /*--ark web()--*/
    virtual bool ResponseIsFileHandle() = 0;

    /**
     * @brief Set StatusCode And ReasonPhrase
     *
     * @param status_code status code
     * @param reasonphrase reason phrase
     */
    /*--ark web()--*/
    virtual void PutResponseStateAndStatuscode(int status_code, const ArkWebString& reason_phrase) = 0;

    /*--ark web()--*/
    virtual void PutResponseReadyCallback(ArkWebRefPtr<ArkWebResourceReadyCallback> callback) = 0;

    /*--ark web()--*/
    virtual void PutResponseDataBuffer(char* buffer, size_t bufferSize) = 0;

    /*--ark web()--*/
    virtual char* GetResponseDataBuffer() = 0;

    /*--ark web()--*/
    virtual size_t GetResponseDataBufferSize() = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_URL_RESOURCE_RESPONSE_H_

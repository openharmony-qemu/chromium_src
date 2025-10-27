/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "webview_scheme_ffi.h"

#include "webview_utils.h"
#include "nweb_helper.h"
#include "nweb_init_params.h"
#include "web_errors.h"
#include "webview_log.h"
#include "parameters.h"
#include "cj_lambda.h"
#include "webview_utils.h"
#include "web_scheme_handler_request.h"
#include "web_scheme_handler_response_impl.h"

using namespace OHOS::FFI;
using namespace OHOS::NWeb;

namespace OHOS {
namespace Webview {
const int32_t MAX_HEADER_SIZE = 200;
extern "C" {
    //WebHttpBodyStream
    int64_t FfiWebHttpBodyStreamConstructor()
    {
        auto nativeWebHttpBodyStream = FFIData::Create<WebHttpBodyStreamImpl>();
        if (nativeWebHttpBodyStream == nullptr) {
            WEBVIEWLOGE("new web http body stream failed");
            return -1;
        }
        return nativeWebHttpBodyStream->GetID();
    }

    void FfiWebHttpBodyStreaminitialize(int64_t id, int32_t *errCode)
    {
        auto nativeWebHttpBodyStream = FFIData::GetData<WebHttpBodyStreamImpl>(id);
        if (nativeWebHttpBodyStream == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return;
        }
        *errCode = nativeWebHttpBodyStream->initialize();
    }

    int32_t FfiWebHttpBodyStreamRead(int32_t bufLen, int64_t id)
    {
        auto nativeWebHttpBodyStream = FFIData::GetData<WebHttpBodyStreamImpl>(id);
        if (nativeWebHttpBodyStream == nullptr) {
            return NWebError::INIT_ERROR;
        }
        if (bufLen <= 0) {
            return NWebError::PARAM_CHECK_ERROR;
        }
        nativeWebHttpBodyStream->Read(bufLen);
        return NWebError::NO_ERROR;
    }

    bool FfiWebHttpBodyStreamIsChunked(int64_t id, int32_t *errCode)
    {
        auto nativeWebHttpBodyStream = FFIData::GetData<WebHttpBodyStreamImpl>(id);
        if (nativeWebHttpBodyStream == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool ischunked = nativeWebHttpBodyStream->IsChunked();
        *errCode = NWebError::NO_ERROR;
        return ischunked;
    }

    bool FfiWebHttpBodyStreamIsEof(int64_t id, int32_t *errCode)
    {
        auto nativeWebHttpBodyStream = FFIData::GetData<WebHttpBodyStreamImpl>(id);
        if (nativeWebHttpBodyStream == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool isEof = nativeWebHttpBodyStream->IsEof();
        *errCode = NWebError::NO_ERROR;
        return isEof;
    }

    bool FfiWebHttpBodyStreamIsInMemory(int64_t id, int32_t *errCode)
    {
        auto nativeWebHttpBodyStream = FFIData::GetData<WebHttpBodyStreamImpl>(id);
        if (nativeWebHttpBodyStream == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool isInMemory = nativeWebHttpBodyStream->IsInMemory();
        *errCode = NWebError::NO_ERROR;
        return isInMemory;
    }

    uint64_t FfiWebHttpBodyStreamGetPostion(int64_t id, int32_t *errCode)
    {
        auto nativeWebHttpBodyStream = FFIData::GetData<WebHttpBodyStreamImpl>(id);
        if (nativeWebHttpBodyStream == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        int64_t postion = static_cast<int64_t>(nativeWebHttpBodyStream->GetPostion());
        *errCode = NWebError::NO_ERROR;
        return postion;
    }

    uint64_t FfiWebHttpBodyStreamGetSize(int64_t id, int32_t *errCode)
    {
        auto nativeWebHttpBodyStream = FFIData::GetData<WebHttpBodyStreamImpl>(id);
        if (nativeWebHttpBodyStream == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        int64_t size = static_cast<int64_t>(nativeWebHttpBodyStream->GetSize());
        *errCode = NWebError::NO_ERROR;
        return size;
    }

    //WebSchemeHandlerRequest
    int64_t FfiWebSchemeHandlerRequestConstructor()
    {
        auto nativeWebSchemeHandlerRequest = FFIData::Create<WebSchemeHandlerRequestImpl>();
        if (nativeWebSchemeHandlerRequest == nullptr) {
            WEBVIEWLOGE("new Web Scheme Handler Request failed");
            return -1;
        }
        return nativeWebSchemeHandlerRequest->GetID();
    }

    int32_t FfiWebSchemeHandlerRequestGetRequestResourceType(int64_t id, int32_t *errCode)
    {
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        int32_t requestResourceType = nativeWebSchemeHandlerRequest->GetRequestResourceType();
        *errCode = NWebError::NO_ERROR;
        return requestResourceType;
    }

    bool FfiWebSchemeHandlerRequestHasGesture(int64_t id, int32_t *errCode)
    {
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool hasGesture = nativeWebSchemeHandlerRequest->HasGesture();
        *errCode = NWebError::NO_ERROR;
        return hasGesture;
    }

    bool FfiWebSchemeHandlerRequestIsMainFrame(int64_t id, int32_t *errCode)
    {
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool isMainFrame = nativeWebSchemeHandlerRequest->IsMainFrame();
        *errCode = NWebError::NO_ERROR;
        return isMainFrame;
    }

    bool FfiWebSchemeHandlerRequestIsRedirect(int64_t id, int32_t *errCode)
    {
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return false;
        }
        bool isRedirect = nativeWebSchemeHandlerRequest->IsRedirect();
        *errCode = NWebError::NO_ERROR;
        return isRedirect;
    }

    RetDataCString FfiWebSchemeHandlerRequestGetRequestUrl(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            return ret;
        }
        std::string requestUrl = nativeWebSchemeHandlerRequest->GetRequestUrl();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(requestUrl);
        return ret;
    }

    RetDataCString FfiWebSchemeHandlerRequestGetMethod(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            return ret;
        }
        std::string method = nativeWebSchemeHandlerRequest->GetMethod();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(method);
        return ret;
    }

    RetDataCString FfiWebSchemeHandlerRequestGetReferrer(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            return ret;
        }
        std::string referrer = nativeWebSchemeHandlerRequest->GetReferrer();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(referrer);
        return ret;
    }

    RetDataCString FfiWebSchemeHandlerRequestGetFrameUrl(int64_t id)
    {
        RetDataCString ret = { .code = NWebError::INIT_ERROR, .data = nullptr };
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            return ret;
        }
        std::string frameUrl = nativeWebSchemeHandlerRequest->GetFrameUrl();
        ret.code = NWebError::NO_ERROR;
        ret.data = MallocCString(frameUrl);
        return ret;
    }

    ArrWebHeader FfiWebSchemeHandlerRequestGetHeader(int64_t id, int32_t *errCode)
    {
        ArrWebHeader ret = { nullptr, 0 };
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return ret;
        }
        std::vector<std::pair<std::string, std::string>> header = nativeWebSchemeHandlerRequest->GetHeader();
        *errCode = NWebError::NO_ERROR;
        size_t headerSize = header.size();
        if (header.empty() || headerSize > MAX_HEADER_SIZE) {
            return ret;
        }
        ret.size = headerSize;
        ret.head = new RetWebHeader[headerSize];
        for (size_t index = 0; index < headerSize; index++) {
            ret.head[index].headerKey = MallocCString(header[index].first.c_str());
            ret.head[index].headerValue = MallocCString(header[index].second.c_str());
        }
        return ret;
    }

    int64_t FfiWebSchemeHandlerRequestGetHttpBodyStream(int64_t id, int32_t *errCode)
    {
        auto nativeWebSchemeHandlerRequest = FFIData::GetData<WebSchemeHandlerRequestImpl>(id);
        if (nativeWebSchemeHandlerRequest == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        ArkWeb_HttpBodyStream* arkWebPostStream = nativeWebSchemeHandlerRequest->GetHttpBodyStream();
        if (!arkWebPostStream) {
            *errCode = NWebError::INIT_ERROR;
            return -1;
        }
        auto nativeWebHttpBodyStream = FFIData::Create<WebHttpBodyStreamImpl>(arkWebPostStream);
        if (nativeWebHttpBodyStream == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            WEBVIEWLOGE("new WebHttpBodyStream failed");
            return -1;
        }
        *errCode = NWebError::NO_ERROR;
        return nativeWebHttpBodyStream->GetID();
    }

    // WebResourceHandler
    int64_t FfiWebResourceHandlerConstructor()
    {
        auto nativeWebResourceHandler = FFIData::Create<WebResourceHandlerImpl>();
        if (nativeWebResourceHandler == nullptr) {
            WEBVIEWLOGE("new Web Resource Handler failed");
            return -1;
        }
        return nativeWebResourceHandler->GetID();
    }

    void FfiWebResourceHandlerDidReceiveResponse(int64_t id, int32_t *errCode, int64_t responceid)
    {
        auto nativeWebResourceHandler = FFIData::GetData<WebResourceHandlerImpl>(id);
        if (nativeWebResourceHandler == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return;
        }
        auto nativeWebSchemeHandlerResponse = FFIData::GetData<WebSchemeHandlerResponseImpl>(responceid);
        if (nativeWebSchemeHandlerResponse == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return;
        }
        int32_t ret = nativeWebResourceHandler->DidReceiveResponse(
            nativeWebSchemeHandlerResponse->GetArkWebResponse());
        if (ret != 0) {
            *errCode = NWebError::RESOURCE_HANDLER_INVALID;
        }
        return;
    }

    void FfiWebResourceHandlerDidReceiveResponseBody(
        int64_t id, int32_t *errCode, CArrUI8 buffer, int64_t buflen)
    {
        auto nativeWebResourceHandler = FFIData::GetData<WebResourceHandlerImpl>(id);
        if (nativeWebResourceHandler == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return;
        }
        int32_t ret = nativeWebResourceHandler->DidReceiveResponseBody(
            buffer.head, static_cast<int64_t>(buflen));
        if (ret != 0) {
            *errCode = NWebError::RESOURCE_HANDLER_INVALID;
        }
        return;
    }

    void FfiWebResourceHandlerDidFinish(int64_t id, int32_t *errCode)
    {
        auto nativeWebResourceHandler = FFIData::GetData<WebResourceHandlerImpl>(id);
        if (nativeWebResourceHandler == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return;
        }
        int32_t ret = nativeWebResourceHandler->DidFinish();
        if (ret != 0) {
            *errCode = NWebError::RESOURCE_HANDLER_INVALID;
        }
        return;
    }

    void FfiWebResourceHandlerDidFail(int64_t id, int32_t *errCode, int32_t errorcode)
    {
        auto nativeWebResourceHandler = FFIData::GetData<WebResourceHandlerImpl>(id);
        if (nativeWebResourceHandler == nullptr) {
            *errCode = NWebError::INIT_ERROR;
            return;
        }
        int32_t ret = nativeWebResourceHandler->DidFailWithError(
            static_cast<ArkWeb_NetError>(errorcode));
        if (ret != 0) {
            *errCode = NWebError::RESOURCE_HANDLER_INVALID;
        }
        return;
    }

    // WebSchemeHandler
    int64_t FfiWebSchemeHandlerConstructor()
    {
        auto nativeWebSchemeHandler = FFIData::Create<WebSchemeHandlerImpl>();
        if (nativeWebSchemeHandler == nullptr) {
            WEBVIEWLOGE("new Web Scheme Handler failed");
            return -1;
        }
        return nativeWebSchemeHandler->GetID();
    }

    void FfiWebSchemeHandlerOnRequestStart(int64_t id, bool (*callback)(int64_t, int64_t))
    {
        auto nativeWebSchemeHandlerImpl = FFIData::GetData<WebSchemeHandlerImpl>(id);
        if (!nativeWebSchemeHandlerImpl) {
            WEBVIEWLOGE("[DOWNLOAD] WebSchemeHandler null");
            return;
        }
        nativeWebSchemeHandlerImpl->PutRequestStart(CJLambda::Create(callback));
    }

    void FfiWebSchemeHandlerOnRequestStop(int64_t id, void (*callback)(int64_t))
    {
        auto nativeWebSchemeHandlerImpl = FFIData::GetData<WebSchemeHandlerImpl>(id);
        if (!nativeWebSchemeHandlerImpl) {
            WEBVIEWLOGE("[DOWNLOAD] WebSchemeHandler is null");
            return;
        }
        nativeWebSchemeHandlerImpl->PutRequestStop(CJLambda::Create(callback));
    }
}
}
}
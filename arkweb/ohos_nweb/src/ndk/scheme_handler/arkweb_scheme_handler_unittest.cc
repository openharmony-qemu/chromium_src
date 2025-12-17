/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "base/logging.h"
#include "cef/include/cef_request.h"
#include "ohos_nweb/src/ndk/scheme_handler/http_body_stream.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#include "ohos_nweb/src/ndk/scheme_handler/response.h"
#include "ohos_nweb/src/ndk/scheme_handler/scheme_handler.h"

using ::testing::_;
using ::testing::Return;

#ifdef __cplusplus
extern "C" {
#endif __cplusplus
int32_t OH_ArkWebSchemeHandler_SetFromEts(ArkWeb_SchemeHandler* scheme_handler,
                                          bool from_ets);
class ArkWeb_SchemeHandler_Test : public testing::Test {
 public:
  void SetUp() {
    list_ = new ArkWeb_RequestHeaderList_(nullptr);
    stream_ = new ArkWeb_HttpBodyStream_(nullptr);
    request_ = new ArkWeb_ResourceRequest_(nullptr);
    response_ = new ArkWeb_Response_();
  }
  void TearDown() {
    if (list_) {
      delete list_;
      list_ = nullptr;
    }
    if (stream_) {
      delete stream_;
      stream_ = nullptr;
    }
    if (request_) {
      delete request_;
      request_ = nullptr;
    }
    if (response_) {
      delete response_;
      response_ = nullptr;
    }
  }
 protected:
  ArkWeb_RequestHeaderList_* list_ = nullptr;
  ArkWeb_HttpBodyStream_* stream_ = nullptr;
  ArkWeb_ResourceRequest_* request_ = nullptr;
  ArkWeb_Response_* response_ = nullptr;
};

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_GetSize_001) {
  uint64_t ret = OH_ArkWebHttpBodyStream_GetSize(nullptr);
  EXPECT_EQ(ret, 0);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_GetSize_002) {
  uint64_t ret = OH_ArkWebHttpBodyStream_GetSize(stream_);
  EXPECT_NE(ret, 0);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebRequestHeaderList_GetSize_001) {
  OH_ArkWebRequestHeaderList_Destroy(nullptr);
  char* key = nullptr;
  char* value = nullptr;
  OH_ArkWebRequestHeaderList_GetHeader(nullptr, 0, &key, &value);
  int32_t ret = OH_ArkWebRequestHeaderList_GetSize(nullptr);
  EXPECT_EQ(ret, -1);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebRequestHeaderList_GetSize_002) {
  char* key = nullptr;
  char* value = nullptr;
  OH_ArkWebRequestHeaderList_GetHeader(list_, 1, &key, &value);
  int32_t ret = OH_ArkWebRequestHeaderList_GetSize(list_);
  EXPECT_NE(ret, -1);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebRequestHeaderList_GetSize_003) {
  char* value = nullptr;
  OH_ArkWebRequestHeaderList_GetHeader(list_, 0, nullptr, &value);
  int32_t ret = OH_ArkWebRequestHeaderList_GetSize(list_);
  EXPECT_NE(ret, -1);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebRequestHeaderList_GetSize_004) {
  char* key = nullptr;
  OH_ArkWebRequestHeaderList_GetHeader(list_, 0, &key, nullptr);
  int32_t ret = OH_ArkWebRequestHeaderList_GetSize(list_);
  EXPECT_NE(ret, -1);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_SetUserData_001) {
  OH_ArkWebResourceRequest_DestroyHttpBodyStream(nullptr);
  char* method = nullptr;
  OH_ArkWebResourceRequest_GetMethod(nullptr, &method);
  OH_ArkWebResourceRequest_GetHttpBodyStream(nullptr, nullptr);
  void* user_data = reinterpret_cast<void*>(1);
  auto ret = OH_ArkWebResourceRequest_SetUserData(nullptr, user_data);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_SetUserData_002) {
  OH_ArkWebResourceRequest_GetMethod(request_, nullptr);
  OH_ArkWebResourceRequest_GetHttpBodyStream(request_, nullptr);
  auto ret = OH_ArkWebResourceRequest_SetUserData(request_, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_SetUserData_003) {
  char* method = nullptr;
  OH_ArkWebResourceRequest_GetMethod(request_, &method);
  OH_ArkWebResourceRequest_GetHttpBodyStream(request_, &stream_);
  int user_data = 1;
  auto ret = OH_ArkWebResourceRequest_SetUserData(request_, &user_data);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_GetUserData_001) {
  auto ret = OH_ArkWebResourceRequest_GetUserData(nullptr);
  EXPECT_EQ(ret, nullptr);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_GetUserData_002) {
  int data = 1;
  request_->user_data = &data;
  auto ret = OH_ArkWebResourceRequest_GetUserData(request_);
  EXPECT_EQ(ret, &data);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceRequest_GetResourceType_001) {
  char* url = nullptr;
  OH_ArkWebResourceRequest_GetUrl(nullptr, &url);
  int32_t ret = OH_ArkWebResourceRequest_GetResourceType(nullptr);
  EXPECT_EQ(ret, -1);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceRequest_GetResourceType_002) {
  OH_ArkWebResourceRequest_GetUrl(request_, nullptr);
  int32_t ret = OH_ArkWebResourceRequest_GetResourceType(request_);
  EXPECT_EQ(ret, -1);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceRequest_GetResourceType_003) {
  char* url = nullptr;
  OH_ArkWebResourceRequest_GetUrl(request_, &url);
  int32_t ret = OH_ArkWebResourceRequest_GetResourceType(request_);
  EXPECT_EQ(ret, -1);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_SetReadCallback_001) {
  OH_ArkWebHttpBodyStream_Read(nullptr, nullptr, 0);
  OH_ArkWebHttpBodyStream_AsyncRead(nullptr, nullptr, 0);
  int32_t ret = OH_ArkWebHttpBodyStream_SetReadCallback(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_SetReadCallback_002) {
  uint8_t buffer[1];
  int buf_len = sizeof(buffer);
  OH_ArkWebHttpBodyStream_Read(stream_, buffer, buf_len);
  OH_ArkWebHttpBodyStream_AsyncRead(stream_, nullptr, -1);
  ArkWeb_HttpBodyStreamReadCallback read_callback =
      [](const ArkWeb_HttpBodyStream* stream, uint8_t* buffer, int bytes_read) {
      };
  int32_t ret = OH_ArkWebHttpBodyStream_SetReadCallback(stream_, nullptr);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_SetReadCallback_003) {
  uint8_t buffer[1];
  int buf_len = sizeof(buffer);
  OH_ArkWebHttpBodyStream_Read(stream_, buffer, buf_len);
  OH_ArkWebHttpBodyStream_AsyncRead(stream_, buffer, 1);
  ArkWeb_HttpBodyStreamReadCallback read_callback =
      [](const ArkWeb_HttpBodyStream* stream, uint8_t* buffer, int bytes_read) {
      };
  int32_t ret = OH_ArkWebHttpBodyStream_SetReadCallback(stream_, nullptr);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebHttpBodyStream_SetAsyncReadCallback_001) {
  OH_ArkWebHttpBodyStream_AsyncRead(nullptr, nullptr, 0);
  int32_t ret = OH_ArkWebHttpBodyStream_SetAsyncReadCallback(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebHttpBodyStream_SetAsyncReadCallback_002) {
  OH_ArkWebHttpBodyStream_AsyncRead(stream_, nullptr, -1);
  ArkWeb_HttpBodyStreamReadCallback async_read_callback =
      [](const ArkWeb_HttpBodyStream* stream, uint8_t* buffer, int bytes_read) {
      };
  int32_t ret = OH_ArkWebHttpBodyStream_SetAsyncReadCallback(
      stream_, async_read_callback);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_SetUserData_001) {
  int32_t ret = OH_ArkWebHttpBodyStream_SetUserData(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_SetUserData_002) {
  void* data = reinterpret_cast<void*>(1);
  int32_t ret = OH_ArkWebHttpBodyStream_SetUserData(stream_, data);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_Init_001) {
  int32_t ret = OH_ArkWebHttpBodyStream_Init(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_GetPosition_001) {
  uint64_t ret = OH_ArkWebHttpBodyStream_GetPosition(nullptr);
  EXPECT_EQ(ret, 0);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_GetPosition_002) {
  uint64_t ret = OH_ArkWebHttpBodyStream_GetPosition(stream_);
  EXPECT_NE(ret, 0);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_IsChunked_001) {
  bool ret = OH_ArkWebHttpBodyStream_IsChunked(nullptr);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_IsChunked_002) {
  bool ret = OH_ArkWebHttpBodyStream_IsChunked(stream_);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_IsEof_001) {
  bool ret = OH_ArkWebHttpBodyStream_IsEof(nullptr);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_IsEof_002) {
  bool ret = OH_ArkWebHttpBodyStream_IsEof(stream_);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_IsInMemory_001) {
  bool ret = OH_ArkWebHttpBodyStream_IsInMemory(nullptr);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebHttpBodyStream_IsInMemory_002) {
  bool ret = OH_ArkWebHttpBodyStream_IsInMemory(stream_);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_Destroy_001) {
  int32_t ret = OH_ArkWebResourceRequest_Destroy(nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_IsRedirect_001) {
  OH_ArkWebResourceRequest_GetReferrer(nullptr, nullptr);
  bool ret = OH_ArkWebResourceRequest_IsRedirect(nullptr);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_IsRedirect_002) {
  char* referrer = nullptr;
  OH_ArkWebResourceRequest_GetReferrer(request_, &referrer);
  bool ret = OH_ArkWebResourceRequest_IsRedirect(request_);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_IsRedirect_003) {
  char* referrer = nullptr;
  OH_ArkWebResourceRequest_GetReferrer(request_, nullptr);
  bool ret = OH_ArkWebResourceRequest_IsRedirect(request_);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_IsMainFrame_001) {
  OH_ArkWebResourceRequest_GetRequestHeaders(nullptr, nullptr);
  bool ret = OH_ArkWebResourceRequest_IsMainFrame(nullptr);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_IsMainFrame_002) {
  OH_ArkWebResourceRequest_GetRequestHeaders(request_, nullptr);
  bool ret = OH_ArkWebResourceRequest_IsMainFrame(request_);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_IsMainFrame_003) {
  ArkWeb_RequestHeaderList* header_list = nullptr;
  OH_ArkWebResourceRequest_GetRequestHeaders(request_, &header_list);
  bool ret = OH_ArkWebResourceRequest_IsMainFrame(request_);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_HasGesture_001) {
  bool ret = OH_ArkWebResourceRequest_HasGesture(nullptr);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceRequest_HasGesture_002) {
  bool ret = OH_ArkWebResourceRequest_HasGesture(request_);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWeb_RegisterCustomSchemes_001) {
  OH_ArkWeb_ReleaseString(nullptr);
  int32_t ret = OH_ArkWeb_RegisterCustomSchemes(nullptr, 0);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWeb_RegisterCustomSchemes_002) {
  int32_t ret = OH_ArkWeb_RegisterCustomSchemes("test", 0);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebServiceWorker_SetSchemeHandler_001) {
  OH_ArkWeb_ReleaseByteArray(nullptr);
  bool ret = OH_ArkWebServiceWorker_SetSchemeHandler(nullptr, nullptr);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebServiceWorker_SetSchemeHandler_002) {
  uint8_t* byte_array = new uint8_t[1];
  OH_ArkWeb_ReleaseByteArray(byte_array);
  bool ret = OH_ArkWebServiceWorker_SetSchemeHandler("test", nullptr);
  EXPECT_FALSE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebServiceWorker_SetSchemeHandler_003) {
  ArkWeb_SchemeHandler scheme_handler = {};
  bool ret = OH_ArkWebServiceWorker_SetSchemeHandler("test", &scheme_handler);
  EXPECT_TRUE(ret);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebServiceWorker_ClearSchemeHandlers_001) {
  int32_t ret = OH_ArkWebServiceWorker_ClearSchemeHandlers();
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWeb_ClearSchemeHandlers_001) {
  OH_ArkWeb_DestroySchemeHandler(nullptr);
  int32_t ret = OH_ArkWeb_ClearSchemeHandlers(nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWeb_ClearSchemeHandlers_002) {
  OH_ArkWeb_CreateSchemeHandler(nullptr);
  int32_t ret = OH_ArkWeb_ClearSchemeHandlers("test");
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWeb_CreateSchemeHandler_001) {
  ArkWeb_SchemeHandler* scheme_handler = nullptr;
  OH_ArkWeb_CreateSchemeHandler(&scheme_handler);
  EXPECT_NE(scheme_handler, nullptr);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_SetUserData_001) {
  int32_t ret = OH_ArkWebSchemeHandler_SetUserData(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_SetUserData_002) {
  ArkWeb_SchemeHandler scheme_handler;
  int32_t ret = OH_ArkWebSchemeHandler_SetUserData(&scheme_handler, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_SetUserData_003) {
  ArkWeb_SchemeHandler scheme_handler;
  void* data = reinterpret_cast<void*>(1);
  int32_t ret = OH_ArkWebSchemeHandler_SetUserData(&scheme_handler, data);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_GetUserData_001) {
  auto ret = OH_ArkWebSchemeHandler_GetUserData(nullptr);
  EXPECT_EQ(ret, nullptr);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_GetUserData_002) {
  ArkWeb_SchemeHandler scheme_handler;
  auto ret = OH_ArkWebSchemeHandler_GetUserData(&scheme_handler);
  EXPECT_EQ(ret, nullptr);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebSchemeHandler_SetOnRequestStart_001) {
  int32_t ret = OH_ArkWebSchemeHandler_SetOnRequestStart(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}
void MockOnRequestStart(const ArkWeb_SchemeHandler* scheme_handler,
                        ArkWeb_ResourceRequest* resource_request,
                        const ArkWeb_ResourceHandler* resource_handler,
                        bool* intercept) {
  *intercept = false;
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebSchemeHandler_SetOnRequestStart_002) {
  ArkWeb_SchemeHandler scheme_handler;
  int32_t ret = OH_ArkWebSchemeHandler_SetOnRequestStart(&scheme_handler,
                                                         MockOnRequestStart);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_SetOnRequestStop_001) {
  int32_t ret = OH_ArkWebSchemeHandler_SetOnRequestStop(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_SetOnRequestStop_002) {
  ArkWeb_OnRequestStop on_request_stop =
      [](const ArkWeb_SchemeHandler* scheme_handler,
         const ArkWeb_ResourceRequest* resource_request) {};
  ArkWeb_SchemeHandler scheme_handler;
  int32_t ret =
      OH_ArkWebSchemeHandler_SetOnRequestStop(&scheme_handler, on_request_stop);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWeb_CreateResponse_001) {
  ArkWeb_Response* response = nullptr;
  OH_ArkWeb_CreateResponse(&response);
  EXPECT_NE(response, nullptr);
  delete response;
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetUrl_001) {
  OH_ArkWeb_CreateResponse(nullptr);
  int32_t ret = OH_ArkWebResponse_SetUrl(nullptr, nullptr);
  char* url = nullptr;
  OH_ArkWebResponse_GetUrl(nullptr, &url);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetUrl_002) {
  OH_ArkWeb_CreateResponse(nullptr);
  int32_t ret = OH_ArkWebResponse_SetUrl(nullptr, nullptr);
  char* url = nullptr;
  OH_ArkWebResponse_GetUrl(response_, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetUrl_003) {
  const char* url = "http://example.com";
  int32_t ret = OH_ArkWebResponse_SetUrl(response_, url);
  char* get_url = nullptr;
  OH_ArkWebResponse_GetUrl(response_, &get_url);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetError_001) {
  int32_t ret = OH_ArkWebResponse_SetError(nullptr, ARKWEB_ERR_IO_PENDING);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetError_002) {
  int32_t ret = OH_ArkWebResponse_SetError(response_, ARKWEB_NET_OK);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_GetError_001) {
  ArkWeb_NetError ret = OH_ArkWebResponse_GetError(nullptr);
  EXPECT_EQ(ret, ARKWEB_ERR_FAILED);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_GetError_002) {
  ArkWeb_NetError ret = OH_ArkWebResponse_GetError(response_);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetStatus_001) {
  int32_t ret = OH_ArkWebResponse_SetStatus(nullptr, 1);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetStatus_002) {
  int32_t ret = OH_ArkWebResponse_SetStatus(response_, 1);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_GetStatus_001) {
  int ret = OH_ArkWebResponse_GetStatus(nullptr);
  EXPECT_EQ(ret, -1);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_GetStatus_002) {
  const ArkWeb_Response* response = new ArkWeb_Response();
  int ret = OH_ArkWebResponse_GetStatus(response);
  EXPECT_EQ(ret, 0);
  delete response;
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetStatusText_001) {
  int32_t ret = OH_ArkWebResponse_SetStatusText(nullptr, nullptr);
  OH_ArkWebResponse_GetStatusText(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetStatusText_002) {
  int32_t ret = OH_ArkWebResponse_SetStatusText(response_, nullptr);
  OH_ArkWebResponse_GetStatusText(response_, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetStatusText_003) {
  int32_t ret = OH_ArkWebResponse_SetStatusText(response_, "test");
  char* status_text = nullptr;
  OH_ArkWebResponse_GetStatusText(response_, &status_text);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetCharset_001) {
  int32_t ret = OH_ArkWebResponse_SetCharset(nullptr, nullptr);
  OH_ArkWebResponse_GetCharset(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetCharset_002) {
  int32_t ret = OH_ArkWebResponse_SetCharset(response_, nullptr);
  OH_ArkWebResponse_GetCharset(response_, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetCharset_003) {
  int32_t ret = OH_ArkWebResponse_SetCharset(response_, "test");
  char* charset = nullptr;
  OH_ArkWebResponse_GetCharset(response_, &charset);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetHeaderByName_001) {
  int32_t ret =
      OH_ArkWebResponse_SetHeaderByName(nullptr, nullptr, nullptr, true);
  OH_ArkWebResponse_GetHeaderByName(nullptr, nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetHeaderByName_002) {
  int32_t ret =
      OH_ArkWebResponse_SetHeaderByName(response_, nullptr, nullptr, true);
  OH_ArkWebResponse_GetHeaderByName(response_, nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetHeaderByName_003) {
  int32_t ret =
      OH_ArkWebResponse_SetHeaderByName(response_, "testName", nullptr, true);
  OH_ArkWebResponse_GetHeaderByName(response_, "testName", nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResponse_SetHeaderByName_004) {
  int32_t ret = OH_ArkWebResponse_SetHeaderByName(response_, "testName",
                                                  "testValue", true);
  char* test_value = nullptr;
  OH_ArkWebResponse_GetHeaderByName(response_, "testName", &test_value);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceHandler_DidReceiveResponse_001) {
  OH_ArkWebResourceHandler_Destroy(nullptr);
  int32_t ret = OH_ArkWebResourceHandler_DidReceiveResponse(nullptr, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceHandler_DidReceiveResponse_002) {
  auto factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme("test");
  auto handler = new ArkWeb_ResourceHandler_(request_, factory, "tag", true);
  int32_t ret = OH_ArkWebResourceHandler_DidReceiveResponse(handler, nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
  delete handler;
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceHandler_DidReceiveResponse_003) {
  auto factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme("test");
  auto handler = new ArkWeb_ResourceHandler_(request_, factory, "tag", true);
  int32_t ret = OH_ArkWebResourceHandler_DidReceiveResponse(handler, response_);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
  delete handler;
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceHandler_DidReceiveData_001) {
  int32_t ret = OH_ArkWebResourceHandler_DidReceiveData(nullptr, nullptr, 1);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceHandler_DidReceiveData_002) {
  auto factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme("test");
  auto handler = new ArkWeb_ResourceHandler_(request_, factory, "tag", true);
  int32_t ret = OH_ArkWebResourceHandler_DidReceiveData(handler, nullptr, 1);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
  delete handler;
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceHandler_DidReceiveData_003) {
  auto factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme("test");
  auto handler = new ArkWeb_ResourceHandler_(request_, factory, "tag", true);
  uint8_t buffer[1] = {0};
  int32_t ret = OH_ArkWebResourceHandler_DidReceiveData(handler, buffer, 1);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
  delete handler;
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceHandler_DidFinish_001) {
  int32_t ret = OH_ArkWebResourceHandler_DidFinish(nullptr);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebResourceHandler_DidFinish_002) {
  auto factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme("test");
  auto handler = new ArkWeb_ResourceHandler_(request_, factory, "tag", true);
  int32_t ret = OH_ArkWebResourceHandler_DidFinish(handler);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
  delete handler;
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceHandler_DidFailWithError_001) {
  int32_t ret =
      OH_ArkWebResourceHandler_DidFailWithError(nullptr, ARKWEB_NET_OK);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceHandler_DidFailWithError_002) {
  auto factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme("test");
  auto handler = new ArkWeb_ResourceHandler_(request_, factory, "tag", true);
  int32_t ret =
      OH_ArkWebResourceHandler_DidFailWithError(handler, ARKWEB_NET_OK);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
  delete handler;
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceHandler_DidFailWithErrorV2_001) {
  int32_t ret = OH_ArkWebResourceHandler_DidFailWithErrorV2(
      nullptr, ARKWEB_NET_OK, false);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test,
       OH_ArkWebResourceHandler_DidFailWithErrorV2_002) {
  auto factory =
      OHOS::NWeb::NWebSchemeHandlerFactory::GetOrCreateForScheme("test");
  auto handler = new ArkWeb_ResourceHandler_(request_, factory, "tag", true);
  int32_t ret = OH_ArkWebResourceHandler_DidFailWithErrorV2(
      handler, ARKWEB_NET_OK, false);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
  delete handler;
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_SetFromEts_001) {
  OH_ArkWeb_ReleaseByteArray(nullptr);
  int32_t ret = OH_ArkWebSchemeHandler_SetFromEts(nullptr, true);
  EXPECT_EQ(ret, ARKWEB_INVALID_PARAM);
}

TEST_F(ArkWeb_SchemeHandler_Test, OH_ArkWebSchemeHandler_SetFromEts_002) {
  uint8_t* byte_array = new uint8_t[1]{0x01};
  OH_ArkWeb_ReleaseByteArray(byte_array);
  ArkWeb_SchemeHandler scheme_handler = {};
  int32_t ret = OH_ArkWebSchemeHandler_SetFromEts(&scheme_handler, true);
  EXPECT_EQ(ret, ARKWEB_NET_OK);
}
#ifdef __cplusplus
}
#endif __cplusplus
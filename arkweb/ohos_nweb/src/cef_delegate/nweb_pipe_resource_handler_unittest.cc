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
#include <gmock/gmock.h>

#include "gtest/gtest.h"

#include "cef/ohos_cef_ext/include/arkweb_request_ext.h"
#define private public
#include "include/cef_response.h"
#include "nweb_pipe_resource_handler.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"

using namespace testing;
using namespace OHOS::NWeb;

class MockCefRequest : public CefRequest {
 public:
  MOCK_METHOD(bool, IsReadOnly, (), (override));
  MOCK_METHOD(CefString, GetURL, (), (override));
  MOCK_METHOD(void, SetURL, (const CefString&), (override));
  MOCK_METHOD(CefString, GetMethod, (), (override));
  MOCK_METHOD(void, SetMethod, (const CefString&), (override));
  MOCK_METHOD(void,
              SetReferrer,
              (const CefString&, ReferrerPolicy),
              (override));
  MOCK_METHOD(CefString, GetReferrerURL, (), (override));
  MOCK_METHOD(ReferrerPolicy, GetReferrerPolicy, (), (override));
  MOCK_METHOD(CefRefPtr<CefPostData>, GetPostData, (), (override));
  MOCK_METHOD(void, SetPostData, (CefRefPtr<CefPostData>), (override));
  MOCK_METHOD(void, GetHeaderMap, (HeaderMap&), (override));
  MOCK_METHOD(void, SetHeaderMap, (const HeaderMap&), (override));
  MOCK_METHOD(CefString, GetHeaderByName, (const CefString&), (override));
  MOCK_METHOD(void,
              SetHeaderByName,
              (const CefString&, const CefString&, bool),
              (override));
  MOCK_METHOD(void,
              Set,
              (const CefString&,
               const CefString&,
               CefRefPtr<CefPostData>,
               const HeaderMap&),
              (override));
  MOCK_METHOD(int, GetFlags, (), (override));
  MOCK_METHOD(void, SetFlags, (int), (override));
  MOCK_METHOD(CefString, GetFirstPartyForCookies, (), (override));
  MOCK_METHOD(void, SetFirstPartyForCookies, (const CefString&), (override));
  MOCK_METHOD(ResourceType, GetResourceType, (), (override));
  MOCK_METHOD(TransitionType, GetTransitionType, (), (override));
  MOCK_METHOD(uint64_t, GetIdentifier, (), (override));
};

class MockCefRequestExt : public ArkWebRequestExt {
 public:
  MOCK_METHOD(bool, IsMainFrame, (), (override));
  MOCK_METHOD(bool, IsRedirect, (), (override));
  MOCK_METHOD(bool, HasUserGesture, (), (override));
  MOCK_METHOD(CefRefPtr<ArkWebCefPostDataStream>, GetUploadStream, (), (override));
};

class MockCefCallback : public CefCallback {
 public:
  MOCK_METHOD(void, Continue, (), (override));
  MOCK_METHOD(void, Cancel, (), (override));
};

class MockCefResourceReadCallback : public CefResourceReadCallback {
 public:
  MOCK_METHOD(void, Continue, (int bytes_read), (override));
};

class MockCefResourceSkipCallback : public CefResourceSkipCallback {
 public:
  MOCK_METHOD(void, Continue, (int64_t bytes_skipped), (override));
};

class MockCefResponse : public CefResponse {
 public:
  MOCK_METHOD(bool, IsReadOnly, (), (override));
  MOCK_METHOD(cef_errorcode_t, GetError, (), (override));
  MOCK_METHOD(void, SetError, (cef_errorcode_t), (override));
  MOCK_METHOD(int, GetStatus, (), (override));
  MOCK_METHOD(void, SetStatus, (int), (override));
  MOCK_METHOD(CefString, GetStatusText, (), (override));
  MOCK_METHOD(void, SetStatusText, (const CefString&), (override));
  MOCK_METHOD(CefString, GetMimeType, (), (override));
  MOCK_METHOD(void, SetMimeType, (const CefString&), (override));
  MOCK_METHOD(CefString, GetCharset, (), (override));
  MOCK_METHOD(void, SetCharset, (const CefString&), (override));
  MOCK_METHOD(CefString, GetHeaderByName, (const CefString&), (override));
  MOCK_METHOD(void,
              SetHeaderByName,
              (const CefString&, const CefString&, bool),
              (override));
  MOCK_METHOD(void, GetHeaderMap, (HeaderMap&), (override));
  MOCK_METHOD(void, SetHeaderMap, (const HeaderMap&), (override));
  MOCK_METHOD(CefString, GetURL, (), (override));
  MOCK_METHOD(void, SetURL, (const CefString&), (override));
};

class NWebPipeResourceHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    CefRefPtr<MockCefRequest> cefRequest;
    ArkWeb_ResourceRequest* resource_request =
        new ArkWeb_ResourceRequest(cefRequest);
    CefRefPtr<OHOS::NWeb::NWebSchemeHandlerFactory> factory;
    std::string web_tag = "test_tag";
    bool from_service_worker = false;
    ArkWeb_ResourceHandler* resource_handler = new ArkWeb_ResourceHandler(
        resource_request, factory, web_tag, from_service_worker);
    handler_ = std::make_shared<NWebPipeResourceHandler>(
        resource_request, resource_handler, factory, web_tag,
        from_service_worker);
  }

  std::shared_ptr<NWebPipeResourceHandler> handler_;
};

TEST_F(NWebPipeResourceHandlerTest, TestOpenWithoutResponse) {
  CefRefPtr<MockCefRequest> request;
  bool handleRequest = true;
  CefRefPtr<MockCefCallback> cefCallback;
  handler_->response_ = nullptr;
  auto ret = handler_->Open(request, handleRequest, cefCallback);
  EXPECT_EQ(ret, true);
}

TEST_F(NWebPipeResourceHandlerTest, ReadTest) {
  void* data_out = nullptr;
  int bytes_to_read = 100;
  int bytes_read = 0;
  CefRefPtr<CefResourceReadCallback> callback;
  handler_->remain_read_ = true;
  auto ret = handler_->Read(data_out, bytes_to_read, bytes_read, callback);
  EXPECT_EQ(ret, false);
}

TEST_F(NWebPipeResourceHandlerTest, TestGetResponseHeadersWithError) {
  CefRefPtr<CefResponse> response = CefResponse::Create();
  handler_->response_ = response;
  response->SetError(ERR_FAILED);
  int64_t response_length = 1;
  CefString redirectUrl;
  handler_->GetResponseHeaders(response, response_length, redirectUrl);
  EXPECT_EQ(response_length, 0);
}

TEST_F(NWebPipeResourceHandlerTest, TestCancel) {
  handler_->Cancel();
  EXPECT_EQ(handler_->canceled_, true);
}

TEST_F(NWebPipeResourceHandlerTest, TestSkip) {
  int64_t bytes_to_skip = 100;
  int64_t bytes_skipped;
  CefRefPtr<MockCefResourceSkipCallback> callback;
  bool ret = handler_->Skip(bytes_to_skip, bytes_skipped, callback);
  EXPECT_EQ(ret, false);
  EXPECT_EQ(bytes_skipped, -2);
}

TEST_F(NWebPipeResourceHandlerTest, TestDidReceiveResponseNullResponse) {
  CefRefPtr<MockCefResponse> nullResponse = nullptr;
  handler_->DidReceiveResponse(nullResponse);
  EXPECT_NE(handler_, nullptr);
}

TEST_F(NWebPipeResourceHandlerTest, TestDidReceiveData) {
  handler_->finished_ = false;
  handler_->finished_with_error_ = false;
  handler_->canceled_ = false;
  uint8_t buffer[] = {1, 2, 3, 4, 5};
  int64_t buf_len = sizeof(buffer);
  handler_->DidReceiveData(buffer, buf_len);
  EXPECT_NE(handler_, nullptr);
}

TEST_F(NWebPipeResourceHandlerTest, TestDidFinish) {
  handler_->remain_read_ = false;
  handler_->DidFinish();
  EXPECT_EQ(handler_->remain_read_, false);
}

TEST_F(NWebPipeResourceHandlerTest, TestDidFailWithError) {
  handler_->remain_read_ = false;
  int errorCode = 1;
  handler_->DidFailWithError(errorCode);
  EXPECT_EQ(handler_->remain_read_, false);
}

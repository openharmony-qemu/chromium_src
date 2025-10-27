/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#include "ohos_nweb/src/ndk/scheme_handler/mock_objects.h"

#include <gmock/gmock.h>

#include "gtest/gtest.h"

using namespace testing;

class ArkWeb_ResourceRequest_Test : public Test {
 protected:
  void SetUp() {
    std::cout << "set up for ArkWeb_ResourceRequest\n";
    _awrrt = std::make_shared<ArkWeb_ResourceRequest_>(nullptr);
  }

  void TearDown() {
    std::cout << "tear down for  ArkWeb_ResourceRequest\n";
    _awrrt.reset();
  }

  std::shared_ptr<ArkWeb_ResourceRequest_> _awrrt;
};

TEST_F(ArkWeb_ResourceRequest_Test,
       ArkWeb_ResourceRequest_Test_GetHttpBodyStream) {
  ArkWeb_HttpBodyStream** null_http_body_stream = nullptr;

  _awrrt->GetHttpBodyStream(null_http_body_stream);
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_GetMethod) {
  char** null_string = nullptr;

  _awrrt->GetMethod(null_string);
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_GetUrl) {
  char** null_url = nullptr;

  _awrrt->GetUrl(null_url);
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_IsRedirect) {
  _awrrt->IsRedirect();
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_IsMainFrame) {
  _awrrt->IsMainFrame();
}

TEST_F(ArkWeb_ResourceRequest_Test,
       ArkWeb_ResourceRequest_Test_HasUserGesture) {
  _awrrt->HasUserGesture();
}

TEST_F(ArkWeb_ResourceRequest_Test, ArkWeb_ResourceRequest_Test_GetReferrer) {
  char** null_string = nullptr;
  _awrrt->GetReferrer(null_string);
}

TEST_F(ArkWeb_ResourceRequest_Test, GetHttpBodyStream_WithNullCefRequest) {
  testing::internal::CaptureStderr();
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(nullptr);
  resource_request->cef_request = nullptr;
  ArkWeb_HttpBodyStream* stream = nullptr;
  resource_request->GetHttpBodyStream(&stream);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler resource request is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_ResourceRequest_Test, GetHttpBodyStream_WithNullUploadStream) {
  testing::internal::CaptureStderr();
  auto mock_request = new MockCefRequest(nullptr);
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  ArkWeb_HttpBodyStream* stream = nullptr;

  resource_request->GetHttpBodyStream(&stream);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler resource request donn't have upload stream."), std::string::npos);
  mock_request->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetHttpBodyStream_NormalExecutionPath) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  resource_request->is_ets_ = false;
  ArkWeb_HttpBodyStream* stream = nullptr;
  resource_request->GetHttpBodyStream(&stream);
  EXPECT_NE(stream, nullptr);
  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetMethod_WithNullCefRequest) {
  testing::internal::CaptureStderr();
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(nullptr);
  resource_request->cef_request = nullptr;
  char* method = nullptr;
  resource_request->GetMethod(&method);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler resource request is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_ResourceRequest_Test, GetMethod_MemoryAllocationFailure) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  mock_request->SetMethodString("GET");

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  char* method = nullptr;
  resource_request->GetMethod(&method);

  EXPECT_STREQ(method, "GET");
  delete[] method;

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetMethod_Strcpy_Failure) {
  StrcpyMock::strcpyMock = true;

  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  mock_request->SetMethodString("GET");

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  char* method = nullptr;
  EXPECT_CALL(StrcpyMock::GetInstance(), strcpy_s(testing::_, testing::_, testing::_))
             .WillRepeatedly(testing::Return(EINVAL));
  resource_request->GetMethod(&method);

  EXPECT_EQ(method, nullptr);

  mock_request->Release();
  mock_post_data_stream->Release();
  StrcpyMock::strcpyMock = false;
}

TEST_F(ArkWeb_ResourceRequest_Test, GetUrl_NormalExecutionPath) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  char* url = nullptr;
  resource_request->GetUrl(&url);
  EXPECT_NE(url, nullptr);
  if (url) {
    delete[] url;
  }

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetUrl_Strcpy_Failure) {
  StrcpyMock::strcpyMock = true;
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  char* url = nullptr;
  EXPECT_CALL(StrcpyMock::GetInstance(), strcpy_s(testing::_, testing::_, testing::_))
             .WillRepeatedly(testing::Return(EINVAL));
  resource_request->GetUrl(&url);
  EXPECT_EQ(url, nullptr);

  mock_request->Release();
  mock_post_data_stream->Release();
  StrcpyMock::strcpyMock = false;
}

TEST_F(ArkWeb_ResourceRequest_Test, IsRedirect_NormalExecutionPath) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  bool result = resource_request->IsRedirect();
  EXPECT_FALSE(result);

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, IsMainFrame_NormalExecutionPath_MainFrame) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  bool result = resource_request->IsMainFrame();
  EXPECT_TRUE(result);

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, HasUserGesture_NormalExecutionPath_WithGesture) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  bool result = resource_request->HasUserGesture();
  EXPECT_FALSE(result);

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetReferrer_NormalExecutionPath) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  mock_request->SetReferrerString("https://example.com");
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  char* referrer = nullptr;
  resource_request->GetReferrer(&referrer);
  EXPECT_STREQ(referrer, "https://example.com");
  delete[] referrer;

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetReferrer_Strcpy_Failure) {
  StrcpyMock::strcpyMock = true;
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  mock_request->SetReferrerString("https://example.com");
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  EXPECT_CALL(StrcpyMock::GetInstance(), strcpy_s(testing::_, testing::_, testing::_))
             .WillRepeatedly(testing::Return(EINVAL));
  char* referrer = nullptr;
  resource_request->GetReferrer(&referrer);
  EXPECT_EQ(referrer, nullptr);

  mock_request->Release();
  mock_post_data_stream->Release();
  StrcpyMock::strcpyMock = false;
}

TEST_F(ArkWeb_ResourceRequest_Test, GetRequestResourceType_WithNullCefRequest) {
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(nullptr);
  resource_request->cef_request = nullptr;

  int32_t result = resource_request->GetRequestResourceType();
  EXPECT_EQ(result, -1);
}

TEST_F(ArkWeb_ResourceRequest_Test, GetRequestResourceType_NormalExecutionPath_MainFrame) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  mock_request->SetResourceType(RT_MAIN_FRAME);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  int32_t result = resource_request->GetRequestResourceType();
  EXPECT_EQ(result, RT_MAIN_FRAME);

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetFrameUrl_WithNullCefRequest) {
  testing::internal::CaptureStderr();
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(nullptr);
  resource_request->cef_request = nullptr;

  char* frame_url = nullptr;
  resource_request->GetFrameUrl(&frame_url);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler resource request is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_ResourceRequest_Test, GetFrameUrl_NormalExecutionPath_EmptyFrameUrl) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  char* frame_url = nullptr;
  resource_request->GetFrameUrl(&frame_url);
  EXPECT_STREQ(frame_url, "");
  delete[] frame_url;

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetFrameUrl_Strcpy_Failure) {
  StrcpyMock::strcpyMock = true;
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;

  char* frame_url = nullptr;
  EXPECT_CALL(StrcpyMock::GetInstance(), strcpy_s(testing::_, testing::_, testing::_))
             .WillRepeatedly(testing::Return(EINVAL));
  resource_request->GetFrameUrl(&frame_url);
  EXPECT_EQ(frame_url, nullptr);

  mock_request->Release();
  mock_post_data_stream->Release();
  StrcpyMock::strcpyMock = false;
}

TEST_F(ArkWeb_ResourceRequest_Test, RequestHeaderList_Constructor_WithNullRequest) {
  ArkWeb_RequestHeaderList_ header_list(nullptr);
  EXPECT_EQ(header_list.GetSize(), 0);
}

TEST_F(ArkWeb_ResourceRequest_Test, RequestHeaderList_Constructor_WithNullCefRequest) {
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(nullptr);
  resource_request->cef_request = nullptr;
  ArkWeb_RequestHeaderList_ header_list(resource_request.get());
  EXPECT_EQ(header_list.GetSize(), 0);
}

TEST_F(ArkWeb_ResourceRequest_Test, RequestHeaderList_Constructor_WithEmptyHeaderList) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  ArkWeb_RequestHeaderList_ header_list(resource_request.get());
  EXPECT_EQ(header_list.GetSize(), 0);
  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, RequestHeaderList_Constructor_WithHeaders) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  mock_request->SetHeaderMap({{"Content-Type", "text/html"}, {"Accept", "*/*"}});
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  ArkWeb_RequestHeaderList_ header_list(resource_request.get());
  EXPECT_EQ(header_list.GetSize(), 2);
  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, RequestHeaderList_Constructor_WithSingleHeader) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  mock_request->SetHeaderMap({{"User-Agent", "TestAgent/1.0"}});
  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  ArkWeb_RequestHeaderList_ header_list(resource_request.get());
  EXPECT_EQ(header_list.GetSize(), 1);
  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetHeader_EmptyHeaderList) {
  testing::internal::CaptureStderr();
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  ArkWeb_RequestHeaderList_ header_list(resource_request.get());

  char* key = nullptr;
  char* value = nullptr;
  header_list.GetHeader(0, &key, &value);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler get header index is invalid "), std::string::npos);
  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetHeader_SingleHeader) {
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  CefRequest::HeaderMap headers;
  headers.insert(std::make_pair("User-Agent", "Test-Agent"));
  mock_request->SetHeaderMap(headers);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  ArkWeb_RequestHeaderList_ header_list(resource_request.get());

  char* key = nullptr;
  char* value = nullptr;
  header_list.GetHeader(0, &key, &value);

  EXPECT_NE(key, nullptr);
  EXPECT_NE(value, nullptr);

  EXPECT_STREQ(key, "User-Agent");
  EXPECT_STREQ(value, "Test-Agent");

  delete[] key;
  delete[] value;

  mock_request->Release();
  mock_post_data_stream->Release();
}

TEST_F(ArkWeb_ResourceRequest_Test, GetHeader_Strcpy_Failure) {
  StrcpyMock::strcpyMock = true;
  auto mock_post_data_stream = new MockPostDataStream();
  auto mock_request = new MockCefRequest(mock_post_data_stream);
  CefRequest::HeaderMap headers;
  headers.insert(std::make_pair("User-Agent", "Test-Agent"));
  mock_request->SetHeaderMap(headers);

  auto resource_request = std::make_shared<ArkWeb_ResourceRequest_>(mock_request);
  resource_request->cef_request = mock_request;
  ArkWeb_RequestHeaderList_ header_list(resource_request.get());

  char* key = nullptr;
  char* value = nullptr;
  EXPECT_CALL(StrcpyMock::GetInstance(), strcpy_s(testing::_, testing::_, testing::_))
             .WillRepeatedly(testing::Return(EINVAL));
  header_list.GetHeader(0, &key, &value);

  EXPECT_EQ(key, nullptr);
  EXPECT_EQ(value, nullptr);

  mock_request->Release();
  mock_post_data_stream->Release();
  StrcpyMock::strcpyMock = false;
}
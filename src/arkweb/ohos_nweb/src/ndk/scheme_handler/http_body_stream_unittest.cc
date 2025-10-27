/*
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "ohos_nweb/src/ndk/scheme_handler/http_body_stream.h"

#include <gmock/gmock.h>

#include "base/logging.h"
#include "cef/include/cef_request.h"
#include "gtest/gtest.h"
#include "ohos_nweb/src/capi/arkweb_scheme_handler.h"
#include "ohos_nweb/src/ndk/scheme_handler/resource_request.h"
#include "ohos_nweb/src/ndk/scheme_handler/mock_objects.h"

using namespace testing;

class ArkWeb_HttpBodyStream_Test : public Test {
 protected:
  void SetUp() {
    std::cout << "set up for ArkWeb_HttpBodyStream_\n";
    _awhbst = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  }

  void TearDown() {
    std::cout << "tear down for  ArkWeb_HttpBodyStream\n";
    _awhbst.reset();
  }

  std::shared_ptr<ArkWeb_HttpBodyStream_> _awhbst;
};
void HttpBodyStreamReadCallbackTest(const ArkWeb_HttpBodyStream* httpBodyStream,
                                    uint8_t* buffer,
                                    int bytesRead) {}
void HttpBodyStreamInitCallbackTest(const ArkWeb_HttpBodyStream* httpBodyStream,
                                    ArkWeb_NetError result) {}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_SetReadCallback) {
  ArkWeb_HttpBodyStreamReadCallback null_read_callback =
      &HttpBodyStreamReadCallbackTest;

  _awhbst->SetReadCallback(null_read_callback);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_Init) {
  ArkWeb_HttpBodyStreamInitCallback HttpBodyStreamInitCallback =
      &HttpBodyStreamInitCallbackTest;
  _awhbst->post_data_stream = nullptr;
  EXPECT_DEATH(_awhbst->Init(HttpBodyStreamInitCallback), ".*");
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_Read) {
  void* TestBuffer = nullptr;
  int64_t buf_len = 0;
  _awhbst->Read(TestBuffer, buf_len);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_GetSize) {
  _awhbst->GetSize();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_GetPosition) {
  _awhbst->GetPosition();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_IsChunked) {
  _awhbst->IsChunked();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_IsEOF) {
  _awhbst->IsEOF();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_IsInMemory) {
  _awhbst->IsInMemory();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_SetUserData) {
  void* TestUserData = (void*)new std::string("test");
  _awhbst->SetUserData(TestUserData);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_GetUserData) {
  void* TestUserData = (void*)new std::string("test");
  _awhbst->SetUserData(TestUserData);
  _awhbst->GetUserData();
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_OnInitComplete) {
  _awhbst->OnInitComplete(0);
}

TEST_F(ArkWeb_HttpBodyStream_Test, ArkWeb_HttpBodyStream_Test_OnReadComplete) {
  _awhbst->OnReadComplete(nullptr, 0);
}

TEST_F(ArkWeb_HttpBodyStream_Test, GetSize_WithValidPostDataStream_ReturnsCorrectSize) {
  auto mock_stream = std::make_shared<MockPostDataStream>();
  const uint64_t expected_size = 1024;

  mock_stream->SetSize(expected_size);
  _awhbst->post_data_stream = mock_stream.get();

  int64_t size = _awhbst->GetSize();
  EXPECT_EQ(static_cast<uint64_t>(size), expected_size);
}

TEST_F(ArkWeb_HttpBodyStream_Test, GetPosition_WithValidPostDataStream_ReturnsCorrectPosition) {
  auto mock_stream = std::make_shared<MockPostDataStream>();
  const uint64_t expected_position = 512;

  mock_stream->SetPosition(expected_position);
  _awhbst->post_data_stream = mock_stream.get();

  int64_t position = _awhbst->GetPosition();
  EXPECT_EQ(static_cast<uint64_t>(position), expected_position);
}

TEST_F(ArkWeb_HttpBodyStream_Test, IsChunked_WithValidPostDataStream_ReturnsCorrectValue) {
  auto mock_stream = std::make_shared<MockPostDataStream>();
  const bool expected_chunked = true;

  mock_stream->SetChunked(expected_chunked);
  _awhbst->post_data_stream = mock_stream.get();

  bool is_chunked = _awhbst->IsChunked();
  EXPECT_EQ(is_chunked, expected_chunked);
}

TEST_F(ArkWeb_HttpBodyStream_Test, IsEOF_WithValidPostDataStream_ReturnsCorrectValue) {
  auto mock_stream = std::make_shared<MockPostDataStream>();
  const bool expected_eof = true;

  mock_stream->SetEOF(expected_eof);
  _awhbst->post_data_stream = mock_stream.get();

  bool is_eof = _awhbst->IsEOF();
  EXPECT_EQ(is_eof, expected_eof);
}

TEST_F(ArkWeb_HttpBodyStream_Test, IsInMemory_WithValidPostDataStream_ReturnsCorrectValue) {
  auto mock_stream = std::make_shared<MockPostDataStream>();
  const bool expected_in_memory = false;

  mock_stream->SetInMemory(expected_in_memory);
  _awhbst->post_data_stream = mock_stream.get();

  bool is_in_memory = _awhbst->IsInMemory();
  EXPECT_EQ(is_in_memory, expected_in_memory);
}

TEST_F(ArkWeb_HttpBodyStream_Test, SetUserData_WithNullptr) {
  _awhbst->SetUserData(nullptr);
  EXPECT_EQ(_awhbst->GetUserData(), nullptr);
}

TEST_F(ArkWeb_HttpBodyStream_Test, Read_WithNegativeBufLen) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  char buffer[10];
  int64_t negative_buf_len = -1;
  stream->Read(buffer, negative_buf_len);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("buf_len must be greater than or equal to 0."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, Read_WithNullPostDataStream) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  void* null_buffer = nullptr;
  int64_t buf_len = -1;
  stream->Read(null_buffer, buf_len);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler read buffer is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, AsyncRead_WithNullBuffer) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  void* null_buffer = nullptr;
  int64_t buf_len = 10;
  stream->AsyncRead(null_buffer, buf_len);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_hadnler read buffer is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, AsyncRead_WithNullPostDataStream) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  stream->post_data_stream = nullptr;
  char buffer[10];
  int64_t buf_len = 10;
  stream->AsyncRead(buffer, buf_len);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler post_data_stream is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, OnInitComplete_WithNullStreamInitCallback) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  stream->stream_init_callback = nullptr;
  stream->OnInitComplete(0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler stream_init_callback is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, OnInitComplete_WithValidCallback) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  ArkWeb_HttpBodyStreamInitCallback test_callback = 
      [](const ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_NetError result) {};
  stream->stream_init_callback = test_callback;
  stream->OnInitComplete(0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("scheme_handler stream_init_callback is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, OnAsyncReadComplete_WithNullPostDataStream) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  stream->post_data_stream = nullptr;
  stream->read_async_callback = nullptr;
  stream->OnAsyncReadComplete(nullptr, 0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler post_data_stream is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, OnAsyncReadComplete_WithNullReadAsyncCallback) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  stream->read_async_callback = nullptr;
  stream->OnAsyncReadComplete(nullptr, 0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler read async callback is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, OnAsyncReadComplete_WithValidCallback) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  ArkWeb_HttpBodyStreamAsyncReadCallback test_callback = 
      [](const ArkWeb_HttpBodyStream* httpBodyStream, uint8_t* buffer, int bytesRead) {};
  stream->read_async_callback = test_callback;
  stream->OnAsyncReadComplete(nullptr, 0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("scheme_handler read async callback is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, OnReadComplete_WithNullReadCallback) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  stream->read_callback = nullptr;
  stream->OnReadComplete(nullptr, 0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_NE(log_output.find("scheme_handler read callback is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, OnReadComplete_NormalExecutionPath) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();

  ArkWeb_HttpBodyStreamReadCallback test_callback = 
      [](const ArkWeb_HttpBodyStream* httpBodyStream, uint8_t* buffer, int bytesRead) {};
  stream->read_callback = test_callback;
  stream->is_ets_ = false;
  stream->OnReadComplete(nullptr, 0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("scheme_handler read callback is nullptr."), std::string::npos);
}

TEST_F(ArkWeb_HttpBodyStream_Test, OnInitComplete_WhenNotOnUITAndIsEtsTrue) {
  testing::internal::CaptureStderr();
  auto stream = std::make_shared<ArkWeb_HttpBodyStream_>(nullptr);
  auto mock_stream = std::make_shared<MockPostDataStream>();
  stream->post_data_stream = mock_stream.get();
  ArkWeb_HttpBodyStreamInitCallback test_callback = 
      [](const ArkWeb_HttpBodyStream* httpBodyStream, ArkWeb_NetError result) {};
  stream->stream_init_callback = test_callback;
  stream->is_ets_ = false;
  stream->OnInitComplete(0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("scheme_handler stream_init_callback is nullptr."), std::string::npos);
}

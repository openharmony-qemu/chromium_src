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

#define private public
#include "arkweb/ohos_adapter_ndk/media_adapter/include/video_encoder_adapter_impl.h"
#undef private

#include "arkweb/chromium_ext/media/base/ohos/codec_config_para_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/graphic_adapter/native_window_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/buffer_info_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/ohos_buffer_adapter_impl.h"
#include "arkweb/ohos_adapter_ndk/media_adapter/codec_format_adapter_impl.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/media_codec_adapter.h"

#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace testing;

namespace OHOS::NWeb {
class EncoderCallbackAdapterMock : public CodecCallbackAdapter {
 public:
  EncoderCallbackAdapterMock() = default;

  ~EncoderCallbackAdapterMock() override = default;

  void OnError(ErrorType errorType, int32_t errorCode) override {}

  void OnStreamChanged(
      const std::shared_ptr<CodecFormatAdapter> formatAdapter_) override {}

  void OnNeedInputData(uint32_t index,
                       std::shared_ptr<OhosBufferAdapter> buffer) override {}
  
  void OnNeedOutputData(uint32_t index,
                        std::shared_ptr<BufferInfoAdapter> info,
                        BufferFlag flag,
                        std::shared_ptr<OhosBufferAdapter> buffer) override {}
};

class VideoEncoderAdapterImplTest : public testing::Test {
 protected:
  void SetUp() {
    codec_adapter_ = std::make_unique<VideoEncoderAdapterImpl>();
    codec_adapter_->CreateVideoCodecByMime("video/avc");
  }

  void TearDown() { codec_adapter_->Release(); }

  std::unique_ptr<VideoEncoderAdapterImpl> codec_adapter_ = nullptr;
};

class EncoderCallbackImplTest : public testing::Test {
 protected:
  void SetUp() {
    std::shared_ptr<CodecCallbackAdapter> callback =
        std::make_shared<EncoderCallbackAdapterMock>();
    encoder_callback_ = std::make_shared<EncoderCallbackImpl>(callback);
  }

  std::shared_ptr<EncoderCallbackImpl> encoder_callback_ = nullptr;
};

TEST_F(VideoEncoderAdapterImplTest, TestCreateVideoCodecByMime001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  CodecCodeAdapter actual_result =
      codec_adapter_->CreateVideoCodecByMime("test");
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestCreateVideoCodecByMime002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::OK;
  CodecCodeAdapter actual_result =
      codec_adapter_->CreateVideoCodecByMime("video/avc");
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestCreateVideoCodecByName001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  CodecCodeAdapter actual_result =
      codec_adapter_->CreateVideoCodecByName("test");
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestCreateVideoCodecByName002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::OK;
  CodecCodeAdapter actual_result =
      codec_adapter_->CreateVideoCodecByName("OMX.hisi.video.encoder.avc");
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestSetCodecCallback001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  CodecCodeAdapter actual_result = codec_adapter_->SetCodecCallback(nullptr);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestSetCodecCallback002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  std::shared_ptr<CodecCallbackAdapter> callback =
      std::make_shared<EncoderCallbackAdapterMock>();
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->SetCodecCallback(callback);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestSetCodecCallback003) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::OK;
  codec_adapter_->callback_index_ = 0;
  std::shared_ptr<CodecCallbackAdapter> callback =
      std::make_shared<EncoderCallbackAdapterMock>();
  CodecCodeAdapter actual_result = codec_adapter_->SetCodecCallback(callback);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestSetCodecCallback004) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::OK;
  codec_adapter_->callback_index_ = 1;
  std::shared_ptr<CodecCallbackAdapter> callback =
      std::make_shared<EncoderCallbackAdapterMock>();
  CodecCodeAdapter actual_result = codec_adapter_->SetCodecCallback(callback);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestSetCodecCallback005) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->callback_index_ = 0;
  codec_adapter_->encoder_ = nullptr;
  std::shared_ptr<CodecCallbackAdapter> callback =
      std::make_shared<EncoderCallbackAdapterMock>();
  CodecCodeAdapter actual_result = codec_adapter_->SetCodecCallback(callback);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestConfigure001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->Configure(nullptr);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestConfigure002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  CodecCodeAdapter actual_result = codec_adapter_->Configure(nullptr);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestConfigure003) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  std::shared_ptr<CodecConfigParaAdapter> config =
      std::make_shared<CodecConfigParaAdapterImpl>();
  CodecCodeAdapter actual_result = codec_adapter_->Configure(config);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestConfigure004) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  std::shared_ptr<CodecConfigParaAdapterImpl> config =
      std::make_shared<CodecConfigParaAdapterImpl>();
  config->SetWidth(1080);
  config->SetHeight(720);
  config->SetBitRate(120);
  config->SetFrameRate(60);
  CodecCodeAdapter actual_result = codec_adapter_->Configure(config);
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestPrepare001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->Prepare();
  EXPECT_EQ(expected_result, actual_result);
}

TEST_F(VideoEncoderAdapterImplTest, TestPrepare002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::OK;
  CodecCodeAdapter actual_result = codec_adapter_->Prepare();
  EXPECT_EQ(expected_result, actual_result);    
}

TEST_F(VideoEncoderAdapterImplTest, TestStart001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->Start();
  EXPECT_EQ(expected_result, actual_result);   
}

TEST_F(VideoEncoderAdapterImplTest, TestStart002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  CodecCodeAdapter actual_result = codec_adapter_->Start();
  EXPECT_EQ(expected_result, actual_result);   
}

TEST_F(VideoEncoderAdapterImplTest, TestStop001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->Stop();
  EXPECT_EQ(expected_result, actual_result);   
}

TEST_F(VideoEncoderAdapterImplTest, TestStop002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  CodecCodeAdapter actual_result = codec_adapter_->Stop();
  EXPECT_EQ(expected_result, actual_result);     
}

TEST_F(VideoEncoderAdapterImplTest, TestReset001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->Reset();
  EXPECT_EQ(expected_result, actual_result);     
}

TEST_F(VideoEncoderAdapterImplTest, TestReset002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::OK;
  CodecCodeAdapter actual_result = codec_adapter_->Reset();
  EXPECT_EQ(expected_result, actual_result);    
}

TEST_F(VideoEncoderAdapterImplTest, TestRelease001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->Release();
  EXPECT_EQ(expected_result, actual_result);      
}

TEST_F(VideoEncoderAdapterImplTest, TestCreateInputSurface001) {
  std::shared_ptr<ProducerSurfaceAdapter> expected_result = nullptr;
  codec_adapter_->encoder_ = nullptr;
  std::shared_ptr<ProducerSurfaceAdapter> actual_result =
      codec_adapter_->CreateInputSurface();
  EXPECT_EQ(expected_result, actual_result);  
}

TEST_F(VideoEncoderAdapterImplTest, TestCreateInputSurface002) {
  std::shared_ptr<ProducerSurfaceAdapter> expected_result = nullptr;
  std::shared_ptr<ProducerSurfaceAdapter> actual_result =
      codec_adapter_->CreateInputSurface();
  EXPECT_EQ(expected_result, actual_result);    
}

TEST_F(VideoEncoderAdapterImplTest, TestReleaseOutputBuffer001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->ReleaseOutputBuffer(0, true);
  EXPECT_EQ(expected_result, actual_result);   
}

TEST_F(VideoEncoderAdapterImplTest, TestReleaseOutputBuffer002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  CodecCodeAdapter actual_result = codec_adapter_->ReleaseOutputBuffer(0, true);
  EXPECT_EQ(expected_result, actual_result);     
}

TEST_F(VideoEncoderAdapterImplTest, TestRequestKeyFrameSoon001) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  codec_adapter_->encoder_ = nullptr;
  CodecCodeAdapter actual_result = codec_adapter_->RequestKeyFrameSoon();
  EXPECT_EQ(expected_result, actual_result);      
}

TEST_F(VideoEncoderAdapterImplTest, TestRequestKeyFrameSoon002) {
  CodecCodeAdapter expected_result = CodecCodeAdapter::ERROR;
  CodecCodeAdapter actual_result = codec_adapter_->RequestKeyFrameSoon();
  EXPECT_EQ(expected_result, actual_result);        
}

TEST_F(VideoEncoderAdapterImplTest, TestGetBufferFlag001) {
  BufferFlag expected_result = BufferFlag::CODEC_BUFFER_FLAG_EOS;
  BufferFlag actual_result = codec_adapter_->GetBufferFlag(
      OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_EOS);
  EXPECT_EQ(expected_result, actual_result);    
}

TEST_F(VideoEncoderAdapterImplTest, TestGetBufferFlag002) {
  BufferFlag expected_result = BufferFlag::CODEC_BUFFER_FLAG_NONE;
  BufferFlag actual_result = codec_adapter_->GetBufferFlag(
      OH_AVCodecBufferFlags::AVCODEC_BUFFER_FLAGS_DISCARD);
  EXPECT_EQ(expected_result, actual_result);    
}

TEST_F(VideoEncoderAdapterImplTest,
       TestOnError_ShouldReturn_WhenUserDataIsNull) {
  void* userData = nullptr;
  ASSERT_NO_FATAL_FAILURE(
    codec_adapter_->OnError(nullptr, 0, userData));
}

TEST_F(VideoEncoderAdapterImplTest,
       TestOnError_ShouldReturn_WhenCallbackIsNull) {
  size_t index = 1;
  void* userData = reinterpret_cast<void*>(index);
  ASSERT_NO_FATAL_FAILURE(
    codec_adapter_->OnError(nullptr, 0, userData));
}

TEST_F(VideoEncoderAdapterImplTest,
       TestOnStreamChanged_ShouldReturn_WhenUserDataIsNull) {
  void* userData = nullptr;
  ASSERT_NO_FATAL_FAILURE(
    codec_adapter_->OnStreamChanged(nullptr, nullptr, userData));
}

TEST_F(VideoEncoderAdapterImplTest,
       TestOnStreamChanged_ShouldReturn_WhenCallbackIsNull) {
  size_t index = 1;
  void* userData = reinterpret_cast<void*>(index);
  ASSERT_NO_FATAL_FAILURE(
    codec_adapter_->OnStreamChanged(nullptr, nullptr, userData));
}

TEST_F(VideoEncoderAdapterImplTest,
       TestOnNeedInputBuffer_ShouldReturn_WhenUserDataIsNull) {
  void* userData = nullptr;
  ASSERT_NO_FATAL_FAILURE(
    codec_adapter_->OnNeedInputBuffer(nullptr, 0, nullptr, userData));
}

TEST_F(VideoEncoderAdapterImplTest,
       TestOnNeedInputBuffer_ShouldReturn_WhenCallbackIsNull) {
  size_t index = 1;
  void* userData = reinterpret_cast<void*>(index);
  ASSERT_NO_FATAL_FAILURE(
    codec_adapter_->OnNeedInputBuffer(nullptr, 0, nullptr, userData));
}

TEST_F(VideoEncoderAdapterImplTest,
       TestOnNewOutputBuffer_ShouldReturn_WhenUserDataIsNull) {
  void* userData = nullptr;
  ASSERT_NO_FATAL_FAILURE(
    codec_adapter_->OnError(nullptr, 0, userData));
}

TEST_F(VideoEncoderAdapterImplTest,
       TestOnNewOutputBuffer_ShouldReturn_WhenCallbackIsNull) {
  size_t index = 1;
  void* userData = reinterpret_cast<void*>(index);
  ASSERT_NO_FATAL_FAILURE(codec_adapter_->OnError(nullptr, 0, userData));
}

TEST_F(EncoderCallbackImplTest, TestOnError001) {
  encoder_callback_->cb_ = nullptr;
  testing::internal::CaptureStderr();
  encoder_callback_->OnError(0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("callback is null"), std::string::npos);
}

TEST_F(EncoderCallbackImplTest, TestOnError002) {
  testing::internal::CaptureStderr();
  encoder_callback_->OnError(0);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("callback is null"), std::string::npos);   
}

TEST_F(EncoderCallbackImplTest, TestOnOutputFormatChanged001) {
  encoder_callback_->cb_ = nullptr;
  testing::internal::CaptureStderr();
  encoder_callback_->OnOutputFormatChanged(nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("callback is null"), std::string::npos);   
}

TEST_F(EncoderCallbackImplTest, TestOnInputBufferAvailable001) {
  encoder_callback_->cb_ = nullptr;
  testing::internal::CaptureStderr();
  encoder_callback_->OnInputBufferAvailable(0, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("callback is null"), std::string::npos);   
}

TEST_F(EncoderCallbackImplTest, TestOnInputBufferAvailable002) {
  testing::internal::CaptureStderr();
  encoder_callback_->OnInputBufferAvailable(0, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("callback is null"), std::string::npos);   
}

TEST_F(EncoderCallbackImplTest, TestOnOutputBufferAvailable001) {
  encoder_callback_->cb_ = nullptr;
  testing::internal::CaptureStderr();
  encoder_callback_->OnOutputBufferAvailable(0, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("callback is null"), std::string::npos);   
}

TEST_F(EncoderCallbackImplTest, TestOnOutputBufferAvailable002) {
  testing::internal::CaptureStderr();
  encoder_callback_->OnOutputBufferAvailable(0, nullptr);
  std::string log_output = testing::internal::GetCapturedStderr();
  EXPECT_EQ(log_output.find("callback is null"), std::string::npos);   
}
} // namespace OHOS::NWeb
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

#include "arkweb/build/features/features.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "ohos_adapter_helper.h"
#define private public
#include "nweb_output_handler.h"

using namespace testing;
namespace OHOS::NWeb {

class NWebOutputHandlerTest : public ::testing::Test {
 protected:
  void SetUp() override {
    handler_ = NWebOutputHandler::Create(100, 100, MockFrameCallback);
    ASSERT_NE(handler_, nullptr);
    handler_->SetNWebId(123);
  }

  std::shared_ptr<NWebOutputHandler> handler_;
  const void* dummy_surface_ = reinterpret_cast<void*>(0x5678);
  static bool MockFrameCallback(const char*, uint32_t, uint32_t) { return true; }
};

TEST_F(NWebOutputHandlerTest, Create_InvalidCallback) {
  auto null_handler = NWebOutputHandler::Create(100, 100, nullptr);
  EXPECT_EQ(null_handler, nullptr);
}

TEST_F(NWebOutputHandlerTest, Create_ValidCallback) {
  EXPECT_NE(handler_, nullptr);
}

TEST_F(NWebOutputHandlerTest, Resize_SameSize) {
  handler_->Resize(100, 100);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 100);
  EXPECT_EQ(h, 100);
}

TEST_F(NWebOutputHandlerTest, Resize_DifferentSize) {
  handler_->Resize(200, 150);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 200);
  EXPECT_EQ(h, 150);
}

TEST_F(NWebOutputHandlerTest, Resize_WithDumpPath_AllocatesBuffer) {
  handler_->SetDumpPath("/tmp/dump");
  handler_->Resize(200, 200);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 200);
  EXPECT_EQ(h, 200);
}

TEST_F(NWebOutputHandlerTest, Resize_WithoutDumpPath_BufferNull) {
  handler_->Resize(0, 0);
  handler_->Resize(200, 200);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 200);
  EXPECT_EQ(h, 200);
}

TEST_F(NWebOutputHandlerTest, Resize_WithoutDumpPath_BufferExists) {
  handler_->SetDumpPath("/tmp/dump");
  handler_->Resize(100, 100);
  handler_->SetDumpPath("");
  handler_->Resize(200, 200);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 200);
  EXPECT_EQ(h, 200);
}

TEST_F(NWebOutputHandlerTest, IsSizeValid_Positive) {
  EXPECT_TRUE(handler_->IsSizeValid());
}

TEST_F(NWebOutputHandlerTest, IsSizeValid_ZeroWidth) {
  auto invalid_handler = NWebOutputHandler::Create(0, 100, MockFrameCallback);
  EXPECT_FALSE(invalid_handler->IsSizeValid());
}

TEST_F(NWebOutputHandlerTest, IsSizeValid_ZeroHeight) {
  auto invalid_handler = NWebOutputHandler::Create(100, 0, MockFrameCallback);
  EXPECT_FALSE(invalid_handler->IsSizeValid());
}

TEST_F(NWebOutputHandlerTest, GetWindowInfo) {
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 100);
  EXPECT_EQ(h, 100);
}

TEST_F(NWebOutputHandlerTest, SetDumpPath_EmptyThenSet) {
  handler_->SetDumpPath("");
  handler_->SetDumpPath("/tmp/dump");
  handler_->StartRenderOutput();
}

TEST_F(NWebOutputHandlerTest, SetNWebId) {
  handler_->SetNWebId(456);
}

TEST_F(NWebOutputHandlerTest, StartRenderOutput_BothFeatures) {
  handler_->SetDumpPath("/tmp");
  handler_->SetFrameInfoDump(true);
  handler_->StartRenderOutput();
}

TEST_F(NWebOutputHandlerTest, StartRenderOutput_NoFeatures) {
  handler_->StartRenderOutput();
}

TEST_F(NWebOutputHandlerTest, FrameStat_EnableDisable) {
  handler_->SetFrameInfoDump(true);
  handler_->StartRenderOutput();
  handler_->SetFrameInfoDump(false);
}

TEST_F(NWebOutputHandlerTest, FrameStat_CalculateFPS) {
  handler_->SetFrameInfoDump(true);
  handler_->StartRenderOutput();
    
  {
    std::unique_lock<std::mutex> lock(handler_->frame_stat_mtx_);
    handler_->local_render_count_ = 10;
    handler_->local_time_base_ = 1000;
    handler_->local_time_curr_ = 2000;
  }
  sleep(2);
}

TEST_F(NWebOutputHandlerTest, Init_CallsResize) {
  auto new_handler = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  new_handler->Init(150, 150);
  uint32_t w, h;
  new_handler->GetWindowInfo(w, h);
  EXPECT_EQ(w, 150);
  EXPECT_EQ(h, 150);
}

TEST_F(NWebOutputHandlerTest, RgbaToRgb_TransparentPixel) {
  const int width = 2;
  const int height = 2;
  const int buf_size = width * height * 4;
  char rgba_buf[buf_size] = {
    255, 0, 0, 255,
    0, 255, 0, 0,
    0, 0, 255, 128,
    255, 255, 255, 255
  };
    
  NWebOutputHandler::BmpDumpHelper helper;
  helper.RgbaToRgb(rgba_buf, width, height);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[0]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[1]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[2]), 255);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[3]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[4]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[5]), 255);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[6]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[7]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[8]), 0);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[9]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[10]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[11]), 255);
}

TEST_F(NWebOutputHandlerTest, RgbaToRgb_NoTransparentPixel) {
  const int width = 2;
  const int height = 2;
  const int buf_size = width * height * 4;
  char rgba_buf[buf_size] = {
    255, 0, 0, 255,
    0, 255, 0, 255,
    0, 0, 255, 255,
    255, 255, 255, 255
  };
    
  NWebOutputHandler::BmpDumpHelper helper;
  helper.RgbaToRgb(rgba_buf, width, height);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[0]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[1]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[2]), 255);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[3]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[4]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[5]), 0);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[6]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[7]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[8]), 0);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[9]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[10]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[11]), 255);
}

TEST_F(NWebOutputHandlerTest, DumpToBmp_FullProcess) {
  const int width = 2;
  const int height = 2;
  const int buf_size = width * height * 4;
  char rgba_buf[buf_size] = {
    255, 0, 0, 255,
    0, 255, 0, 0,
    0, 0, 255, 128,
    255, 255, 255, 255
  };

  NWebOutputHandler::BmpDumpHelper helper;
  helper.DumpToBmp("/tmp/test.bmp", rgba_buf, width, height);
}

TEST_F(NWebOutputHandlerTest, Create_InvalidCallback1) {
  auto null_handler = NWebOutputHandler::Create(100, 100, nullptr);
  EXPECT_EQ(null_handler, nullptr);
}

TEST_F(NWebOutputHandlerTest, Create_ValidCallback1) {
  EXPECT_NE(handler_, nullptr);
}

TEST_F(NWebOutputHandlerTest, Resize_SameSize1) {
  handler_->Resize(100, 100);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 100);
  EXPECT_EQ(h, 100);
}

TEST_F(NWebOutputHandlerTest, Resize_DifferentSize1) {
  handler_->Resize(200, 150);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 200);
  EXPECT_EQ(h, 150);
}

TEST_F(NWebOutputHandlerTest, Resize_WithDumpPath_AllocatesBuffer1) {
  handler_->SetDumpPath("/tmp/dump");
  handler_->Resize(200, 200);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 200);
  EXPECT_EQ(h, 200);
}

TEST_F(NWebOutputHandlerTest, Resize_WithoutDumpPath_BufferNull1) {
  handler_->Resize(0, 0);
  handler_->Resize(200, 200);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 200);
  EXPECT_EQ(h, 200);
}

TEST_F(NWebOutputHandlerTest, Resize_WithoutDumpPath_BufferExists1) {
  handler_->SetDumpPath("/tmp/dump");
  handler_->Resize(100, 100);
  handler_->SetDumpPath("");
  handler_->Resize(200, 200);
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 200);
  EXPECT_EQ(h, 200);
}

TEST_F(NWebOutputHandlerTest, IsSizeValid_Positive1) {
  EXPECT_TRUE(handler_->IsSizeValid());
}

TEST_F(NWebOutputHandlerTest, IsSizeValid_ZeroWidth1) {
  auto invalid_handler = NWebOutputHandler::Create(0, 100, MockFrameCallback);
  EXPECT_FALSE(invalid_handler->IsSizeValid());
}

TEST_F(NWebOutputHandlerTest, IsSizeValid_ZeroHeight1) {
  auto invalid_handler = NWebOutputHandler::Create(100, 0, MockFrameCallback);
  EXPECT_FALSE(invalid_handler->IsSizeValid());
}

TEST_F(NWebOutputHandlerTest, GetWindowInfo1) {
  uint32_t w, h;
  handler_->GetWindowInfo(w, h);
  EXPECT_EQ(w, 100);
  EXPECT_EQ(h, 100);
}

TEST_F(NWebOutputHandlerTest, SetDumpPath_EmptyThenSet1) {
  handler_->SetDumpPath("");
  handler_->SetDumpPath("/tmp/dump");
  handler_->StartRenderOutput();
}

TEST_F(NWebOutputHandlerTest, SetNWebId1) {
  handler_->SetNWebId(456);
}

TEST_F(NWebOutputHandlerTest, StartRenderOutput_BothFeatures1) {
  handler_->SetDumpPath("/tmp");
  handler_->SetFrameInfoDump(true);
  handler_->StartRenderOutput();
}

TEST_F(NWebOutputHandlerTest, StartRenderOutput_NoFeatures1) {
  handler_->StartRenderOutput();
}

TEST_F(NWebOutputHandlerTest, FrameStat_EnableDisable1) {
  handler_->SetFrameInfoDump(true);
  handler_->StartRenderOutput();
  handler_->SetFrameInfoDump(false);
}

TEST_F(NWebOutputHandlerTest, FrameStat_CalculateFPS1) {
  handler_->SetFrameInfoDump(true);
  handler_->StartRenderOutput();
    
  {
    std::unique_lock<std::mutex> lock(handler_->frame_stat_mtx_);
    handler_->local_render_count_ = 10;
    handler_->local_time_base_ = 1000;
    handler_->local_time_curr_ = 2000;
  }
  sleep(2);
}

TEST_F(NWebOutputHandlerTest, Init_CallsResize1) {
  auto new_handler = std::make_shared<NWebOutputHandler>(MockFrameCallback);
  new_handler->Init(150, 150);
  uint32_t w, h;
  new_handler->GetWindowInfo(w, h);
  EXPECT_EQ(w, 150);
  EXPECT_EQ(h, 150);
}

TEST_F(NWebOutputHandlerTest, RgbaToRgb_TransparentPixel1) {
  const int width = 2;
  const int height = 2;
  const int buf_size = width * height * 4;
  char rgba_buf[buf_size] = {
    255, 0, 0, 255,
    0, 255, 0, 0,
    0, 0, 255, 128,
    255, 255, 255, 255
  };
    
  NWebOutputHandler::BmpDumpHelper helper;
  helper.RgbaToRgb(rgba_buf, width, height);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[0]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[1]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[2]), 255);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[3]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[4]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[5]), 255);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[6]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[7]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[8]), 0);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[9]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[10]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[11]), 255);
}

TEST_F(NWebOutputHandlerTest, RgbaToRgb_NoTransparentPixel1) {
  const int width = 2;
  const int height = 2;
  const int buf_size = width * height * 4;
  char rgba_buf[buf_size] = {
    255, 0, 0, 255,
    0, 255, 0, 255,
    0, 0, 255, 255,
    255, 255, 255, 255
  };
    
  NWebOutputHandler::BmpDumpHelper helper;
  helper.RgbaToRgb(rgba_buf, width, height);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[0]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[1]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[2]), 255);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[3]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[4]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[5]), 0);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[6]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[7]), 0);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[8]), 0);

  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[9]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[10]), 255);
  EXPECT_EQ(static_cast<unsigned char>(rgba_buf[11]), 255);
}

TEST_F(NWebOutputHandlerTest, DumpToBmp_FullProcess1) {
  const int width = 2;
  const int height = 2;
  const int buf_size = width * height * 4;
  char rgba_buf[buf_size] = {
    255, 0, 0, 255,
    0, 255, 0, 0,
    0, 0, 255, 128,
    255, 255, 255, 255
  };

  NWebOutputHandler::BmpDumpHelper helper;
  helper.DumpToBmp("/tmp/test.bmp", rgba_buf, width, height);
}

}  // namespace OHOS::NWeb
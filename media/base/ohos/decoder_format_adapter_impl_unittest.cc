#include "gtest/gtest.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "gmock/gmock.h"

#define private public
#include "decoder_format_adapter_impl.h"
#undef private

using namespace testing;
using namespace OHOS::NWeb;


class DecoderFormatAdapterImplTest : public ::testing::Test {
public:
    DecoderFormatAdapterImpl adapter_impl;
};

TEST_F(DecoderFormatAdapterImplTest, GetWidthReturnsCorrectValue) {
  const int32_t kExpectedWidth = 1920;
  adapter_impl.width_ = kExpectedWidth;
  EXPECT_EQ(adapter_impl.GetWidth(), kExpectedWidth);
}

TEST_F(DecoderFormatAdapterImplTest, GetHeightReturnsCorrectValue) {
  const int32_t kExpectedHeight = 1920;
  adapter_impl.height_ = kExpectedHeight;
  EXPECT_EQ(adapter_impl.GetHeight(), kExpectedHeight);
}

TEST_F(DecoderFormatAdapterImplTest, GetFrameRateReturnsCorrectValue) {
  const double kExpectedFrameRate = 1.1;
  adapter_impl.frame_rate_ = kExpectedFrameRate;
  EXPECT_EQ(adapter_impl.GetFrameRate(), kExpectedFrameRate);
}

TEST_F(DecoderFormatAdapterImplTest, SetWidthReturnsCorrectValue) {
  const int32_t kExpectedWidth = 1920;
  adapter_impl.SetWidth(kExpectedWidth);
  EXPECT_EQ(adapter_impl.width_, kExpectedWidth);
}

TEST_F(DecoderFormatAdapterImplTest, SetHeightReturnsCorrectValue) {
  const double kExpectedHeight = 1920;
  adapter_impl.SetHeight(kExpectedHeight);
  EXPECT_EQ(adapter_impl.height_, kExpectedHeight);
}

TEST_F(DecoderFormatAdapterImplTest, SetFrameRateReturnsCorrectValue) {
  const double kExpectedFrameRate = 1.1;
  adapter_impl.SetFrameRate(kExpectedFrameRate);
  EXPECT_EQ(adapter_impl.frame_rate_, kExpectedFrameRate);
}
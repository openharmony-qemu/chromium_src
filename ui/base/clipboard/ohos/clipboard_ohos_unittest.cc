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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "ui/base/clipboard/clipboard_data.h"
#include "ui/base/clipboard/ohos/clipboard_ohos.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wheader-hygiene"
#include "ui/base/clipboard/ohos/clipboard_ohos.cc"
#undef private

namespace ui {
class ClipboardOHOSMock : public ClipboardOHOS {
 public:
  MOCK_METHOD(bool,
              IsFormatAvailable,
              (const ClipboardFormatType& format,
               ClipboardBuffer buffer,
               const DataTransferEndpoint* data_dst),
              (const override));
};

class ClipboardOHOSTest : public ::testing::Test {
 public:
  void SetUp() override {
    clip_board_shared = std::make_shared<ClipboardOHOS>();
    clipboard_internal = std::make_shared<ClipboardOHOSInternal>();
  }

  void TearDown() override {}

  std::shared_ptr<ClipboardOHOS> clip_board_shared;
  ClipboardOHOS clip_board_ohos;
  ClipboardDataBuilder clip_board_data;
  std::shared_ptr<ClipboardOHOSInternal> clipboard_internal;
  ClipboardData board_data;
};

TEST_F(ClipboardOHOSTest, ReadText_001) {
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  std::u16string* result_ = nullptr;
  clip_board_shared->ReadText(buffer_, data_dst_, result_);
  EXPECT_EQ(clip_board_ohos.clipboard_internal_->state_,
            ClipboardOHOSInternal::ClipboardState::kOutOfDate);
}

TEST_F(ClipboardOHOSTest, ReadHTML_001) {
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  std::u16string* markup_ = new std::u16string(u"Example markup");
  std::string* src_url_ = new std::string("src_url_");
  uint32_t* fragment_start = new uint32_t(10);
  uint32_t* fragment_end = new uint32_t(20);
  clip_board_shared->ReadHTML(buffer_, data_dst_, markup_, src_url_,
                              fragment_start, fragment_end);
  EXPECT_EQ(*fragment_start, 0);
  delete markup_;
  delete src_url_;
  delete fragment_start;
  delete fragment_end;
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable_001) {
  ui::ClipboardInternalFormat format_ = ui::ClipboardInternalFormat::kText;
  bool is_available =
      clip_board_ohos.clipboard_internal_->IsFormatAvailable(format_);
  EXPECT_FALSE(is_available);
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable_002) {
  ui::ClipboardInternalFormat format_ = ui::ClipboardInternalFormat::kText;
  clip_board_ohos.clipboard_internal_->state_ =
      ui::ClipboardOHOSInternal::ClipboardState::kUpToDate;
  bool is_available =
      clip_board_ohos.clipboard_internal_->IsFormatAvailable(format_);
  EXPECT_FALSE(is_available);
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable_003) {
  ui::ClipboardInternalFormat format_ = ui::ClipboardInternalFormat::kHtml;
  bool is_available =
      clip_board_ohos.clipboard_internal_->IsFormatAvailable(format_);
  EXPECT_FALSE(is_available);
}

TEST_F(ClipboardOHOSTest, ChangeCopyOptionMode_001) {
  ui::CopyOptionMode copy_option = ui::CopyOptionMode::NONE;
  OHOS::NWeb::CopyOptionMode copy_option_mode =
      clip_board_ohos.clipboard_internal_->ChangeCopyOptionMode(copy_option);
  EXPECT_EQ(copy_option_mode, OHOS::NWeb::CopyOptionMode::NONE);
}

TEST_F(ClipboardOHOSTest, ChangeCopyOptionMode_002) {
  ui::CopyOptionMode copy_option = ui::CopyOptionMode::IN_APP;
  OHOS::NWeb::CopyOptionMode copy_option_mode =
      clip_board_ohos.clipboard_internal_->ChangeCopyOptionMode(copy_option);
  EXPECT_EQ(copy_option_mode, OHOS::NWeb::CopyOptionMode::IN_APP);
}

TEST_F(ClipboardOHOSTest, ChangeCopyOptionMode_003) {
  ui::CopyOptionMode copy_option = ui::CopyOptionMode::LOCAL_DEVICE;
  OHOS::NWeb::CopyOptionMode copy_option_mode =
      clip_board_ohos.clipboard_internal_->ChangeCopyOptionMode(copy_option);
  EXPECT_EQ(copy_option_mode, OHOS::NWeb::CopyOptionMode::LOCAL_DEVICE);
}

TEST_F(ClipboardOHOSTest, ChangeCopyOptionMode_004) {
  ui::CopyOptionMode copy_option = ui::CopyOptionMode::CROSS_DEVICE;
  OHOS::NWeb::CopyOptionMode copy_option_mode =
      clip_board_ohos.clipboard_internal_->ChangeCopyOptionMode(copy_option);
  EXPECT_EQ(copy_option_mode, OHOS::NWeb::CopyOptionMode::CROSS_DEVICE);
}

TEST_F(ClipboardOHOSTest, GetStandardFormats_001) {
  ClipboardBuffer buffer = ClipboardBuffer::kCopyPaste;
  std::unique_ptr<ClipboardOHOSMock> clipboard =
      std::make_unique<ClipboardOHOSMock>();
  EXPECT_CALL(*clipboard, IsFormatAvailable(testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(1));
  std::vector<std::u16string> result =
      clipboard->GetStandardFormats(buffer, nullptr);
  EXPECT_EQ(result.size(), 6);
}

TEST_F(ClipboardOHOSTest, GetStandardFormats_002) {
  ClipboardBuffer buffer = ClipboardBuffer::kCopyPaste;
  std::unique_ptr<ClipboardOHOSMock> clipboard =
      std::make_unique<ClipboardOHOSMock>();
  EXPECT_CALL(*clipboard, IsFormatAvailable(testing::_, testing::_, testing::_))
      .WillRepeatedly(testing::Return(0));
  std::vector<std::u16string> result =
      clipboard->GetStandardFormats(buffer, nullptr);
  EXPECT_EQ(result.size(), 0);
}

TEST_F(ClipboardOHOSTest, GetSource_001) {
  ClipboardBuffer buffer = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* end_point = clip_board_ohos.GetSource(buffer);
  EXPECT_EQ(end_point, nullptr);
}

TEST_F(ClipboardOHOSTest, GetSource_002) {
  ClipboardBuffer buffer = ClipboardBuffer::kCopyPaste;
  std::unique_ptr<ClipboardData> data(new ClipboardData());
  std::unique_ptr<DataTransferEndpoint> src(
      new DataTransferEndpoint(EndpointType::kDefault, true));
  data->set_source(std::move(src));
  clip_board_ohos.clipboard_internal_->WriteData(std::move(data));
  DataTransferEndpoint* end_point = clip_board_ohos.GetSource(buffer);
  EXPECT_NE(end_point, nullptr);
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable01) {
  const ui::ClipboardFormatType format = ClipboardFormatType::PlainTextType();
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  EXPECT_FALSE(clip_board_ohos.IsFormatAvailable(format, buffer_, data_dst_));
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable02) {
  const ui::ClipboardFormatType format = ClipboardFormatType::UrlType();
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  EXPECT_FALSE(clip_board_ohos.IsFormatAvailable(format, buffer_, data_dst_));
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable03) {
  const ui::ClipboardFormatType format = ClipboardFormatType::HtmlType();
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  EXPECT_FALSE(clip_board_ohos.IsFormatAvailable(format, buffer_, data_dst_));
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable04) {
  const ui::ClipboardFormatType format = ClipboardFormatType::BitmapType();
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  EXPECT_FALSE(clip_board_ohos.IsFormatAvailable(format, buffer_, data_dst_));
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable05) {
  const ui::ClipboardFormatType format =
      ClipboardFormatType::WebKitSmartPasteType();
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  EXPECT_FALSE(clip_board_ohos.IsFormatAvailable(format, buffer_, data_dst_));
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable06) {
  const ui::ClipboardFormatType format =
      ClipboardFormatType::WebCustomDataType();
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  EXPECT_FALSE(clip_board_ohos.IsFormatAvailable(format, buffer_, data_dst_));
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable07) {
  const ui::ClipboardFormatType format =
      ClipboardFormatType::WebCustomDataType();
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  clip_board_ohos.clipboard_internal_->data_ =
      std::make_unique<ClipboardData>();
  clip_board_ohos.clipboard_internal_->data_->custom_data_format_ = "123";
  EXPECT_FALSE(clip_board_ohos.IsFormatAvailable(format, buffer_, data_dst_));
}

TEST_F(ClipboardOHOSTest, IsFormatAvailable08) {
  const ui::ClipboardFormatType format =
      ClipboardFormatType::WebCustomDataType();
  ClipboardBuffer buffer_ = ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst_;
  clip_board_ohos.clipboard_internal_->data_ =
      std::make_unique<ClipboardData>();
  clip_board_ohos.clipboard_internal_->data_->custom_data_format_ =
      "chromium/x-web-custom-data";
  bool is_available =
      clip_board_ohos.IsFormatAvailable(format, buffer_, data_dst_);
  EXPECT_TRUE(is_available);
}

TEST_F(ClipboardOHOSTest, GetForCurrentThread_001) {
  auto is_instancing = clip_board_ohos.GetForCurrentThread();
  EXPECT_NE(is_instancing, nullptr);
}

TEST_F(ClipboardOHOSTest, UpdateClipboardData_001) {
  clip_board_ohos.clipboard_internal_->state_ =
      ClipboardOHOSInternal::ClipboardState::kUpToDate;
  OHOS::NWeb::PasteRecordVector record_vector;
  auto read = std::make_shared<ClipboardOhosReadData>(record_vector);
  clip_board_ohos.clipboard_internal_->read_data_ = read;
  clip_board_ohos.clipboard_internal_->UpdateClipboardData();
  EXPECT_NE(clip_board_ohos.clipboard_internal_->read_data_, nullptr);
}

TEST_F(ClipboardOHOSTest, UpdateClipboardData_002) {
  clip_board_ohos.clipboard_internal_->state_ =
      ClipboardOHOSInternal::ClipboardState::kOutOfDate;
  OHOS::NWeb::PasteRecordVector record_vector;
  auto read = std::make_shared<ClipboardOhosReadData>(record_vector);
  clip_board_ohos.clipboard_internal_->read_data_ = read;
  clip_board_ohos.clipboard_internal_->UpdateClipboardData();
  EXPECT_EQ(clip_board_ohos.clipboard_internal_->read_data_, nullptr);
}

TEST_F(ClipboardOHOSTest, IsReadAllowed_001) {
  DataTransferEndpoint* data_dst;
  absl::optional<ClipboardInternalFormat> format =
      ui::ClipboardInternalFormat::kText;
  DataTransferPolicyController::g_data_transfer_policy_controller_ = nullptr;
  bool read_allowed_value =
      clip_board_ohos.clipboard_internal_->IsReadAllowed(data_dst, format);

  EXPECT_TRUE(read_allowed_value);
}

TEST_F(ClipboardOHOSTest, ReadAvailableTypes_001) {
  ClipboardBuffer buffer = ui::ClipboardBuffer::kCopyPaste;
  DataTransferEndpoint* data_dst =
      new DataTransferEndpoint(ui::EndpointType::kUrl, true);
  std::vector<std::u16string>* types = new std::vector<std::u16string>();
  types->push_back(u"Example markup");
  clip_board_shared->ReadAvailableTypes(buffer, data_dst, types);
  ASSERT_NE(types, nullptr);
  delete types;
  delete data_dst;
}

TEST_F(ClipboardOHOSTest, ImageToClipboardColorType_001) {
  SkColorType color_type = kRGBA_8888_SkColorType;
  EXPECT_EQ(ImageToClipboardColorType(color_type),
            ClipBoardImageColorType::COLOR_TYPE_RGBA_8888);
}

TEST_F(ClipboardOHOSTest, ImageToClipboardColorType_002) {
  SkColorType color_type = kBGRA_8888_SkColorType;
  EXPECT_EQ(ImageToClipboardColorType(color_type),
            ClipBoardImageColorType::COLOR_TYPE_BGRA_8888);
}

TEST_F(ClipboardOHOSTest, ImageToClipboardColorType_003) {
  SkColorType color_type = kBGRA_1010102_SkColorType;
  EXPECT_EQ(ImageToClipboardColorType(color_type),
            ClipBoardImageColorType::COLOR_TYPE_UNKNOWN);
}

TEST_F(ClipboardOHOSTest, GetCurrentData_001) {
  const ClipboardData& board_data_ = board_data;
  clip_board_data.current_data_ = new ClipboardData(board_data_);
  EXPECT_EQ(clip_board_data.current_data_, clip_board_data.GetCurrentData());
  delete clip_board_data.current_data_;
}

TEST_F(ClipboardOHOSTest, GetCurrentData_002) {
  auto result = clip_board_data.GetCurrentData();
  ASSERT_NE(nullptr, result);
}

TEST_F(ClipboardOHOSTest, WriteBitmap) {
  SkBitmap bitmap_;
  SkBitmap& bitmap1 = bitmap_;
  clip_board_data.WriteBitmap(bitmap1);
  EXPECT_EQ(board_data.maybe_png_, absl::nullopt);
}

TEST_F(ClipboardOHOSTest, CommitToClipboard) {
  EndpointType test;
  ClipboardOHOSInternal* clipboard = new ClipboardOHOSInternal;
  std::unique_ptr<DataTransferEndpoint> data_src =
      std::make_unique<DataTransferEndpoint>(test);
  clip_board_data.CommitToClipboard(clipboard, std::move(data_src));
  std::unique_ptr<ClipboardData> previous_data =
      std::move(clip_board_ohos.clipboard_internal_->data_);
  EXPECT_TRUE(clipboard->WriteData(ClipboardDataBuilder::TakeCurrentData()));
}

TEST_F(ClipboardOHOSTest, WriteHTML_001) {
  char something1[] = "hello";
  char something2[] = "world";
  const char* markup_data = something1;
  const char* url_data = something2;
  size_t markup_len = 20;
  size_t url_len = 30;
  const CopyOptionMode copy_option = CopyOptionMode::NONE;
  clip_board_data.WriteHTML(markup_data, markup_len, url_data, url_len,
                            copy_option);
  EXPECT_EQ(copy_option, board_data.copy_option());
}

TEST_F(ClipboardOHOSTest, WriteText) {
  char something1[] = "hello";
  const char* text_data = something1;
  size_t text_len = 40;
  const CopyOptionMode copy_option = CopyOptionMode::NONE;
  clip_board_data.WriteText(text_data, text_len, copy_option);
  EXPECT_EQ(copy_option, board_data.copy_option());
}

TEST_F(ClipboardOHOSTest, ImageToClipboardAlphaType001) {
  SkAlphaType alpha_type = kUnknown_SkAlphaType;
  ImageToClipboardAlphaType(alpha_type);
  EXPECT_EQ(ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN,
            ImageToClipboardAlphaType(alpha_type));
}

TEST_F(ClipboardOHOSTest, ImageToClipboardAlphaType002) {
  SkAlphaType alpha_type = kOpaque_SkAlphaType;
  ImageToClipboardAlphaType(alpha_type);
  EXPECT_EQ(ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE,
            ImageToClipboardAlphaType(alpha_type));
}

TEST_F(ClipboardOHOSTest, ImageToClipboardAlphaType003) {
  SkAlphaType alpha_type = kPremul_SkAlphaType;
  ImageToClipboardAlphaType(alpha_type);
  EXPECT_EQ(ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED,
            ImageToClipboardAlphaType(alpha_type));
}

TEST_F(ClipboardOHOSTest, ImageToClipboardAlphaType004) {
  SkAlphaType alpha_type = kUnpremul_SkAlphaType;
  ImageToClipboardAlphaType(alpha_type);
  EXPECT_EQ(ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN,
            ImageToClipboardAlphaType(alpha_type));
}

TEST_F(ClipboardOHOSTest, ReadText001) {
  std::u16string* read_text = nullptr;
  clip_board_ohos.clipboard_internal_->ReadText(read_text);
  EXPECT_EQ(clip_board_ohos.clipboard_internal_->state_,
            ClipboardOHOSInternal::ClipboardState::kOutOfDate);
}

TEST_F(ClipboardOHOSTest, ReadText002) {
  std::u16string* read_text = nullptr;
  clip_board_ohos.clipboard_internal_->read_data_ =
      std::shared_ptr<ClipboardOhosReadData>();
  clip_board_ohos.clipboard_internal_->ReadText(read_text);
  EXPECT_EQ(clip_board_ohos.clipboard_internal_->read_data_, nullptr);
}

TEST_F(ClipboardOHOSTest, ReadText003) {
  std::u16string* read_text = nullptr;
  clip_board_ohos.clipboard_internal_->read_data_ =
      std::shared_ptr<ClipboardOhosReadData>();
  std::shared_ptr<std::string> text_ = std::shared_ptr<std::string>();
  clip_board_ohos.clipboard_internal_->ReadText(read_text);
  EXPECT_EQ(read_text, nullptr);
}

TEST_F(ClipboardOHOSTest, ReadHTML001) {
  std::u16string* markup = new std::u16string(u"Example markup");
  std::string* src_url = nullptr;
  uint32_t* fragment_start = new uint32_t(10);
  uint32_t* fragment_end = new uint32_t(20);
  clip_board_ohos.clipboard_internal_->ReadHTML(markup, src_url, fragment_start,
                                                fragment_end);
  EXPECT_EQ(*fragment_start, 0);
  delete markup;
  delete fragment_start;
  delete fragment_end;
}

TEST_F(ClipboardOHOSTest, ReadHTML002) {
  std::u16string* markup = new std::u16string(u"Example markup");
  std::string* src_url = new std::string("src_url_");
  uint32_t* fragment_start = new uint32_t(10);
  uint32_t* fragment_end = new uint32_t(20);
  clip_board_ohos.clipboard_internal_->ReadHTML(markup, src_url, fragment_start,
                                                fragment_end);
  EXPECT_EQ(*fragment_end, 0);
  delete markup;
  delete src_url;
  delete fragment_start;
  delete fragment_end;
}

TEST_F(ClipboardOHOSTest, ReadHTML003) {
  std::u16string* markup = new std::u16string(u"Example markup");
  std::string* src_url = new std::string("src_url_");
  uint32_t* fragment_start = new uint32_t(10);
  uint32_t* fragment_end = new uint32_t(20);

  clip_board_ohos.clipboard_internal_->read_data_ =
      std::shared_ptr<ClipboardOhosReadData>();
  clip_board_ohos.clipboard_internal_->ReadHTML(markup, src_url, fragment_start,
                                                fragment_end);
  EXPECT_EQ(*fragment_start, 0);
  delete markup;
  delete src_url;
  delete fragment_start;
  delete fragment_end;
}

TEST_F(ClipboardOHOSTest, ReadHTML004) {
  std::u16string* markup = new std::u16string(u"Example markup");
  std::string* src_url = nullptr;
  uint32_t* fragment_start = new uint32_t(10);
  uint32_t* fragment_end = new uint32_t(20);
  std::string htmlString;
  std::string textString;
  clip_board_ohos.clipboard_internal_->read_data_ =
      std::shared_ptr<ClipboardOhosReadData>();
  std::shared_ptr<std::string> html_ =
      std::make_shared<std::string>(htmlString.c_str());
  clip_board_ohos.clipboard_internal_->ReadHTML(markup, src_url, fragment_start,
                                                fragment_end);
  EXPECT_EQ(*fragment_end, static_cast<uint32_t>(markup->length()));
  delete markup;
  delete fragment_start;
  delete fragment_end;
}

TEST_F(ClipboardOHOSTest, ReadHTML005) {
  std::u16string* markup = new std::u16string(u"Example markup");
  std::string* src_url = nullptr;
  uint32_t* fragment_start = new uint32_t(10);
  uint32_t* fragment_end = new uint32_t(20);
  std::string htmlString;
  std::string textString;
  clip_board_ohos.clipboard_internal_->read_data_ =
      std::shared_ptr<ClipboardOhosReadData>();
  std::shared_ptr<std::string> text_ =
      std::make_shared<std::string>(htmlString.c_str());
  clip_board_ohos.clipboard_internal_->ReadHTML(markup, src_url, fragment_start,
                                                fragment_end);
  EXPECT_EQ(*fragment_end, static_cast<uint32_t>(markup->length()));
  delete markup;
  delete src_url;
  delete fragment_start;
  delete fragment_end;
}

TEST_F(ClipboardOHOSTest, AlphaTypeToSkAlphaType_001) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  auto result =
      clip_board_ohos.clipboard_internal_->AlphaTypeToSkAlphaType(imgData);
  EXPECT_EQ(result, SkAlphaType::kUnknown_SkAlphaType);
}

TEST_F(ClipboardOHOSTest, AlphaTypeToSkAlphaType_002) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
  auto result =
      clip_board_ohos.clipboard_internal_->AlphaTypeToSkAlphaType(imgData);
  EXPECT_EQ(result, SkAlphaType::kUnknown_SkAlphaType);
}

TEST_F(ClipboardOHOSTest, AlphaTypeToSkAlphaType_003) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
  imgData->alpha_type_ = ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE;
  auto result =
      clip_board_ohos.clipboard_internal_->AlphaTypeToSkAlphaType(imgData);
  EXPECT_EQ(result, SkAlphaType::kOpaque_SkAlphaType);
}

TEST_F(ClipboardOHOSTest, AlphaTypeToSkAlphaType_004) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
  imgData->alpha_type_ = ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED;
  auto result =
      clip_board_ohos.clipboard_internal_->AlphaTypeToSkAlphaType(imgData);
  EXPECT_EQ(result, SkAlphaType::kPremul_SkAlphaType);
}

TEST_F(ClipboardOHOSTest, AlphaTypeToSkAlphaType_005) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
  imgData->alpha_type_ = ClipBoardImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED;
  auto result =
      clip_board_ohos.clipboard_internal_->AlphaTypeToSkAlphaType(imgData);
  EXPECT_EQ(result, SkAlphaType::kUnpremul_SkAlphaType);
}

TEST_F(ClipboardOHOSTest, AlphaTypeToSkAlphaType_006) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
  imgData->alpha_type_ = static_cast<ClipBoardImageAlphaType>(-1);
  auto result =
      clip_board_ohos.clipboard_internal_->AlphaTypeToSkAlphaType(imgData);
  EXPECT_EQ(result, SkAlphaType::kUnknown_SkAlphaType);
}

TEST_F(ClipboardOHOSTest, PixelFormatToSkColorType_001) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  auto result =
      clip_board_ohos.clipboard_internal_->PixelFormatToSkColorType(imgData);
  EXPECT_EQ(result, SkColorType::kUnknown_SkColorType);
}

TEST_F(ClipboardOHOSTest, PixelFormatToSkColorType_002) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
  imgData->color_type_ = ClipBoardImageColorType::COLOR_TYPE_RGBA_8888;
  auto result =
      clip_board_ohos.clipboard_internal_->PixelFormatToSkColorType(imgData);
  EXPECT_EQ(result, SkColorType::kRGBA_8888_SkColorType);
}

TEST_F(ClipboardOHOSTest, PixelFormatToSkColorType_003) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
  imgData->color_type_ = ClipBoardImageColorType::COLOR_TYPE_BGRA_8888;
  auto result =
      clip_board_ohos.clipboard_internal_->PixelFormatToSkColorType(imgData);
  EXPECT_EQ(result, SkColorType::kBGRA_8888_SkColorType);
}

TEST_F(ClipboardOHOSTest, PixelFormatToSkColorType_004) {
  std::shared_ptr<ClipBoardImageDataAdapterImpl> imgData;
  imgData = std::make_shared<ClipBoardImageDataAdapterImpl>();
  imgData->color_type_ = static_cast<ClipBoardImageColorType>(-1);
  auto result =
      clip_board_ohos.clipboard_internal_->PixelFormatToSkColorType(imgData);
  EXPECT_EQ(result, SkColorType::kUnknown_SkColorType);
}

TEST(ClipboardOHOSInternalTest, DidGetPng001) {
  ClipboardOHOSInternal clipboard;
  std::vector<uint8_t> result;
  bool callbackCalled = false;
  auto callback = base::BindOnce(
      [](bool* called, std::vector<uint8_t>* result,
         const std::vector<uint8_t>& data) {
        *called = true;
        *result = data;
      },
      &callbackCalled, &result);
  clipboard.DidGetPng(std::move(callback), result);
  EXPECT_TRUE(callbackCalled);
  EXPECT_TRUE(result.empty());
}

TEST(ClipboardOHOSInternalTest, DidGetPng002) {
  ClipboardOHOSInternal clipboard;
  std::vector<uint8_t> result{1, 2, 3, 4, 5};
  bool callbackCalled = false;
  auto callback = base::BindOnce(
      [](bool* called, std::vector<uint8_t>* result,
         const std::vector<uint8_t>& data) {
        *called = true;
        *result = data;
      },
      &callbackCalled, &result);
  clipboard.DidGetPng(std::move(callback), result);
  EXPECT_TRUE(callbackCalled);
  EXPECT_FALSE(result.empty());
  EXPECT_EQ(result, (std::vector<uint8_t>{1, 2, 3, 4, 5}));
}

TEST_F(ClipboardOHOSTest, ClipboardOHOSInternal_001) {
  std::string text_data = "Example text";
  ClipboardData data;
  data.set_text(text_data);
  clipboard_internal->data_ = std::make_unique<ClipboardData>(data);
  bool result = clipboard_internal->HasFormat(ClipboardInternalFormat::kText);
  EXPECT_TRUE(result);
}

TEST_F(ClipboardOHOSTest, ClipboardOHOSInternal_002) {
  std::string html_data = "<html>Example</html>";
  ClipboardData data;
  data.set_markup_data(html_data);
  clipboard_internal->data_ = std::make_unique<ClipboardData>(data);
  bool result = clipboard_internal->HasFormat(ClipboardInternalFormat::kText);
  EXPECT_FALSE(result);
}

TEST_F(ClipboardOHOSTest, ClipboardOHOSInternal_003) {
  clipboard_internal->read_data_ = nullptr;
  clipboard_internal->HasFormatInMisc(ClipboardInternalFormat::kText);
  EXPECT_EQ(clipboard_internal->read_data_, nullptr);
}

TEST_F(ClipboardOHOSTest, ClipboardOHOSInternal_005) {
  std::shared_ptr<PasteDataRecordAdapter> record_vector;
  record_vector.reset();
  EXPECT_EQ(record_vector, nullptr);
}

TEST_F(ClipboardOHOSTest, ClipboardOHOSInternal_006) {
  std::shared_ptr<PasteDataRecordAdapter> record_vector =
      PasteDataRecordAdapter::NewRecord("text/html");
  clipboard_internal->HasFormatInMisc(ClipboardInternalFormat::kText);
  EXPECT_NE(record_vector, nullptr);
}

TEST_F(ClipboardOHOSTest, ReadPng_001) {
  ClipboardBuffer buffer = ClipboardBuffer::kCopyPaste;
  const DataTransferEndpoint* data_dst =
      new DataTransferEndpoint(ui::EndpointType::kUrl, true);
  ClipboardOHOS::ReadPngCallback callback = ClipboardOHOS::ReadPngCallback();
  callback = base::DoNothing();
  clip_board_ohos.ReadPng(buffer, data_dst, std::move(callback));
  EXPECT_EQ(clip_board_ohos.clipboard_internal_->IsReadAllowed(
                data_dst, ClipboardInternalFormat::kPng),
            true);
}

}  // namespace ui

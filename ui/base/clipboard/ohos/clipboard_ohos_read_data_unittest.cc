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

#include "third_party/libaom/source/libaom/third_party/vector/vector.h"
#define private public
#include "clipboard_ohos_read_data.h"
#undef private
#include <gtest/gtest.h>

#include <memory>
#include <vector>
#include "base/logging.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/pasteboard_client_adapter.h"

using PasteCustomData = std::map<std::string, std::vector<uint8_t>>;

const std::string SPAN_STRING_TAG = "openharmony.styled-string";
const std::string SPAN_STRING_TAG_ = "test";
namespace ui {
using namespace testing;

class MOCKNWebSpanstringConvertHtmlCallback
    : public OHOS::NWeb::NWebSpanstringConvertHtmlCallback {
 public:
  MOCKNWebSpanstringConvertHtmlCallback() = default;
  ~MOCKNWebSpanstringConvertHtmlCallback() override = default;
  MOCK_METHOD(std::string,
              SpanstringConvertHtml,
              (const std::vector<uint8_t>& content),
              (override));
};

class MOCKPasteDataRecordAdapter : public OHOS::NWeb::PasteDataRecordAdapter {
 public:
  MOCKPasteDataRecordAdapter() {}
  ~MOCKPasteDataRecordAdapter() override {}
  MOCK_METHOD(bool, SetHtmlText, (std::shared_ptr<std::string>), (override));
  MOCK_METHOD(bool, SetPlainText, (std::shared_ptr<std::string>), (override));
  MOCK_METHOD(bool,
              SetImgData,
              (std::shared_ptr<OHOS::NWeb::ClipBoardImageDataAdapter>),
              (override));
  MOCK_METHOD(std::string, GetMimeType, (), (override));
  MOCK_METHOD(std::shared_ptr<std::string>, GetHtmlText, (), (override));
  MOCK_METHOD(std::shared_ptr<std::string>, GetPlainText, (), (override));
  MOCK_METHOD(bool,
              GetImgData,
              (std::shared_ptr<OHOS::NWeb::ClipBoardImageDataAdapter>),
              (override));
  MOCK_METHOD(bool, SetUri, (const std::string&), (override));
  MOCK_METHOD(bool, SetCustomData, (PasteCustomData&), (override));
  MOCK_METHOD(std::shared_ptr<std::string>, GetUri, (), (override));
  MOCK_METHOD(std::shared_ptr<OHOS::NWeb::PasteCustomData>,
              GetCustomData,
              (),
              (override));
};

class ClipboardOhosReadDataTest : public ::testing::Test {
 public:
  ClipboardOhosReadDataTest() : data_(refToMyVector) {}
  OHOS::NWeb::PasteRecordVector test;
  OHOS::NWeb::PasteRecordVector& refToMyVector = test;

 protected:
  ClipboardOhosReadData data_;
};

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_001) {
  OHOS::NWeb::PasteRecordVector test_;
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_002) {
  OHOS::NWeb::PasteRecordVector test_;
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      firstPasteDataRecordAdapter = nullptr;
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      secondPasteDataRecordAdapter = nullptr;
  test_.push_back(firstPasteDataRecordAdapter);
  test_.push_back(secondPasteDataRecordAdapter);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_003) {
  OHOS::NWeb::PasteRecordVector test_;
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      firstPasteDataRecordAdapter = nullptr;
  test_.push_back(firstPasteDataRecordAdapter);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_004) {
  OHOS::NWeb::PasteRecordVector test_;
  auto htmlText = std::make_shared<std::string>("<html>Some HTML text</html>");
  auto plainText = std::make_shared<std::string>("Some plain text");
  auto firstPasteDataRecordAdapter =
      OHOS::NWeb::PasteDataRecordAdapter::NewRecord("text/html", htmlText,
                                                    plainText);
  test_.push_back(firstPasteDataRecordAdapter);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
  EXPECT_EQ(*(myClipboardOhosReadData.html_), *htmlText);
  EXPECT_EQ(*(myClipboardOhosReadData.text_), *plainText);
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_005) {
  OHOS::NWeb::PasteRecordVector test_;
  auto htmlText = std::make_shared<std::string>();
  auto plainText = std::make_shared<std::string>();
  auto firstPasteDataRecordAdapter =
      OHOS::NWeb::PasteDataRecordAdapter::NewRecord("text/html", htmlText,
                                                    plainText);
  test_.push_back(firstPasteDataRecordAdapter);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
  EXPECT_EQ(*(myClipboardOhosReadData.html_), *htmlText);
  EXPECT_EQ(*(myClipboardOhosReadData.text_), *plainText);
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_006) {
  OHOS::NWeb::PasteRecordVector test_;
  auto htmlText = std::make_shared<std::string>("<html>Some HTML text</html>");
  auto plainText = std::make_shared<std::string>();
  auto firstPasteDataRecordAdapter =
      OHOS::NWeb::PasteDataRecordAdapter::NewRecord("text/html", htmlText,
                                                    plainText);
  test_.push_back(firstPasteDataRecordAdapter);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
  EXPECT_EQ(*(myClipboardOhosReadData.html_), *htmlText);
  EXPECT_EQ(*(myClipboardOhosReadData.text_), *plainText);
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_007) {
  OHOS::NWeb::PasteRecordVector test_;
  auto htmlText = std::make_shared<std::string>();
  auto plainText = std::make_shared<std::string>("Some plain text");
  auto firstPasteDataRecordAdapter =
      OHOS::NWeb::PasteDataRecordAdapter::NewRecord("text/html", htmlText,
                                                    plainText);
  test_.push_back(firstPasteDataRecordAdapter);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
  EXPECT_EQ(*(myClipboardOhosReadData.html_), *htmlText);
  EXPECT_EQ(*(myClipboardOhosReadData.text_), *plainText);
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_008) {
  OHOS::NWeb::PasteRecordVector test_;
  auto htmlText = std::make_shared<std::string>("<html>Some HTML text</html>");
  auto plainText = std::make_shared<std::string>("Some plain text");
  auto firstPasteDataRecordAdapter =
      OHOS::NWeb::PasteDataRecordAdapter::NewRecord("text/html", htmlText,
                                                    plainText);
  test_.push_back(firstPasteDataRecordAdapter);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
  EXPECT_EQ(*(myClipboardOhosReadData.html_), *htmlText);
  EXPECT_EQ(*(myClipboardOhosReadData.text_), *plainText);
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_009) {
  OHOS::NWeb::PasteRecordVector test_;
  std::shared_ptr<MOCKPasteDataRecordAdapter> paste_data_record_adapter_ =
      std::make_shared<MOCKPasteDataRecordAdapter>();
  std::map<std::string, std::vector<uint8_t>> customData;
  customData[SPAN_STRING_TAG] = {
      0x3C, 0x73, 0x70, 0x61, 0x6E, 0x3E, 0x53, 0x74, 0x79, 0x6C, 0x65, 0x64,
      0x20, 0x74, 0x65, 0x78, 0x74, 0x3C, 0x2F, 0x73, 0x70, 0x61, 0x6E, 0x3E};
  std::shared_ptr<OHOS::NWeb::PasteCustomData> sharedCustomData =
      std::make_shared<OHOS::NWeb::PasteCustomData>(customData);
  EXPECT_CALL(*paste_data_record_adapter_, GetCustomData())
      .WillOnce(Return(sharedCustomData));
  EXPECT_CALL(*paste_data_record_adapter_, GetHtmlText())
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*paste_data_record_adapter_, GetPlainText())
      .WillOnce(Return(nullptr));
  test_.push_back(paste_data_record_adapter_);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
  EXPECT_EQ(*(myClipboardOhosReadData.html_), "");
  EXPECT_EQ(*(myClipboardOhosReadData.text_), "");
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_010) {
  OHOS::NWeb::PasteRecordVector test_;
  std::shared_ptr<MOCKPasteDataRecordAdapter> paste_data_record_adapter_ =
      std::make_shared<MOCKPasteDataRecordAdapter>();
  std::map<std::string, std::vector<uint8_t>> customData;
  customData[SPAN_STRING_TAG_] = {
      0x3C, 0x73, 0x70, 0x61, 0x6E, 0x3E, 0x53, 0x74, 0x79, 0x6C, 0x65, 0x64,
      0x20, 0x74, 0x65, 0x78, 0x74, 0x3C, 0x2F, 0x73, 0x70, 0x61, 0x6E, 0x3E};
  std::shared_ptr<OHOS::NWeb::PasteCustomData> sharedCustomData =
      std::make_shared<OHOS::NWeb::PasteCustomData>(customData);
  EXPECT_CALL(*paste_data_record_adapter_, GetCustomData())
      .WillOnce(Return(sharedCustomData));
  EXPECT_CALL(*paste_data_record_adapter_, GetHtmlText())
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*paste_data_record_adapter_, GetPlainText())
      .WillOnce(Return(nullptr));
  test_.push_back(paste_data_record_adapter_);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
  EXPECT_EQ(*(myClipboardOhosReadData.html_), "");
  EXPECT_EQ(*(myClipboardOhosReadData.text_), "");
}

TEST_F(ClipboardOhosReadDataTest, ClipboardOhosReadData_011) {
  OHOS::NWeb::PasteRecordVector test_;
  std::string expected_string = "24";
  std::shared_ptr<MOCKPasteDataRecordAdapter> paste_data_record_adapter_ =
      std::make_shared<MOCKPasteDataRecordAdapter>();
  std::map<std::string, std::vector<uint8_t>> customData;
  customData[SPAN_STRING_TAG] = {
      0x3C, 0x73, 0x70, 0x61, 0x6E, 0x3E, 0x53, 0x74, 0x79, 0x6C, 0x65, 0x64,
      0x20, 0x74, 0x65, 0x78, 0x74, 0x3C, 0x2F, 0x73, 0x70, 0x61, 0x6E, 0x3E};
  std::vector<uint8_t> expected_content = {
      0x3C, 0x73, 0x70, 0x61, 0x6E, 0x3E, 0x53, 0x74, 0x79, 0x6C, 0x65, 0x64,
      0x20, 0x74, 0x65, 0x78, 0x74, 0x3C, 0x2F, 0x73, 0x70, 0x61, 0x6E, 0x3E};
  std::shared_ptr<OHOS::NWeb::PasteCustomData> sharedCustomData =
      std::make_shared<OHOS::NWeb::PasteCustomData>(customData);
  auto convert_html_callback =
      std::make_shared<MOCKNWebSpanstringConvertHtmlCallback>();
  EXPECT_CALL(*convert_html_callback, SpanstringConvertHtml(expected_content))
      .WillOnce(Return(expected_string));
  ClipboardOhosReadData::convert_html_callback_ =
      std::move(convert_html_callback);
  EXPECT_CALL(*paste_data_record_adapter_, GetCustomData())
      .WillOnce(Return(sharedCustomData));
  EXPECT_CALL(*paste_data_record_adapter_, GetHtmlText())
      .WillOnce(Return(nullptr));
  EXPECT_CALL(*paste_data_record_adapter_, GetPlainText())
      .WillOnce(Return(nullptr));
  test_.push_back(paste_data_record_adapter_);
  OHOS::NWeb::PasteRecordVector& refToMyVector_ = test_;
  ClipboardOhosReadData myClipboardOhosReadData(refToMyVector_);
  EXPECT_NE(myClipboardOhosReadData.html_, nullptr);
  EXPECT_NE(myClipboardOhosReadData.text_, nullptr);
  EXPECT_EQ(*(myClipboardOhosReadData.html_), "24");
  EXPECT_EQ(*(myClipboardOhosReadData.text_), "");
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST001) {
  std::shared_ptr<std::string> result = nullptr;
  data_.html_ = nullptr;
  EXPECT_EQ(data_.ReadHtml(), result);
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST002) {
  std::shared_ptr<std::string> html;
  data_.html_ = html;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(result, html);
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST003) {
  data_.is_in_app_ = true;
  data_.has_been_read_html_ = true;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(data_.has_been_read_html_, true);
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST004) {
  data_.is_in_app_ = true;
  data_.has_been_read_html_ = false;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(data_.has_been_read_html_, false);
}
TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST005) {
  data_.is_in_app_ = false;
  data_.has_been_read_html_ = true;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(data_.has_been_read_html_, true);
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST006) {
  data_.is_in_app_ = false;
  data_.has_been_read_html_ = false;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(data_.has_been_read_html_, false);
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST007) {
  data_.record_vector_.clear();
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      firstPasteDataRecordAdapter = nullptr;
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      secondPasteDataRecordAdapter = nullptr;
  data_.record_vector_.push_back(firstPasteDataRecordAdapter);
  data_.record_vector_.push_back(secondPasteDataRecordAdapter);
  data_.is_in_app_ = true;
  data_.has_been_read_html_ = true;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(data_.has_been_read_html_, true);
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST008) {
  data_.record_vector_.clear();
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      firstPasteDataRecordAdapter = nullptr;
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      secondPasteDataRecordAdapter = nullptr;
  data_.record_vector_.push_back(firstPasteDataRecordAdapter);
  data_.record_vector_.push_back(secondPasteDataRecordAdapter);
  data_.is_in_app_ = true;
  data_.has_been_read_html_ = false;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(data_.has_been_read_html_, false);
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST009) {
  data_.record_vector_.clear();
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      firstPasteDataRecordAdapter = nullptr;
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      secondPasteDataRecordAdapter = nullptr;
  data_.record_vector_.push_back(firstPasteDataRecordAdapter);
  data_.record_vector_.push_back(secondPasteDataRecordAdapter);
  data_.is_in_app_ = false;
  data_.has_been_read_html_ = true;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(data_.has_been_read_html_, true);
}

TEST_F(ClipboardOhosReadDataTest, ReadHtml_TEST010) {
  data_.record_vector_.clear();
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      firstPasteDataRecordAdapter = nullptr;
  std::shared_ptr<OHOS::NWeb::PasteDataRecordAdapter>
      secondPasteDataRecordAdapter = nullptr;
  data_.record_vector_.push_back(firstPasteDataRecordAdapter);
  data_.record_vector_.push_back(secondPasteDataRecordAdapter);
  data_.is_in_app_ = false;
  data_.has_been_read_html_ = false;
  std::shared_ptr<std::string> result = data_.ReadHtml();
  EXPECT_EQ(data_.has_been_read_html_, true);
}
}  // namespace ui

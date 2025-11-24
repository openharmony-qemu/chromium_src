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

#include <cstdint>
#include <cstring>
#include <memory>
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "arkweb/ohos_nweb/src/nweb_hilog.h"
#include <database/udmf/udmf_err_code.h>
#include <database/udmf/udmf_meta.h>
#include <database/pasteboard/oh_pasteboard_err_code.h>
#include "ohos_sdk/openharmony/native/sysroot/usr/include/AbilityKit/ability_runtime/ability_runtime_common.h"
#include "arkweb/ohos_adapter_ndk/mock_ndk_api/include/mock_ndk_api_new.h"
#define private public
#include "arkweb/ohos_adapter_ndk/pasteboard_adapter/include/pasteboard_client_adapter_impl.h"
#undef private

using namespace testing;
using testing::_;
using testing::Return;
using namespace MockNdkApi;

namespace OHOS::NWeb {
const int RESULT_OK = 0;
const std::string g_test_str = "testhtml";
const std::string g_empty_uri = "";
const std::string g_test_uri = "/data/test/path";
std::shared_ptr<PasteDataRecordAdapterImpl> g_paster;
std::shared_ptr<PasteDataRecordAdapterImpl> g_datarecord;
std::shared_ptr<PasteDataRecordAdapterImpl> g_datarecord_null;
std::shared_ptr<PasteDataAdapterImpl> g_dataadapter;
std::shared_ptr<PasteDataAdapterImpl> g_dataadapter_null;
std::shared_ptr<std::string> g_empty_text;
std::shared_ptr<std::string> g_test_text;
PasteCustomData g_empty_custom_data;
PasteCustomData g_test_custom_data;
PasteCustomData g_fake_custom_data;

class PasteboardClientAdapterImplTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void PasteboardClientAdapterImplTest::SetUpTestCase() {
    g_empty_text = std::make_shared<std::string>("");
    g_test_text = std::make_shared<std::string>("test");
    std::string format = "ohos/custom-data";
    std::vector<uint8_t> data = { 0, 1, 2 };
    g_test_custom_data.insert(std::make_pair(format, data));

    std::string fake_format = "format";
    std::vector<uint8_t> fake_data = { 0, 1, 2 };
    g_fake_custom_data.insert(std::make_pair(fake_format, fake_data));

    g_datarecord_null = std::make_shared<PasteDataRecordAdapterImpl>(nullptr, std::shared_ptr<OH_UdmfData>());
    ASSERT_NE(g_datarecord_null, nullptr);
    EXPECT_EQ(g_datarecord_null->record_, nullptr);

    OH_UdmfRecord* record = OH_UdmfRecord_Create();
    EXPECT_NE(record, nullptr);
    g_datarecord = std::make_shared<PasteDataRecordAdapterImpl>(record, std::shared_ptr<OH_UdmfData>());
    ASSERT_NE(g_datarecord, nullptr);
    

    OH_UdmfData* udmfData = OH_UdmfData_Create();
    g_dataadapter = std::make_shared<PasteDataAdapterImpl>(udmfData);
    ASSERT_NE(g_dataadapter, nullptr);

    g_dataadapter_null = std::make_shared<PasteDataAdapterImpl>(nullptr);
    ASSERT_NE(g_dataadapter_null, nullptr);
}

void PasteboardClientAdapterImplTest::TearDownTestCase() {}

void PasteboardClientAdapterImplTest::SetUp() {}

void PasteboardClientAdapterImplTest::TearDown() {}

class MockClipBoardImageDataAdapter : public ClipBoardImageDataAdapter {
public:
    MockClipBoardImageDataAdapter() = default;

    ClipBoardImageColorType GetColorType() override
    {
        return colorType;
    }

    ClipBoardImageAlphaType GetAlphaType() override
    {
        return alphaType;
    }

    uint32_t* GetData() override
    {
        return data;
    }

    size_t GetDataSize() override
    {
        return dataSize;
    }

    size_t GetRowBytes() override
    {
        return rowBytes;
    }

    int32_t GetWidth() override
    {
        return width;
    }

    int32_t GetHeight() override
    {
        return height;
    }

    void SetColorType(ClipBoardImageColorType color) override
    {
        colorType = color;
    }

    void SetAlphaType(ClipBoardImageAlphaType alpha) override
    {
        alphaType = alpha;
    }

    void SetData(uint32_t* d) override
    {
        data = d;
    }

    void SetDataSize(size_t size) override
    {
        dataSize = size;
    }

    void SetRowBytes(size_t r) override
    {
        rowBytes = r;
    }

    void SetWidth(int32_t w) override
    {
        width = w;
    }

    void SetHeight(int32_t h) override
    {
        height = h;
    }

    ClipBoardImageColorType colorType;
    ClipBoardImageAlphaType alphaType;
    uint32_t* data;
    size_t dataSize;
    size_t rowBytes;
    int32_t width;
    int32_t height;
};

class MockPasteboardObserver : public PasteboardObserverAdapter {
public:
    MockPasteboardObserver() = default;
    void OnPasteboardChanged() override {}
};

void SetMockState(bool status) {
    MockOHOSFunction::pasteboard_GetDataParams_Create = status;
    MockOHOSFunction::abilityRuntime_ApplicationContextGetCacheDir = status;
    MockOHOSFunction::pasteboard_GetDataParams_SetProgressIndicator = status;
    MockOHOSFunction::pasteboard_GetDataParams_SetDestUri = status;
    MockOHOSFunction::pasteboard_GetDataParams_SetFileConflictOptions = status;
    MockOHOSFunction::pasteboard_GetDataWithProgress = status;
    MockOHOSFunction::pasteboard_HasData = status;
    MockOHOSFunction::pasteboard_GetDataParams_Destroy = status;
    MockOHOSFunction::udmfData_GetRecords = status;
    MockOHOSFunction::udmfData_IsLocal = status;
}

TEST_F(PasteboardClientAdapterImplTest, SetAndGetHtmlText)
{
    std::shared_ptr<std::string> html_text = g_datarecord->GetHtmlText();
    EXPECT_EQ(html_text, nullptr);

    std::shared_ptr<std::string> plain_text = g_datarecord->GetPlainText();
    EXPECT_EQ(plain_text, nullptr);

    EXPECT_EQ(g_datarecord->GetMimeType(), "");

    EXPECT_EQ(g_datarecord->SetHtmlText(g_test_text), true);
    EXPECT_NE(g_datarecord->GetMimeType(), "");

    auto mimeTypes = g_datarecord->GetMimeTypes();
    bool isEmpty = mimeTypes.empty();
    EXPECT_EQ(isEmpty, false);

    html_text = g_datarecord->GetHtmlText();
    EXPECT_NE(html_text, nullptr);

    plain_text = g_datarecord->GetPlainText();
    EXPECT_NE(plain_text, nullptr);

    // record is nullptr
    EXPECT_EQ(g_datarecord_null->GetPlainText(), nullptr);
    EXPECT_EQ(g_datarecord_null->SetHtmlText(g_test_text), false);
    EXPECT_EQ(g_datarecord_null->GetMimeType(), "");
    mimeTypes = g_datarecord_null->GetMimeTypes();
    isEmpty = mimeTypes.empty();
    EXPECT_EQ(isEmpty, true);
    html_text = g_datarecord_null->GetHtmlText();
    EXPECT_EQ(html_text, nullptr);
}

TEST_F(PasteboardClientAdapterImplTest, SetAndGetPlainText)
{
    EXPECT_EQ(g_datarecord->SetPlainText(g_test_text), true);
    std::shared_ptr<std::string> plain_text = g_datarecord->GetPlainText();
    EXPECT_NE(plain_text, nullptr);


    EXPECT_EQ(g_datarecord_null->SetPlainText(g_test_text), false);
    plain_text = g_datarecord->GetPlainText();
    EXPECT_NE(plain_text, nullptr);
}

TEST_F(PasteboardClientAdapterImplTest, SetAndGetUri)
{
    std::shared_ptr<std::string> uri = g_datarecord->GetUri();
    EXPECT_EQ(uri, nullptr);
    EXPECT_EQ(g_datarecord->SetUri(g_empty_uri), false);
    EXPECT_EQ(g_datarecord->SetUri(g_test_uri), true);
    uri = g_datarecord->GetUri();
    EXPECT_NE(uri, nullptr);

    EXPECT_EQ(g_datarecord_null->SetUri(g_test_uri), false);
    uri = g_datarecord_null->GetUri();
    EXPECT_EQ(uri, nullptr);
}

TEST_F(PasteboardClientAdapterImplTest, SetAndGetCustomData)
{
    EXPECT_EQ(g_datarecord->SetCustomData(g_empty_custom_data), false);

    std::shared_ptr<PasteCustomData> custom_data = g_datarecord->GetCustomData();
    EXPECT_EQ(custom_data, nullptr);

    EXPECT_EQ(g_datarecord->SetCustomData(g_fake_custom_data), true);
    custom_data = g_datarecord->GetCustomData();
    EXPECT_EQ(custom_data, nullptr);

    EXPECT_EQ(g_datarecord->SetCustomData(g_test_custom_data), true);
    custom_data = g_datarecord->GetCustomData();
    EXPECT_NE(custom_data, nullptr);

    EXPECT_EQ(g_datarecord_null->SetCustomData(g_test_custom_data), false);
    custom_data = g_datarecord_null->GetCustomData();
    EXPECT_EQ(custom_data, nullptr);
}

TEST_F(PasteboardClientAdapterImplTest, SetAndGetImgData)
{
    uint32_t storage[][5] = {
        { 0xCA, 0xDA, 0xCA, 0xC9, 0xA3 },
        { 0xAC, 0xA8, 0x89, 0x47, 0x87 },
        { 0x4B, 0x25, 0x25, 0x25, 0x46 },
        { 0x90, 0x1, 0x25, 0x41, 0x33 },
        { 0x75, 0x55, 0x44, 0x20, 0x00 },
    };

    std::shared_ptr<MockClipBoardImageDataAdapter> image = std::make_shared<MockClipBoardImageDataAdapter>();
    image->SetColorType(ClipBoardImageColorType::COLOR_TYPE_BGRA_8888);
    image->SetAlphaType(ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN);
    image->SetData(storage[0]);
    image->SetDataSize(sizeof(storage));
    image->SetRowBytes(5);
    image->SetWidth(5);
    image->SetHeight(5);

    EXPECT_EQ(g_datarecord->SetImgData(image), true);

    EXPECT_EQ(g_datarecord->GetImgData(nullptr), false);
    std::shared_ptr<MockClipBoardImageDataAdapter> getted_image_data =
        std::make_shared<MockClipBoardImageDataAdapter>();
    EXPECT_EQ(g_datarecord->GetImgData(getted_image_data), true);

    EXPECT_EQ(g_datarecord->SetImgData(nullptr), false);

    image->SetData(nullptr);
    EXPECT_EQ(g_datarecord->SetImgData(image), false);
    image->SetData(storage[0]);
    image->SetDataSize(sizeof(storage));
    EXPECT_EQ(g_datarecord_null->SetImgData(image), false);
    EXPECT_EQ(g_datarecord_null->GetImgData(nullptr), false);
}

TEST_F(PasteboardClientAdapterImplTest, CheckTypeTranslate)
{
    EXPECT_EQ(g_datarecord->ClipBoardToPixelColorType(ClipBoardImageColorType::COLOR_TYPE_RGBA_8888),
        PIXEL_FORMAT::PIXEL_FORMAT_RGBA_8888);
    EXPECT_EQ(g_datarecord->ClipBoardToPixelColorType(ClipBoardImageColorType::COLOR_TYPE_BGRA_8888),
        PIXEL_FORMAT::PIXEL_FORMAT_BGRA_8888);
    EXPECT_EQ(g_datarecord->ClipBoardToPixelColorType(ClipBoardImageColorType::COLOR_TYPE_UNKNOWN),
        PIXEL_FORMAT::PIXEL_FORMAT_UNKNOWN);
    
    EXPECT_EQ(g_datarecord->PixelToClipBoardColorType(PIXEL_FORMAT::PIXEL_FORMAT_RGBA_8888),
        ClipBoardImageColorType::COLOR_TYPE_RGBA_8888);
    EXPECT_EQ(g_datarecord->PixelToClipBoardColorType(PIXEL_FORMAT::PIXEL_FORMAT_BGRA_8888),
        ClipBoardImageColorType::COLOR_TYPE_BGRA_8888);
    EXPECT_EQ(g_datarecord->PixelToClipBoardColorType(PIXEL_FORMAT::PIXEL_FORMAT_UNKNOWN),
        ClipBoardImageColorType::COLOR_TYPE_UNKNOWN);

    EXPECT_EQ(g_datarecord->ClipboardToPixelAlphaType(ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN),
        PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNKNOWN);
    EXPECT_EQ(g_datarecord->ClipboardToPixelAlphaType(ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE),
        PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_OPAQUE);
    EXPECT_EQ(g_datarecord->ClipboardToPixelAlphaType(ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED),
        PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_PREMULTIPLIED);
    EXPECT_EQ(g_datarecord->ClipboardToPixelAlphaType(ClipBoardImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED),
        PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNPREMULTIPLIED);
    
    EXPECT_EQ(g_datarecord->PixelToClipboardAlphaType(PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNKNOWN),
        ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN);
    EXPECT_EQ(g_datarecord->PixelToClipboardAlphaType(PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_OPAQUE),
        ClipBoardImageAlphaType::ALPHA_TYPE_OPAQUE);
    EXPECT_EQ(g_datarecord->PixelToClipboardAlphaType(PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_PREMULTIPLIED),
        ClipBoardImageAlphaType::ALPHA_TYPE_PREMULTIPLIED);
    EXPECT_EQ(g_datarecord->PixelToClipboardAlphaType(PIXELMAP_ALPHA_TYPE::PIXELMAP_ALPHA_TYPE_UNPREMULTIPLIED),
        ClipBoardImageAlphaType::ALPHA_TYPE_POSTMULTIPLIED);
}

TEST_F(PasteboardClientAdapterImplTest, ClearImgBuffer)
{
    g_datarecord->Clear();
    EXPECT_EQ(g_datarecord->imgBuffer_, nullptr);

    g_datarecord->bufferSize_ = 100;
    g_datarecord->imgBuffer_ = static_cast<uint8_t *>(calloc(g_datarecord->bufferSize_, sizeof(uint8_t)));
    EXPECT_NE(g_datarecord->imgBuffer_, nullptr);

    g_datarecord->Clear();
    EXPECT_EQ(g_datarecord->imgBuffer_, nullptr);
}

TEST_F(PasteboardClientAdapterImplTest, AddAndGetRecord)
{
    // get paste data while data is empty
    std::vector<std::string> mimeTypes = g_dataadapter->GetMimeTypes();
    bool isEmpty = mimeTypes.empty();
    EXPECT_EQ(isEmpty, true);
    std::shared_ptr<std::string> primaryHtml = g_dataadapter->GetPrimaryHtml();
    EXPECT_EQ(primaryHtml, nullptr);
    std::shared_ptr<std::string> primaryText = g_dataadapter->GetPrimaryText();
    EXPECT_EQ(primaryText, nullptr);
    std::shared_ptr<std::string> primaryMimeType = g_dataadapter->GetPrimaryMimeType();
    EXPECT_EQ(primaryMimeType, nullptr);
    std::shared_ptr<PasteDataRecordAdapter> record = g_dataadapter->GetRecordAt(0);
    EXPECT_EQ(record, nullptr);
    std::size_t count = g_dataadapter->GetRecordCount();
    EXPECT_EQ(count, 0);
    PasteRecordVector recordVector = g_dataadapter_null->AllRecords();
    isEmpty = recordVector.empty();
    EXPECT_EQ(isEmpty, true);

    // set and get paste data
    g_dataadapter->AddHtmlRecord(g_test_str);
    mimeTypes = g_dataadapter->GetMimeTypes();
    isEmpty = mimeTypes.empty();
    EXPECT_EQ(isEmpty, false);

    g_dataadapter_null->AddHtmlRecord(g_test_str);
    mimeTypes = g_dataadapter_null->GetMimeTypes();
    isEmpty = mimeTypes.empty();
    EXPECT_EQ(isEmpty, true);

    g_dataadapter->AddTextRecord(g_test_str);
    mimeTypes = g_dataadapter->GetMimeTypes();
    isEmpty = mimeTypes.empty();
    EXPECT_EQ(isEmpty, false);

    g_dataadapter_null->AddTextRecord(g_test_str);

    primaryHtml = g_dataadapter->GetPrimaryHtml();
    EXPECT_NE(primaryHtml, nullptr);
    primaryHtml = g_dataadapter_null->GetPrimaryHtml();
    EXPECT_EQ(primaryHtml, nullptr);

    primaryText = g_dataadapter->GetPrimaryText();
    EXPECT_NE(primaryText, nullptr);
    primaryText = g_dataadapter_null->GetPrimaryText();
    EXPECT_EQ(primaryText, nullptr);

    std::shared_ptr<std::string> primary = g_dataadapter_null->GetPrimaryMimeType();
    EXPECT_EQ(primary, nullptr);

    std::shared_ptr<PasteDataRecordAdapter> record_null = g_dataadapter_null->GetRecordAt(0);
    EXPECT_EQ(record_null, nullptr);
    count = g_dataadapter_null->GetRecordCount();
    EXPECT_EQ(count, 0);

    PasteRecordVector recordVector_null = g_dataadapter_null->AllRecords();
    isEmpty = recordVector_null.empty();
    EXPECT_EQ(isEmpty, true);
}

TEST_F(PasteboardClientAdapterImplTest, SetAndGetPasteData)
{
    PasteRecordVector data;
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);
    std::shared_ptr<PasteDataRecordAdapter> record = PasteDataRecordAdapter::NewRecord("text/html");
    EXPECT_NE(record, nullptr);
    std::shared_ptr<std::string> pasteData = std::make_shared<std::string>("test");
    EXPECT_NE(pasteData, nullptr);
    record->SetHtmlText(pasteData);
    data.push_back(record);
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(data);

    PasteBoardClientAdapterImpl::GetInstance().Clear();
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);

    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(data, CopyOptionMode::NONE);
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);

    std::shared_ptr<PasteboardObserverAdapter> observer = std::make_shared<MockPasteboardObserver>();
    int32_t id = PasteBoardClientAdapterImpl::GetInstance().AddPasteboardChangedObserver(observer);
    EXPECT_EQ(id, 0);
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(id);
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().reg_.size(), 0);

    id = PasteBoardClientAdapterImpl::GetInstance().AddPasteboardChangedObserver(nullptr);
    EXPECT_EQ(id, -1);
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(-1);
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().reg_.size(), 0);
}

TEST_F(PasteboardClientAdapterImplTest, CheckTransitionCopyOption)
{
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().TransitionCopyOption(CopyOptionMode::IN_APP),
        Udmf_ShareOption::SHARE_OPTIONS_IN_APP);
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().TransitionCopyOption(CopyOptionMode::LOCAL_DEVICE),
        Udmf_ShareOption::SHARE_OPTIONS_CROSS_APP);
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().TransitionCopyOption(CopyOptionMode::CROSS_DEVICE),
        Udmf_ShareOption::SHARE_OPTIONS_CROSS_APP);
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().TransitionCopyOption(CopyOptionMode::NONE),
        Udmf_ShareOption::SHARE_OPTIONS_INVALID);
}

TEST_F(PasteboardClientAdapterImplTest, PasteDataNull)
{
    PasteRecordVector data;
    PasteBoardClientAdapterImpl::GetInstance().pasteboard_ = nullptr;
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data),
        false);
    std::shared_ptr<PasteDataRecordAdapter> record = PasteDataRecordAdapter::NewRecord("text/html");
    EXPECT_NE(record, nullptr);
    std::shared_ptr<std::string> pasteData = std::make_shared<std::string>("test");
    EXPECT_NE(pasteData, nullptr);
    record->SetHtmlText(pasteData);
    data.push_back(record);
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(data);
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    std::shared_ptr<PasteboardObserverAdapter> observer = std::make_shared<MockPasteboardObserver>();
    int32_t id = PasteBoardClientAdapterImpl::GetInstance().AddPasteboardChangedObserver(observer);
    EXPECT_NE(id, -1);

    PasteBoardClientAdapterImpl::GetInstance().pasteboard_ = OH_Pasteboard_Create();
    int32_t newId = PasteBoardClientAdapterImpl::GetInstance().AddPasteboardChangedObserver(observer);
    EXPECT_EQ(id, newId - 1);
    PasteBoardClientAdapterImpl::GetInstance().pasteboard_ = nullptr;
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(id);
    PasteBoardClientAdapterImpl::GetInstance().RemovePasteboardChangedObserver(newId);
}

TEST_F(PasteboardClientAdapterImplTest, ReleaseMemory)
{
    g_datarecord->DestoryOptions(0, nullptr, "options is nullptr");
    g_datarecord->DestoryPixelmapNative(nullptr);
    g_datarecord->ReleaseMemory(nullptr, nullptr, nullptr, nullptr, "test all input is nullptr", 0);
}

TEST_F(PasteboardClientAdapterImplTest, HtmlToPlainText001)
{
    std::string html = "";
    std::string expected = "";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
    html = "aa";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
}

TEST_F(PasteboardClientAdapterImplTest, HtmlToPlainText002)
{
    std::string html = "a<div";
    std::string expected = "";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
}

TEST_F(PasteboardClientAdapterImplTest, HtmlToPlainText003)
{
    std::string html = "<p class=\"content\">aa</p>";
    std::string expected = "aa";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
}

TEST_F(PasteboardClientAdapterImplTest, HtmlToPlainText004)
{
    std::string html = "<h1>aa</h1>";
    std::string expected = "aa";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
}

TEST_F(PasteboardClientAdapterImplTest, HtmlToPlainText005)
{
    std::string html = "<div>aa</div><ul>bb</ul>";
    std::string expected = "aa\r\nbb";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
}

TEST_F(PasteboardClientAdapterImplTest, HtmlToPlainText006)
{
    std::string html = "<p>aa";
    std::string expected = "aa";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
}

TEST_F(PasteboardClientAdapterImplTest, HtmlToPlainText007)
{
    std::string html = "<span>aa</span>bb<span>cc</span>";
    std::string expected = "aabbcc";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
}

TEST_F(PasteboardClientAdapterImplTest, HtmlToPlainText008)
{
    std::string html = "aa<h2>bb</h2><p>cc</p>dd";
    std::string expected = "bb\r\nccdd";
    EXPECT_EQ(g_datarecord->HtmlToPlainText(html), expected);
}

TEST_F(PasteboardClientAdapterImplTest, AllRecords001)
{
    PasteRecordVector recordVector = g_dataadapter->AllRecords();
    bool isEmpty = recordVector.empty();
    EXPECT_EQ(isEmpty, true);
}

TEST_F(PasteboardClientAdapterImplTest, GetPasteDataTest001)
{
    PasteBoardClientAdapterImpl::GetInstance();
    SetMockState(true);
    PasteRecordVector data;
    auto& mock = MockOHOSFunction::GetInstance();
    Pasteboard_GetDataParams* params = reinterpret_cast<Pasteboard_GetDataParams*>(0x1);
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Create()).WillRepeatedly(Return(params));
    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID));
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);

    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_NO_ERROR));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetProgressIndicator(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetDestUri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetFileConflictOptions(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataWithProgress(_, _, _)).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock, OH_Pasteboard_HasData(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Destroy(_)).WillRepeatedly(Return());
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);

    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Create()).WillRepeatedly(Return(params));
    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_NO_ERROR));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetProgressIndicator(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetDestUri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetFileConflictOptions(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataWithProgress(_, _, _)).WillRepeatedly(testing::DoAll(
            testing::SetArgPointee<2>(-1),testing::Return(nullptr)));
    EXPECT_CALL(mock, OH_Pasteboard_HasData(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Destroy(_)).WillRepeatedly(Return());
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);
    SetMockState(false);
}

TEST_F(PasteboardClientAdapterImplTest, GetPasteDataTest002)
{
    PasteBoardClientAdapterImpl::GetInstance();
    SetMockState(true);
    PasteRecordVector data;
    auto& mock = MockOHOSFunction::GetInstance();
    Pasteboard_GetDataParams* params = reinterpret_cast<Pasteboard_GetDataParams*>(0x1);
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Create()).WillRepeatedly(Return(params));
    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_NO_ERROR));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetProgressIndicator(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetDestUri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetFileConflictOptions(_, _)).WillRepeatedly(Return());

    EXPECT_CALL(mock, OH_Pasteboard_GetDataWithProgress(_, _, _)).WillRepeatedly(testing::DoAll(
            testing::SetArgPointee<2>(ERR_OK),testing::Return(nullptr)));
    EXPECT_CALL(mock, OH_Pasteboard_HasData(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, OH_UdmfData_GetRecords(_, _)).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Destroy(_)).WillRepeatedly(Return());
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);

    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Create()).WillRepeatedly(Return(params));
    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_NO_ERROR));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetProgressIndicator(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetDestUri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetFileConflictOptions(_, _)).WillRepeatedly(Return());

    EXPECT_CALL(mock, OH_Pasteboard_GetDataWithProgress(_, _, _)).WillRepeatedly(testing::DoAll(
            testing::SetArgPointee<2>(ERR_OK),testing::Return(nullptr)));
    EXPECT_CALL(mock, OH_Pasteboard_HasData(_)).WillRepeatedly(Return(true));

    OH_UdmfRecord* records = reinterpret_cast<OH_UdmfRecord *>(0x1);
    EXPECT_CALL(mock, OH_UdmfData_GetRecords(_, _)).WillRepeatedly(testing::DoAll(
            testing::SetArgPointee<1>(0),testing::Return(&records)));
    EXPECT_CALL(mock, OH_UdmfData_IsLocal(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Destroy(_)).WillRepeatedly(Return());
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), true);
    SetMockState(false);
}

TEST_F(PasteboardClientAdapterImplTest, GetPasteDataTest003)
{
    PasteBoardClientAdapterImpl::GetInstance();
    SetMockState(true);
    PasteRecordVector data;
    auto& mock = MockOHOSFunction::GetInstance();
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Create()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_PARAM_INVALID));
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);

    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_NO_ERROR));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetProgressIndicator(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetDestUri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetFileConflictOptions(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataWithProgress(_, _, _)).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock, OH_Pasteboard_HasData(_)).WillRepeatedly(Return(false));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Destroy(_)).WillRepeatedly(Return());
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);

    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Create()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_NO_ERROR));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetProgressIndicator(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetDestUri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetFileConflictOptions(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataWithProgress(_, _, _)).WillRepeatedly(testing::DoAll(
            testing::SetArgPointee<2>(-1),testing::Return(nullptr)));
    EXPECT_CALL(mock, OH_Pasteboard_HasData(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Destroy(_)).WillRepeatedly(Return());
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);
    SetMockState(false);
}

TEST_F(PasteboardClientAdapterImplTest, GetPasteDataTest004)
{
    PasteBoardClientAdapterImpl::GetInstance();
    SetMockState(true);
    PasteRecordVector data;
    auto& mock = MockOHOSFunction::GetInstance();
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Create()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_NO_ERROR));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetProgressIndicator(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetDestUri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetFileConflictOptions(_, _)).WillRepeatedly(Return());

    EXPECT_CALL(mock, OH_Pasteboard_GetDataWithProgress(_, _, _)).WillRepeatedly(testing::DoAll(
            testing::SetArgPointee<2>(ERR_OK),testing::Return(nullptr)));
    EXPECT_CALL(mock, OH_Pasteboard_HasData(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, OH_UdmfData_GetRecords(_, _)).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Destroy(_)).WillRepeatedly(Return());
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), false);

    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Create()).WillRepeatedly(Return(nullptr));
    EXPECT_CALL(mock, OH_AbilityRuntime_ApplicationContextGetCacheDir(_, _,
        _)).WillRepeatedly(Return(ABILITY_RUNTIME_ERROR_CODE_NO_ERROR));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetProgressIndicator(_, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetDestUri(_, _, _)).WillRepeatedly(Return());
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_SetFileConflictOptions(_, _)).WillRepeatedly(Return());

    EXPECT_CALL(mock, OH_Pasteboard_GetDataWithProgress(_, _, _)).WillRepeatedly(testing::DoAll(
            testing::SetArgPointee<2>(ERR_OK),testing::Return(nullptr)));
    EXPECT_CALL(mock, OH_Pasteboard_HasData(_)).WillRepeatedly(Return(true));

    OH_UdmfRecord* records = reinterpret_cast<OH_UdmfRecord *>(0x1);
    EXPECT_CALL(mock, OH_UdmfData_GetRecords(_, _)).WillRepeatedly(testing::DoAll(
            testing::SetArgPointee<1>(0),testing::Return(&records)));
    EXPECT_CALL(mock, OH_UdmfData_IsLocal(_)).WillRepeatedly(Return(true));
    EXPECT_CALL(mock, OH_Pasteboard_GetDataParams_Destroy(_)).WillRepeatedly(Return());
    EXPECT_EQ(PasteBoardClientAdapterImpl::GetInstance().GetPasteData(data), true);
    SetMockState(false);
}

}

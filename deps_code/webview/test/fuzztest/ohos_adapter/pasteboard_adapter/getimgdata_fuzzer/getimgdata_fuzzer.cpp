/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "getimgdata_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>

#define private public
#include "pasteboard_client_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;

namespace OHOS {
namespace {
constexpr uint8_t TEST_STRING_LENGTH = 8;

class MockClipBoardImageDataAdapter : public ClipBoardImageDataAdapter {
public:
    MockClipBoardImageDataAdapter() = default;

    virtual ~MockClipBoardImageDataAdapter() = default;

    ClipBoardImageColorType GetColorType() override
    {
        return ClipBoardImageColorType::COLOR_TYPE_UNKNOWN;
    }

    ClipBoardImageAlphaType GetAlphaType() override
    {
        return ClipBoardImageAlphaType::ALPHA_TYPE_UNKNOWN;
    }

    uint32_t* GetData() override
    {
        return nullptr;
    }

    size_t GetDataSize() override
    {
        return 0;
    }

    size_t GetRowBytes() override
    {
        return 0;
    }

    int32_t GetWidth() override
    {
        return 0;
    }

    int32_t GetHeight() override
    {
        return 0;
    }

    void SetColorType(ClipBoardImageColorType color) override {}
    void SetAlphaType(ClipBoardImageAlphaType alpha) override {}
    void SetData(uint32_t* data) override {}
    void SetDataSize(size_t size) override {}
    void SetRowBytes(size_t rowBytes) override {}
    void SetWidth(int32_t width) override {}
    void SetHeight(int32_t height) override {}
};
}
bool GetImgDataFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string text = dataProvider.ConsumeRandomLengthString(TEST_STRING_LENGTH);
    std::shared_ptr<PasteDataRecordAdapterImpl> dataRecordAdapterImpl =
        std::make_shared<PasteDataRecordAdapterImpl>(text);
    std::shared_ptr<ClipBoardImageDataAdapter> imageData;
    dataRecordAdapterImpl->GetImgData(nullptr);
    std::shared_ptr<MockClipBoardImageDataAdapter> mockClipBoardImageData =
        std::make_shared<MockClipBoardImageDataAdapter>();
    dataRecordAdapterImpl->GetImgData(mockClipBoardImageData);
    dataRecordAdapterImpl->GetPixelMap();

    dataRecordAdapterImpl->SetImgData(mockClipBoardImageData);
    dataRecordAdapterImpl->GetImgData(mockClipBoardImageData);
    dataRecordAdapterImpl->GetPixelMap();

    dataRecordAdapterImpl->GetUri();
    dataRecordAdapterImpl->GetCustomData();
    dataRecordAdapterImpl->ClearImgBuffer();
    dataRecordAdapterImpl->Clear();
    dataRecordAdapterImpl->GetRecord();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GetImgDataFuzzTest(data, size);
    return 0;
}

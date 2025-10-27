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

#include "getrecordat_fuzzer.h"

#include <securec.h>

#include <fuzzer/FuzzedDataProvider.h>

#define private public
#include "pasteboard_client_adapter_impl.h"
#undef private

using namespace OHOS::NWeb;
using namespace OHOS::MiscServices;
namespace OHOS {
namespace {
    constexpr uint8_t TEST_STRING_LENGTH = 8;
}

bool GetRecordAtFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(size_t))) {
        return false;
    }
    std::size_t index = 0;
    if (memcpy_s(&index, sizeof(size_t), data, sizeof(size_t)) != 0) {
        return false;
    }
    std::shared_ptr<PasteData> dataName = std::make_shared<PasteData>();
    std::shared_ptr<PasteDataAdapterImpl> dataAdapterImpl = std::make_shared<PasteDataAdapterImpl>(dataName);
    dataAdapterImpl->GetRecordAt(index);
    dataAdapterImpl->GetRecordCount();
    dataAdapterImpl->AllRecords();
    return true;
}

bool TestNullDataAtFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(size_t))) {
        return false;
    }
    std::shared_ptr<PasteData> dataName = std::make_shared<PasteData>();
    std::shared_ptr<PasteDataAdapterImpl> dataAdapterImpl = std::make_shared<PasteDataAdapterImpl>(dataName);
    dataAdapterImpl->data_ = nullptr;

    dataAdapterImpl->GetRecordAt(0);
    dataAdapterImpl->GetRecordCount();
    dataAdapterImpl->AllRecords();
    dataAdapterImpl->GetMimeTypes();
    dataAdapterImpl->GetPrimaryHtml();
    dataAdapterImpl->GetPrimaryText();
    dataAdapterImpl->GetPrimaryMimeType();

    FuzzedDataProvider dataProvider(data, size);
    std::string stringParam = dataProvider.ConsumeRandomLengthString(TEST_STRING_LENGTH);
    dataAdapterImpl->AddTextRecord(stringParam);
    dataAdapterImpl->AddHtmlRecord(stringParam);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GetRecordAtFuzzTest(data, size);
    OHOS::TestNullDataAtFuzzTest(data, size);
    return 0;
}

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

#include "printdocument_adapter_fuzz.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <fuzzer/FuzzedDataProvider.h>
#include <fcntl.h>
#include <unistd.h>
#define private public
#include "print_manager_adapter_impl.h"

namespace OHOS::NWeb {
constexpr uint8_t MAX_STRING_LENGTH = 255;

class PrintDocumentAdapterImplMock : public PrintDocumentAdapterAdapter {
public:
    PrintDocumentAdapterImplMock() = default;
    ~PrintDocumentAdapterImplMock() override = default;

    void OnStartLayoutWrite(const std::string& jobId, const PrintAttributesAdapter& oldAttrs,
        const PrintAttributesAdapter& newAttrs, uint32_t fd,
        std::shared_ptr<PrintWriteResultCallbackAdapter> callback) override {}

    void OnJobStateChanged(const std::string& jobId, uint32_t state) override {}
};

bool PrintDocumentFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string jobId = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
#if defined(NWEB_PRINT_ENABLE)
    std::shared_ptr<PrintDocumentAdapterAdapter> mock = std::make_shared<PrintDocumentAdapterImplMock>();
    PrintDocumentAdapterImpl documentAdapter(mock);
    OHOS::Print::PrintAttributes oldAttrs = OHOS::Print::PrintAttributes();
    OHOS::Print::PrintAttributes newAttrs = OHOS::Print::PrintAttributes();
    uint32_t state = dataProvider.ConsumeIntegral<uint32_t>();
    uint32_t fd = dataProvider.ConsumeIntegral<uint32_t>();
    auto writeResultCallback = [] (std::string str, uint32_t index) {};
    documentAdapter.onStartLayoutWrite(jobId, oldAttrs, newAttrs, fd, writeResultCallback);
    documentAdapter.onJobStateChanged(jobId, state);
    documentAdapter.ConvertPrintingParameters(newAttrs);
#endif
    std::function<void(std::string, uint32_t)> func = [] (std::string str, uint32_t index) {};
    std::shared_ptr<PrintWriteResultCallbackAdapter> callback =
        std::make_shared<PrintWriteResultCallbackAdapterImpl>(func);
    uint32_t code = dataProvider.ConsumeIntegral<uint32_t>();
    callback->WriteResultCallback(jobId, code);
    return true;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    PrintDocumentFuzzTest(data, size);
    return 0;
}
} // namespace OHOS::NWeb
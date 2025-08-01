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

#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "fuzzer/FuzzedDataProvider.h"
#define private public
#include "arkweb/ohos_adapter_ndk/print_manager_adapter/print_manager_adapter_impl.h"

using namespace OHOS::NWeb;
namespace {
constexpr uint8_t MAX_STRING_LENGTH = 255;
const char *TESTFILE_PATH = "./fuzz_testfile";
class PrintDocumentAdapterImplMock : public PrintDocumentAdapterAdapter {
public:
    PrintDocumentAdapterImplMock() = default;
    ~PrintDocumentAdapterImplMock() override = default;

    void OnStartLayoutWrite(const std::string& jobId, const PrintAttributesAdapter& oldAttrs,
        const PrintAttributesAdapter& newAttrs, uint32_t fd,
        std::shared_ptr<PrintWriteResultCallbackAdapter> callback) override {}

    void OnJobStateChanged(const std::string& jobId, uint32_t state) override {}
};

void SetPrintPrintAttributes(FuzzedDataProvider& dataProvider, Print_PrintAttributes& attrs)
{
    Print_Range pageRange;
    pageRange.startPage = dataProvider.ConsumeIntegral<uint32_t>();
    pageRange.endPage = dataProvider.ConsumeIntegral<uint32_t>();
    pageRange.pagesArrayLen = 0;
    Print_PageSize pageSize;
    pageSize.width = dataProvider.ConsumeIntegral<uint32_t>();
    pageSize.height = dataProvider.ConsumeIntegral<uint32_t>();
    Print_Margin pageMargin;
    pageMargin.leftMargin = dataProvider.ConsumeIntegral<uint32_t>();
    pageMargin.topMargin = dataProvider.ConsumeIntegral<uint32_t>();
    pageMargin.rightMargin = dataProvider.ConsumeIntegral<uint32_t>();
    pageMargin.bottomMargin = dataProvider.ConsumeIntegral<uint32_t>();
    attrs.pageRange = pageRange;
    attrs.pageSize = pageSize;
    attrs.pageMargin = pageMargin;
}

void PrintDocumentFuzzTest(FuzzedDataProvider& dataProvider)
{
    std::string jobId = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
#if defined(NWEB_PRINT_ENABLE)
    std::shared_ptr<PrintDocumentAdapterAdapter> mock = std::make_shared<PrintDocumentAdapterImplMock>();
    PrintDocumentAdapterImpl documentAdapter(mock);
    Print_PrintAttributes oldAttrs;
    SetPrintPrintAttributes(dataProvider, oldAttrs);
    Print_PrintAttributes newAttrs;
    SetPrintPrintAttributes(dataProvider, newAttrs);
    uint32_t state = dataProvider.ConsumeIntegral<uint32_t>();
    uint32_t fd = dataProvider.ConsumeIntegral<uint32_t>();
    auto writeResultCallback = [] (const char *jobId, uint32_t code) {};
    documentAdapter.OnStartLayoutWrite(jobId.c_str(), fd, &oldAttrs, &newAttrs, writeResultCallback);
    documentAdapter.OnJobStateChanged(jobId.c_str(), state);
    documentAdapter.ConvertPrintingParameters(&newAttrs);
#endif
    auto func = [] (const char *jobId, uint32_t code) {};
    PrintWriteResultCallbackAdapterImpl callback(func);
    uint32_t code = dataProvider.ConsumeIntegral<uint32_t>();
    callback.WriteResultCallback(jobId.c_str(), code);
}

void PrintManagerAdapterFuzzTest(const uint8_t* data, size_t size)
{
    std::vector<std::string> fileList = { TESTFILE_PATH };
    int32_t fd = open(TESTFILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        return;
    }
    (void)write(fd, data, size);

    std::vector<uint32_t> fdList = { fd };
    FuzzedDataProvider dataProvider(data, size);
    std::string taskId = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    PrintManagerAdapterImpl::GetInstance().StartPrint(fileList, fdList, taskId);
    std::shared_ptr<PrintDocumentAdapterAdapter> printDocumentAdapterImpl;
    PrintAttributesAdapter printAttributesAdapter;
    PrintManagerAdapterImpl::GetInstance().Print("webPrintTestJob", printDocumentAdapterImpl, printAttributesAdapter);
    void* token = nullptr;
    PrintManagerAdapterImpl::GetInstance().Print("webPrintTestJob", printDocumentAdapterImpl,
        printAttributesAdapter, token);
    close(fd);
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    PrintDocumentFuzzTest(fdp);
    PrintManagerAdapterFuzzTest(data, size);
    return 0;
}

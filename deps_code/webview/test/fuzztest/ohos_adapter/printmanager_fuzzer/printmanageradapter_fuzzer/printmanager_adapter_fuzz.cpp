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

#include "printmanager_adapter_fuzz.h"

#include <cstdint>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <fuzzer/FuzzedDataProvider.h>
#include <fcntl.h>
#include <unistd.h>

#include "print_manager_adapter_impl.h"

namespace OHOS::NWeb {
constexpr uint8_t MAX_STRING_LENGTH = 255;
const char *TESTFILE_PATH = "/data/test/fuzz_testfile";

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if (size == 0) {
        return 0;
    }

    std::vector<std::string> fileList = { TESTFILE_PATH };
    int32_t fd = open(TESTFILE_PATH, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if (fd < 0) {
        return 0;
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
    return 0;
}

} // namespace OHOS::NWeb
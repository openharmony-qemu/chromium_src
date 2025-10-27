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

#include "openremoteuri_fuzzer.h"

#include "pasteboard_client_adapter_impl.h"

#include <fuzzer/FuzzedDataProvider.h>

using namespace OHOS::NWeb;

namespace OHOS {
namespace {
    constexpr uint8_t TEST_STRING_LENGTH = 8;
}
bool OpenRemoteUriFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    std::string path((const char*)data, size);
    PasteBoardClientAdapterImpl::GetInstance().OpenRemoteUri(path);
    PasteBoardClientAdapterImpl::GetInstance().IsLocalPaste();
    PasteBoardClientAdapterImpl::GetInstance().GetTokenId();
    return true;
}

bool SetPastDataFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::string html = dataProvider.ConsumeRandomLengthString(TEST_STRING_LENGTH);
    std::shared_ptr<PasteDataAdapterImpl> dataAdapterImpl = std::make_shared<PasteDataAdapterImpl>();
    dataAdapterImpl->AddHtmlRecord(html);
    dataAdapterImpl->AddTextRecord(html);
    PasteRecordVector datas = dataAdapterImpl->AllRecords();
    PasteRecordVector pasteDatas;
    PasteBoardClientAdapterImpl::GetInstance().GetPasteData(pasteDatas);

    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(datas, CopyOptionMode::NONE);
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(datas, CopyOptionMode::IN_APP);
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(datas, CopyOptionMode::LOCAL_DEVICE);
    PasteBoardClientAdapterImpl::GetInstance().SetPasteData(datas, CopyOptionMode::CROSS_DEVICE);

    PasteBoardClientAdapterImpl::GetInstance().GetPasteData(pasteDatas);
    PasteBoardClientAdapterImpl::GetInstance().Clear();
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::OpenRemoteUriFuzzTest(data, size);
    OHOS::SetPastDataFuzzTest(data, size);
    return 0;
}

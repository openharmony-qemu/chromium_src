/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "selectcertrequest_fuzzer.h"
#include <fuzzer/FuzzedDataProvider.h>

#include <cstring>
#include <securec.h>

#include "nweb.h"
#include "nweb_adapter_helper.h"
#include "nweb_handler.h"
#include "nweb_helper.h"
#include "nweb_js_ssl_select_cert_result.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 125;
constexpr int MAX_SET_NUMBER = 1000;
constexpr int MAX_VECTOR_SIZE = 10;
bool SelectCertRequestFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size < sizeof(int))) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    OHOS::NWeb::NWebHandler handler;
    std::shared_ptr<NWebJSSslSelectCertResult> result = nullptr;
    std::string host = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    int port = dataProvider.ConsumeIntegralInRange<int>(0, MAX_SET_NUMBER);
    std::vector<std::string> keyTypes;
    std::vector<std::string> issuers;
    int vec_size = dataProvider.ConsumeIntegralInRange<int>(0, MAX_VECTOR_SIZE);
    for (size_t i = 0; i < vec_size; i++) {
        std::string item = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        keyTypes.push_back(item);
        issuers.push_back(item);
    }
    handler.OnSslSelectCertRequestByJS(result, host, port, keyTypes, issuers);
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SelectCertRequestFuzzTest(data, size);
    return 0;
}

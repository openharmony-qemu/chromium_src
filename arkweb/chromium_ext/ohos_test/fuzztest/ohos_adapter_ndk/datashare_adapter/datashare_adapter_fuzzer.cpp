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

#include "arkweb/ohos_adapter_ndk/datashare_adapter/datashare_adapter_impl.h"

using namespace OHOS::NWeb;

namespace {
void DatashareAdapterFuzzTest(const uint8_t* data, size_t size)
{
    std::string uriString(reinterpret_cast<const char*>(data), size);

    DatashareAdapterImpl& adapter = DatashareAdapterImpl::GetInstance();

    adapter.GetRealPath(uriString);
    adapter.OpenDataShareUriForRead(uriString);
    adapter.GetFileDisplayName(uriString);
}
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return 0;
    }
    DatashareAdapterFuzzTest(data, size);
    return 0;
}
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

#include "arkweb/ohos_nweb/src/native_media_player/nweb_media_info_impl.h"

#include <fuzzer/FuzzedDataProvider.h>
using namespace OHOS::NWeb;

namespace OHOS {
constexpr int MAX_SET_NUMBER = 1000;

class CefCustomMediaInfoMock : public CefCustomMediaInfo {
public:
    CefCustomMediaInfoMock() = default;
};

bool PlayerFrameworkAdapterImpl_SetMediaSourceHeaderFuzzTest(FuzzedDataProvider* fdp) {
    std::shared_ptr<CefCustomMediaInfo> mock =
        std::make_shared<CefCustomMediaInfoMock>();
    std::shared_ptr<NWebMediaInfoImpl> adapter =
        std::make_shared<NWebMediaInfoImpl>(*mock);
    adapter->GetPreload();
    adapter->GetIsMuted();
    adapter->GetEmbedId();
    adapter->GetPosterUrl();
    adapter->GetMediaType();
    adapter->GetIsControlsShown();
    adapter->GetControls();
    adapter->GetHeaders();
    adapter->GetAttributes();
    adapter->GetSourceInfos();
    adapter->GetSurfaceInfo();
    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    FuzzedDataProvider fdp(data, size);
    OHOS::PlayerFrameworkAdapterImpl_SetMediaSourceHeaderFuzzTest(&fdp);

    return 0;
}
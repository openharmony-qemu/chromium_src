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

#include "arkweb/chromium_ext/media/base/ohos/ohos_media_codec_bridge_impl.h"

#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>

std::string g_hevc = "video/hevc";
std::string g_avc = "video/avc";

namespace media {
using namespace OHOS::NWeb;
std::unique_ptr<OHOSMediaCodecBridge> CreateCocdecMock()
{
    return OHOSMediaCodecBridgeImpl::CreateVideoEncoder(g_hevc);
}

bool OHOSMediaCodecBridgeImplFuzzTest(const uint8_t* data, size_t size)
{
    FuzzedDataProvider fdp(data, size);
    auto adapter = CreateCocdecMock();
    CodecConfigPara config{.width = 640, .height = 480, .bitRate = 1000000, .frameRate = 30};
    adapter->Prepare();
    adapter->Start();
    adapter->CreateInputSurface();
    auto rawValue = fdp.ConsumeIntegralInRange<int32_t>(0, 1);
    adapter->ReleaseOutputBuffer(rawValue,true);
    adapter->ClearKeyFrameCache();
    adapter->RequestKeyFrameSoon();
    adapter->Stop();
    adapter->Reset();
    adapter->Release();
    return true;
}

} // namespace media

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr || size == 0) {
        return 0;
    }
    media::OHOSMediaCodecBridgeImplFuzzTest(data, size);
    return 0;
}
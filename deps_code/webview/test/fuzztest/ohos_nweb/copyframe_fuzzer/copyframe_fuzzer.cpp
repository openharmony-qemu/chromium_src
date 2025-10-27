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

#include "copyframe_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <ui/rs_surface_node.h>

#include "nweb.h"
#include "nweb_adapter_helper.h"

#define private public
#include "nweb_surface_adapter.h"

using namespace OHOS::NWeb;

namespace OHOS {
const uint32_t DEFAULT_WIDTH = 2560;
const uint32_t DEFAULT_HEIGHT = 1396;
constexpr int MAX_SET_NUMBER = 1000;
constexpr int BITS_PER_PIXEL = 4;

bool CopyFrameFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    uint32_t width = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER);
    uint32_t height = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER);
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    char* src = new char[DEFAULT_WIDTH * DEFAULT_HEIGHT * BITS_PER_PIXEL] { 0 };
    if (src == nullptr) {
        return false;
    }
    surfaceAdapter.CopyFrame(surfaceBuffer, src, width, height);
    int32_t releaseFence = -1;
    BufferRequestConfig requestConfig = {
        .width = DEFAULT_WIDTH,
        .height = DEFAULT_HEIGHT,
        .strideAlignment = sizeof(void *),
        .format = GRAPHIC_PIXEL_FMT_RGBA_8888,
        .usage = BUFFER_USAGE_CPU_READ | BUFFER_USAGE_CPU_WRITE | BUFFER_USAGE_MEM_DMA,
        .timeout = 0,
    };
    sptr<Surface> fuzzSurface = nullptr;
    Rosen::RSSurfaceNodeConfig config;
    config.SurfaceNodeName = "webTestSurfaceName";
    auto surfaceNode = Rosen::RSSurfaceNode::Create(config, false);
    if (surfaceNode == nullptr) {
        return false;
    }
    fuzzSurface = surfaceNode->GetSurface();
    if (fuzzSurface == nullptr) {
        return false;
    }
    fuzzSurface->RequestBuffer(surfaceBuffer, releaseFence, requestConfig);
    if (surfaceBuffer == nullptr) {
        return false;
    }
    surfaceAdapter.CopyFrame(surfaceBuffer, src, width, height);
    delete[] src;
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::CopyFrameFuzzTest(data, size);
    return 0;
}

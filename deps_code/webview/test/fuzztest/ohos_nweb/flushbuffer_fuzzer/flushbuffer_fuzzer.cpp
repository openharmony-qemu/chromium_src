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

#include "flushbuffer_fuzzer.h"

#include <cstring>
#include <securec.h>
#include <fuzzer/FuzzedDataProvider.h>
#include <ui/rs_surface_node.h>

#include "nweb.h"
#include "nweb_adapter_helper.h"

#define private public
#include "nweb_surface_adapter.h"

using namespace OHOS::NWeb;
using namespace OHOS::Rosen;

namespace OHOS {
namespace {
    sptr<Surface> g_surface = nullptr;
}
constexpr int MAX_SET_NUMBER = 1000;
constexpr int BITS_PER_PIXEL = 4;

bool FlushBufferFuzzTest(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    uint32_t width = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER);
    uint32_t height = dataProvider.ConsumeIntegralInRange<uint32_t>(1, MAX_SET_NUMBER);
    char* buffer = new char[width * height * BITS_PER_PIXEL] { 0 };
    if (buffer == nullptr) {
        return false;
    }
    sptr<SurfaceBuffer> surfaceBuffer = nullptr;
    auto surfaceAdapter = NWebSurfaceAdapter::Instance();
    surfaceAdapter.FlushBuffer(g_surface, surfaceBuffer, width, height);
    wptr<Surface> surfaceWeakPtr(g_surface);
    surfaceAdapter.OutputFrameCallback(buffer, width, height, surfaceWeakPtr);
    surfaceAdapter.RequestBuffer(g_surface, width, height);
    if (!g_surface) {
        RSSurfaceNodeConfig config;
        config.SurfaceNodeName = "webTestSurfaceName";
        auto surfaceNode = RSSurfaceNode::Create(config, false);
        if (surfaceNode == nullptr) {
            delete[] buffer;
            return false;
        }
        g_surface = surfaceNode->GetSurface();
        if (g_surface == nullptr) {
            delete[] buffer;
            return false;
        }
    }
    surfaceAdapter.FlushBuffer(g_surface, surfaceBuffer, width, height);
    wptr<Surface> consumerSurfaceWeakPtr(Surface::CreateSurfaceAsConsumer());
    surfaceAdapter.OutputFrameCallback(buffer, width, height, consumerSurfaceWeakPtr);
    surfaceAdapter.RequestBuffer(g_surface, width, height);
    delete[] buffer;
    return true;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::FlushBufferFuzzTest(data, size);
    return 0;
}

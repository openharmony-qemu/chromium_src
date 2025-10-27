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

#include <cstdint>
#include <cstring>
#include <fuzzer/FuzzedDataProvider.h>
#include "nweb_output_handler.h"

using namespace OHOS::NWeb;

void NwebOutputHandlerFuzzTest1(FuzzedDataProvider* fdp) {
    auto output_frame_cb = [](const char* data, uint32_t width, uint32_t height) -> bool {
        return false;
    };
    uint32_t width = 10;
    uint32_t height = 10;
    std::shared_ptr<NWebOutputHandler> handler = 
        NWebOutputHandler::Create(width, height, output_frame_cb);

    width = fdp->ConsumeIntegralInRange<uint32_t>(1, 10);
    height = fdp->ConsumeIntegralInRange<uint32_t>(1, 10);
    handler->GetWindowInfo(width, height);
}

void NwebOutputHandlerFuzzTest(FuzzedDataProvider* fdp) {
    auto output_frame_cb = [](const char* data, uint32_t width, uint32_t height) -> bool {
        return false;
    };
    uint32_t width = 10;
    uint32_t height = 10;
    std::shared_ptr<NWebOutputHandler> handler = 
        NWebOutputHandler::Create(width, height, output_frame_cb);
    handler->StartRenderOutput();
}

void NwebOutputHandlerFuzzTest2(FuzzedDataProvider* fdp) {
    auto output_frame_cb = [](const char* data, uint32_t width, uint32_t height) -> bool {
        return false;
    };
    uint32_t width = 10;
    uint32_t height = 10;
    std::shared_ptr<NWebOutputHandler> handler = 
        NWebOutputHandler::Create(width, height, output_frame_cb);

    bool flag = fdp->ConsumeBool();
    handler->SetFrameInfoDump(flag);
}

void NwebOutputHandlerFuzzTest3(FuzzedDataProvider* fdp) {
    auto output_frame_cb = [](const char* data, uint32_t width, uint32_t height) -> bool {
        return false;
    };
    uint32_t width = 10;
    uint32_t height = 10;
    std::shared_ptr<NWebOutputHandler> handler = 
        NWebOutputHandler::Create(width, height, output_frame_cb);

    std::string path = fdp->ConsumeRandomLengthString(64);
    handler->SetDumpPath(path);
}

void NwebOutputHandlerFuzzTest4(FuzzedDataProvider* fdp) {
    auto output_frame_cb = [](const char* data, uint32_t width, uint32_t height) -> bool {
        return false;
    };
    uint32_t width = 10;
    uint32_t height = 10;
    std::shared_ptr<NWebOutputHandler> handler = 
        NWebOutputHandler::Create(width, height, output_frame_cb);

    uint32_t id = fdp->ConsumeIntegralInRange<uint32_t>(1, 10);
    handler->SetNWebId(id);
}

void NwebOutputHandlerFuzzTest5(FuzzedDataProvider* fdp) {
    auto output_frame_cb = [](const char* data, uint32_t width, uint32_t height) -> bool {
        return false;
    };
    uint32_t width = 10;
    uint32_t height = 10;
    std::shared_ptr<NWebOutputHandler> handler = 
        NWebOutputHandler::Create(width, height, output_frame_cb);
    handler->IsSizeValid();
    void* surface = nullptr;
    handler->GetNativeWindowFromSurface(surface);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if((data == nullptr) || (size < sizeof(int32_t))) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    NwebOutputHandlerFuzzTest1(&fdp);
    NwebOutputHandlerFuzzTest(&fdp);
    NwebOutputHandlerFuzzTest2(&fdp);
    NwebOutputHandlerFuzzTest3(&fdp);
    NwebOutputHandlerFuzzTest4(&fdp);
    NwebOutputHandlerFuzzTest5(&fdp);
    return 0;
}
 
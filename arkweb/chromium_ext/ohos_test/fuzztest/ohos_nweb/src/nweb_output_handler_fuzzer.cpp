/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <fuzzer/FuzzedDataProvider.h>
#include "nweb_output_handler.h"

using namespace OHOS::NWeb;

void NwebOutputHandlerFuzzTest(FuzzedDataProvider* fdp) {
    auto output_frame_cb = [](const char* data, uint32_t width, uint32_t height) -> bool {
        return true;
    };
    uint32_t width = fdp->ConsumeIntegral<uint32_t>();
    uint32_t height = fdp->ConsumeIntegral<uint32_t>();
    std::shared_ptr<NWebOutputHandler> handler = 
        NWebOutputHandler::Create(width, height, output_frame_cb);

    width = fdp->ConsumeIntegral<uint32_t>();
    height = fdp->ConsumeIntegral<uint32_t>();
    handler->GetWindowInfo(width, height);
    handler->StartFrameStat();
    handler->StartRenderOutput();
    
    bool flag = fdp->ConsumeBool();
    handler->SetFrameInfoDump(flag);
    std::string path = fdp->ConsumeRandomLengthString(256);
    handler->SetDumpPath(path);

    uint32_t id = fdp->ConsumeIntegral<uint32_t>();
    handler->SetNWebId(id);
    handler->IsSizeValid();
    
    void* surface = nullptr;
    handler->GetNativeWindowFromSurface(surface);
}


extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if((data == nullptr) || (size < sizeof(int32_t))) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    NwebOutputHandlerFuzzTest(&fdp);
    return 0;
}
 
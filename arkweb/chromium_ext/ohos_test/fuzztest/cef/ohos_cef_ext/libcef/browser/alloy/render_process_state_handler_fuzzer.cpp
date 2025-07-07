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

#include <fuzzer/FuzzedDataProvider.h>
#include "cef/ohos_cef_ext/libcef/browser/alloy/render_process_state_handler.h"
#include "base/trace_event/trace_event.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/res_sched_client_adapter.h"
#include "base/logging.h"

void RenderProcessHandlersFuzzTest(FuzzedDataProvider* fdp){
    RenderProcessStateHandler* handler = RenderProcessStateHandler::GetInstance();

    uint32_t render_process_id = fdp->ConsumeIntegralInRange<uint32_t>(1, 64);
    int nweb_id = fdp->ConsumeIntegralInRange<int>(1, 64);
    handler->InitRenderProcessState(render_process_id, nweb_id);

    render_process_id = fdp->ConsumeIntegralInRange<uint32_t>(1, 64);
    nweb_id = fdp->ConsumeIntegralInRange<int>(1, 64);
    bool is_to_background = fdp->ConsumeBool();
    handler->UpdateRenderProcessState(render_process_id, nweb_id, is_to_background);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if((data == nullptr) || (size < sizeof(int32_t))) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    RenderProcessHandlersFuzzTest(&fdp);
    return 0;
}
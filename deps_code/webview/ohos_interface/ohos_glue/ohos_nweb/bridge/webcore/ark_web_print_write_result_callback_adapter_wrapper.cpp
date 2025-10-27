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

#include "ohos_nweb/bridge/ark_web_print_write_result_callback_adapter_wrapper.h"

namespace OHOS::ArkWeb {

void ArkWebPrintWriteResultCallbackAdapterWrapper::WriteResultCallback(const std::string& jobId, uint32_t code)
{
    ArkWebString str = ArkWebStringClassToStruct(jobId);
    ref_->WriteResultCallback(str, code);
    ArkWebStringStructRelease(str);
}

} // namespace OHOS::ArkWeb
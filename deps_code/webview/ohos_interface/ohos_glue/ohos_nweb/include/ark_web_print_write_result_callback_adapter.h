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

#ifndef ARK_WEB_PRINT_WRITE_RESULT_CALLBACK_ADAPTER_H_
#define ARK_WEB_PRINT_WRITE_RESULT_CALLBACK_ADAPTER_H_

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkWebPrintWriteResultCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void WriteResultCallback(const ArkWebString& jobId, uint32_t code) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_PRINT_WRITE_RESULT_CALLBACK_ADAPTER_H_

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

#ifndef ARK_WEB_PRINT_DOCUMENT_ADAPTER_ADAPTER_H_
#define ARK_WEB_PRINT_DOCUMENT_ADAPTER_ADAPTER_H_

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"
#include "ohos_nweb/include/ark_web_print_write_result_callback_adapter.h"
#include "ohos_nweb/include/ark_web_print_attributes_adapter.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebPrintDocumentAdapterAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnStartLayoutWrite(const ArkWebString& jobId, ArkWebRefPtr<ArkWebPrintAttributesAdapter> oldAttrs,
        ArkWebRefPtr<ArkWebPrintAttributesAdapter> newAttrs, uint32_t fd,
        ArkWebRefPtr<ArkWebPrintWriteResultCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual void OnJobStateChanged(const ArkWebString& jobId, uint32_t state) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_PRINT_DOCUMENT_ADAPTER_ADAPTER_H_

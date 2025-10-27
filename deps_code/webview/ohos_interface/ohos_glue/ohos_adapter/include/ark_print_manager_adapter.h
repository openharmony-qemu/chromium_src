/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef ARK_PRINT_MANAGER_ADAPTER_H
#define ARK_PRINT_MANAGER_ADAPTER_H
#pragma once

#include "ohos_adapter/include/ark_web_adapter_structs.h"

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webview)--*/
class ArkPrintWriteResultCallbackAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void WriteResultCallback(ArkWebString jobId, uint32_t code) = 0;
};

/*--ark web(source=webcore)--*/
class ArkPrintDocumentAdapterAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual void OnStartLayoutWrite(const ArkWebString& jobId, const ArkPrintAttributesAdapter& oldAttrs,
        const ArkPrintAttributesAdapter& newAttrs, uint32_t fd,
        ArkWebRefPtr<ArkPrintWriteResultCallbackAdapter> callback) = 0;

    /*--ark web()--*/
    virtual void OnJobStateChanged(const ArkWebString& jobId, uint32_t state) = 0;
};

/*--ark web(source=webview)--*/
class ArkPrintManagerAdapter : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual int32_t StartPrint(
        const ArkWebStringVector& fileList, const ArkWebUint32Vector& fdList, ArkWebString& taskId) = 0;

    /*--ark web()--*/
    virtual int32_t Print(const ArkWebString& printJobName, const ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> listener,
        const ArkPrintAttributesAdapter& printAttributes) = 0;

    /*--ark web()--*/
    virtual int32_t Print(const ArkWebString& printJobName, const ArkWebRefPtr<ArkPrintDocumentAdapterAdapter> listener,
        const ArkPrintAttributesAdapter& printAttributes, void* contextToken) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_PRINT_MANAGER_ADAPTER_H

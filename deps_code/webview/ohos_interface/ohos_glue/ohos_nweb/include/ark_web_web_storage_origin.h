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

#ifndef ARK_WEB_WEB_STORAGE_ORIGIN_H_
#define ARK_WEB_WEB_STORAGE_ORIGIN_H_
#pragma once

#include "base/include/ark_web_base_ref_counted.h"
#include "base/include/ark_web_types.h"

namespace OHOS::ArkWeb {

/*--ark web(source=webcore)--*/
class ArkWebWebStorageOrigin : public virtual ArkWebBaseRefCounted {
public:
    /*--ark web()--*/
    virtual long GetQuota() = 0;

    /*--ark web()--*/
    virtual void SetQuota(long quota) = 0;

    /*--ark web()--*/
    virtual long GetUsage() = 0;

    /*--ark web()--*/
    virtual void SetUsage(long usage) = 0;

    /*--ark web()--*/
    virtual ArkWebString GetOrigin() = 0;

    /*--ark web()--*/
    virtual void SetOrigin(const ArkWebString& origin) = 0;
};

} // namespace OHOS::ArkWeb

#endif // ARK_WEB_WEB_STORAGE_ORIGIN_H_

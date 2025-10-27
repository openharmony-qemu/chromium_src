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

#ifndef ARK_DATASHARE_ADAPTER_IMPL_H
#define ARK_DATASHARE_ADAPTER_IMPL_H
#pragma once

#include "datashare_adapter.h"
#include "ohos_adapter/include/ark_datashare_adapter.h"

namespace OHOS::ArkWeb {

class ArkDatashareAdapterImpl : public ArkDatashareAdapter {
public:
    explicit ArkDatashareAdapterImpl(NWeb::DatashareAdapter&);

    int OpenDataShareUriForRead(const ArkWebString& uriStr) override;

    ArkWebString GetFileDisplayName(const ArkWebString& uriStr) override;

    ArkWebString GetRealPath(const ArkWebString& uriStr) override;

private:
    NWeb::DatashareAdapter& real_;

    IMPLEMENT_REFCOUNTING(ArkDatashareAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_DATASHARE_ADAPTER_IMPL_H

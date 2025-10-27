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

#ifndef ARK_IMF_SELECTION_RANGE_ADAPTER_IMPL_H
#define ARK_IMF_SELECTION_RANGE_ADAPTER_IMPL_H
#pragma once

#include "imf_adapter.h"
#include "ohos_adapter/include/ark_imf_adapter.h"

namespace OHOS::ArkWeb {

class ArkIMFSelectionRangeAdapterImpl : public ArkIMFSelectionRangeAdapter {
public:
    explicit ArkIMFSelectionRangeAdapterImpl(std::shared_ptr<OHOS::NWeb::IMFSelectionRangeAdapter>);

    int32_t GetStart() override;

    int32_t GetEnd() override;

private:
    std::shared_ptr<OHOS::NWeb::IMFSelectionRangeAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkIMFSelectionRangeAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_IMF_SELECTION_RANGE_ADAPTER_IMPL_H

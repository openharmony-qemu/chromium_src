/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_OHOS_FILE_MAPPER_IMPL_H
#define ARK_OHOS_FILE_MAPPER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_ohos_resource_adapter.h"
#include "ohos_resource_adapter.h"

namespace OHOS::ArkWeb {

class ArkOhosFileMapperImpl : public ArkOhosFileMapper {
public:
    explicit ArkOhosFileMapperImpl(std::shared_ptr<OHOS::NWeb::OhosFileMapper>);

    int32_t GetFd() override;

    int32_t GetOffset() override;

    ArkWebString GetFileName() override;

    bool IsCompressed() override;

    void* GetDataPtr() override;

    size_t GetDataLen() override;

    bool UnzipData(uint8_t** dest, size_t& len) override;

private:
    std::shared_ptr<OHOS::NWeb::OhosFileMapper> real_;

    IMPLEMENT_REFCOUNTING(ArkOhosFileMapperImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_OHOS_FILE_MAPPER_IMPL_H

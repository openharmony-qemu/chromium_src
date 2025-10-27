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

#ifndef ARK_CLIP_BOARD_IMAGE_DATA_ADAPTER_IMPL_H
#define ARK_CLIP_BOARD_IMAGE_DATA_ADAPTER_IMPL_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkClipBoardImageDataAdapterImpl : public ArkClipBoardImageDataAdapter {
public:
    explicit ArkClipBoardImageDataAdapterImpl(std::shared_ptr<OHOS::NWeb::ClipBoardImageDataAdapter>);

    virtual int32_t GetColorType() override;

    virtual int32_t GetAlphaType() override;

    virtual uint32_t* GetData() override;

    virtual size_t GetDataSize() override;

    virtual size_t GetRowBytes() override;

    virtual int32_t GetWidth() override;

    virtual int32_t GetHeight() override;

    virtual void SetColorType(int32_t color) override;

    virtual void SetAlphaType(int32_t alpha) override;

    virtual void SetData(uint32_t* data) override;

    virtual void SetDataSize(size_t size) override;

    virtual void SetRowBytes(size_t rowBytes) override;

    virtual void SetWidth(int32_t width) override;

    virtual void SetHeight(int32_t height) override;

private:
    std::shared_ptr<OHOS::NWeb::ClipBoardImageDataAdapter> real_;

    IMPLEMENT_REFCOUNTING(ArkClipBoardImageDataAdapterImpl);
};

} // namespace OHOS::ArkWeb

#endif // ARK_CLIP_BOARD_IMAGE_DATA_ADAPTER_IMPL_H

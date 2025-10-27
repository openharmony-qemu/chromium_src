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

#ifndef ARK_CLIP_BOARD_IMAGE_DATA_ADAPTER_WRAPPER_H
#define ARK_CLIP_BOARD_IMAGE_DATA_ADAPTER_WRAPPER_H
#pragma once

#include "ohos_adapter/include/ark_pasteboard_client_adapter.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

class ArkClipBoardImageDataAdapterWrapper : public NWeb::ClipBoardImageDataAdapter {
public:
    explicit ArkClipBoardImageDataAdapterWrapper(ArkWebRefPtr<ArkClipBoardImageDataAdapter>);

    NWeb::ClipBoardImageColorType GetColorType() override;

    NWeb::ClipBoardImageAlphaType GetAlphaType() override;

    uint32_t* GetData() override;

    size_t GetDataSize() override;

    size_t GetRowBytes() override;

    int32_t GetWidth() override;

    int32_t GetHeight() override;

    void SetColorType(NWeb::ClipBoardImageColorType color) override;

    void SetAlphaType(NWeb::ClipBoardImageAlphaType alpha) override;

    void SetData(uint32_t* data) override;

    void SetDataSize(size_t size) override;

    void SetRowBytes(size_t rowBytes) override;

    void SetWidth(int32_t width) override;

    void SetHeight(int32_t height) override;

private:
    ArkWebRefPtr<ArkClipBoardImageDataAdapter> ctocpp_;
};

} // namespace OHOS::ArkWeb

#endif // ARK_CLIP_BOARD_IMAGE_DATA_ADAPTER_WRAPPER_H

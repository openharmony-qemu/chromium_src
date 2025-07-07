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

#include <fuzzer/FuzzedDataProvider.h>
#include <cstring>
#include <cstdint>
#include "ohos_drawing_text_adapter_impl.h"
#include "ohos_drawing_text_adapter.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/native_drawing/drawing_types.h"
#include "ohos_sdk/openharmony/native/sysroot/usr/include/native_drawing/drawing_text_typography.h"

using namespace OHOS::NWeb;

void FuzzdDrawingTextAdapter(FuzzedDataProvider* fdp) {
    OhosDrawingTextFontAdapterImpl& adapter =
     OhosDrawingTextFontAdapterImpl::GetInstance();
    int32_t systemFontType = fdp->ConsumeIntegral<int32_t>();
    void* drawingArray = nullptr;
    adapter.GetSystemFontFullNamesByType(systemFontType, &drawingArray);

    OH_Drawing_String obj;
    uint8_t dataNweb = fdp->ConsumeIntegral<uint8_t>();
    obj.strData=&dataNweb;
    obj.strLen=fdp->ConsumeIntegral<uint32_t>();
    void* drawingFontDescriptor=nullptr;
    adapter.GetFontDescriptorByFullName(&obj, systemFontType, &drawingFontDescriptor);

    int32_t indexOfFullName = fdp->ConsumeIntegral<int32_t>();
    const void* drawingString=nullptr;
    adapter.GetSystemFontFullNameByIndex(drawingArray, indexOfFullName, &drawingString);
    adapter.DestroySystemFontFullNames(drawingArray);
    void* descriptor=nullptr;
    adapter.DestroyFontDescriptor(descriptor);
}

void FuzzdDrawingTextTypographyAdapter(FuzzedDataProvider* fdp) {
    OhosDrawingTextTypographyAdapter& adapter = 
    OhosDrawingTextTypographyAdapter::GetInstance();
     
    int errorCode=fdp->ConsumeIntegralInRanger<int>(0, 5);
    OH_Drawing_FontConfigInfoErrorCode errorPtr=static_cast<OH_Drawing_FontConfigInfoErrorCode>(errorCode);
    void* fontConfigInfo=nullptr;
    adapter.GetSystemFontConfigInfo(&errorPtr, &fontConfigInfo);

    void* drawingArray=nullptr;
    int32_t sizeOfArray=fdp->ConsumeIntegral<int32_t>();
    adapter.GetDrawingArraySize(drawingArray, sizeOfArray);

    adapter.DestroySystemFontConfigInfo(nullptr);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if((data == nullptr) || (size < sizeof(int32_t))) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    FuzzdDrawingTextAdapter(&fdp);
    FuzzdDrawingTextTypographyAdapter(&fdp);
    return 0;
}
 
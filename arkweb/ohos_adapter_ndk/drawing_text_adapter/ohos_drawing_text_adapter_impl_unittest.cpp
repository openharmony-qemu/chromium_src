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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "native_drawing/drawing_text_typography.h"
#include "ohos_drawing_text_adapter_impl.h"

namespace OHOS::NWeb {

class OhosDrawingTextAdapterImplTest : public testing::Test {};

TEST_F(OhosDrawingTextAdapterImplTest, GetSystemFontInfoTest)
{
    auto& font_adapter = OhosDrawingTextFontAdapterImpl::GetInstance();
    void* font_array = nullptr;

    int result = font_adapter.GetSystemFontFullNamesByType(1, &font_array);
    EXPECT_EQ(result, 0);

    if(result != -1){
        auto& typo_adapter = OhosDrawingTextTypographyAdapterImpl::GetInstance();
        int32_t array_size = 0;
        int get_size_result = typo_adapter.GetDrawingArraySize(font_array, array_size);
        EXPECT_EQ(get_size_result, 0);

        const void* font_name = nullptr;
        int get_font_name_result = font_adapter.GetSystemFontFullNameByIndex(font_array, 0, &font_name);
        EXPECT_EQ(get_font_name_result, 0);

        void* font_discriptor = nullptr;
        int get_font_disc_result = font_adapter.GetFontDescriptorByFullName(const_cast<void*>(font_name), 1, &font_discriptor);
        EXPECT_EQ(get_font_name_result, 0);

        font_adapter.DestroyFontDescriptor(font_discriptor);

    }
    font_adapter.DestroySystemFontFullNames(font_array);

}

TEST_F(OhosDrawingTextAdapterImplTest, GetSystemFontConfigInfoSuccess)
{
    auto& typo_adapter = OhosDrawingTextTypographyAdapterImpl::GetInstance();
    OH_Drawing_FontConfigInfoErrorCode success_code = OH_Drawing_FontConfigInfoErrorCode::SUCCESS_FONT_CONFIG_INFO;
    void* font_config = nullptr;

    int get_config_result;
    get_config_result = typo_adapter.GetSystemFontConfigInfo(&success_code, &font_config);
    EXPECT_EQ(get_config_result, 0);
    typo_adapter.DestroySystemFontConfigInfo(font_config);

}

}   // namespace OHOS::NWeb
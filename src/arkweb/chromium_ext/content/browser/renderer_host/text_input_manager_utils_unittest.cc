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
 
#include <memory>

#include "content/browser/renderer_host/text_input_manager_utils.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"

namespace content {
using namespace ui::mojom;

constexpr int OFFSET_VAL = 0;
class TextInputManagerUtilsTest: public::testing::Test {
public:
    void SetUp() {
        text_input_manager_ = std::make_shared<TextInputManager>();
        EXPECT_TRUE(text_input_manager_);
    }

    std::shared_ptr<TextInputManager> GetTextInputManager() {
        return text_input_manager_;
    }

    void SetMap(RenderWidgetHostViewBase* view, const std::u16string& text, 
                size_t offset, const gfx::Range& range) {
        GetTextInputManager()->text_selection_map_.clear();
        GetTextInputManager()->text_selection_map_[view] = TextInputManager::TextSelection();
        GetTextInputManager()->text_selection_map_[view].SetSelection(text, offset, range);
    }

private:
    std::shared_ptr<TextInputManager> text_input_manager_;
};

TEST_F(TextInputManagerUtilsTest, ArkWebUpdateSelectionMap)
{
    auto utils = GetTextInputManager()->imp_utils_.get();
    TextInputState text_input_state;
    ASSERT_NO_FATAL_FAILURE(utils->ArkWebUpdateSelectionMap(nullptr, text_input_state));

    TextInputManager::TextSelection text_selection;
    std::u16string text;
    SetMap(nullptr, text, OFFSET_VAL, gfx::Range());
    ASSERT_NO_FATAL_FAILURE(utils->ArkWebUpdateSelectionMap(nullptr, text_input_state));

    text_input_state.value = u"a";
    ASSERT_NO_FATAL_FAILURE(utils->ArkWebUpdateSelectionMap(nullptr, text_input_state));

    SetMap(nullptr, u"a", OFFSET_VAL, gfx::Range());
    ASSERT_NO_FATAL_FAILURE(utils->ArkWebUpdateSelectionMap(nullptr, text_input_state));
}

}  // namespace content
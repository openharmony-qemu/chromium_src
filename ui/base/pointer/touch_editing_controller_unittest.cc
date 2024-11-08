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

#define private public
#include "ui/base/pointer/touch_editing_controller.h"
#undef private
#include "gtest/gtest.h"

namespace ui {
class TouchEditableTest : public ::testing::Test {
 public:
  std::shared_ptr<TouchEditable> touch_edit;
};

TEST_F(TouchEditableTest, ConvertMenuCommands001) {
  int menu_commands = TouchEditable::OhosMenuCommands::QM_EDITFLAG_CAN_CUT;
  touch_edit->ConvertMenuCommands(menu_commands);
  EXPECT_EQ(TouchEditable::MenuCommands::kCut,
            touch_edit->ConvertMenuCommands(menu_commands));
}

TEST_F(TouchEditableTest, ConvertMenuCommands002) {
  int menu_commands = TouchEditable::OhosMenuCommands::QM_EDITFLAG_CAN_COPY;
  touch_edit->ConvertMenuCommands(menu_commands);
  EXPECT_EQ(TouchEditable::MenuCommands::kCopy,
            touch_edit->ConvertMenuCommands(menu_commands));
}

TEST_F(TouchEditableTest, ConvertMenuCommands003) {
  int menu_commands = TouchEditable::OhosMenuCommands::QM_EDITFLAG_CAN_PASTE;
  touch_edit->ConvertMenuCommands(menu_commands);
  EXPECT_EQ(TouchEditable::MenuCommands::kPaste,
            touch_edit->ConvertMenuCommands(menu_commands));
}

TEST_F(TouchEditableTest, ConvertMenuCommands004) {
  int menu_commands =
      TouchEditable::OhosMenuCommands::QM_EDITFLAG_CAN_SELECT_ALL;
  touch_edit->ConvertMenuCommands(menu_commands);
  EXPECT_EQ(TouchEditable::MenuCommands::kSelectAll,
            touch_edit->ConvertMenuCommands(menu_commands));
}

TEST_F(TouchEditableTest, ConvertMenuCommands005) {
  int menu_commands = TouchEditable::OhosMenuCommands::QM_EDITFLAG_NONE;
  touch_edit->ConvertMenuCommands(menu_commands);
  EXPECT_EQ(TouchEditable::OhosMenuCommands::QM_EDITFLAG_NONE,
            touch_edit->ConvertMenuCommands(menu_commands));
}

}  // namespace ui

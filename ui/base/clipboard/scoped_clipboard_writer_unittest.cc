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

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#define private public
#include "ui/base/clipboard/scoped_clipboard_writer.h"
#undef private

namespace ui {
class ScopedClipboardWriterTest : public ::testing::Test {
 public:
  void SetUp() override {}

  void TearDown() override {}

  std::shared_ptr<ScopedClipboardWriter> scoped_clip_board;
};

TEST_F(ScopedClipboardWriterTest, TransitionCopyOption_0001) {
  blink::mojom::CopyOptionMode copy_option_ =
      blink::mojom::CopyOptionMode::NONE;
  auto result = scoped_clip_board->TransitionCopyOption(copy_option_);
  EXPECT_EQ(result, static_cast<ui::CopyOptionMode>(0));
}

TEST_F(ScopedClipboardWriterTest, TransitionCopyOption_0002) {
  blink::mojom::CopyOptionMode copy_option_ =
      blink::mojom::CopyOptionMode::IN_APP;
  auto result = scoped_clip_board->TransitionCopyOption(copy_option_);
  EXPECT_EQ(result, static_cast<ui::CopyOptionMode>(1));
}

TEST_F(ScopedClipboardWriterTest, TransitionCopyOption_0003) {
  blink::mojom::CopyOptionMode copy_option_ =
      blink::mojom::CopyOptionMode::LOCAL_DEVICE;
  auto result = scoped_clip_board->TransitionCopyOption(copy_option_);
  EXPECT_EQ(result, static_cast<ui::CopyOptionMode>(2));
}

TEST_F(ScopedClipboardWriterTest, TransitionCopyOption_0004) {
  blink::mojom::CopyOptionMode copy_option_ =
      blink::mojom::CopyOptionMode::CROSS_DEVICE;
  auto result = scoped_clip_board->TransitionCopyOption(copy_option_);
  EXPECT_EQ(result, static_cast<ui::CopyOptionMode>(3));
}

TEST_F(ScopedClipboardWriterTest, TransitionCopyOption_0005) {
  blink::mojom::CopyOptionMode copy_option_ =
      blink::mojom::CopyOptionMode::kMaxValue;
  auto result = scoped_clip_board->TransitionCopyOption(copy_option_);
  EXPECT_EQ(result, static_cast<ui::CopyOptionMode>(3));
}
}  // namespace ui

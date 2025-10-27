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
namespace blink {
TEST_F(FrameSelectionTest, MoveRangeSelection) {
  SetBodyContent("<div id=sample>0123456789</div>abc");
  Element* anchor = GetDocument().getElementById(AtomicString("anchor"));
  gfx::Point point;
  Selection().AsFrameSelectionExt()->MoveRangeSelection(point, true, TextGranularity::kCharacter);
  Selection().AsFrameSelectionExt()->MoveRangeSelection(gfx::Point(-5, -5), true, TextGranularity::kCharacter);
  Selection().AsFrameSelectionExt()->MoveRangeSelection(gfx::Point(5, 5), true, TextGranularity::kCharacter);
  Selection().AsFrameSelectionExt()->MoveRangeSelection(gfx::Point(1000, 1000), true, TextGranularity::kCharacter);

  Selection().AsFrameSelectionExt()->MoveRangeSelection(point, false, TextGranularity::kCharacter);  
  Selection().AsFrameSelectionExt()->MoveRangeSelection(gfx::Point(-5, -5), false, TextGranularity::kCharacter);
  Selection().AsFrameSelectionExt()->MoveRangeSelection(gfx::Point(5, 5), false, TextGranularity::kCharacter);
  Selection().AsFrameSelectionExt()->MoveRangeSelection(gfx::Point(1000, 1000), false, TextGranularity::kCharacter);
}

}  // namespace blink

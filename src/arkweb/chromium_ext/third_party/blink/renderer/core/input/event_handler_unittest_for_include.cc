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
#if BUILDFLAG(ARKWEB_EXT_FREE_COPY)
TEST_F(EventHandlerSimTest, SendContextMenuEventExt) {
  WebMouseEvent mouse_down_event(
      WebMouseEvent::Type::kMouseDown, gfx::PointF(0, 0), gfx::PointF(100, 200),
      WebPointerProperties::Button::kRight, 1,
      WebInputEvent::Modifiers::kRightButtonDown, base::TimeTicks::Now());
  GetDocument().GetFrame()->GetEventHandler().SendContextMenuEvent(
      mouse_down_event);

  mouse_down_event.menu_source_type = kMenuSourceShowFreeCopyMenu;
  GetDocument().GetFrame()->GetEventHandler().SendContextMenuEvent(
      mouse_down_event);
}

TEST_F(EventHandlerSimTest, ShowNonLocatedContextMenuExt) {
  GetDocument().GetFrame()->GetEventHandler().ShowNonLocatedContextMenu(
      nullptr, kMenuSourceTouchHandle);

  GetDocument().GetFrame()->GetEventHandler().ShowNonLocatedContextMenu(
      nullptr, kMenuSourceShowFreeCopyMenu);
}
#endif
}  // namespace blink
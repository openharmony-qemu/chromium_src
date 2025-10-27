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

#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/googletest/src/googletest/include/gtest/gtest.h"

namespace blink {

namespace {
  constexpr char BASE64_RED_IMAGE_DATA_3_3[] =
    "iVBORw0KGgoAAAANSUhEUgAAAAMAAAADCAIAAADZSiLoAAAAEUlEQVR4n"
    "GP8zwAFTDAGEgsAHVsBBeg3qMUAAAAASUVORK5CYII=";
  constexpr int SCALE_VAL_A = 1;
  constexpr int SCALE_VAL_B = 2;
  constexpr int POINT_VAL_A = 5;
  constexpr int POINT_VAL_B = 10;
}

TEST_F(DragControllerTest, DragLinkWithRestoreDragLinkEffects) {
  SetBodyInnerHTML(R"HTML(
    <style>
      * { margin: 0; }
      a {
        width: 50px;
        height: 40px;
        font-size: 30px;
        margin-top: 2px;
        display: block;
      }
    </style>
    <a id='drag' href='https://foobarbaz.com'>foobarbaz</a>
  )HTML");
  const int page_scale_factor = 2;
  GetFrame().GetPage()->SetPageScaleFactor(page_scale_factor);
  GetFrame().Selection().SelectAll();

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kNoModifiers,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.button = WebMouseEvent::Button::kRight;
  mouse_event.SetFrameScale(1);
  mouse_event.SetPositionInWidget(5, 10);
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_type_ = kDragSourceActionLink;
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  drag_state.drag_data_transfer_ = DataTransfer::Create(
      DataTransfer::kDragAndDrop, DataTransferAccessPolicy::kWritable,
      DataObject::Create());
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
  EXPECT_TRUE(GetFrame().GetPage()->GetDragController().StartDrag(
      &GetFrame(), drag_state, mouse_event, gfx::Point(5, 10)));
  EXPECT_TRUE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInImageDraging());
  GetFrame().GetPage()->GetDragController().DragEnded();
}

TEST_F(DragControllerTest, DragLinkWithRestoreDragLinkEffectsCustomStyle) {
  SetBodyInnerHTML(R"HTML(
    <style>
      * { margin: 0; }
      a {
        width: 50px;
        height: 40px;
        font-size: 30px;
        margin-top: 2px;
        display: block;
      }
    </style>
    <a id='drag' href='https://foobarbaz.com' style='color: gray;'>foobarbaz</a>
  )HTML");
  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_type_ = kDragSourceActionLink;
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  drag_state.drag_data_transfer_ = DataTransfer::Create(
      DataTransfer::kDragAndDrop, DataTransferAccessPolicy::kWritable,
      DataObject::Create());
  GetFrame().GetPage()->GetDragController().DragEnded();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());

  GetFrame().Selection().SelectAll();

  SetBodyInnerHTML(R"HTML(
    <a id='drag' href='https://foobarbaz.com' style='color: gray'>foobarbaz</a>
  )HTML");
  EXPECT_NE(GetDocument().getElementById(AtomicString("drag")), nullptr);
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  GetFrame().GetPage()->GetDragController().DragEnded();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());

  SetBodyInnerHTML(R"HTML(
    <a id='drag' href='https://foobarbaz.com' style='color: gray'>foobarbaz</a>
  )HTML");
  EXPECT_NE(GetDocument().getElementById(AtomicString("drag")), nullptr);
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  GetFrame().GetPage()->GetDragController().DragEnded();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
}

TEST_F(DragControllerTest, DragImageForIsInImageDraging) {
  SetBodyInnerHTML(R"HTML(
    <img id='drag' src="data:image/png;base64,)HTML" +
                     String(BASE64_RED_IMAGE_DATA_3_3) +
                     R"HTML(" width="100%" height="auto">
  )HTML");
  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kNoModifiers,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.button = WebMouseEvent::Button::kRight;
  mouse_event.SetFrameScale(1);
  mouse_event.SetPositionInWidget(5, 10);
  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_type_ = kDragSourceActionImage;
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  drag_state.drag_data_transfer_ = DataTransfer::Create(
      DataTransfer::kDragAndDrop, DataTransferAccessPolicy::kWritable,
      DataObject::Create());
  GetFrame().GetPage()->GetDragController().StartDrag(
      &GetFrame(), drag_state, mouse_event, gfx::Point(5, 10));
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInImageDraging());
  GetFrame().GetPage()->GetDragController().DragEnded();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInImageDraging());
  drag_state.drag_type_ = kDragSourceActionLink;
  GetFrame().GetPage()->GetDragController().StartDragTextEffects();
}

TEST_F(DragControllerTest, FindAndRemoveGrayStyle) {
  SetBodyInnerHTML(R"HTML(
    <style>
      * { margin: 0; }
      a {
        width: 50px;
        height: 40px;
        font-size: 30px;
        margin-top: 2px;
        display: block;
        ;
      }
    </style>
    <a id='drag' href='https://foobarbaz.com'>foobarbaz</a>
  )HTML");
  Element* element = GetDocument().documentElement();
  ASSERT_NE(element, nullptr);

  element->setAttribute(html_names::kStyleAttr, AtomicString("; color: gray;"));
  GetFrame().GetPage()->GetDragController().FindAndRemoveGrayStyle(element);

  element->setAttribute(html_names::kStyleAttr, AtomicString("; color: gray; display: block;"));
  GetFrame().GetPage()->GetDragController().FindAndRemoveGrayStyle(element);

  element->setAttribute(html_names::kStyleAttr, AtomicString("color: gray;"));
  GetFrame().GetPage()->GetDragController().FindAndRemoveGrayStyle(element);

  element->setAttribute(html_names::kStyleAttr, AtomicString("color: gray"));
  GetFrame().GetPage()->GetDragController().FindAndRemoveGrayStyle(element);

  element->setAttribute(html_names::kStyleAttr, AtomicString("display: block;"));
  GetFrame().GetPage()->GetDragController().FindAndRemoveGrayStyle(element);
}

TEST_F(DragControllerTest, DragStateNull) {
  SetBodyInnerHTML(R"HTML(
    <style>
      * { margin: 0; }
      a {
        width: 50px;
        height: 40px;
        font-size: 30px;
        margin-top: 2px;
        display: block;
      }
    </style>
    <a id='drag' href='https://foobarbaz.com'>foobarbaz</a>
  )HTML");
  const int page_scale_factor = 2;
  GetFrame().GetPage()->SetPageScaleFactor(page_scale_factor);
  GetFrame().Selection().SelectAll();

  // set drag_state to null
  GetFrame().GetPage()->GetDragController().ContextDestroyed();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
  GetFrame().GetPage()->GetDragController().StartDragTextEffects();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInTextDraging());
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInImageDraging());
  GetFrame().GetPage()->GetDragController().StartDragImageEffects();
  GetFrame().GetPage()->GetDragController().UpdateLinkStyle(nullptr);
  GetFrame().GetPage()->GetDragController().RestoreLinkStyle(nullptr);
  GetFrame().GetPage()->GetDragController().InvalidateSelectionForDrag(nullptr);
  GetFrame().GetPage()->GetDragController().GetVisibleRectToUIInRootFrame(nullptr);

  // make drag_state
  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kNoModifiers,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.button = WebMouseEvent::Button::kRight;
  mouse_event.SetFrameScale(1);
  mouse_event.SetPositionInWidget(5, 10);
  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_type_ = kDragSourceActionLink;
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  drag_state.drag_data_transfer_ = DataTransfer::Create(
      DataTransfer::kDragAndDrop, DataTransferAccessPolicy::kWritable,
      DataObject::Create());
  EXPECT_TRUE(GetFrame().GetPage()->GetDragController().StartDrag(
      &GetFrame(), drag_state, mouse_event, gfx::Point(5, 10)));
  drag_state.drag_type_ = kDragSourceActionSelection;
  EXPECT_TRUE(GetFrame().GetPage()->GetDragController().IsInTextDraging());
  drag_state.drag_type_ = kDragSourceActionImage;
  EXPECT_TRUE(GetFrame().GetPage()->GetDragController().IsInImageDraging());
  drag_state.drag_src_ = nullptr;
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().DragLinkCheckSrcAndType());
  GetFrame().GetPage()->GetDragController().DragEnded();
  drag_state.drag_type_ = kDragSourceActionSelection;
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInTextDraging());
}

TEST_F(DragControllerTest, StartDragTextEffects001) {
  SetBodyInnerHTML(R"HTML(
    <img id='drag' src="data:image/png;base64,)HTML" +
                     String(BASE64_RED_IMAGE_DATA_3_3) +
                     R"HTML(" width="100%" height="auto">
  )HTML");
  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kNoModifiers,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.button = WebMouseEvent::Button::kRight;
  mouse_event.SetFrameScale(1);
  mouse_event.SetPositionInWidget(5, 10);
  ASSERT_TRUE(GetFrame().GetPage());
  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_type_ = kDragSourceActionImage;
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  drag_state.drag_data_transfer_ = DataTransfer::Create(
      DataTransfer::kDragAndDrop, DataTransferAccessPolicy::kWritable,
      DataObject::Create());
  GetFrame().GetPage()->GetDragController().StartDrag(
      &GetFrame(), drag_state, mouse_event, gfx::Point(5, 10));
  GetFrame().GetPage()->GetDragController().StartDragTextEffects();
  GetFrame().GetPage()->GetDragController().DragEnded();

  drag_state.drag_src_ = nullptr;
  GetFrame().GetPage()->GetDragController().StartDragTextEffects();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInTextDraging());
}

TEST_F(DragControllerTest, RestoreDragTextEffects001) {
  SetBodyInnerHTML(R"HTML(
    <img id='drag' src="data:image/png;base64,)HTML" +
                     String(BASE64_RED_IMAGE_DATA_3_3) +
                     R"HTML(" width="100%" height="auto">
  )HTML");
  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kNoModifiers,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.button = WebMouseEvent::Button::kRight;
  mouse_event.SetFrameScale(1);
  mouse_event.SetPositionInWidget(5, 10);
  ASSERT_TRUE(GetFrame().GetPage());
  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_type_ = kDragSourceActionImage;
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  drag_state.drag_data_transfer_ = DataTransfer::Create(
      DataTransfer::kDragAndDrop, DataTransferAccessPolicy::kWritable,
      DataObject::Create());
  GetFrame().GetPage()->GetDragController().StartDrag(
      &GetFrame(), drag_state, mouse_event, gfx::Point(5, 10));
  GetFrame().GetPage()->GetDragController().DragEnded();
  drag_state.drag_type_ = kDragSourceActionLink;
  GetFrame().GetPage()->GetDragController().RestoreDragTextEffects();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInTextDraging());
}

TEST_F(DragControllerTest, StartDragImageEffects001) {
  SetBodyInnerHTML(R"HTML(
    <style>
      * { margin: 0; }
      a {
        width: 50px;
        height: 40px;
        font-size: 30px;
        margin-top: 2px;
        display: block;
      }
    </style>
    <a id='drag' href='https://foobarbaz.com'>foobarbaz</a>
  )HTML");
  const int page_scale_factor = SCALE_VAL_B;
  ASSERT_TRUE(GetFrame().GetPage());
  GetFrame().GetPage()->SetPageScaleFactor(page_scale_factor);
  GetFrame().Selection().SelectAll();

  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_src_ = nullptr;
  GetFrame().GetPage()->GetDragController().StartDragImageEffects();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInImageDraging());
}

TEST_F(DragControllerTest, RestoreDragImageEffects001) {
  SetBodyInnerHTML(R"HTML(
    <style>
      * { margin: 0; }
      a {
        width: 50px;
        height: 40px;
        font-size: 30px;
        margin-top: 2px;
        display: block;
      }
    </style>
    <a id='drag' href='https://foobarbaz.com'>foobarbaz</a>
  )HTML");
  const int page_scale_factor = SCALE_VAL_B;
  ASSERT_TRUE(GetFrame().GetPage());
  GetFrame().GetPage()->SetPageScaleFactor(page_scale_factor);
  GetFrame().Selection().SelectAll();

  GetFrame().GetPage()->GetDragController().ContextDestroyed();
  GetFrame().GetPage()->GetDragController().StartDragTextEffects();
  GetFrame().GetPage()->GetDragController().RestoreDragImageEffects();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInImageDraging());
}

TEST_F(DragControllerTest, StartDragLinkEffects001) {
  SetBodyInnerHTML(R"HTML(
    <style>
      * { margin: 0; }
      a {
        width: 50px;
        height: 40px;
        font-size: 30px;
        margin-top: 2px;
        display: block;
      }
    </style>
    <a id='drag' href='https://foobarbaz.com'>foobarbaz</a>
  )HTML");
  const int page_scale_factor = SCALE_VAL_B;
  ASSERT_TRUE(GetFrame().GetPage());
  GetFrame().GetPage()->SetPageScaleFactor(page_scale_factor);
  GetFrame().Selection().SelectAll();

  WebMouseEvent mouse_event(WebInputEvent::Type::kMouseDown,
                            WebInputEvent::kNoModifiers,
                            WebInputEvent::GetStaticTimeStampForTests());
  mouse_event.button = WebMouseEvent::Button::kRight;
  mouse_event.SetFrameScale(SCALE_VAL_A);
  mouse_event.SetPositionInWidget(POINT_VAL_A, POINT_VAL_B);

  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_type_ = kDragSourceActionLink;
  drag_state.drag_src_ = GetDocument().getElementById(AtomicString("drag"));
  drag_state.drag_data_transfer_ = DataTransfer::Create(
      DataTransfer::kDragAndDrop, DataTransferAccessPolicy::kWritable,
      DataObject::Create());

  EXPECT_TRUE(GetFrame().GetPage()->GetDragController().StartDrag(
      &GetFrame(), drag_state, mouse_event, gfx::Point(POINT_VAL_A, POINT_VAL_B)));

  Node* node = drag_state.drag_src_.Get();
  ASSERT_TRUE(node);
  node->SetLayoutObject(nullptr);
  GetFrame().GetPage()->GetDragController().StartDragLinkEffects();
  GetFrame().GetPage()->GetDragController().RestoreDragLinkEffects();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
}

TEST_F(DragControllerTest, DragLinkCheckSrcAndType) {
  SetBodyInnerHTML(R"HTML(
    <style>
      * { margin: 0; }
      a {
        width: 50px;
        height: 40px;
        font-size: 30px;
        margin-top: 2px;
        display: block;
      }
    </style>
    <a id='drag' href='https://foobarbaz.com'>foobarbaz</a>
  )HTML");
  const int page_scale_factor = 2;
  ASSERT_TRUE(GetFrame().GetPage());
  GetFrame().GetPage()->SetPageScaleFactor(page_scale_factor);
  GetFrame().Selection().SelectAll();

  // set drag_state to null
  GetFrame().GetPage()->GetDragController().ContextDestroyed();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsHyperLinkDragging());
  GetFrame().GetPage()->GetDragController().StartDragTextEffects();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInTextDraging());
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().IsInImageDraging());

  auto& drag_state = GetFrame().GetPage()->GetDragController().GetDragState();
  drag_state.drag_type_ = kDragSourceActionLink;
  drag_state.drag_src_ = nullptr;

  GetFrame().GetPage()->GetDragController().SetDragInitState(true);
  GetFrame().GetPage()->GetDragController().StartDragTextEffects();
  GetFrame().GetPage()->GetDragController().StartDragImageEffects();
  EXPECT_FALSE(GetFrame().GetPage()->GetDragController().DragLinkCheckSrcAndType());
}

}
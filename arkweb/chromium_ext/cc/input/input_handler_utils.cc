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

#include "input_handler_utils.h"

#include "base/trace_event/trace_event.h"
#include "base/trace_event/traced_value.h"
#include "cc/input/scrollbar_controller.h"
#include "cc/layers/layer_impl.h"
#include "cc/trees/layer_tree_impl.h"
#include "cc/trees/layer_tree_impl_utils.h"

namespace cc {

InputHandlerUtils::InputHandlerUtils(InputHandler* handler)
  : handler_(handler) {}

// LCOV_EXCL_START
InputHandlerUtils::~InputHandlerUtils() {}

#if BUILDFLAG(ARKWEB_INPUT_EVENTS)
void InputHandlerUtils::HandleScrollUpdateForInternalBeginFrame(
    const viz::BeginFrameArgs& args) {
  TRACE_EVENT0("cc",
               "InputHandlerUtils::HandleScrollUpdateForInternalBeginFrame");
  handler_->input_handler_client_->WillHandleScrollUpdateForInternalBeginFrame(args);
  handler_->scrollbar_controller_->WillBeginImplFrame();
  handler_->input_handler_client_->DeliverInputForBeginFrame(args);
}
#endif  // BUILDFLAG(ARKWEB_INPUT_EVENTS)
// LCOV_EXCL_STOP

#if BUILDFLAG(ARKWEB_SAME_LAYER)
LayerImpl* InputHandlerUtils::GetLayerImplIsHitByPoint(
    const gfx::Point& viewport_point) {
  gfx::PointF device_viewport_point = gfx::ScalePoint(
      gfx::PointF(viewport_point), handler_->compositor_delegate_->DeviceScaleFactor());
  return handler_->ActiveTree().FindLayerThatIsHitByPoint(device_viewport_point);
}

LayerImpl* InputHandlerUtils::GetNativeLayerImpl(const gfx::Point& viewport_point) {
  gfx::PointF device_viewport_point = gfx::ScalePoint(
      gfx::PointF(viewport_point), handler_->compositor_delegate_->DeviceScaleFactor());
  return handler_->ActiveTree().FindLayerThatIsHitByPointNative(device_viewport_point);
}

// LCOV_EXCL_START
LayerImpl* InputHandlerUtils::GetLayerImplById(int id) {
  return handler_->ActiveTree().LayerById(id);
}
bool InputHandlerUtils::IsNativeLayer(gfx::PointF device_viewport_point) {
  LayerImpl* layer_impl =
        handler_->ActiveTree().FindLayerThatIsHitByPoint(device_viewport_point);
  return layer_impl && layer_impl->layer_impl_utils()->may_contain_native();
}
// LCOV_EXCL_STOP
#endif

// LCOV_EXCL_START
#if BUILDFLAG(ARKWEB_PDF)
double InputHandlerUtils::pdf_delta_x_ = 0;
double InputHandlerUtils::pdf_delta_y_ = 0;

bool InputHandlerUtils::PdfOverSpeed() {  
  constexpr float kMaxPdfOverSpeed = 400;
  if (InputHandlerUtils::pdf_delta_x_ > kMaxPdfOverSpeed)
    return true;
  else if (InputHandlerUtils::pdf_delta_x_ < -kMaxPdfOverSpeed)
    return true;

  if (InputHandlerUtils::pdf_delta_y_ > kMaxPdfOverSpeed)
    return true;
  else if (InputHandlerUtils::pdf_delta_y_ < -kMaxPdfOverSpeed)
    return true;
  
  return false;
}

void InputHandlerUtils::SetScrollStopped() {
  pdf_delta_x_ = 0;
  pdf_delta_y_ = 0;
}
#endif
// LCOV_EXCL_STOP

}
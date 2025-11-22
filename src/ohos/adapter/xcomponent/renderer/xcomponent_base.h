/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_BASE_H_
#define OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_BASE_H_

#include <string>

#include "ohos/adapter/xcomponent/xcomponent_delegate.h"

namespace ohos::adapter::xcomponent {

class XComponentBase {
 public:
  XComponentBase(const std::string& id) { this->id_ = id; }
  virtual ~XComponentBase() = default;

  std::string GetId() { return id_; }
  XComponentType GetType() { return type_; }
  void SetWidget(int32_t widget_id) { widget_id_ = widget_id; }
  int32_t GetWidget() const { return widget_id_; }

  virtual void SendWindowMouseEventForTabDrag(
      Input_MouseEvent* window_mouse_event) {}  // NOT_IMPLEMENTED
  virtual void SendWindowTouchEventForTabDrag(
      Input_TouchEvent* window_touch_event) {}  // NOT_IMPLEMENTED

 protected:
  XComponentDelegate* delegate_ = nullptr;
  std::string id_;
  XComponentType type_;
  int32_t widget_id_;

  bool is_event_reissuance_required_ = false;
  bool has_hover_event_before_register_ = false;
  bool is_hover_before_register_ = false;
};

}  // namespace ohos::adapter::xcomponent

#endif  // OHOS_ADAPTER_XCOMPONENT_RENDERER_XCOMPONENT_BASE_H_

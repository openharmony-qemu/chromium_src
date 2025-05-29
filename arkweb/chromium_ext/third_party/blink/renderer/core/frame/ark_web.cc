/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "third_party/blink/renderer/core/frame/ark_web.h"

#include "base/base_switches.h"
#include "base/strings/stringprintf.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/core/page/page_utils.h"

namespace blink {

ArkWeb::ArkWeb(LocalDOMWindow* window) : ExecutionContextClient(window) {}

void ArkWeb::Trace(Visitor* visitor) const {
  ScriptWrappable::Trace(visitor);
  ExecutionContextClient::Trace(visitor);
}

void ArkWeb::setScrollbarAvoidAreaTop(double margin,
                                      ExceptionState& exception_state) {
  LOG(INFO) << " func:" << __FUNCTION__ << " margin=" << margin;
  if (DomWindow() && DomWindow()->GetFrame() &&
      DomWindow()->GetFrame()->GetPage()) {
    if (DomWindow()->GetFrame()->GetPage()->page_utils()->SetScrollbarAvoidAreaTop(margin)) {
      scrollbar_avoid_area_top_ = margin;
    }
  } else {
    exception_state.ThrowSecurityError(
        "May not use a ArkWeb object associated with a Document that is "
        "not fully active");
  }
}

void ArkWeb::setScrollbarAvoidAreaBottom(double margin,
                                         ExceptionState& exception_state) {
  LOG(INFO) << " func:" << __FUNCTION__ << "margin=" << margin;
  if (DomWindow() && DomWindow()->GetFrame() &&
      DomWindow()->GetFrame()->GetPage()) {
    if (DomWindow()->GetFrame()->GetPage()->page_utils()->SetScrollbarAvoidAreaBottom(margin)) {
      scrollbar_avoid_area_bottom_ = margin;
    }
  } else {
    exception_state.ThrowSecurityError(
        "May not use a ArkWeb object associated with a Document that is "
        "not fully active");
  }
}

}  // namespace blink
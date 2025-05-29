/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ARK_WEB_H_
#define THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ARK_WEB_H_

#include <limits>

#include "third_party/blink/renderer/core/core_export.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_observer.h"
#include "third_party/blink/renderer/platform/bindings/script_wrappable.h"
#include "third_party/blink/renderer/platform/heap/garbage_collected.h"

namespace blink {

class LocalDOMWindow;
class ExceptionState;

// This class corresponds to the ArkWeb interface.
class CORE_EXPORT ArkWeb final : public ScriptWrappable,
                                 public ExecutionContextClient {
  DEFINE_WRAPPERTYPEINFO();

 public:
  explicit ArkWeb(LocalDOMWindow*);

  double scrollbarAvoidAreaTop(ExceptionState& exception_state) const {
    return scrollbar_avoid_area_top_;
  }
  void setScrollbarAvoidAreaTop(double margin,
                                ExceptionState& exception_state);
  double scrollbarAvoidAreaBottom(ExceptionState& exception_state) const {
    return scrollbar_avoid_area_bottom_;
  }
  void setScrollbarAvoidAreaBottom(double margin,
                                   ExceptionState& exception_state);
  void Trace(Visitor*) const override;

 private:
  double scrollbar_avoid_area_top_ = 0.0;
  double scrollbar_avoid_area_bottom_ = 0.0;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_CORE_FRAME_ARK_WEB_H_
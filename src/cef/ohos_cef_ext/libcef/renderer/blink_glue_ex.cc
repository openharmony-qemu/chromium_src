// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "cef/libcef/renderer/blink_glue.h"
#include "third_party/blink/public/mojom/v8_cache_options.mojom-blink.h"
#include "third_party/blink/public/platform/web_string.h"
#include "third_party/blink/public/platform/web_url_response.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_local_frame_client.h"
#include "third_party/blink/public/web/web_node.h"
#include "third_party/blink/public/web/web_view_client.h"
#include "third_party/blink/renderer/bindings/core/v8/sanitize_script_errors.h"
#include "third_party/blink/renderer/bindings/core/v8/script_evaluation_result.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_core.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/element.h"
#include "third_party/blink/renderer/core/dom/node.h"
#include "third_party/blink/renderer/core/editing/serializers/serialization.h"
#include "third_party/blink/renderer/core/execution_context/execution_context.h"
#include "third_party/blink/renderer/core/execution_context/execution_context_lifecycle_state_observer.h"
#include "third_party/blink/renderer/core/exported/web_view_impl.h"
#include "third_party/blink/renderer/core/frame/frame_owner.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/frame/settings.h"
#include "third_party/blink/renderer/core/frame/visual_viewport.h"
#include "third_party/blink/renderer/core/frame/web_local_frame_impl.h"
#include "third_party/blink/renderer/core/geometry/dom_rect.h"
#include "third_party/blink/renderer/core/loader/frame_load_request.h"
#include "third_party/blink/renderer/core/page/page.h"
#include "third_party/blink/renderer/core/script/classic_script.h"
#include "third_party/blink/renderer/platform/bindings/script_forbidden_scope.h"
#include "third_party/blink/renderer/platform/bindings/v8_binding.h"
#include "third_party/blink/renderer/platform/loader/fetch/resource_response.h"
#include "third_party/blink/renderer/platform/loader/fetch/script_fetch_options.h"
#include "third_party/blink/renderer/platform/weborigin/scheme_registry.h"
#undef LOG

#include "base/logging.h"

namespace blink_glue {

#if BUILDFLAG(IS_ARKWEB)
bool CanGoBackOrForward(blink::WebView* view, int num_steps) {
  if (!view) {
    return false;
  }
  if (num_steps == 0) {
    return true;
  }
  if (num_steps > 0 && view->HistoryForwardListCount() >= num_steps) {
    return true;
  }
  if (num_steps < 0 && view->HistoryBackListCount() >= -num_steps) {
    return true;
  }
  return false;
}

void GoBackOrForward(blink::WebView* view, int num_steps) {
  if (!view) {
    return;
  }

  blink::WebFrame* main_frame = view->MainFrame();
  if (main_frame && main_frame->IsWebLocalFrame()) {
    if (num_steps > 0 && view->HistoryForwardListCount() > 0) {
      blink::Frame* core_frame = blink::WebFrame::ToCoreFrame(*main_frame);
      blink::To<blink::LocalFrame>(core_frame)
          ->GetLocalFrameHostRemote()
          .GoToEntryAtOffset(num_steps, true /* has_user_gesture */,
                             base::TimeTicks::Now(), std::nullopt);
    }
    if (num_steps < 0 && view->HistoryBackListCount() > 0) {
      blink::Frame* core_frame = blink::WebFrame::ToCoreFrame(*main_frame);
      blink::To<blink::LocalFrame>(core_frame)
          ->GetLocalFrameHostRemote()
          .GoToEntryAtOffset(num_steps, true /* has_user_gesture */,
                             base::TimeTicks::Now(), std::nullopt);
    }
  }
}
#endif
#if BUILDFLAG(ARKWEB_SYNC_RENDER)
gfx::Size GetContentSize(blink::WebLocalFrame* frame) {
  blink::WebDocument web_document = frame->GetDocument();
  if (web_document.IsNull()) {
    return gfx::Size();
  }

  blink::WebElement web_document_element = web_document.DocumentElement();
  if (web_document_element.IsNull()) {
    return gfx::Size();
  }
  blink::Element* document_element =
      web_document_element.Unwrap<blink::Element>();

  blink::Document* document = web_document.Unwrap<blink::Document>();
  bool inQuirksMode = document->InQuirksMode();

  int width = 0;
  int height = 0;
  if (inQuirksMode) {
    width = document_element->scrollWidth();
    height = document_element->scrollHeight();
  } else {
    int body_height = 0;
    blink::WebElement web_body = web_document.Body();
    if (!web_body.IsNull()) {
      blink::Element* body = web_body.Unwrap<blink::Element>();
      blink::DOMRect* body_dom_rect = body->GetBoundingClientRect();
      gfx::Rect body_rect = body_dom_rect->ToEnclosingRect();
      body_height = body->scrollHeight() + body_rect.y();
    }
    width = document_element->OffsetWidth();
    height = std::max({document_element->OffsetHeight(), body_height});
  }

  blink::Frame* core_frame = blink::WebFrame::ToCoreFrame(*frame);
  float page_scale = blink::To<blink::LocalFrame>(core_frame)
                         ->GetPage()
                         ->GetVisualViewport()
                         .Scale();

  return gfx::Size(width * page_scale, height * page_scale);
}

gfx::Size GetVisualViewportSize(blink::WebLocalFrame* frame) {
  blink::Frame* core_frame = blink::WebFrame::ToCoreFrame(*frame);
  return blink::To<blink::LocalFrame>(core_frame)
      ->GetPage()
      ->GetVisualViewport()
      .Size();
}
#endif

}  // namespace blink_glue
                         
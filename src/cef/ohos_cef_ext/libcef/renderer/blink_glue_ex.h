// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_BLINK_GLUE_EX_H_
#define CEF_LIBCEF_RENDERER_BLINK_GLUE_EX_H_

#include <stdint.h>

#include <memory>
#include <string>

#include "cef/include/internal/cef_types.h"
#include "third_party/blink/public/mojom/frame/lifecycle.mojom-blink-forward.h"
#include "third_party/blink/public/platform/web_common.h"
#include "ui/gfx/geometry/size.h"
#include "v8/include/v8.h"

namespace blink {
class WebElement;
class WebLocalFrame;
class WebNode;
class WebString;
class WebURLRequest;
class WebURLResponse;
class WebView;
}  // namespace blink

namespace blink_glue {

#if BUILDFLAG(IS_ARKWEB)
BLINK_EXPORT bool CanGoBackOrForward(blink::WebView* view, int num_steps);
BLINK_EXPORT void GoBackOrForward(blink::WebView* view, int num_steps);
#endif

#if BUILDFLAG(ARKWEB_SYNC_RENDER)
BLINK_EXPORT gfx::Size GetContentSize(blink::WebLocalFrame* frame);

BLINK_EXPORT gfx::Size GetVisualViewportSize(blink::WebLocalFrame* frame);
#endif

}  // namespace blink_glue

#endif  // CEF_LIBCEF_RENDERER_BLINK_GLUE_EX_H_

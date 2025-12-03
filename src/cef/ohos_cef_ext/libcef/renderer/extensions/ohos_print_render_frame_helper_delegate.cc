/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "libcef/renderer/extensions/ohos_print_render_frame_helper_delegate.h"

#include <utility>
#include <vector>

#include "base/check.h"
#include "base/command_line.h"
#include "base/strings/string_util.h"
#include "chrome/common/chrome_switches.h"
#include "content/public/renderer/render_frame.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "pdf/buildflags.h"
#include "third_party/blink/public/platform/browser_interface_broker_proxy.h"
#include "third_party/blink/public/web/web_document.h"
#include "third_party/blink/public/web/web_element.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "url/origin.h"

#if BUILDFLAG(ENABLE_PDF)
#include "components/pdf/common/pdf_util.h"
#include "extensions/renderer/guest_view/mime_handler_view/post_message_support.h"
#endif  // BUILDFLAG(ENABLE_PDF)

namespace extensions {

OhosPrintRenderFrameHelperDelegate::OhosPrintRenderFrameHelperDelegate() =
    default;

OhosPrintRenderFrameHelperDelegate::~OhosPrintRenderFrameHelperDelegate() =
    default;

// Return the PDF object element if `frame` is the out of process PDF extension
// or its child frame.
blink::WebElement OhosPrintRenderFrameHelperDelegate::GetPdfElement(
    blink::WebLocalFrame* frame) {
#if BUILDFLAG(ENABLE_PDF)
  if (frame && frame->Parent() &&
      IsPdfInternalPluginAllowedOrigin(frame->Parent()->GetSecurityOrigin(),
                                       base::span<const url::Origin>{})) {
    auto plugin_element = frame->GetDocument().QuerySelector("embed");
    DCHECK(!plugin_element.IsNull());
    return plugin_element;
  }
#endif  // BUILDFLAG(ENABLE_PDF)
  return blink::WebElement();
}

bool OhosPrintRenderFrameHelperDelegate::IsPrintPreviewEnabled() {
  base::CommandLine* command_line = base::CommandLine::ForCurrentProcess();
  return !command_line->HasSwitch(switches::kDisablePrintPreview);
}

bool OhosPrintRenderFrameHelperDelegate::OverridePrint(
    blink::WebLocalFrame* frame) {
#if BUILDFLAG(ENABLE_PDF)
  auto* post_message_support =
      extensions::PostMessageSupport::FromWebLocalFrame(frame);
  if (post_message_support) {
    base::Value::Dict message;
    message.Set("type", "print");
    post_message_support->PostMessageFromValue(base::Value(std::move(message)));
    return true;
  }
#endif  // BUILDFLAG(ENABLE_PDF)
  return false;
}

bool OhosPrintRenderFrameHelperDelegate::ShouldGenerateTaggedPDF() {
  return true;
}

}  // namespace extensions

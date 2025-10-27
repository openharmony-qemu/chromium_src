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

#ifndef OHOS_PRINT_RENDER_FRAME_HELPER_DELEGATE_H_
#define OHOS_PRINT_RENDER_FRAME_HELPER_DELEGATE_H_

#include "components/printing/renderer/print_render_frame_helper.h"

namespace extensions {

class OhosPrintRenderFrameHelperDelegate
    : public printing::PrintRenderFrameHelper::Delegate {
 public:
  OhosPrintRenderFrameHelperDelegate();

  OhosPrintRenderFrameHelperDelegate(
      const OhosPrintRenderFrameHelperDelegate&) = delete;
  OhosPrintRenderFrameHelperDelegate& operator=(
      const OhosPrintRenderFrameHelperDelegate&) = delete;

  ~OhosPrintRenderFrameHelperDelegate() override;

 private:
  // printing::PrintRenderFrameHelper::Delegate:
  blink::WebElement GetPdfElement(blink::WebLocalFrame* frame) override;
  bool IsPrintPreviewEnabled() override;
  bool OverridePrint(blink::WebLocalFrame* frame) override;
  bool ShouldGenerateTaggedPDF() override;
};

}  // namespace extensions

#endif  // OHOS_PRINT_RENDER_FRAME_HELPER_DELEGATE_H_

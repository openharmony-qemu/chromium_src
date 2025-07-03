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

#include "arkweb/chromium_ext/third_party/blink/renderer/core/paint/timing/paint_timing_detector_utils.h"
#include "third_party/blink/renderer/core/paint/timing/paint_timing_detector.h"

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/renderer/core/loader/document_loader.h"
#endif

namespace blink {

PaintTimingDetectorUtils::PaintTimingDetectorUtils(PaintTimingDetector* paint_timing_detector) {
  this->paint_timing_detector_ = paint_timing_detector;
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void PaintTimingDetectorUtils::NotifyLcpForBlankless() {
  LOG(DEBUG) << "blankless PaintTimingDetectorUtils::NotifyLcpForBlankless";
  if (!base::ohos::BlanklessController::SimpleCheck()) {
    return;
  }
  Document* document = paint_timing_detector_->frame_view_->GetFrame().GetDocument();
  if (!document) {
    return;
  }
  DocumentLoader* loader = document->Loader();
  if (!loader || !loader->AsArkWebDocumentLoaderExt()) {
    return;
  }
  loader->AsArkWebDocumentLoaderExt()->NotifyLcpForBlankless();
}
#endif

}  // namespace blink
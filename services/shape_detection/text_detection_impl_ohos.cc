/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
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

#include "services/shape_detection/text_detection_impl_ohos.h"

#include <algorithm>
#include <memory>

#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "ohos/adapter/shape_detection/shape_detection_adapter.h"
#include "services/shape_detection/text_detection_impl.h"
#include "ui/gfx/geometry/rect_f.h"

namespace shape_detection {

// static
void TextDetectionImpl::Create(
    mojo::PendingReceiver<mojom::TextDetection> receiver) {
  mojo::MakeSelfOwnedReceiver(std::make_unique<TextDetectionImplOhos>(),
                              std::move(receiver));
}

void TextDetectionImplOhos::Detect(const SkBitmap& bitmap,
                                   DetectCallback callback) {
  SkImageInfo info = SkImageInfo::Make(bitmap.width(), bitmap.height(),
                                       SkColorType::kRGBA_8888_SkColorType,
                                       bitmap.alphaType());
  auto buffer = std::make_unique<char[]>(bitmap.computeByteSize());
  bitmap.readPixels(info, buffer.get(), bitmap.rowBytes(), 0, 0);
  std::vector<mojom::TextDetectionResultPtr> results;
  std::vector<::ohos::adapter::ShapeDetectionAdapter::TextLine> lineVec =
      ::ohos::adapter::ShapeDetectionAdapter::TextDetect(
          buffer.get(), bitmap.computeByteSize(), bitmap.width(),
          bitmap.height());
  for (const auto& it : lineVec) {
    float x = std::min(it.topLeft.x, it.bottomLeft.x);
    float y = std::min(it.topLeft.y, it.topRight.y);
    float height = std::max(it.bottomRight.y, it.bottomLeft.y) -
                   std::min(it.topLeft.y, it.topRight.y);
    float width = std::max(it.topRight.x, it.bottomRight.x) -
                  std::min(it.bottomLeft.x, it.topLeft.x);
    auto result = mojom::TextDetectionResult::New();
    result->raw_value = it.value;
    result->bounding_box = gfx::RectF(x, y, width, height);
    result->corner_points.emplace_back(it.topLeft.x, it.topLeft.y);
    result->corner_points.emplace_back(it.topRight.x, it.topRight.y);
    result->corner_points.emplace_back(it.bottomRight.x, it.bottomRight.y);
    result->corner_points.emplace_back(it.bottomLeft.x, it.bottomLeft.y);

    results.push_back(std::move(result));
  }
  std::move(callback).Run(std::move(results));
}
}  // namespace shape_detection

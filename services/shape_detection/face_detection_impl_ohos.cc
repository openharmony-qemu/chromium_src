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

#include "services/shape_detection/face_detection_impl_ohos.h"

#include <memory>

#include "ohos/adapter/shape_detection/shape_detection_adapter.h"
#include "ui/gfx/geometry/rect_f.h"

namespace shape_detection {
FaceDetectionImplOhos::FaceDetectionImplOhos(
    shape_detection::mojom::FaceDetectorOptionsPtr options) {}

void FaceDetectionImplOhos::Detect(const SkBitmap& bitmap,
                                   DetectCallback callback) {
  SkImageInfo info = SkImageInfo::Make(bitmap.width(), bitmap.height(),
                                       SkColorType::kRGBA_8888_SkColorType,
                                       bitmap.alphaType());
  auto buffer = std::make_unique<char[]>(bitmap.computeByteSize());
  bitmap.readPixels(info, buffer.get(), bitmap.rowBytes(), 0, 0);

  std::vector<mojom::FaceDetectionResultPtr> results;
  std::vector<::ohos::adapter::ShapeDetectionAdapter::Face> faces =
      ::ohos::adapter::ShapeDetectionAdapter::FaceDetect(
          buffer.get(), bitmap.computeByteSize(), bitmap.width(),
          bitmap.height());

  for (const auto& it : faces) {
    auto face = shape_detection::mojom::FaceDetectionResult::New();
    face->bounding_box = gfx::RectF(it.x, it.y, it.width, it.height);
    auto lefteye = shape_detection::mojom::Landmark::New();
    lefteye->type = shape_detection::mojom::LandmarkType::EYE;
    lefteye->locations.emplace_back(it.leftEye.x, it.leftEye.y);
    face->landmarks.push_back(std::move(lefteye));

    auto righteye = shape_detection::mojom::Landmark::New();
    righteye->type = shape_detection::mojom::LandmarkType::EYE;
    righteye->locations.emplace_back(it.rightEye.x, it.rightEye.y);
    face->landmarks.push_back(std::move(righteye));

    auto nose = shape_detection::mojom::Landmark::New();
    nose->type = shape_detection::mojom::LandmarkType::NOSE;
    nose->locations.emplace_back(it.nose.x, it.nose.y);
    face->landmarks.push_back(std::move(nose));

    auto mouth = shape_detection::mojom::Landmark::New();
    mouth->type = shape_detection::mojom::LandmarkType::MOUTH;
    mouth->locations.emplace_back(it.mouth.x, it.mouth.y);
    face->landmarks.push_back(std::move(mouth));

    results.push_back(std::move(face));
  }
  std::move(callback).Run(std::move(results));
}
}  // namespace shape_detection

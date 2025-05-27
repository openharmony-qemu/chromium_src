// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

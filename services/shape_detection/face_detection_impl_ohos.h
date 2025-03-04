// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_SHAPE_DETECTION_FACE_DETECTION_IMPL_OHOS_H_
#define SERVICES_SHAPE_DETECTION_FACE_DETECTION_IMPL_OHOS_H_

#include "services/shape_detection/public/mojom/facedetection.mojom.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace shape_detection {

class FaceDetectionImplOhos : public shape_detection::mojom::FaceDetection {
 public:
  explicit FaceDetectionImplOhos(
      shape_detection::mojom::FaceDetectorOptionsPtr options);

  FaceDetectionImplOhos(const FaceDetectionImplOhos&) = delete;
  FaceDetectionImplOhos& operator=(const FaceDetectionImplOhos&) = delete;

  ~FaceDetectionImplOhos() override = default;

  void Detect(
      const SkBitmap& bitmap,
      shape_detection::mojom::FaceDetection::DetectCallback callback) override;
};

}  // namespace shape_detection

#endif  // SERVICES_SHAPE_DETECTION_FACE_DETECTION_IMPL_OHOS_H_

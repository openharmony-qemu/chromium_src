// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_SHAPE_DETECTION_TEXT_DETECTION_IMPL_OHOS_H_
#define SERVICES_SHAPE_DETECTION_TEXT_DETECTION_IMPL_OHOS_H_

#include "services/shape_detection/public/mojom/textdetection.mojom.h"

namespace shape_detection {

class TextDetectionImplOhos : public mojom::TextDetection {
 public:
  TextDetectionImplOhos() = default;

  TextDetectionImplOhos(const TextDetectionImplOhos&) = delete;
  TextDetectionImplOhos& operator=(const TextDetectionImplOhos&) = delete;

  ~TextDetectionImplOhos() override = default;

  void Detect(const SkBitmap& bitmap,
              mojom::TextDetection::DetectCallback callback) override;
};

}  // namespace shape_detection

#endif  // SERVICES_SHAPE_DETECTION_TEXT_DETECTION_IMPL_OHOS_H_

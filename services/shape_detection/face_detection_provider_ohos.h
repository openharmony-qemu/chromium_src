// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_SHAPE_DETECTION_FACE_DETECTION_PROVIDER_OHOS_H_
#define SERVICES_SHAPE_DETECTION_FACE_DETECTION_PROVIDER_OHOS_H_

#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "services/shape_detection/public/mojom/facedetection_provider.mojom.h"

namespace shape_detection {

class FaceDetectionProviderOhos
    : public shape_detection::mojom::FaceDetectionProvider {
 public:
  FaceDetectionProviderOhos();

  FaceDetectionProviderOhos(const FaceDetectionProviderOhos&) = delete;
  FaceDetectionProviderOhos& operator=(const FaceDetectionProviderOhos&) =
      delete;

  ~FaceDetectionProviderOhos() override;

  // Binds FaceDetection provider receiver to the implementation of
  // mojom::FaceDetectionProvider.
  static void Create(
      mojo::PendingReceiver<mojom::FaceDetectionProvider> receiver);

  void CreateFaceDetection(mojo::PendingReceiver<mojom::FaceDetection> receiver,
                           mojom::FaceDetectorOptionsPtr options) override;
};

}  // namespace shape_detection

#endif  // SERVICES_SHAPE_DETECTION_FACE_DETECTION_PROVIDER_OHOS_H_

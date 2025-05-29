// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/shape_detection/face_detection_provider_ohos.h"

#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "services/shape_detection/face_detection_impl_ohos.h"

namespace shape_detection {

FaceDetectionProviderOhos::FaceDetectionProviderOhos() = default;

FaceDetectionProviderOhos::~FaceDetectionProviderOhos() = default;

// static
void FaceDetectionProviderOhos::Create(
    mojo::PendingReceiver<mojom::FaceDetectionProvider> receiver) {
  mojo::MakeSelfOwnedReceiver(std::make_unique<FaceDetectionProviderOhos>(),
                              std::move(receiver));
}

void FaceDetectionProviderOhos::CreateFaceDetection(
    mojo::PendingReceiver<mojom::FaceDetection> receiver,
    mojom::FaceDetectorOptionsPtr options) {
  mojo::MakeSelfOwnedReceiver(
      std::make_unique<FaceDetectionImplOhos>(std::move(options)),
      std::move(receiver));
}

}  // namespace shape_detection

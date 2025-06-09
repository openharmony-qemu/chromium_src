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

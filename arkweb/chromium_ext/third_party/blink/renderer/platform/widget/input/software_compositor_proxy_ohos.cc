// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Based on synchronous_compositor_proxy.cc originally written by
// Copyright (c) 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. 

#include "third_party/blink/renderer/platform/widget/input/software_compositor_proxy_ohos.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/shared_memory_mapping.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkRegion.h"

namespace blink {

SoftwareCompositorProxyOhos::SoftwareCompositorProxyOhos() {}

SoftwareCompositorProxyOhos::~SoftwareCompositorProxyOhos() {
  software_draw_shm_.reset();
}

struct SoftwareCompositorProxyOhos::SharedMemoryWithSize {
  base::WritableSharedMemoryMapping shared_memory;
  const size_t buffer_size;
  bool zeroed;

  SharedMemoryWithSize(base::WritableSharedMemoryMapping shm_mapping,
                       size_t buffer_size)
      : shared_memory(std::move(shm_mapping)),
        buffer_size(buffer_size),
        zeroed(true) {}
};

void SoftwareCompositorProxyOhos::DemandDrawSwAsync(
    mojom::blink::SoftwareCompositorDemandDrawSwParamsPtr params,
    DemandDrawSwAsyncCallback callback) {
  LOG(INFO) << "DemandDrawSwAsync in blink";

  SkImageInfo info =
      SkImageInfo::MakeN32Premul(params->size.width(), params->size.height());
  size_t stride = info.minRowBytes();

  SkBitmap bitmap;
  if (!bitmap.installPixels(info, software_draw_shm_->shared_memory.memory(),
                            stride)) {
    LOG(ERROR) << "bit map installPixels error";
    std::move(callback).Run(false);
    return;
  }
  SkCanvas canvas(bitmap);
  if (!software_render_) {
    LOG(ERROR) << "software render init error";
    std::move(callback).Run(false);
    return;
  }
  bool result =
      software_render_->DemandDrawSw(&canvas, params->size, params->offset);
  if (!result) {
    LOG(ERROR) << "software render error in blink";
  }
  std::move(callback).Run(result);
}

void SoftwareCompositorProxyOhos::BindChannel(
    mojo::PendingReceiver<mojom::blink::SoftwareCompositorOhos>
        compositor_request) {
  receiver_.Bind(std::move(compositor_request));
}

void SoftwareCompositorProxyOhos::DrawRect(const gfx::Rect& rect) {
  LOG(DEBUG) << "DrawRect in blink";
  if (!software_render_) {
    LOG(ERROR) << "software render init error";
    return;
  }
  software_render_->DrawRect(rect);
}

void SoftwareCompositorProxyOhos::SetSharedMemory(
    base::WritableSharedMemoryRegion shm_region,
    SetSharedMemoryCallback callback) {
  bool result = false;
  if (shm_region.IsValid()) {
    base::WritableSharedMemoryMapping shm_mapping = shm_region.Map();
    if (shm_mapping.IsValid()) {
      software_draw_shm_ = std::make_unique<SharedMemoryWithSize>(
          std::move(shm_mapping), shm_mapping.size());
      result = true;
    }
  }
  std::move(callback).Run(result);
}

void SoftwareCompositorProxyOhos::SetSoftwareRenderer(
    cc::mojo_embedder::SoftwareCompositorRendererOhos* software_render) {
  software_render_ = software_render;
}
}  // namespace blink
                     
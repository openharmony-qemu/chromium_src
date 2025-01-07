/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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
#include "content/browser/ohos/software_compositor_host_ohos.h"

#include "base/functional/bind.h"
#include "base/logging.h"
#include "base/memory/shared_memory_mapping.h"
#include "base/memory/writable_shared_memory_region.h"
#include "base/trace_event/trace_event.h"

#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkImageInfo.h"
#include "third_party/skia/include/core/SkPixmap.h"
#include "third_party/skia/include/core/SkRect.h"
#include "third_party/skia/include/core/SkRegion.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/encode/SkPngEncoder.h"

namespace content {

const float MAX_DRAW_SW_SIZE = 16000.0;
const std::string DUMP_FILE_PATH = "/data/storage/el2/base/haps/entry/files/";
const std::string DUMP_FILE_PRE = "web_frame_sceenshot_";
const std::string DUMP_FILE_TYPE = ".png";

struct SoftwareCompositorHostOhos::SharedMemoryWithSize {
  base::WritableSharedMemoryMapping shared_memory;
  const size_t stride;
  const size_t buffer_size;

  SharedMemoryWithSize(size_t stride, size_t buffer_size)
      : stride(stride), buffer_size(buffer_size) {}

  SharedMemoryWithSize(const SharedMemoryWithSize&) = delete;
  SharedMemoryWithSize& operator=(const SharedMemoryWithSize&) = delete;
};

SoftwareCompositorHostOhos::SoftwareCompositorHostOhos(
    RenderWidgetHostImpl* widget)
    : widget_(widget) {
  widget_->GetWidgetInputHandler()->AttachSoftwareCompositorOhos(
      software_compositor_.BindNewPipeAndPassReceiver());
}

SoftwareCompositorHostOhos::~SoftwareCompositorHostOhos() {
  if (software_draw_shm_) {
    software_draw_shm_.reset();
  }
}

void SoftwareCompositorHostOhos::DumpSnapshot(std::string type) {
  LOG(INFO) << "Dump Web Snapshot";
  SkImageInfo info =
      SkImageInfo::MakeN32Premul(current_.width(), current_.height());
  size_t stride = info.minRowBytes();
  SkBitmap bitmap;
  SkPixmap pixmap(info, software_draw_shm_->shared_memory.memory(), stride);

  std::string filename;
  filename.append(DUMP_FILE_PATH);
  filename.append(DUMP_FILE_PRE);
  filename.append(type);
  filename.append(DUMP_FILE_TYPE);
  SkFILEWStream file(filename.c_str());

  SkPngEncoder::Options opts;
  opts.fFilterFlags = SkPngEncoder::FilterFlag::kAll;
  opts.fZLibLevel = 6;
  bool res = SkPngEncoder::Encode(&file, pixmap, opts);
  if (!res) {
    LOG(ERROR) << "Dump Web Snapshot to png file error";
  }
}

void SoftwareCompositorHostOhos::OnDrawSwCallback(WebSnapchatCallback callback,
                                                  std::string id,
                                                  bool result) {
  TRACE_EVENT0("browser", "SoftwareCompositorHostOhos::OnDrawSwCallback");
  LOG(INFO) << "OnDrawSwCallback result:" << result;

  if (!result) {
    std::move(callback).Run(id.c_str(), result, nullptr, 0, 0);
    return;
  }
  std::move(callback).Run(id.c_str(), result, software_draw_shm_->shared_memory.memory(),
                          current_.width(), current_.height());
}

void SoftwareCompositorHostOhos::DemandDrawSwAsync(const char* id,
                                                   float clip_width,
                                                   float clip_height,
                                                   gfx::SizeF size,
                                                   gfx::PointF offset,
                                                   WebSnapchatCallback callback) {
  TRACE_EVENT0("browser", "SoftwareCompositorHostOhos::DemandDrawSwAsync");
  LOG(INFO) << "start DemandDrawSwAsync width:" << clip_width << ", height:" << clip_height;

  if (clip_width == 0) {
    clip_width = MAX_DRAW_SW_SIZE;
  }
  if (clip_height == 0) {
    clip_height = MAX_DRAW_SW_SIZE;
  }
  current_.set_width(std::min({size.width(), clip_width, MAX_DRAW_SW_SIZE}));
  current_.set_height(std::min({size.height(), clip_height, MAX_DRAW_SW_SIZE}));

  SkImageInfo info =
      SkImageInfo::MakeN32Premul(current_.width(), current_.height());
  size_t stride = info.minRowBytes();
  size_t buffer_size = info.computeByteSize(stride);
  if (SkImageInfo::ByteSizeOverflowed(buffer_size)) {
    LOG(ERROR) << "request snapshot size is too large make skia overflowed";
    GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE, base::BindOnce(std::move(callback), id, false,
                                  nullptr, 0, 0));
    return;
  }

  SetSharedMemory(stride, buffer_size);
  if (!software_draw_shm_) {
    LOG(ERROR) << "set shared memory error";
    GetUIThreadTaskRunner({})->PostTask(
        FROM_HERE, base::BindOnce(std::move(callback), id, false,
                                  nullptr, 0, 0));
    return;
  }

  blink::mojom::SoftwareCompositorDemandDrawSwParamsPtr params =
      blink::mojom::SoftwareCompositorDemandDrawSwParams::New();
  params->size = current_;
  params->offset = offset;
  std::string inputId(id);

  software_compositor_->DemandDrawSwAsync(
      std::move(params),
      base::BindOnce(&SoftwareCompositorHostOhos::OnDrawSwCallback,
                     weak_factory_.GetWeakPtr(), std::move(callback),
                     std::move(inputId)));
}

void SoftwareCompositorHostOhos::DrawRect(const gfx::Rect& rect) {
  LOG(DEBUG) << "DrawRect in SoftwareCompositorHostOhos, rect x : " << rect.x()
             << ",y: " << rect.y() << ",width: " << rect.width()
             << ",height: " << rect.height();
  software_compositor_->DrawRect(rect);
}

void SoftwareCompositorHostOhos::SetSharedMemory(size_t stride,
                                                 size_t buffer_size) {
  software_draw_shm_.reset();
  auto software_draw_shm =
      std::make_unique<SharedMemoryWithSize>(stride, buffer_size);

  base::WritableSharedMemoryRegion shm_region;

  {
    TRACE_EVENT1("browser", "AllocateSharedMemory", "buffer_size", buffer_size);
    shm_region = base::WritableSharedMemoryRegion::Create(buffer_size);
    if (!shm_region.IsValid()) {
      LOG(ERROR) << "create shm region error";
      return;
    }

    software_draw_shm->shared_memory = shm_region.Map();
    if (!software_draw_shm->shared_memory.IsValid()) {
      LOG(ERROR) << "shm region map error";
      return;
    }
  }

  bool result = false;
  software_compositor_->SetSharedMemory(std::move(shm_region), &result);
  if (!result) {
    LOG(ERROR) << "sw compositor SetSharedMemory error from client: " << result;
    return;
  }
  software_draw_shm_ = std::move(software_draw_shm);
}

}  // namespace content
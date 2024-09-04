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

#include "components/viz/service/display/frame_dump_copy_output_request.h"

#include "base/logging.h"
#include "components/viz/common/frame_sinks/copy_output_request.h"
#include "components/viz/common/frame_sinks/copy_output_result.h"
#include "components/viz/common/frame_sinks/copy_output_util.h"

#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/encode/SkPngEncoder.h"

namespace viz {

namespace {
  const std::string DUMP_FILE_PATH = "/data/storage/el2/base/haps/entry/files/";
  const std::string DUMP_FILE_PRE = "web_frame_";
  const std::string DUMP_FILE_TYPE = ".png";
  const int MAX_DUMP_FRAME_SIZE = 1000;
}

FrameDumpCopyOutputRequest::FrameDumpCopyOutputRequest(uint64_t id, const std::string& dump_path) : CopyOutputRequest(
            ResultFormat::RGBA,
            ResultDestination::kSystemMemory,
            base::BindOnce([](std::unique_ptr<CopyOutputResult> result) {
              uint64_t dump_frame_id = result->DumpFrameId() % MAX_DUMP_FRAME_SIZE;
              std::string dump_frame_path = DUMP_FILE_PATH;
              std::string debug_dump_path = result->DumpFramePath();
              if (!(debug_dump_path.empty())) {
                dump_frame_path = debug_dump_path;
              }
              std::string filename;
              filename.append(dump_frame_path);
              filename.append(DUMP_FILE_PRE);
              filename.append(std::to_string(dump_frame_id));
              filename.append(DUMP_FILE_TYPE); 
              SkFILEWStream file(filename.c_str());
              SkBitmap bitmap = result->ScopedAccessSkBitmap().bitmap();
              SkPngEncoder::Options opts;
              opts.fFilterFlags = SkPngEncoder::FilterFlag::kAll;
              opts.fZLibLevel = 6;
              bool res = SkPngEncoder::Encode(&file, bitmap.pixmap(), opts);
              if (!res) {
                LOG(ERROR) << "frame dump png file error, filename = " << filename;
              }
            }), id, dump_path) {
            }
}  // namespace viz

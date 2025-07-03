/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef COMPONENTS_VIZ_SERVICE_DISPLAY_FRAME_SNAPSHOT_COPY_OUTPUT_REQUEST_H_
#define COMPONENTS_VIZ_SERVICE_DISPLAY_FRAME_SNAPSHOT_COPY_OUTPUT_REQUEST_H_

#include "components/viz/common/frame_sinks/copy_output_request.h"
#include "components/viz/service/viz_service_export.h"

namespace viz {
class VIZ_SERVICE_EXPORT FrameSnapshotCopyOutputRequest : public CopyOutputRequest {
 public:
  FrameSnapshotCopyOutputRequest();
  ~FrameSnapshotCopyOutputRequest() override = default;
};
}  // namespace viz

#endif  // COMPONENTS_VIZ_SERVICE_DISPLAY_FRAME_SNAPSHOT_COPY_OUTPUT_REQUEST_H_
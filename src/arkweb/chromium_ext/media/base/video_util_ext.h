// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_VIDEO_UTIL_EXT_H_
#define MEDIA_BASE_VIDEO_UTIL_EXT_H_

#include <stdint.h>
#include <vector>
#include "base/memory/scoped_refptr.h"
#include "media/base/encoder_status.h"
#include "media/base/media_export.h"
#include "media/base/video_types.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkYUVAInfo.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/size.h"

namespace media {

class VideoFramePool;
class VideoFrame;

// Copy pixel data from |src_frame| to |dst_frame| applying scaling and pixel
// format conversion as needed. Both frames need to be mappabale and have either
// I420 or NV12 pixel format.
[[nodiscard]] MEDIA_EXPORT EncoderStatus
ConvertAndScaleFrame(const VideoFrame& src_frame,
                     VideoFrame& dst_frame,
                     std::vector<uint8_t>& tmp_buf);
}

#endif // MEDIA_BASE_VIDEO_UTIL_EXT_H_
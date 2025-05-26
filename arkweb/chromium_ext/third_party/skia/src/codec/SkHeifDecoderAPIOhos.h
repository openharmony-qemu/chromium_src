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

#ifndef SKHEIFDECODERAPI
#define SKHEIFDECODERAPI

#include <memory>
#include <stddef.h>
#include <stdint.h>
#include <vector>

#include "include/core/SkStream.h"

enum HeifColorFormat {
    kHeifColorFormat_INVALID = -1,
    kHeifColorFormat_RGB565,
    kHeifColorFormat_RGBA_8888,
    kHeifColorFormat_BGRA_8888,
    kHeifColorFormat_RGBA_1010102,
};

struct HeifFrameInfo {
    uint32_t mWidth;
    uint32_t mHeight;
    int32_t  mRotationAngle;           // Rotation angle, clockwise, should be multiple of 90
    uint32_t mBytesPerPixel;           // Number of bytes for one pixel
    int64_t mDurationUs;               // Duration of the frame in us
    std::vector<uint8_t> mIccData;     // ICC data array
};

namespace OHOS::NWeb {
class OhosImageDecoderAdapter;
}

class HeifDecoder {
public:
    HeifDecoder() : colorFormat_(HeifColorFormat::kHeifColorFormat_INVALID), data_(nullptr) {}

    bool init(std::unique_ptr<SkStream> stream, HeifFrameInfo* heifInfo);
    bool decode(HeifFrameInfo* heifInfo);
    bool setOutputColor(HeifColorFormat colorFormat);
    void* getDecodeData(uint64_t& size);
    void closeDecodeData(void* ptr, uint64_t size);
    int32_t getStride();

    bool getSequenceInfo(HeifFrameInfo* frameInfo, size_t *frameCount) { return false; }
    bool decodeSequence(int frameIndex, HeifFrameInfo* frameInfo) { return false; }
    int32_t skipScanlines(int) { return 0; }
    uint32_t getColorDepth() { return 0; }

private:
    static std::unique_ptr<OHOS::NWeb::OhosImageDecoderAdapter> decoderAdapter_;
    static OHOS::NWeb::OhosImageDecoderAdapter* getDecoderAdapter();

    HeifColorFormat colorFormat_;
    sk_sp<SkData> data_;
};

static inline HeifDecoder* createHeifDecoder() { return new HeifDecoder(); }

#endif // SKHEIFDECODERAPI
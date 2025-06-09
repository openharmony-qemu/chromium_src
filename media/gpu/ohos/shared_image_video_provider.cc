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

#include "media/gpu/ohos/shared_image_video_provider.h"

namespace media {

SharedImageVideoProvider::ImageSpec::ImageSpec() = default;
SharedImageVideoProvider::ImageSpec::ImageSpec(const gfx::Size& our_size,
                                               uint64_t our_generation_id)
    : coded_size(our_size), generation_id(our_generation_id) {}
SharedImageVideoProvider::ImageSpec::ImageSpec(const ImageSpec&) = default;
SharedImageVideoProvider::ImageSpec::~ImageSpec() = default;

bool SharedImageVideoProvider::ImageSpec::operator==(
    const ImageSpec& rhs) const {
  return coded_size == rhs.coded_size && generation_id == rhs.generation_id &&
         color_space == rhs.color_space;
}

bool SharedImageVideoProvider::ImageSpec::operator!=(
    const ImageSpec& rhs) const {
  return !(*this == rhs);
}

SharedImageVideoProvider::ImageRecord::ImageRecord() = default;
SharedImageVideoProvider::ImageRecord::ImageRecord(ImageRecord&&) = default;
SharedImageVideoProvider::ImageRecord::~ImageRecord() = default;

}  // namespace media

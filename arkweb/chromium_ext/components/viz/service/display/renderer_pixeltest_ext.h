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

#ifndef RENDERER_PIXELTEST_EXT_H
#define RENDERER_PIXELTEST_EXT_H

#define ARKWEB_UNITTESTS_DEFINE_COLOR() \
  auto temp_color_rect1 = \
    is_software_renderer() \
      ? SkColor4f::FromColor(SkColorSetARGB(0, 120, 255, 255)) \
      : SkColor4f::FromColor(SkColorSetARGB(0, 255, 255, 120)); \
  auto temp_color_rect2 = \
    is_software_renderer() \
      ? SkColor4f::FromColor(SkColorSetARGB(204, 120, 0, 255)) \
      : SkColor4f::FromColor(SkColorSetARGB(204, 255, 0, 120))

#define ARKWEB_UNITTESTS_DEFINE_ONE_COLOR() \
  auto temp_color_rect = \
    is_software_renderer() \
      ? SkColor4f::FromColor(SkColorSetARGB(255, 0, 0, 255)) \
      : SkColor4f::FromColor(SkColorSetARGB(255, 255, 0, 0))

#define ARKWEB_UNITTESTS_PASS_PARAM() \
  temp_color_rect1, \
  temp_color_rect2,

#define ARKWEB_UNITTESTS_RUN_PIXEL_TEST_2() \
  cc::FuzzyPixelComparator() \
    .DiscardAlpha() \
    .SetErrorPixelsPercentageLimit(100.f) \
    .SetAvgAbsErrorLimit(2.f) \
    .SetAbsErrorLimit(2)

#define ARKWEB_UNITTESTS_RUN_PIXEL_TEST_7() \
  cc::FuzzyPixelComparator() \
    .DiscardAlpha() \
    .SetErrorPixelsPercentageLimit(100.f) \
    .SetAvgAbsErrorLimit(5.f) \
    .SetAbsErrorLimit(7)

#define ARKWEB_UNITTESTS_BASIC() \
  if ((src_color_space_.GetTransferID() == TransferID::PQ && \
    !dst_color_space_.IsHDR()) || \
    (src_color_space_.GetPrimaryID() == PrimaryID::BT2020 && \
    src_color_space_.GetTransferID() == TransferID::PQ && \
    dst_color_space_.GetPrimaryID() == PrimaryID::XYZ_D50 && \
    dst_color_space_.GetTransferID() == TransferID::SRGB_HDR)) { \
      GTEST_SKIP() << "Skipping tonemapped output"; \
  }

#define ARKWEB_UNITTESTS_OUTPUT() \
  expected_output_colors[i] = \
    SkColorSetARGB(255, static_cast<size_t>(255.f * color.z() + 0.5f), \
                   static_cast<size_t>(255.f * color.y() + 0.5f), \
                   static_cast<size_t>(255.f * color.x() + 0.5f))

#endif // RENDERER_PIXELTEST_EXT_H
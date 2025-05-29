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

#ifndef YUV_READBACK_UNITTEST_EXT_H
#define YUV_READBACK_UNITTEST_EXT_H

#define ARKWEB_UNITTESTS_INIT_DISPLAY() \
  gl::init::InitializeGLNoExtensionsOneOff(true, gl::GpuPreference::kLowPower)


#define ARKWEB_UNITTESTS_COMPARE_PLANE_Y() \
    int maxdiff = 2; \
    ComparePlane( \
        Y, y_stride, output_frame->visible_data(media::VideoFrame::Plane::kY), \
        output_frame->stride(media::VideoFrame::Plane::kY), maxdiff, \
        output_xsize, output_ysize, &input_pixels, message + " Y plane")

#define ARKWEB_UNITTESTS_COMPARE_PLANE_U() \
    ComparePlane(U, u_stride, \
                 output_frame->visible_data(media::VideoFrame::Plane::kU), \
                 output_frame->stride(media::VideoFrame::Plane::kU), maxdiff, \
                 output_xsize / 2, output_ysize / 2, &input_pixels, \
                 message + " U plane")

#define ARKWEB_UNITTESTS_COMPARE_PLANE_V() \
    ComparePlane(V, v_stride, \
                 output_frame->visible_data(media::VideoFrame::Plane::kV), \
                 output_frame->stride(media::VideoFrame::Plane::kV), maxdiff, \
                 output_xsize / 2, output_ysize / 2, &input_pixels, \
                 message + " V plane")

#define ARKWEB_UNITTESTS_TEST_YUV_READBACK() \
TestYUVReadback(800, 400, 800, 400, 0, 0, 0, false, use_mrt == 1, \
                    gpu::GLHelper::SCALER_QUALITY_FAST)


#define ARKWEB_UNITTESTS_DRAW_RESULT() \
    EXPECT_EQ(3, draw_arrays_calls); \
    EXPECT_EQ(0, draw_buffer_calls)

#endif // YUV_READBACK_UNITTEST_EXT_H
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

#ifndef HOST_GPU_MEMORY_BUFFER_MANAGER_UNITTEST_EXT_H
#define HOST_GPU_MEMORY_BUFFER_MANAGER_UNITTEST_EXT_H

#define ARKWEB_UNITTESTS_TEST_GPU_SERVICE_PART1() \
  void GetSurfaceId(int32_t native_embed_id, \
                    GetSurfaceIdCallback callback) override {} \
  void SetVisible(int32_t nweb_id, bool visible) override {} \
  void Discard(uint32_t native_window_id) override {} \
  void SetTransformHint(uint32_t rotation, uint32_t window_id) override {}

#define ARKWEB_UNITTESTS_TEST_GPU_SERVICE_PART2() \
  void DestroyNativeWindow(uint32_t native_window_id) override {} \
  void StartMonitor(int32_t nweb_id) override {} \
  void StopMonitor() override {} \
  void SetHasTouchPoint(bool has_touch_point) override {} \
  void ReportSlidingFrameRate(int32_t frame_rate) override {} \
  void SetLTPOStrategy(int32_t strategy) override {} \
  void DumpGpuInfo(DumpGpuInfoCallback callback) override {}

#endif // HOST_GPU_MEMORY_BUFFER_MANAGER_UNITTEST_EXT_H
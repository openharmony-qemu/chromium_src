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

#ifndef FRAME_SINK_MANAGER_UNITTEST_EXT_H
#define FRAME_SINK_MANAGER_UNITTEST_EXT_H

#define ARKWEB_UNITTESTS_MY_EVICT_SURFACES_PART1() \
  void MyEvictSurfaces(const std::vector<SurfaceId>& surface_ids) { \
    for (const SurfaceId& surface_id : surface_ids) { \
      auto it = manager_.support_map_.find(surface_id.frame_sink_id()); \
      if (it != manager_.support_map_.end()) { \
        bool should_evict = true; \
        if (it->second->is_root()) { \
          auto root_it = manager_.root_sink_map_.find(surface_id.frame_sink_id()); \
          if (root_it != manager_.root_sink_map_.end()) { \
            should_evict = root_it->second->WillEvictSurface(surface_id)

#define ARKWEB_UNITTESTS_MY_EVICT_SURFACES_PART2() \
          } \
        } \
        if (should_evict) { \
          it->second->EvictSurface(surface_id.local_surface_id()); \
        } \
      } \
    } \
    manager_.surface_manager_.GarbageCollectSurfaces(); \
  }

#define ARKWEB_UNITTESTS_SUBMIT_COMPOSITOR_GRAME_SYNC() \
  GetRootCompositorFrameSinkImpl()->SubmitCompositorFrameSync( \
    local_surface_id2, MakeDefaultCompositorFrame(), absl::nullopt, 0, \
    CompositorFrameSinkImpl::SubmitCompositorFrameSyncCallback())

#endif // FRAME_SINK_MANAGER_UNITTEST_EXT_H
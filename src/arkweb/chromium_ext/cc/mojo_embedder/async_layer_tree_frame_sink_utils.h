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

#ifndef CC_MOJO_EMBEDDER_ASYNC_LAYER_TREE_FRAME_SINK_UTIL_H_
#define CC_MOJO_EMBEDDER_ASYNC_LAYER_TREE_FRAME_SINK_UTIL_H_

#include "arkweb/build/features/features.h"
#include "arkweb/chromium_ext/base/trace_event/common/trace_event_common_oh.h"
#include "arkweb/chromium_ext/base/trace_event/trace_event_oh.h"
#include "base/memory/raw_ptr.h"
#include "components/viz/common/surfaces/surface_id.h"
#include "components/viz/common/quads/compositor_frame.h"
#include "components/viz/common/frame_sinks/begin_frame_args.h"
#include "components/viz/common/frame_sinks/begin_frame_source.h"
#include "services/viz/public/mojom/compositing/compositor_frame_sink.mojom.h"
#include "cc/scheduler/scheduler.h"
#include "cc/trees/layer_tree_frame_sink_client.h"


#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
#include "cc/mojo_embedder/software_compositor_registry_ohos.h"
#include "cc/mojo_embedder/software_compositor_renderer_ohos.h"
#endif

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
#include "base/process/process_handle.h"
#endif

namespace cc {


namespace mojo_embedder {
class AsyncLayerTreeFrameSink;

class AsyncLayerTreeFrameSinkUtils {
 public:
  AsyncLayerTreeFrameSink* asyncLayerTreeFrameSink;
  AsyncLayerTreeFrameSinkUtils(AsyncLayerTreeFrameSink* sink);

#if BUILDFLAG(ARKWEB_PERFORMANCE_SCHEDULING)
  void BindToClientArkWebPerSch(const raw_ptr<viz::mojom::CompositorFrameSink>& compositor_frame_sink_ptr_,
      int32_t currentRealPid_,
      int32_t getCurrentRealPid_);
#endif

#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  void BindToClientArkWebSoftCom(LayerTreeFrameSinkClient* client, viz::BeginFrameSource* source);
  void DetachFromClientArkWebSoftCom();
  bool SubmitCompositorFrameArkWebSoftCom(viz::CompositorFrame& frame);
  void InitSoftComRenderArkWebSoftCom(SoftwareCompositorRegistryOhos* registry);
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  void SubmitCompositorFrameDfxDumpLog(viz::LocalSurfaceId local_surface_id_);
#endif

#if BUILDFLAG(ARKWEB_DFX_TRACING)
  void SubmitCompositorFrameDfxDumpTrace(int64_t trace_id);
  void OnBeginFrameArkWebDfxTrace(int64_t trace_id);
#endif

#if BUILDFLAG(ARKWEB_SWAP_BUFFER_TRACE)
  void SubmitCompositorFrameArkWebSwapBuTr(int64_t trace_id, FrameSkippedReason reason);
  void OnBeginFrameArkWebSwBuTr(int64_t trace_id);
#endif

 private:
#if BUILDFLAG(ARKWEB_SOFTWARE_COMPOSITOR)
  std::unique_ptr<SoftwareCompositorRendererOhos> software_renderer_ohos_;
#endif

#if BUILDFLAG(ARKWEB_DFX_DUMP)
  bool is_first_submit_ = true;
#endif
};

}  // namespace mojo_embedder
}  // namespace cc

#endif  // CC_MOJO_EMBEDDER_ASYNC_LAYER_TREE_FRAME_SINK_H_

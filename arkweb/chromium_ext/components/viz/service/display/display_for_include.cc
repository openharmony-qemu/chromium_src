/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "arkweb/ohos_adapter_ndk/interfaces/ohos_adapter_helper.h"

namespace viz {
#if BUILDFLAG(ARKWEB_PARTIAL_DRAW)
namespace {
bool IsPartialDrawDebugEnable() {
  static std::atomic<int> flag = -1;
  if (flag < 0) {
    auto& system_properties = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                .GetSystemPropertiesInstance();
    flag = system_properties.GetStringParameter("web.gpu.partial.draw.debug", "none") == "debug";
  }

  return !!flag;
}
}

void AddDebugQuad(AggregatedRenderPass& last_render_pass) {
  if (!IsPartialDrawDebugEnable()) {
    return;
  }

  TRACE_EVENT1("viz", "partial debug ", " damage_rect ", last_render_pass.damage_rect.ToString());

  SharedQuadState* shared_quad_state = last_render_pass.CreateAndAppendSharedQuadState();
  shared_quad_state->SetAll(
    gfx::Transform(),
    last_render_pass.damage_rect,
    last_render_pass.damage_rect,
    gfx::MaskFilterInfo(),
    std::nullopt,
    false,
    1.0f,
    SkBlendMode::kSrcOver,
    1,
    0, false
  );

  auto* debug_quad = last_render_pass.CreateAndAppendDrawQuad<viz::DebugBorderDrawQuad>();

  debug_quad->SetNew(
    shared_quad_state,
    last_render_pass.damage_rect,
    last_render_pass.damage_rect,
    SkColors::kRed,
    10
  );
}
#endif
}

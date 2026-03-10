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

namespace gl {
#if BUILDFLAG(ARKWEB_PARTIAL_DRAW)
bool IsPartialDrawEnable() {
  static std::atomic<int> flag = -1;
  if (flag < 0) {
    auto& system_properties = OHOS::NWeb::OhosAdapterHelper::GetInstance()
                                .GetSystemPropertiesInstance();
    flag = system_properties.GetStringParameter("web.gpu.partial.draw", "none") == "single";
  }

  return !!flag;
}

bool NativeViewGLSurfaceEGL::SetPresentBufferDamage(gfx::Rect damage_rect, gfx::Rect curr_rect) {
  TRACE_EVENT1("base", "SetPresentBufferDamage", "damage_rect", damage_rect.ToString());
  if (!supports_post_sub_buffer_) {
    return true;
  }

  if (last_damage_rects_ == curr_rect) {
    same_damage_count_++;
  } else {
    same_damage_count_ = 0;
    last_damage_rects_ = curr_rect;
  }

  if (damage_rect.IsEmpty()) {
    if (present_buffer_age_ == 0) {
      same_damage_count_ = 0;
    }
    return true;
  }

  std::vector<int> rects = {damage_rect.x(), GetSize().height() - damage_rect.y() - damage_rect.height(),
                damage_rect.width(), damage_rect.height()};
  EGLBoolean retVal = eglSetDamageRegionKHR(display_->GetDisplay(), surface_,
                  const_cast<EGLint*>(rects.data()),
                  static_cast<EGLint>(rects.size() / 4));
  if (!retVal) {
    LOG(ERROR) << "SetPresentBufferDamage fail retVal " << retVal;
    same_damage_count_ = 0;
    return false;
  }
  return true;
}

int NativeViewGLSurfaceEGL::GetPresentBufferAge() {
  if (!supports_post_sub_buffer_) {
    return 0;
  }
  return present_buffer_age_;
}

gfx::Rect NativeViewGLSurfaceEGL::GetLastBufferDamageRect() {
  return last_damage_rects_;
}

int NativeViewGLSurfaceEGL::GetSameBufferDamageCnt() {
  return same_damage_count_;
}
#endif
}

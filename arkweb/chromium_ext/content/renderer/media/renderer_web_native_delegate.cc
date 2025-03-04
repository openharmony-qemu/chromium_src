/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "content/renderer/media/renderer_web_native_delegate.h"

#include <stdint.h>

#include "base/auto_reset.h"
#include "base/functional/bind.h"
#include "base/system/sys_info.h"
#include "content/public/common/content_client.h"
#include "content/public/renderer/content_renderer_client.h"
#include "content/public/renderer/render_frame.h"
#include "content/public/renderer/render_thread.h"
#include "media/base/media_content_type.h"
#include "third_party/blink/public/mojom/frame/user_activation_notification_type.mojom.h"
#include "third_party/blink/public/web/web_local_frame.h"
#include "ui/gfx/geometry/size.h"

namespace media {

// TODO: Limit native numbers with the logic for player.
RendererWebNativeDelegate::RendererWebNativeDelegate(
    content::RenderFrame* render_frame)
    : RenderFrameObserver(render_frame) {}

RendererWebNativeDelegate::~RendererWebNativeDelegate() {}

bool RendererWebNativeDelegate::IsFrameHidden() {
  return (render_frame() && render_frame()->IsHidden());
}

int RendererWebNativeDelegate::AddObserver(Observer* observer) {
  return id_map_.Add(observer);
}

void RendererWebNativeDelegate::RemoveObserver(int native_id) {
  DCHECK(id_map_.Lookup(native_id));
  id_map_.Remove(native_id);
}

void RendererWebNativeDelegate::WasHidden() {
  for (base::IDMap<Observer*>::iterator it(&id_map_); !it.IsAtEnd();
       it.Advance()) {
    it.GetCurrentValue()->OnFrameHidden();
  }
}

void RendererWebNativeDelegate::WasShown() {
  for (base::IDMap<Observer*>::iterator it(&id_map_); !it.IsAtEnd();
       it.Advance()) {
    it.GetCurrentValue()->OnFrameShown();
  }
}

void RendererWebNativeDelegate::OnDestruct() {
  delete this;
}

}  // namespace media
                     
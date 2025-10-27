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

#ifndef CONTENT_RENDERER_MEDIA_RENDERER_WEB_NATIVE_DELEGATE_H_
#define CONTENT_RENDERER_MEDIA_RENDERER_WEB_NATIVE_DELEGATE_H_

#include <map>
#include <memory>
#include <set>

#include "base/containers/flat_set.h"
#include "base/containers/id_map.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/metrics/single_sample_metrics.h"
#include "base/task/single_thread_task_runner.h"
#include "base/time/default_tick_clock.h"
#include "base/timer/timer.h"
#include "content/common/content_export.h"
#include "content/public/renderer/render_frame_observer.h"
#include "third_party/blink/public/platform/media/web_native_delegate.h"

namespace media {

// Standard implementation of WebNativeDelegate; communicates state to
// the MediaPlayerDelegateHost.
class CONTENT_EXPORT RendererWebNativeDelegate
    : public content::RenderFrameObserver,
      public blink::WebNativeDelegate {
  // public base::SupportsWeakPtr<RendererWebNativeDelegate> {
 public:
  explicit RendererWebNativeDelegate(content::RenderFrame* render_frame);

  RendererWebNativeDelegate(const RendererWebNativeDelegate&) = delete;
  RendererWebNativeDelegate& operator=(const RendererWebNativeDelegate&) =
      delete;

  ~RendererWebNativeDelegate() override;

  // blink::WebNativeDelegate implementation.
  bool IsFrameHidden() override;
  int AddObserver(Observer* observer) override;
  void RemoveObserver(int native_id) override;

  // content::RenderFrameObserver overrides.
  void WasHidden() override;
  void WasShown() override;
  void OnDestruct() override;

 private:
  base::IDMap<Observer*> id_map_;
};

}  // namespace media

#endif  // CONTENT_RENDERER_MEDIA_RENDERER_WEB_NATIVE_DELEGATE_H_

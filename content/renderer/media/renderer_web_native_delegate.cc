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
    : RenderFrameObserver(render_frame),
      allow_idle_cleanup_(
          content::GetContentClient()->renderer()->IsIdleMediaSuspendEnabled()),
      tick_clock_(base::DefaultTickClock::GetInstance()) {
  idle_cleanup_interval_ = base::Seconds(5);
  idle_timeout_ = base::Seconds(15);

  is_low_end_ = base::SysInfo::IsLowEndDevice();
  idle_cleanup_timer_.SetTaskRunner(
      render_frame->GetTaskRunner(blink::TaskType::kInternalMedia));
}

RendererWebNativeDelegate::~RendererWebNativeDelegate() {}

bool RendererWebNativeDelegate::IsFrameHidden() {
  return (render_frame() && render_frame()->IsHidden());
}

int RendererWebNativeDelegate::AddObserver(Observer* observer) {
  const auto result = id_map_.Add(observer);
  if (id_map_.size() > peak_player_count_) {
    peak_player_count_ = id_map_.size();
  }
  return result;
}

void RendererWebNativeDelegate::RemoveObserver(int native_id) {
  DCHECK(id_map_.Lookup(native_id));
  id_map_.Remove(native_id);
  idle_player_map_.erase(native_id);
  stale_players_.erase(native_id);

  ScheduleUpdateTask();
}

void RendererWebNativeDelegate::WasHidden() {
  for (base::IDMap<Observer*>::iterator it(&id_map_); !it.IsAtEnd();
       it.Advance())
    it.GetCurrentValue()->OnFrameHidden();

  ScheduleUpdateTask();
}

void RendererWebNativeDelegate::WasShown() {
  for (base::IDMap<Observer*>::iterator it(&id_map_); !it.IsAtEnd();
       it.Advance())
    it.GetCurrentValue()->OnFrameShown();

  ScheduleUpdateTask();
}

void RendererWebNativeDelegate::ScheduleUpdateTask() {
  if (!pending_update_task_) {
    base::SingleThreadTaskRunner::GetCurrentDefault()->PostTask(
        FROM_HERE,
        base::BindOnce(&RendererWebNativeDelegate::UpdateTask, AsWeakPtr()));
    pending_update_task_ = true;
  }
}

void RendererWebNativeDelegate::UpdateTask() {
  DVLOG(3) << __func__;
  pending_update_task_ = false;

  if (!allow_idle_cleanup_)
    return;

  // Clean up idle players.
  bool aggressive_cleanup = false;

  // When we reach the maximum number of idle players, clean them up
  // aggressively. Values chosen after testing on a Galaxy Nexus device for
  // http://crbug.com/612909.
  if (idle_player_map_.size() > (is_low_end_ ? 2u : 8u))
    aggressive_cleanup = true;

  CleanUpIdlePlayers(aggressive_cleanup ? base::TimeDelta() : idle_timeout_);

  // If there are still idle players, schedule an attempt to clean them up.
  // This construct ensures that the next callback is always
  // |idle_cleanup_interval_| from now.
  idle_cleanup_timer_.Stop();
  if (!idle_player_map_.empty()) {
    idle_cleanup_timer_.Start(
        FROM_HERE, idle_cleanup_interval_,
        base::BindOnce(&RendererWebNativeDelegate::UpdateTask,
                       base::Unretained(this)));
  }
}

void RendererWebNativeDelegate::CleanUpIdlePlayers(base::TimeDelta timeout) {
  const base::TimeTicks now = tick_clock_->NowTicks();

  // Create a list of stale players before making any possibly reentrant calls
  // to OnIdleTimeout().
  std::vector<int> stale_players;
  for (const auto& it : idle_player_map_) {
    if (now - it.second >= timeout)
      stale_players.push_back(it.first);
  }

  // Notify stale players.
  for (int player_id : stale_players) {
    Observer* player = id_map_.Lookup(player_id);
    if (player && idle_player_map_.erase(player_id)) {
      stale_players_.insert(player_id);
    }
  }
}

void RendererWebNativeDelegate::OnDestruct() {
  delete this;
}

}  // namespace media

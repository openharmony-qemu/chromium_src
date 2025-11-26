// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "content/browser/media/ohos/ohos_media_player_renderer.h"

#include <memory>

#include "base/functional/bind.h"
#include "base/functional/callback_helpers.h"
#include "content/browser/media/ohos/ohos_media_player_renderer_web_contents_observer.h"
#include "content/browser/media/session/media_session_impl.h"
#include "content/browser/media/ohos/ohos_media_resource_getter_impl.h"
#include "content/browser/web_contents/web_contents_impl.h"
#include "content/public/browser/browser_context.h"
#include "content/public/browser/browser_task_traits.h"
#include "content/public/browser/browser_thread.h"
#include "content/public/browser/content_browser_client.h"
#include "content/public/browser/render_process_host.h"
#include "content/public/browser/storage_partition.h"
#include "content/public/browser/web_contents.h"
#include "content/public/common/content_client.h"
#include "media/audio/ohos/ohos_audio_focus_controller.h"
#include "media/base/timestamp_constants.h"
#include "media/mojo/mojom/renderer_extensions.mojom.h"

namespace content {

namespace {
const float kDefaultVolume = 1.0;
constexpr double kPlaybackRateLevel0 = 0;
constexpr double kPlaybackRateLevel1 = 1;
constexpr double kPlaybackRateLevel2 = 1.25;
constexpr double kPlaybackRateLevel3 = 1.75;
constexpr double kPlaybackRateLevel4 = 2;
enum InterruptHint {
  INTERRUPT_HINT_NONE = 0,
  INTERRUPT_HINT_RESUME,
  INTERRUPT_HINT_PAUSE,
  INTERRUPT_HINT_STOP,
  INTERRUPT_HINT_DUCK,
  INTERRUPT_HINT_UNDUCK
};
}  // namespace



}  // namespace content
// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_player_listener.h"

namespace media {

OHOSMediaPlayerListener::OHOSMediaPlayerListener(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<OHOSMediaPlayerBridge> media_player)
    : task_runner_(task_runner), media_player_(media_player) {
  DCHECK(task_runner_.get());
  DCHECK(media_player_);
}

void OHOSMediaPlayerListener::OnBufferAvailable(
    std::shared_ptr<OHOS::NWeb::SurfaceBufferAdapter> buffer) {
  task_runner_->PostTask(
      FROM_HERE,
      base::BindOnce(&OHOSMediaPlayerBridge::OnVideoSizeChanged, media_player_,
                     buffer->GetWidth(), buffer->GetHeight()));
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSMediaPlayerBridge::OnBufferAvailable,
                                media_player_, std::move(buffer)));
}

OHOSMediaPlayerListener::~OHOSMediaPlayerListener() {}

}  // namespace media

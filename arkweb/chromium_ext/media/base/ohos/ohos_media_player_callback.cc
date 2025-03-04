// Copyright 2022 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_player_callback.h"

namespace media {

OHOSMediaPlayerCallback::OHOSMediaPlayerCallback(
    const scoped_refptr<base::SingleThreadTaskRunner>& task_runner,
    base::WeakPtr<OHOSMediaPlayerBridge> media_player)
    : task_runner_(task_runner), media_player_(media_player) {
  DCHECK(task_runner_.get());
  DCHECK(media_player_);
}

OHOSMediaPlayerCallback::~OHOSMediaPlayerCallback() {}

void OHOSMediaPlayerCallback::OnError(
    OHOS::NWeb::PlayerAdapterErrorType errorType) {
  LOG(ERROR) << "media player error";
  int media_error_type =
      OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_INVALID_CODE;
  if (errorType == OHOS::NWeb::PlayerAdapterErrorType::UNSUPPORT_TYPE) {
    media_error_type =
        OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_FORMAT;
  } else if (errorType == OHOS::NWeb::PlayerAdapterErrorType::FATAL_ERROR) {
    media_error_type =
        OHOSMediaPlayerBridge::MediaErrorType::MEDIA_ERROR_DECODE;
  }
  task_runner_->PostTask(
      FROM_HERE, base::BindOnce(&OHOSMediaPlayerBridge::OnError, media_player_,
                                media_error_type));
}

void OHOSMediaPlayerCallback::OnInfo(OHOS::NWeb::PlayerOnInfoType type,
                                     int32_t extra,
                                     int32_t value) {
  switch (type) {
    case OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_EOS:
      task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(&OHOSMediaPlayerBridge::OnEnd, media_player_));
      break;
    case OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_STATE_CHANGE:
      task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(
              &OHOSMediaPlayerBridge::OnPlayerStateUpdate, media_player_,
              static_cast<OHOS::NWeb::PlayerAdapter::PlayerStates>(extra)));
      break;
    case OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_POSITION_UPDATE:
      task_runner_->PostTask(
          FROM_HERE, base::BindOnce(&OHOSMediaPlayerBridge::OnSeekBack,
                                    media_player_, base::Milliseconds(extra)));
      break;
    case OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_MESSAGE:
      break;
    case OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_INTERRUPT_EVENT:
      task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(&OHOSMediaPlayerBridge::OnPlayerInterruptEvent,
                         media_player_, value));
      break;
    case OHOS::NWeb::PlayerOnInfoType::INFO_TYPE_SEEKDONE:
      task_runner_->PostTask(
          FROM_HERE,
          base::BindOnce(&OHOSMediaPlayerBridge::SeekDone, media_player_));
      break;
    default:
      break;
  }
}
}  // namespace media

// Copyright (c) 2022 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/base/ohos/ohos_media_player_recorder.h"

namespace media {

void OHOSMediaPlayerRecorder::StartRecord() {
  if (start_time_ > base::TimeTicks()) {
    return;
  }
  start_time_ = base::TimeTicks::Now();
}

void OHOSMediaPlayerRecorder::PauseRecord() {
  if (start_time_ > base::TimeTicks()) {
    base::TimeDelta elapsed = base::TimeTicks::Now() - start_time_;
    accumulated_duration_ += elapsed;
    start_time_ = base::TimeTicks();
  }
}

void OHOSMediaPlayerRecorder::StopRecord() {
  if (total_duration_ > base::TimeDelta()) {
    return;
  }
  PauseRecord();
  total_duration_ = accumulated_duration_;
  accumulated_duration_ = base::TimeDelta();
}

base::TimeDelta OHOSMediaPlayerRecorder::GetDuration() {
  if (start_time_ > base::TimeTicks()) {
    base::TimeDelta elapsed = base::TimeTicks::Now() - start_time_;
    total_duration_ = accumulated_duration_ + elapsed;
    return total_duration_;
  } else {
    total_duration_ = accumulated_duration_;
    return total_duration_;
  }
}

void OHOSMediaPlayerRecorder::Reset() {
  start_time_ = base::TimeTicks();
  accumulated_duration_ = base::TimeDelta();
  total_duration_ = base::TimeDelta();
}

} // namespace media
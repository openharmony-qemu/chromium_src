// Copyright 2025 The Huawei Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_BASE_OHOS_MEDIA_PLAYER_RECORDER_H_
#define MEDIA_BASE_OHOS_MEDIA_PLAYER_RECORDER_H_

#include "base/timer/timer.h"
#include "media/base/media_export.h"

// Class for obtaining the playback time of HLS live stream
namespace media {
  class MEDIA_EXPORT OHOSMediaPlayerRecorder {
    public:
      // Start recording the media playback time.
      void StartRecord();

      // Pause recording the media playback time.
      void PauseRecord();

      // Stop recording the media playback time.
      void StopReocrd();
    
      // Obtain the media playback time.
      base::TimeDelta GetDuration();

      // Reset recording the media playback time.
      void Reset();
    private:
      base::TimeTicks start_time_;
      base::TimeDelta accumulated_duration_;
      base::TimeDelta total_duration_;
};

} // namespace media

#endif // MEDIA_BASE_OHOS_MEDIA_PLAYER_RECORDER_H_
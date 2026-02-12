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

#include "arkweb/chromium_ext/net/quiche/quic_stream_frame_detector.h"
 
#include "net/third_party/quiche/src/quiche/quic/core/quic_constants.h"
#include "net/third_party/quiche/src/quiche/quic/core/quic_time.h"
#include "net/third_party/quiche/src/quiche/common/platform/api/quiche_logging.h"
 
namespace quic {
 
QuicStreamFrameDetector::QuicStreamFrameDetector(Delegate* delegate,
                                                 QuicTime now,
                                                 QuicAlarmProxy alarm)
    : delegate_(delegate),
      time_of_last_stream_frame_(now),
      timeout_(QuicTime::Delta::FromSeconds(10)),
      alarm_(alarm),
      stopped_(false) {
  SetAlarm();
}
 
void QuicStreamFrameDetector::OnAlarm() {
  if (stopped_) {
    return;
  }
  delegate_->OnBrokenDetect();
}
 
void QuicStreamFrameDetector::OnStreamFrameReceived(QuicTime now) {
  time_of_last_stream_frame_ = std::max(time_of_last_stream_frame_, now);
  SetAlarm();
}
 
void QuicStreamFrameDetector::StopDetection() {
  alarm_.PermanentCancel();
  stopped_ = true;
}
 
void QuicStreamFrameDetector::SetAlarm() {
  if (stopped_) {
    return;
  }
  QuicTime deadline = time_of_last_stream_frame_ + timeout_;
  alarm_.Update(deadline, kAlarmGranularity);
}
 
}  // namespace quic
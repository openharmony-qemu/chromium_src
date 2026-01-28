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

#ifndef NET_QUICHE_QUIC_STREAM_FRAME_DETECTOR_H_
#define NET_QUICHE_QUIC_STREAM_FRAME_DETECTOR_H_
 
#include "net/third_party/quiche/src/quiche/quic/core/quic_alarm.h"
#include "net/third_party/quiche/src/quiche/quic/core/quic_alarm_factory.h"
#include "net/third_party/quiche/src/quiche/quic/core/quic_connection_alarms.h"
#include "net/third_party/quiche/src/quiche/quic/core/quic_one_block_arena.h"
#include "net/third_party/quiche/src/quiche/quic/core/quic_time.h"
#include "net/third_party/quiche/src/quiche/quic/platform/api/quic_export.h"
 
namespace quic {

namespace test {
class QuicConnectionPeer;
class QuicStreamFrameDetectorTestPeer;
}  // namespace test
 
// QuicStreamFrameDetector detects when no STREAM_FRAME has been received
// for more than a fixed timeout (10 seconds).
class QUICHE_EXPORT QuicStreamFrameDetector {
 public:
  class QUICHE_EXPORT Delegate {
   public:
    virtual ~Delegate() {}
 
    // Called when no STREAM_FRAME has been received for the timeout duration.
    virtual void OnBrokenDetect() = 0;
  };
 
  QuicStreamFrameDetector(Delegate* delegate, QuicTime now, QuicAlarmProxy alarm);
 
  void OnAlarm();
 
  // Called when a STREAM_FRAME is received.
  void OnStreamFrameReceived(QuicTime now);
 
  // Stop the detection once and for all.
  void StopDetection();
 
  QuicTime time_of_last_stream_frame() const {
    return time_of_last_stream_frame_;
  }
 
 private:
  void SetAlarm();
 
  Delegate* delegate_;  // Not owned.
 
  // Time that last STREAM_FRAME is received for this connection.
  QuicTime time_of_last_stream_frame_;
 
  // Timeout duration for detecting lack of STREAM_FRAME reception.
  const QuicTime::Delta timeout_;
 
  // Alarm used to detect the timeout.
  QuicAlarmProxy alarm_;
 
  // True if the detection has been stopped.
  bool stopped_;
};
 
}  // namespace quic
 
#endif  // NET_QUICHE_QUIC_STREAM_FRAME_DETECTOR_H_
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
 
#include "net/third_party/quiche/src/quiche/quic/core/quic_connection_alarms.h"
#include "net/third_party/quiche/src/quiche/quic/core/quic_one_block_arena.h"
#include "net/third_party/quiche/src/quiche/quic/core/quic_time.h"
#include "net/third_party/quiche/src/quiche/quic/platform/api/quic_expect_bug.h"
#include "net/third_party/quiche/src/quiche/quic/platform/api/quic_flags.h"
#include "net/third_party/quiche/src/quiche/quic/platform/api/quic_test.h"
#include "net/third_party/quiche/src/quiche/quic/test_tools/mock_quic_connection_alarms.h"
#include "net/third_party/quiche/src/quiche/quic/test_tools/quic_connection_peer.h"
#include "net/third_party/quiche/src/quiche/quic/test_tools/quic_test_utils.h"
 
namespace quic {
namespace test {
 
class QuicStreamFrameDetectorTestPeer {
 public:
  static QuicAlarmProxy GetAlarm(QuicStreamFrameDetector* detector) {
    return detector->alarm_;
  }
};
 
namespace {
 
class MockDelegate : public QuicStreamFrameDetector::Delegate {
 public:
  MOCK_METHOD(void, OnBrokenDetect, (), (override));
};
 
class QuicStreamFrameDetectorTest : public QuicTest {
 public:
  QuicStreamFrameDetectorTest()
      : alarms_(&connection_alarms_delegate_, alarm_factory_, arena_),
        detector_(&delegate_, clock_.Now() + QuicTimeDelta::FromSeconds(1),
                  alarms_.stream_frame_detector_alarm()),
        alarm_(alarms_.stream_frame_detector_alarm()) {
    clock_.AdvanceTime(QuicTime::Delta::FromSeconds(1));
    ON_CALL(connection_alarms_delegate_, OnStreamFrameDetectorAlarm())
        .WillByDefault([&] { detector_.OnAlarm(); });
  }
 
 protected:
  testing::StrictMock<MockDelegate> delegate_;
  MockConnectionAlarmsDelegate connection_alarms_delegate_;
  QuicConnectionArena arena_;
  MockAlarmFactory alarm_factory_;
  QuicConnectionAlarms alarms_;
  MockClock clock_;
  QuicStreamFrameDetector detector_;
  QuicTestAlarmProxy alarm_;
};
 
TEST_F(QuicStreamFrameDetectorTest, StreamFrameDetected) {
  EXPECT_TRUE(alarm_->IsSet());
  detector_.OnStreamFrameReceived(clock_->Now());
  EXPECT_EQ(clock_.Now() + QuicTime::Delta::FromSeconds(10),
            alarm_->deadline());
 
  // No stream frame activity for 10s.
  clock_.AdvanceTime(QuicTime::Delta::FromSeconds(10));
  EXPECT_CALL(delegate_, OnBrokenDetect());
  alarm_->Fire();
}
 
TEST_F(QuicIdleNetworkDetectorTest, StreamFrameReceivedExtendsDeadline) {
  EXPECT_TRUE(alarm_->IsSet());
  detector_.OnStreamFrameReceived(clock_->Now());
 
  // No stream frame activity for 5s.
  clock_.AdvanceTime(QuicTime::Delta::FromSeconds(5));
  detector_.OnStreamFrameReceived(clock_->Now());
  EXPECT_EQ(clock_.Now() + QuicTime::Delta::FromSeconds(10),
            alarm_->deadline());
  alarm_->Fire();
}
 
TEST_F(QuicIdleNetworkDetectorTest, NoAlarmAfterStopped) {
  detector_.StopDetection();
  detector_.OnStreamFrameReceived(clock_->Now());
  EXPECT_FALSE(alarm_->IsSet());
}
 
}  // namespace

}  // namespace test
}  // namespace quic

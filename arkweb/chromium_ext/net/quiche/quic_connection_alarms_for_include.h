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

#ifndef NET_QUICHE_QUIC_CONNECTION_ALARMS_FOR_INCLUDE_H_
#define NET_QUICHE_QUIC_CONNECTION_ALARMS_FOR_INCLUDE_H_
 
#ifndef QUICHE_QUIC_CORE_QUIC_CONNECTION_ALARMS_H_
#error "must be in include form QUICHE_QUIC_CORE_QUIC_CONNECTION_ALARMS_H_"
#endif

#if BUILDFLAG(ARKWEB_NETWORK_LOAD)
  AlarmProxy stream_frame_detector_alarm() {
    if (use_multiplexer_) {
      return AlarmProxy(QuicAlarmMultiplexer::AlarmProxy(
          &*multiplexer_, QuicAlarmSlot::kStreamFrameDetector));
    }
    return AlarmProxy(QuicConnectionAlarmHolder::AlarmProxy(
        holder_->stream_frame_detector_alarm()));
  }
  ConstAlarmProxy stream_frame_detector_alarm() const {
    if (use_multiplexer_) {
      return ConstAlarmProxy(QuicAlarmMultiplexer::ConstAlarmProxy(
          &*multiplexer_, QuicAlarmSlot::kStreamFrameDetector));
    }
    return ConstAlarmProxy(QuicConnectionAlarmHolder::ConstAlarmProxy(
        holder_->stream_frame_detector_alarm()));
  }
#endif  // ARKWEB_NETWORK_LOAD

#endif  // NET_QUICHE_QUIC_CONNECTION_ALARMS_FOR_INCLUDE_H_
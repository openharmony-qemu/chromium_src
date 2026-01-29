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

#ifndef QUICHE_QUIC_CORE_QUIC_SESSION_H_
#error "must be in include form QUICHE_QUIC_CORE_QUIC_SESSION_H_"
#endif

#include "net/third_party/quiche/src/quiche/quic/core/quic_session.h"
#include "base/strings/strcat.h"
#include "base/strings/string_number_conversions.h"
#include "arkweb/chromium_ext/net/quiche/quic_stream_frame_detector.h"

namespace quic {

std::string QuicSession::GetStreamsInfoForQuicBroken() const {
  std::string info = base::StrCat({
      "connection_id: ",
      connection_->connection_id().ToString(),
      ", num_active_streams: ",
      base::NumberToString(GetNumActiveStreams()),
      ", num_pending_streams: ",
      base::NumberToString(pending_streams_size()),
      ", num_outgoing_draining_streams: ",
      base::NumberToString(num_outgoing_draining_streams()),
      " " });
  // Log info for up to 10 streams.
  size_t i = 10;
  for (const auto& it : stream_map_) {
    if (it.second->is_static()) {
      continue;
    }
    base::StrAppend(
        &info, 
        { "{", base::NumberToString(it.second->id()), ":", 
          base::NumberToString(it.second->BufferedBytesInSequence()), "}" });
    --i;
    if (i == 0) {
      break;
    }
  }
  return info;
}

size_t QuicSession::GetNumActiveStreamsForInterface() const {
  return GetNumActiveStreams();
}

}  // namespace quic

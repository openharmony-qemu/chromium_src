/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
#include "base/rand_util.h"
#include "base/atomic_sequence_num.h"
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE))

namespace network::cors {

namespace {

#if BUILDFLAG(ARKWEB_NETWORK_BASE)
int GetInitialRequestID() {
  // Starting with a random number speculatively avoids RDH_INVALID_REQUEST_ID
  // which are assumed to have been caused by restarting RequestID at 0 when
  // restarting a renderer after a crash - this would cause collisions if
  // requests from the previously crashed renderer are still active.  See
  // https://crbug.com/614281#c61 for more details about this hypothesis.
  //
  // To avoid increasing the likelihood of overflowing the range of available
  // RequestIDs, kMax is set to a relatively low value of 2^20 (rather than
  // to something higher like 2^31).
  const int kMin = 1 << 26;
  const int kMax = 1 << 28;
  return base::RandInt(kMin + 1, kMax);
}

int GenerateRequestId() {
  static const int kInitialRequestID = GetInitialRequestID();
  static base::AtomicSequenceNumber sequence;
  return kInitialRequestID + sequence.GetNext();
}
#endif  // BUILDFLAG(ARKWEB_NETWORK_BASE)

} // namespace

} // namespace network::cors
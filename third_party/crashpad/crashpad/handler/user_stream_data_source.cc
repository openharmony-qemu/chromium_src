// Copyright 2017 The Crashpad Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "handler/user_stream_data_source.h"

#include "arkweb/build/features/features.h"
#include "base/logging.h"
#include "minidump/minidump_file_writer.h"
#include "minidump/minidump_user_extension_stream_data_source.h"
#include "snapshot/process_snapshot.h"

#if BUILDFLAG(ARKWEB_CRASHPAD)
#include <atomic>
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)

namespace crashpad {

#if BUILDFLAG(ARKWEB_CRASHPAD)
namespace {
std::atomic<int> g_gwp_asan_detection{
    static_cast<int>(GwpAsanDetection::kNone)};
}  // namespace

void SetGwpAsanDetection(GwpAsanDetection detection) {
  g_gwp_asan_detection.store(static_cast<int>(detection),
                             std::memory_order_release);
}

GwpAsanDetection GetGwpAsanDetection() {
  return static_cast<GwpAsanDetection>(
      g_gwp_asan_detection.load(std::memory_order_acquire));
}
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)

void AddUserExtensionStreams(
    const UserStreamDataSources* user_stream_data_sources,
    ProcessSnapshot* process_snapshot,
    MinidumpFileWriter* minidump_file_writer) {
#if BUILDFLAG(ARKWEB_CRASHPAD)
  // Reset the GWP-ASan detection result before producing this minidump so that
  // a previous crash's result does not leak into the current one when the
  // handler stays alive across multiple crashes.
  SetGwpAsanDetection(GwpAsanDetection::kNone);
#endif  // BUILDFLAG(ARKWEB_CRASHPAD)
  if (!user_stream_data_sources)
    return;
  for (const auto& source : *user_stream_data_sources) {
    std::unique_ptr<MinidumpUserExtensionStreamDataSource> data_source(
        source->ProduceStreamData(process_snapshot));
    if (data_source &&
        !minidump_file_writer->AddUserExtensionStream(std::move(data_source))) {
      // This should only happen if multiple user stream sources yield the
      // same stream type. It's the user's responsibility to make sure
      // sources don't collide on the same stream type.
      LOG(ERROR) << "AddUserExtensionStream failed";
    }
  }
}

}  // namespace crashpad

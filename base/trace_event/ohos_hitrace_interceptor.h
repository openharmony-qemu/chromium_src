// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_OHOS_HITRACE_INTERCEPTOR_H_
#define BASE_TRACE_EVENT_OHOS_HITRACE_INTERCEPTOR_H_

#include <cstddef>
#include <cstdint>
#include <string_view>

#include "absl/container/flat_hash_map.h"
#include "base/base_export.h"
#include "base/trace_event/ohos_fix_buffer.h"
#include "base/trace_event/ohos_hitrace_adapter.h"
#include "third_party/perfetto/include/perfetto/tracing/interceptor.h"

namespace base::trace_event::ohos {

/**
 * store the interned data required to use to map iid back
 */
struct BASE_EXPORT OhosSeqData {
 public:
  /** map between iid(uint64_t) to std::string, to map interned_data */
  using IIdMap = absl::flat_hash_map<uint64_t, std::string>;
  /** used for event_names in track_event */
  IIdMap event_names_map_;
  /** used for event_categories in track_event */
  IIdMap event_categories_map_;
  /** used for debug annotation names */
  IIdMap debug_annotation_names_map_;
  /** used for debug annotation type proto name */
  IIdMap proto_type_map_;
};

// OhosHiTraceInterceptor is a high performance perfetto interceptor, it
// directly write to hitrace. To prevent protential jank, except update interned
// iid map, all track parse and hitrace format don't use heap allocation
class BASE_EXPORT OhosHiTraceInterceptor
    : public perfetto::Interceptor<OhosHiTraceInterceptor> {
 public:
  /** ThreadLocalState store a buffer to append to serialize trace packet */
  struct ThreadLocalState : public InterceptorBase::ThreadLocalState {
   public:
    ThreadLocalState(ThreadLocalStateArgs&);
    ~ThreadLocalState() override;
    /** sequence data required to use */
    OhosSeqData seq_data_;
    /** fix buffer used for append to hitrace name */
    FixBuffer fix_buffer_;
    /** actual buffer to write to */
    char buffer_[HitraceAdapter::kLimit];
  };

  /** Helper class used to parse track event and output to hitrace */
  class SeqTrackProcessor;

  /**
   * in `tracing_muxer_impl.cc TracingMuxerImpl::RegisterInterceptor` only two
   * special names are allowed, fake the name to pass register process
   */
  static constexpr const char* kInterceptorName = "etwexport";

  /**
   * there are 3 hitrace level, edit it at compile time to enable / disable
   * feature
   */
  static constexpr enum HitraceLevel : int32_t {
    /** just print the name categories without any debug annotation */
    kNoAnnotation = 0,
    /** name categories and debug annotation, without handle legacy events */
    kNoLegacyEvent = 1,
    /** print all, include name categories, debug annotation, legacy events */
    KAll = 2,
  } kHitraceLevel = HitraceLevel::KAll;

  /**
   * actual handler for each trace packet
   */
  static void OnTracePacket(InterceptorContext context);
  /** register the event to trace */
  static void Register();
  /** get current hitrace adapter */
  static HitraceAdapter& GetHitraceAdapter();

  OhosHiTraceInterceptor();

  /** for unit test */
  static void SetHitraceAdapterForTesting(HitraceAdapter& adapter);

 private:
  /** hitrace sink */
  static HitraceAdapter* hitrace_adapter_;
};
}  // namespace base::trace_event::ohos

#endif  // BASE_TRACE_EVENT_OHOS_HITRACE_INTERCEPTOR_H_

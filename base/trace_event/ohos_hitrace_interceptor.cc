// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/ohos_hitrace_interceptor.h"

#include <string_view>
#include <type_traits>

#include "base/compiler_specific.h"
#include "base/trace_event/ohos_fix_buffer.h"
#include "base/trace_event/ohos_hitrace_adapter.h"
#include "perfetto/protozero/field.h"
#include "perfetto/protozero/proto_decoder.h"
#include "protos/perfetto/trace/interned_data/interned_data.pbzero.h"
#include "protos/perfetto/trace/track_event/debug_annotation.pbzero.h"
#include "protos/perfetto/trace/track_event/track_event.pbzero.h"
#include "third_party/perfetto/protos/perfetto/common/interceptor_descriptor.gen.h"
#include "third_party/perfetto/protos/perfetto/trace/trace_packet.pbzero.h"

namespace base::trace_event::ohos {
namespace pbzero = perfetto::protos::pbzero;

namespace {
/**
 * convert `protozero::ConstChars` into `std::string_view`
 * @param const_chars the protobuf char span
 * @returns return the string_view can be used to append to FixBuf
 */
std::string_view ToStringView(
    const protozero::ConstChars& const_chars) noexcept {
  return std::string_view{const_chars.data, const_chars.size};
}

/**
 * helper to append comma for second time and later
 */
struct CommaHelper {
 public:
  static constexpr std::string_view kDefaultComma{","};
  std::string_view comma_str;
  bool is_first = true;

  explicit CommaHelper(std::string_view comma_str = kDefaultComma) noexcept
      : comma_str(comma_str) {}
};

/**
 * @param fix_buffer the fix_buffer input
 */
FixBuffer& operator<<(FixBuffer& fix_buffer,
                      CommaHelper& comma_helper) noexcept {
  if (comma_helper.is_first) [[unlikely]] {
    comma_helper.is_first = false;
    return fix_buffer;
  } else {
    return fix_buffer << comma_helper.comma_str;
  }
}

/**
 * helper class to convert `protozero::RepeatedFieldIterator<T>` into ranged for
 * iteration. Can extra take a `FixBuffer` pointer to stop if buffer is full.
 */
template <typename T>
struct IterProto final {
 public:
  explicit IterProto(protozero::RepeatedFieldIterator<T> iter,
                     FixBuffer* fix_buffer = nullptr)
      : iter_(iter), fix_buffer_(fix_buffer) {}
  struct IterEnd {};

  struct Iter {
    explicit Iter(protozero::RepeatedFieldIterator<T> iter,
                  FixBuffer* fix_buffer)
        : iter_(iter), fix_buffer_(fix_buffer) {}

    /**
     * ranged based for loop not require `begin()` and `end()` has same type,
     */
    ALWAYS_INLINE bool operator==(const IterEnd&) const { return !IsValid(); }

    /**
     * if iter_ reach end or `fix_buffer_` is full, break the loop
     */
    ALWAYS_INLINE bool IsValid() const { return iter_ && IsBufferValid(); }

    /**
     * fix_buffer_ is nullptr is always valid,
     * or check if not `IsFull()`
     */
    ALWAYS_INLINE bool IsBufferValid() const {
      return !fix_buffer_ || !fix_buffer_->IsFull();
    }

    Iter& operator++() {
      if (iter_) {
        ++iter_;
      }
      return *this;
    }

    T operator*() const { return *iter_; }
    protozero::RepeatedFieldIterator<T> iter_;
    FixBuffer* fix_buffer_;
  };

  Iter begin() const { return Iter(iter_, fix_buffer_); }

  IterEnd end() const { return IterEnd{}; }

  protozero::RepeatedFieldIterator<T> iter_;
  FixBuffer* fix_buffer_;
};

/**
 * wrapper for IterProto, if the IterProto iterate over `ConstBytes`,
 * use TypedIterProto with the decoder type to iter and create decoder facade in
 * the same type
 */
template <typename T>
struct TypedIterProto {
 public:
  static_assert(std::is_constructible_v<T, const protozero::ConstBytes&>,
                "T must be a pbzero SomeType::Decoder class");
  static_assert(std::is_base_of_v<protozero::TypedProtoDecoderBase, T>,
                "T must be a pbzero SomeType::Decoder class");
  explicit TypedIterProto(
      protozero::RepeatedFieldIterator<protozero::ConstBytes> iter,
      FixBuffer* fix_buffer = nullptr)
      : iter_proto_(iter, fix_buffer) {}

  // note struct Iter is not final, we override the `operator*` to directly
  // return the Decoder instance
  struct Iter : public IterProto<protozero::ConstBytes>::Iter {
   public:
    T operator*() const { return T(*iter_); }
  };

  TypedIterProto::Iter begin() const {
    return TypedIterProto::Iter(iter_proto_.begin());
  }

  // reuse the `end` and `operator==`
  IterProto<protozero::ConstBytes>::IterEnd end() const {
    return iter_proto_.end();
  }

  IterProto<protozero::ConstBytes> iter_proto_;
};

/**
 * helper to query the string from iid, or output `"[iid: <iid>]"` if not found
 */
struct QueryIId {
 public:
  explicit QueryIId(const OhosSeqData::IIdMap& map, uint64_t iid);
  const OhosSeqData::IIdMap& map_;
  uint64_t iid_;
};

QueryIId::QueryIId(const OhosSeqData::IIdMap& map, uint64_t iid)
    : map_(map), iid_(iid) {}

FixBuffer& operator<<(FixBuffer& fix_buffer, QueryIId queryIId) {
  OhosSeqData::IIdMap::const_iterator iter = queryIId.map_.find(queryIId.iid_);
  if (iter != queryIId.map_.cend()) {
    return fix_buffer << iter->second;
  } else {
    return fix_buffer << "[iid:" << queryIId.iid_ << "]";
  }
}
}  // namespace

OhosHiTraceInterceptor::ThreadLocalState::ThreadLocalState(
    ThreadLocalStateArgs&)
    : fix_buffer_(buffer_) {}

OhosHiTraceInterceptor::ThreadLocalState::~ThreadLocalState() = default;

/**
 * SeqTrackProcessor is response for process tracks
 */
class OhosHiTraceInterceptor::SeqTrackProcessor final {
 public:
  /** interceptor per invoke of OnTraceEvent */
  InterceptorContext& context_;
  /** thread local state */
  ThreadLocalState& tls_;

  /** Delegate should be only local variable inside `OnTracePacket` */
  explicit SeqTrackProcessor(InterceptorContext& context,
                             ThreadLocalState& tls) noexcept;
  ~SeqTrackProcessor() = default;

  /** entry point to process data */
  void ProcessData();

 private:
  /**
   * update the interned data or clear interned data
   */
  void UpdateSeqData(const pbzero::TracePacket::Decoder& trace_packet);

  /**
   * handle normal track event, that is handle slice begin and slice end
   */
  void HandleTrackEvent(const pbzero::TrackEvent::Decoder& track_event);

  /** print event name */
  void PrintName(const pbzero::TrackEvent::Decoder& track_event);

  /** print formatted categories */
  void PrintCategories(const pbzero::TrackEvent::Decoder& track_event);

  /**
   * handle the legacy event, can be disabled by `kHitraceLevel`,
   * that is unspecified event with `legacy_event` present,
   * can convert to async begin, async end, counter value
   */
  void HandleLegacyEvent(const pbzero::TrackEvent::Decoder& track_event);

  /**
   * get the track id from legacy_event, first get `local_id` otherwise get
   * `unscoped_id`
   */
  int32_t GetLegacyAsyncId(
      const pbzero::TrackEvent::LegacyEvent::Decoder& legacy_event);

  /** print the legacy event with special key */
  void PrintLegacyEvent(const pbzero::TrackEvent::Decoder& track_event);

  /**
   * print single formated debug annotation, is recursive for `dict_entries` or
   * `array_values` */
  void PrintDebugAnnotation(const pbzero::DebugAnnotation::Decoder& annotation);
  /**
   * print debug annotation for track_event, can be disabled by `kHitraceLevel`
   */
  void PrintDebugAnnotations(const pbzero::TrackEvent::Decoder& track_event);
  /**
   * special treatment for nested value in debug annotation
   */
  void PrintNestedValues(
      const pbzero::DebugAnnotation::NestedValue::Decoder& nested_value);
  /**
   * handle basic debug annotation, return `true` if already printed
   */
  bool PrintDebugAnnotationBasic(
      const pbzero::DebugAnnotation::Decoder& annotation);
};

OhosHiTraceInterceptor::SeqTrackProcessor::SeqTrackProcessor(
    InterceptorContext& context,
    ThreadLocalState& tls) noexcept
    : context_(context), tls_(tls) {}

void OhosHiTraceInterceptor::SeqTrackProcessor::ProcessData() {
  pbzero::TracePacket::Decoder packet(context_.packet_data.data,
                                      context_.packet_data.size);
  UpdateSeqData(packet);
  if (!packet.has_track_event()) {
    return;
  }
  pbzero::TrackEvent::Decoder track_event(packet.track_event());
  HandleTrackEvent(track_event);
  HandleLegacyEvent(track_event);
}

void OhosHiTraceInterceptor::SeqTrackProcessor::UpdateSeqData(
    const pbzero::TracePacket::Decoder& trace_packet) {
  OhosSeqData& seq_data = tls_.seq_data_;
  if (trace_packet.sequence_flags() &
      pbzero::TracePacket::SEQ_INCREMENTAL_STATE_CLEARED) [[unlikely]] {
    seq_data.event_names_map_.clear();
    seq_data.event_categories_map_.clear();
    seq_data.debug_annotation_names_map_.clear();
    seq_data.proto_type_map_.clear();
  }

  if (trace_packet.has_interned_data()) [[unlikely]] {
    pbzero::InternedData::Decoder interned_data{trace_packet.interned_data()};
    for (pbzero::EventName::Decoder entry :
         TypedIterProto<pbzero::EventName::Decoder>(
             interned_data.event_names())) {
      seq_data.event_names_map_.try_emplace(entry.iid(),
                                            entry.name().ToStdString());
    }
    for (pbzero::EventCategory::Decoder entry :
         TypedIterProto<pbzero::EventCategory::Decoder>(
             interned_data.event_categories())) {
      seq_data.event_categories_map_.try_emplace(entry.iid(),
                                                 entry.name().ToStdString());
    }
    for (pbzero::DebugAnnotationName::Decoder entry :
         TypedIterProto<pbzero::DebugAnnotationName::Decoder>(
             interned_data.debug_annotation_names())) {
      seq_data.debug_annotation_names_map_.try_emplace(
          entry.iid(), entry.name().ToStdString());
    }
    for (pbzero::DebugAnnotationValueTypeName::Decoder entry :
         TypedIterProto<pbzero::DebugAnnotationValueTypeName::Decoder>(
             interned_data.debug_annotation_value_type_names())) {
      seq_data.proto_type_map_.try_emplace(entry.iid(),
                                           entry.name().ToStdString());
    }
  }
}

void OhosHiTraceInterceptor::SeqTrackProcessor::HandleTrackEvent(
    const pbzero::TrackEvent::Decoder& track_event) {
  HitraceAdapter& adapter = OhosHiTraceInterceptor::GetHitraceAdapter();
  FixBuffer& fix_buffer = tls_.fix_buffer_;
  fix_buffer.Clear();
  if (track_event.type() == pbzero::TrackEvent::TYPE_SLICE_END) {
    // slice end just return
    adapter.EndSyncTrace();
    return;
  }

  // first print the name
  PrintName(track_event);
  // then output the full categories
  PrintCategories(track_event);
  // print other metadatas, like uuid, flow
  if (track_event.has_track_uuid()) [[unlikely]] {
    fix_buffer << ",uuid:" << track_event.track_uuid();
  }
  if (track_event.has_flow_ids()) [[unlikely]] {
    for (uint64_t flow_id : IterProto(track_event.flow_ids(), &fix_buffer)) {
      fix_buffer << ",flow:" << flow_id;
    }
  }
  if (track_event.has_terminating_flow_ids()) [[unlikely]] {
    for (uint64_t flow_id :
         IterProto(track_event.terminating_flow_ids(), &fix_buffer)) {
      fix_buffer << ",termflow:" << flow_id;
    }
  }
  PrintDebugAnnotations(track_event);
  if (track_event.type() == pbzero::TrackEvent::TYPE_SLICE_BEGIN) {
    // start the hitrace sync trace
    adapter.StartSyncTrace(fix_buffer.CStr());
  } else if (track_event.type() == pbzero::TrackEvent::TYPE_INSTANT) {
    // simulate the instance by a start and end extra with an instant suffix
    fix_buffer << ", instant";
    adapter.StartSyncTrace(fix_buffer.CStr());
    adapter.EndSyncTrace();
  }
}

void OhosHiTraceInterceptor::SeqTrackProcessor::PrintName(
    const pbzero::TrackEvent::Decoder& track_event) {
  if (track_event.has_name_iid()) {
    tls_.fix_buffer_ << QueryIId(tls_.seq_data_.event_names_map_,
                                 track_event.name_iid());
  } else if (track_event.has_name()) {
    tls_.fix_buffer_ << ToStringView(track_event.name());
  }
}

void OhosHiTraceInterceptor::SeqTrackProcessor::PrintCategories(
    const pbzero::TrackEvent::Decoder& track_event) {
  tls_.fix_buffer_ << "[";
  // auto add comma for non first
  CommaHelper auto_comma{CommaHelper::kDefaultComma};
  // note the `categories` and `category_iids` are exclussive, only one can be
  // not empty
  for (uint64_t category_iid :
       IterProto(track_event.category_iids(), &tls_.fix_buffer_)) {
    tls_.fix_buffer_ << auto_comma
                     << QueryIId(tls_.seq_data_.event_categories_map_,
                                 category_iid);
  }
  for (protozero::ConstChars str :
       IterProto(track_event.categories(), &tls_.fix_buffer_)) {
    tls_.fix_buffer_ << auto_comma << ToStringView(str);
  }
  tls_.fix_buffer_ << "]";
}

enum class AsyncType : int32_t {
  kNone,
  kBegin = 1,
  kEnd = 2,
  kInstant = 3,
  kCounter = 4,
};

namespace {
// Event phases copied from
// `third_party/perfetto/include/perfetto/tracing/internal/track_event_legacy.h`
static constexpr char TRACE_EVENT_PHASE_ASYNC_BEGIN = 'S';
static constexpr char TRACE_EVENT_PHASE_ASYNC_STEP_INTO = 'T';
static constexpr char TRACE_EVENT_PHASE_ASYNC_STEP_PAST = 'p';
static constexpr char TRACE_EVENT_PHASE_ASYNC_END = 'F';
static constexpr char TRACE_EVENT_PHASE_NESTABLE_ASYNC_BEGIN = 'b';
static constexpr char TRACE_EVENT_PHASE_NESTABLE_ASYNC_END = 'e';
static constexpr char TRACE_EVENT_PHASE_NESTABLE_ASYNC_INSTANT = 'n';
static constexpr char TRACE_EVENT_PHASE_COUNTER = 'C';
static constexpr char TRACE_EVENT_PHASE_MARK = 'R';

/** get phase type, 9 supported phase map to 3 types */
AsyncType GetPhaseType(int32_t phase) {
  switch (phase) {
    case TRACE_EVENT_PHASE_ASYNC_BEGIN:
    case TRACE_EVENT_PHASE_NESTABLE_ASYNC_BEGIN:
      return AsyncType::kBegin;
    case TRACE_EVENT_PHASE_ASYNC_END:
    case TRACE_EVENT_PHASE_NESTABLE_ASYNC_END:
      return AsyncType::kEnd;
    case TRACE_EVENT_PHASE_ASYNC_STEP_INTO:
    case TRACE_EVENT_PHASE_NESTABLE_ASYNC_INSTANT:
    case TRACE_EVENT_PHASE_MARK:
      return AsyncType::kInstant;
    case TRACE_EVENT_PHASE_COUNTER:
      return AsyncType::kCounter;
    default:
      return AsyncType::kNone;
  }
}
}  // namespace

void OhosHiTraceInterceptor::SeqTrackProcessor::HandleLegacyEvent(
    const pbzero::TrackEvent::Decoder& track_event) {
  // compile time skip handle legacy event
  if constexpr (OhosHiTraceInterceptor::kHitraceLevel <=
                OhosHiTraceInterceptor::HitraceLevel::kNoLegacyEvent) {
    return;
  }
  if (track_event.type() != pbzero::TrackEvent::TYPE_UNSPECIFIED) [[likely]] {
    return;
  }
  if (!track_event.has_legacy_event()) [[unlikely]] {
    return;
  }
  // handle legacy event by different case
  pbzero::TrackEvent::LegacyEvent::Decoder legacy_event{
      track_event.legacy_event()};
  int32_t phase = legacy_event.phase();
  AsyncType type = GetPhaseType(phase);
  if (type == AsyncType::kNone) [[unlikely]] {
    return;
  }
  HitraceAdapter& adapter = OhosHiTraceInterceptor::GetHitraceAdapter();
  FixBuffer& fix_buffer_ = tls_.fix_buffer_;
  fix_buffer_.Clear();
  if (type == AsyncType::kBegin || type == AsyncType::kEnd ||
      type == AsyncType::kInstant) {
    // Note format is `[category]name:I/S`, and I for instant, S for slice begin
    // end
    PrintCategories(track_event);
    PrintName(track_event);
    if (type == AsyncType::kInstant) {
      fix_buffer_ << ":I";
    } else {
      fix_buffer_ << ":S";
    }
    int32_t id = GetLegacyAsyncId(legacy_event);
    if (type == AsyncType::kBegin) {
      adapter.StartAsyncTrace(fix_buffer_.CStr(), id);
    } else if (type == AsyncType::kEnd) {
      adapter.EndAsyncTrace(fix_buffer_.CStr(), id);
    } else if (type == AsyncType::kInstant) {
      adapter.StartAsyncTrace(fix_buffer_.CStr(), id);
      adapter.EndAsyncTrace(fix_buffer_.CStr(), id);
    }
  } else if (type == AsyncType::kCounter &&
             track_event.has_debug_annotations()) {
    PrintCategories(track_event);
    PrintName(track_event);
    fix_buffer_ << ":";
    size_t trim_len = fix_buffer_.Len();
    // counter will traverse debug annotations to get result
    for (pbzero::DebugAnnotation::Decoder debug_annotation :
         TypedIterProto<pbzero::DebugAnnotation::Decoder>(
             track_event.debug_annotations())) {
      // keep common part `[category]name:subname:`
      fix_buffer_.ShrinkTo(trim_len);
      if (debug_annotation.has_name_iid()) {
        fix_buffer_ << QueryIId(tls_.seq_data_.debug_annotation_names_map_,
                                debug_annotation.name_iid());
      } else if (debug_annotation.has_name()) {
        fix_buffer_ << ToStringView(debug_annotation.name());
      }
      if (debug_annotation.has_int_value()) [[likely]] {
        // format is `[category]name:subname:I` and has integer count
        fix_buffer_ << ":I";
        adapter.CounterTrace(fix_buffer_.CStr(), debug_annotation.int_value());
      } else if (debug_annotation.has_double_value()) {
        // format is `[category]name:subname:D*10000` and double value * 10000
        // to convert to integer since hitrace v1 only support int64_t counter
        fix_buffer_ << ":D*10000";
        int64_t double_to_int =
            static_cast<int64_t>(debug_annotation.double_value() * 10000);
        adapter.CounterTrace(fix_buffer_.CStr(), double_to_int);
      }
    }
  }
}

int32_t OhosHiTraceInterceptor::SeqTrackProcessor::GetLegacyAsyncId(
    const pbzero::TrackEvent::LegacyEvent::Decoder& legacy_event) {
  uint64_t result = 0;
  if (legacy_event.has_local_id() && legacy_event.local_id() != 0) {
    result = legacy_event.local_id();
  }
  if (legacy_event.has_unscoped_id() && legacy_event.unscoped_id() != 0) {
    result = legacy_event.unscoped_id();
  }
  return static_cast<int32_t>(result);
}

void OhosHiTraceInterceptor::SeqTrackProcessor::PrintLegacyEvent(
    const pbzero::TrackEvent::Decoder& track_event) {
  if (!track_event.has_legacy_event()) {
    return;
  }
  FixBuffer& fix_buffer_ = tls_.fix_buffer_;
  fix_buffer_ << ",[legacy_event]:{";
  CommaHelper auto_comma{};
  pbzero::TrackEvent::LegacyEvent::Decoder legacy_event{
      track_event.legacy_event()};
  fix_buffer_ << auto_comma << "phase:" << legacy_event.phase();
  if (legacy_event.has_duration_us()) {
    fix_buffer_ << auto_comma << "duration_us:" << legacy_event.duration_us();
  }
  if (legacy_event.has_thread_duration_us()) {
    fix_buffer_ << auto_comma
                << "thread_duration_us:" << legacy_event.thread_duration_us();
  }
  if (legacy_event.has_thread_instruction_delta()) {
    fix_buffer_ << auto_comma << "thread_instruction_delta:"
                << legacy_event.thread_instruction_delta();
  }
  if (legacy_event.has_unscoped_id()) {
    fix_buffer_ << auto_comma << "unscoped_id:" << legacy_event.unscoped_id();
  }
  if (legacy_event.has_local_id()) {
    fix_buffer_ << auto_comma << "local_id:" << legacy_event.local_id();
  }
  if (legacy_event.has_global_id()) {
    fix_buffer_ << auto_comma << "global_id:" << legacy_event.global_id();
  }
  if (legacy_event.has_id_scope()) {
    fix_buffer_ << auto_comma
                << "id_scope:" << ToStringView(legacy_event.id_scope());
  }
  if (legacy_event.has_use_async_tts()) {
    fix_buffer_ << auto_comma << "use_async_tts:"
                << FixBuffer::ToBool(legacy_event.use_async_tts());
  }
  if (legacy_event.has_bind_id()) {
    fix_buffer_ << auto_comma << "bind_id:" << legacy_event.bind_id();
  }
  if (legacy_event.has_bind_to_enclosing()) {
    fix_buffer_ << auto_comma << "bind_to_enclosing:"
                << FixBuffer::ToBool(legacy_event.bind_to_enclosing());
  }
  if (legacy_event.has_flow_direction()) {
    fix_buffer_ << auto_comma
                << "flow_direction:" << legacy_event.flow_direction();
  }
  if (legacy_event.has_instant_event_scope()) {
    fix_buffer_ << auto_comma
                << "instant_event_scope:" << legacy_event.instant_event_scope();
  }
  if (legacy_event.has_pid_override()) {
    fix_buffer_ << auto_comma << "pid_override:" << legacy_event.pid_override();
  }
  if (legacy_event.has_tid_override()) {
    fix_buffer_ << auto_comma << "tid_override:" << legacy_event.tid_override();
  }
  fix_buffer_ << "}";
}

void OhosHiTraceInterceptor::SeqTrackProcessor::PrintDebugAnnotation(
    const pbzero::DebugAnnotation::Decoder& annotation) {
  FixBuffer& fix_buffer_ = tls_.fix_buffer_;
  // if has name we print the `<name>:`
  if (annotation.has_name_iid()) {
    fix_buffer_ << QueryIId(tls_.seq_data_.debug_annotation_names_map_,
                            annotation.name_iid())
                << ":";
  } else if (annotation.has_name()) {
    fix_buffer_ << ToStringView(annotation.name()) << ":";
  }
  // see
  // `third_party/perfetto/protos/perfetto/trace/track_event/debug_annotation.proto`
  if (PrintDebugAnnotationBasic(annotation)) {
    // means the basic is printed
    return;
  } else if (annotation.has_dict_entries()) {
    CommaHelper auto_comma{};
    fix_buffer_ << "{";
    // dict_entries is just debug annotations with name
    for (pbzero::DebugAnnotation::Decoder entry :
         TypedIterProto<pbzero::DebugAnnotation::Decoder>(
             annotation.dict_entries(), &tls_.fix_buffer_)) {
      fix_buffer_ << auto_comma;
      PrintDebugAnnotation(entry);
    }
    fix_buffer_ << "}";
  } else if (annotation.has_array_values()) {
    CommaHelper auto_comma{};
    fix_buffer_ << "[";
    // array_values is just debug annotations without name
    for (pbzero::DebugAnnotation::Decoder no_name_entry :
         TypedIterProto<pbzero::DebugAnnotation::Decoder>(
             annotation.array_values(), &tls_.fix_buffer_)) {
      fix_buffer_ << auto_comma;
      PrintDebugAnnotation(no_name_entry);
    }
    fix_buffer_ << "]";
  } else if (annotation.has_proto_value()) {
    // currently for performance don't print proto value, just print the name
    fix_buffer_ << "[proto value";
    // extra print the proto name
    if (annotation.has_proto_type_name_iid()) {
      fix_buffer_ << ":"
                  << QueryIId(tls_.seq_data_.proto_type_map_,
                              annotation.proto_type_name_iid());
    } else if (annotation.has_proto_type_name()) {
      fix_buffer_ << ":" << ToStringView(annotation.proto_type_name());
    }
    fix_buffer_ << "]";
  } else if (annotation.has_nested_value()) {
    pbzero::DebugAnnotation::NestedValue::Decoder nested_value{
        annotation.nested_value()};
    PrintNestedValues(nested_value);
  } else {
    // other type we just print an unknown type
    fix_buffer_ << "[unknown type]";
  }
}

void OhosHiTraceInterceptor::SeqTrackProcessor::PrintDebugAnnotations(
    const pbzero::TrackEvent::Decoder& track_event) {
  // compile time skip print annotation
  if constexpr (OhosHiTraceInterceptor::kHitraceLevel <=
                OhosHiTraceInterceptor::HitraceLevel::kNoAnnotation) {
    return;
  }
  if (!track_event.has_debug_annotations()) [[likely]] {
    return;
  }
  CommaHelper auto_comma{};
  tls_.fix_buffer_ << ",{";
  for (pbzero::DebugAnnotation::Decoder annotation :
       TypedIterProto<pbzero::DebugAnnotation::Decoder>(
           track_event.debug_annotations())) {
    tls_.fix_buffer_ << auto_comma;
    PrintDebugAnnotation(annotation);
  }
  tls_.fix_buffer_ << "}";
}

void OhosHiTraceInterceptor::SeqTrackProcessor::PrintNestedValues(
    const pbzero::DebugAnnotation::NestedValue::Decoder& nested_value) {
  // has three types: `UNSPECIFIED` for leaf, DICT for dict, `ARRAY` for array
  if (nested_value.nested_type() ==
      pbzero::DebugAnnotation::NestedValue::UNSPECIFIED) {
    if (nested_value.has_bool_value()) {
      tls_.fix_buffer_ << FixBuffer::ToBool(nested_value.bool_value());
    } else if (nested_value.has_int_value()) {
      tls_.fix_buffer_ << nested_value.int_value();
    } else if (nested_value.has_double_value()) {
      tls_.fix_buffer_ << nested_value.double_value();
    } else if (nested_value.has_string_value()) {
      tls_.fix_buffer_ << "\"" << ToStringView(nested_value.string_value())
                       << "\"";
    } else {
      tls_.fix_buffer_ << "[unknown nested value]";
    }
  } else if (nested_value.nested_type() ==
             pbzero::DebugAnnotation::NestedValue::DICT) {
    // it is a zip of both key and value
    CommaHelper auto_comma{};
    tls_.fix_buffer_ << "{";
    protozero::RepeatedFieldIterator<protozero::ConstChars> key_iter =
        nested_value.dict_keys();
    protozero::RepeatedFieldIterator<protozero::ConstBytes> value_iter =
        nested_value.dict_values();
    for (; !tls_.fix_buffer_.IsFull() && key_iter && value_iter;
         ++key_iter, ++value_iter) {
      tls_.fix_buffer_ << auto_comma << ToStringView(*key_iter) << ":";
      pbzero::DebugAnnotation::NestedValue::Decoder inner_value{*value_iter};
      PrintNestedValues(inner_value);
    }
    tls_.fix_buffer_ << "}";
  } else if (nested_value.nested_type() ==
             pbzero::DebugAnnotation::NestedValue::ARRAY) {
    CommaHelper auto_comma{};
    tls_.fix_buffer_ << "[";
    for (pbzero::DebugAnnotation::NestedValue::Decoder inner_value :
         TypedIterProto<pbzero::DebugAnnotation::NestedValue::Decoder>(
             nested_value.array_values(), &tls_.fix_buffer_)) {
      tls_.fix_buffer_ << auto_comma;
      PrintNestedValues(inner_value);
    }
    tls_.fix_buffer_ << "]";
  } else {
    tls_.fix_buffer_ << "[unknown nested type]";
  }
}

bool OhosHiTraceInterceptor::SeqTrackProcessor::PrintDebugAnnotationBasic(
    const pbzero::DebugAnnotation::Decoder& annotation) {
  FixBuffer& fix_buffer_ = tls_.fix_buffer_;
  if (annotation.has_bool_value()) {
    fix_buffer_ << FixBuffer::ToBool(annotation.bool_value());
  } else if (annotation.has_uint_value()) {
    fix_buffer_ << annotation.uint_value();
  } else if (annotation.has_int_value()) {
    fix_buffer_ << annotation.int_value();
  } else if (annotation.has_double_value()) {
    fix_buffer_ << annotation.double_value();
  } else if (annotation.has_string_value()) {
    fix_buffer_ << "\"" << ToStringView(annotation.string_value()) << "\"";
  } else if (annotation.has_string_value_iid()) {
    fix_buffer_ << "\""
                << QueryIId(tls_.seq_data_.debug_annotation_names_map_,
                            annotation.string_value_iid())
                << "\"";
  } else if (annotation.has_pointer_value()) {
    fix_buffer_ << FixBuffer::Ptr(
        reinterpret_cast<const void*>(annotation.pointer_value()));
  } else if (annotation.has_legacy_json_value()) {
    fix_buffer_ << ToStringView(annotation.legacy_json_value());
  } else {
    // means is not basic value, need recursion and futher handle
    return false;
  }
  return true;
}

HitraceAdapter& OhosHiTraceInterceptor::GetHitraceAdapter() {
  if (!OhosHiTraceInterceptor::hitrace_adapter_) [[unlikely]] {
    return GetSystemSingleton();
  } else {
    return *OhosHiTraceInterceptor::hitrace_adapter_;
  }
}

void OhosHiTraceInterceptor::OnTracePacket(InterceptorContext context) {
  OhosHiTraceInterceptor::ThreadLocalState& tls = context.GetThreadLocalState();
  SeqTrackProcessor processor(context, tls);
  // do the actual process data
  processor.ProcessData();
}

void OhosHiTraceInterceptor::Register() {
  perfetto::protos::gen::InterceptorDescriptor desc;
  // fake the name to pass register process
  desc.set_name(kInterceptorName);
  // if the adapter is empty, init it
  if (!OhosHiTraceInterceptor::hitrace_adapter_) {
    OhosHiTraceInterceptor::hitrace_adapter_ = &(GetSystemSingleton());
  }
  Interceptor<OhosHiTraceInterceptor>::Register(desc);
}

OhosHiTraceInterceptor::OhosHiTraceInterceptor() = default;

void OhosHiTraceInterceptor::SetHitraceAdapterForTesting(
    HitraceAdapter& adapter) {
  hitrace_adapter_ = &adapter;
}

HitraceAdapter* OhosHiTraceInterceptor::hitrace_adapter_ = nullptr;
}  // namespace base::trace_event::ohos

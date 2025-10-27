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

#ifndef TRACE_EVENT_COMMON_OH_H
#define TRACE_EVENT_COMMON_OH_H

#include "base/trace_event/trace_arguments.h"
#include "base/trace_event/trace_event_ohos.h"

#ifdef TRACE_EVENT0
#undef TRACE_EVENT0
#endif
#define TRACE_EVENT0(category_group, name)  \
  BYTRACE_SCOPED_INIT(category_group);      \
  do {                                      \
    if (IsCategoryEnable(category_group)) { \
      BYTRACE_SCOPED_TRACE_EVENT(name);     \
    }                                       \
  } while (0);                              \
  INTERNAL_TRACE_EVENT_ADD_SCOPED(category_group, name)

#ifdef TRACE_EVENT1
#undef TRACE_EVENT1
#endif
#define TRACE_EVENT1(category_group, name, arg1_name, arg1_val) \
  BYTRACE_SCOPED_INIT(category_group);                          \
  do {                                                          \
    if (IsCategoryEnable(category_group)) {                     \
      BYTRACE_SCOPED_TRACE_EVENT(                               \
          GetStringFromArgs(name, arg1_name, arg1_val));        \
    }                                                           \
  } while (0);                                                  \
  INTERNAL_TRACE_EVENT_ADD_SCOPED(                              \
      category_group, name, arg1_name,                          \
      ::perfetto::internal::PossiblyNull(arg1_val))

#ifdef TRACE_EVENT2
#undef TRACE_EVENT2
#endif
#define TRACE_EVENT2(category_group, name, arg1_name, arg1_val, arg2_name,    \
                     arg2_val)                                                \
  BYTRACE_SCOPED_INIT(category_group);                                        \
  do {                                                                        \
    if (IsCategoryEnable(category_group)) {                                   \
      BYTRACE_SCOPED_TRACE_EVENT(                                             \
          GetStringFromArgs(name, arg1_name, arg1_val, arg2_name, arg2_val)); \
    }                                                                         \
  } while (0);                                                                \
  INTERNAL_TRACE_EVENT_ADD_SCOPED(                                            \
      category_group, name, arg1_name,                                        \
      ::perfetto::internal::PossiblyNull(arg1_val), arg2_name,                \
      ::perfetto::internal::PossiblyNull(arg2_val))

#ifdef TRACE_EVENT_WITH_FLOW0
#undef TRACE_EVENT_WITH_FLOW0
#endif
#define TRACE_EVENT_WITH_FLOW0(category_group, name, bind_id, flow_flags)  \
  BYTRACE_SCOPED_INIT(category_group);                                     \
  do {                                                                     \
    if (IsCategoryEnable(category_group)) {                                \
      (void)(bind_id);                                                     \
      (void)(flow_flags);                                                  \
      BYTRACE_SCOPED_TRACE_EVENT(name);                                    \
    }                                                                      \
  } while (0);                                                             \
  INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW(category_group, name, bind_id, \
                                            flow_flags)
#ifdef TRACE_EVENT_WITH_FLOW1
#undef TRACE_EVENT_WITH_FLOW1
#endif
#define TRACE_EVENT_WITH_FLOW1(category_group, name, bind_id, flow_flags, \
                               arg1_name, arg1_val)                       \
  BYTRACE_SCOPED_INIT(category_group);                                    \
  do {                                                                    \
    if (IsCategoryEnable(category_group)) {                               \
      (void)(bind_id);                                                    \
      (void)(flow_flags);                                                 \
      BYTRACE_SCOPED_TRACE_EVENT(                                         \
          GetStringFromArgs(name, arg1_name, arg1_val));                  \
    }                                                                     \
  } while (0);                                                            \
  INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW(                              \
      category_group, name, bind_id, flow_flags, arg1_name,               \
      ::perfetto::internal::PossiblyNull(arg1_val))
#ifdef TRACE_EVENT_WITH_FLOW2
#undef TRACE_EVENT_WITH_FLOW2
#endif
#define TRACE_EVENT_WITH_FLOW2(category_group, name, bind_id, flow_flags,     \
                               arg1_name, arg1_val, arg2_name, arg2_val)      \
  BYTRACE_SCOPED_INIT(category_group);                                        \
  do {                                                                        \
    if (IsCategoryEnable(category_group)) {                                   \
      (void)(bind_id);                                                        \
      (void)(flow_flags);                                                     \
      BYTRACE_SCOPED_TRACE_EVENT(                                             \
          GetStringFromArgs(name, arg1_name, arg1_val, arg2_name, arg2_val)); \
    }                                                                         \
  } while (0);                                                                \
  INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW(                                  \
      category_group, name, bind_id, flow_flags, arg1_name,                   \
      ::perfetto::internal::PossiblyNull(arg1_val), arg2_name,                \
      ::perfetto::internal::PossiblyNull(arg2_val))

// Begin events.
#ifdef TRACE_EVENT_BEGIN0
#undef TRACE_EVENT_BEGIN0
#endif
#define TRACE_EVENT_BEGIN0(category_group, name)                          \
  do {                                                                    \
    if (IsCategoryEnable(category_group)) {                               \
      StartBytrace(name);                                                 \
    }                                                                     \
  } while (0);                                                            \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_BEGIN, category_group, name, \
                           TRACE_EVENT_FLAG_NONE)
#ifdef OHOS_TRACE_BEGIN
#undef OHOS_TRACE_BEGIN
#endif
#define OHOS_TRACE_BEGIN(category_group, name)                          \
  do {                                                                    \
    if (IsCategoryEnable(category_group)) {                               \
      StartBytrace(name);                                                 \
    }                                                                     \
  } while (0)

#ifdef TRACE_EVENT_BEGIN1
#undef TRACE_EVENT_BEGIN1
#endif
#define TRACE_EVENT_BEGIN1(category_group, name, arg1_name, arg1_val)     \
  do {                                                                    \
    if (IsCategoryEnable(category_group)) {                               \
      StartBytrace(GetStringFromArgs(name, arg1_name, arg1_val));         \
    }                                                                     \
  } while (0);                                                            \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_BEGIN, category_group, name, \
                           TRACE_EVENT_FLAG_NONE, arg1_name,              \
                           ::perfetto::internal::PossiblyNull(arg1_val))
#ifdef TRACE_EVENT_BEGIN2
#undef TRACE_EVENT_BEGIN2
#endif
#define TRACE_EVENT_BEGIN2(category_group, name, arg1_name, arg1_val,         \
                           arg2_name, arg2_val)                               \
  do {                                                                        \
    if (IsCategoryEnable(category_group)) {                                   \
      StartBytrace(                                                           \
          GetStringFromArgs(name, arg1_name, arg1_val, arg2_name, arg2_val)); \
    }                                                                         \
  } while (0);                                                                \
  INTERNAL_TRACE_EVENT_ADD(                                                   \
      TRACE_EVENT_PHASE_BEGIN, category_group, name, TRACE_EVENT_FLAG_NONE,   \
      arg1_name, ::perfetto::internal::PossiblyNull(arg1_val), arg2_name,     \
      ::perfetto::internal::PossiblyNull(arg2_val))
#ifdef TRACE_EVENT_BEGIN_WITH_FLAGS0
#undef TRACE_EVENT_BEGIN_WITH_FLAGS0
#endif
#define TRACE_EVENT_BEGIN_WITH_FLAGS0(category_group, name, flags) \
  do {                                                             \
    if (IsCategoryEnable(category_group)) {                        \
      (void)(flags);                                               \
      StartBytrace(name);                                          \
    }                                                              \
  } while (0);                                                     \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_BEGIN, category_group, name, flags)
#ifdef TRACE_EVENT_BEGIN_WITH_FLAGS1
#undef TRACE_EVENT_BEGIN_WITH_FLAGS1
#endif
#define TRACE_EVENT_BEGIN_WITH_FLAGS1(category_group, name, flags, arg1_name, \
                                      arg1_val)                               \
  do {                                                                        \
    if (IsCategoryEnable(category_group)) {                                   \
      (void)(flags);                                                          \
      StartBytrace(GetStringFromArgs(name, arg1_name, arg1_val));             \
    }                                                                         \
  } while (0);                                                                \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_BEGIN, category_group, name,     \
                           flags, arg1_name,                                  \
                           ::perfetto::internal::PossiblyNull(arg1_val))
#ifdef TRACE_EVENT_COPY_BEGIN2
#undef TRACE_EVENT_COPY_BEGIN2
#endif
#define TRACE_EVENT_COPY_BEGIN2(category_group, name, arg1_name, arg1_val,    \
                                arg2_name, arg2_val)                          \
  do {                                                                        \
    if (IsCategoryEnable(category_group)) {                                   \
      StartBytrace(                                                           \
          GetStringFromArgs(name, arg1_name, arg1_val, arg2_name, arg2_val)); \
    }                                                                         \
  } while (0);                                                                \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_BEGIN, category_group,           \
                           ::perfetto::DynamicString{name},                   \
                           TRACE_EVENT_FLAG_NONE,                             \
                           ::perfetto::DynamicString{arg1_name},              \
                           ::perfetto::internal::PossiblyNull(arg1_val),      \
                           ::perfetto::DynamicString{arg2_name},              \
                           ::perfetto::internal::PossiblyNull(arg2_val))

// End events.
#ifdef TRACE_EVENT_END0
#undef TRACE_EVENT_END0
#endif
#define TRACE_EVENT_END0(category_group, name)                          \
  do {                                                                  \
    if (IsCategoryEnable(category_group)) {                             \
      (void)(name);                                                     \
      FinishBytrace();                                                  \
    }                                                                   \
  } while (0);                                                          \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_END, category_group, name, \
                           TRACE_EVENT_FLAG_NONE)
#ifdef TRACE_EVENT_END1
#undef TRACE_EVENT_END1
#endif
#define TRACE_EVENT_END1(category_group, name, arg1_name, arg1_val)     \
  do {                                                                  \
    if (IsCategoryEnable(category_group)) {                             \
      (void)(name);                                                     \
      (void)(arg1_name);                                                \
      (void)(arg1_val);                                                 \
      FinishBytrace();                                                  \
    }                                                                   \
  } while (0);                                                          \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_END, category_group, name, \
                           TRACE_EVENT_FLAG_NONE, arg1_name,            \
                           ::perfetto::internal::PossiblyNull(arg1_val))
#ifdef TRACE_EVENT_END2
#undef TRACE_EVENT_END2
#endif
#define TRACE_EVENT_END2(category_group, name, arg1_name, arg1_val, arg2_name, \
                         arg2_val)                                             \
  do {                                                                         \
    if (IsCategoryEnable(category_group)) {                                    \
      (void)(name);                                                            \
      (void)(arg1_name);                                                       \
      (void)(arg1_val);                                                        \
      (void)(arg2_name);                                                       \
      (void)(arg2_val);                                                        \
      FinishBytrace();                                                         \
    }                                                                          \
  } while (0);                                                                 \
  INTERNAL_TRACE_EVENT_ADD(                                                    \
      TRACE_EVENT_PHASE_END, category_group, name, TRACE_EVENT_FLAG_NONE,      \
      arg1_name, ::perfetto::internal::PossiblyNull(arg1_val), arg2_name,      \
      ::perfetto::internal::PossiblyNull(arg2_val))
#ifdef TRACE_EVENT_END_WITH_FLAGS0
#undef TRACE_EVENT_END_WITH_FLAGS0
#endif
#define TRACE_EVENT_END_WITH_FLAGS0(category_group, name, flags) \
  do {                                                           \
    if (IsCategoryEnable(category_group)) {                      \
      (void)(name);                                              \
      (void)(flags);                                             \
      FinishBytrace();                                           \
    }                                                            \
  } while (0);                                                   \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_END, category_group, name, flags)
#ifdef TRACE_EVENT_END_WITH_FLAGS1
#undef TRACE_EVENT_END_WITH_FLAGS1
#endif
#define TRACE_EVENT_END_WITH_FLAGS1(category_group, name, flags, arg1_name,    \
                                    arg1_val)                                  \
  do {                                                                         \
    if (IsCategoryEnable(category_group)) {                                    \
      (void)(name);                                                            \
      (void)(arg1_name);                                                       \
      (void)(arg1_val);                                                        \
      (void)(flags);                                                           \
      FinishBytrace();                                                         \
    }                                                                          \
  } while (0);                                                                 \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_END, category_group, name, flags, \
                           arg1_name,                                          \
                           ::perfetto::internal::PossiblyNull(arg1_val))
#ifdef TRACE_EVENT_COPY_END2
#undef TRACE_EVENT_COPY_END2
#endif
#define TRACE_EVENT_COPY_END2(category_group, name, arg1_name, arg1_val,      \
                              arg2_name, arg2_val)                            \
  do {                                                                        \
    if (IsCategoryEnable(category_group)) {                                   \
      (void)(name);                                                           \
      (void)(arg1_name);                                                      \
      (void)(arg1_val);                                                       \
      (void)(arg2_name);                                                      \
      (void)(arg2_val);                                                       \
      FinishBytrace();                                                        \
    }                                                                         \
  } while (0);                                                                \
  INTERNAL_TRACE_EVENT_ADD(                                                   \
      TRACE_EVENT_PHASE_END, category_group, ::perfetto::DynamicString{name}, \
      TRACE_EVENT_FLAG_NONE, ::perfetto::DynamicString{arg1_name},            \
      ::perfetto::internal::PossiblyNull(arg1_val),                           \
      ::perfetto::DynamicString{arg2_name},                                   \
      ::perfetto::internal::PossiblyNull(arg2_val))

// TimeStamp
#ifdef TRACE_EVENT_MARK_WITH_TIMESTAMP0
#undef TRACE_EVENT_MARK_WITH_TIMESTAMP0
#endif
#define TRACE_EVENT_MARK_WITH_TIMESTAMP0(category_group, name, timestamp)  \
  BYTRACE_SCOPED_INIT(category_group);                                     \
  do {                                                                     \
    if (IsCategoryEnable(category_group)) {                                \
      (void)(timestamp);                                                   \
      BYTRACE_SCOPED_TRACE_EVENT(name);                                    \
    }                                                                      \
  } while (0);                                                             \
  INTERNAL_TRACE_EVENT_ADD_WITH_TIMESTAMP(TRACE_EVENT_PHASE_MARK,          \
                                          category_group, name, timestamp, \
                                          TRACE_EVENT_FLAG_NONE)
#ifdef TRACE_EVENT_MARK_WITH_TIMESTAMP1
#undef TRACE_EVENT_MARK_WITH_TIMESTAMP1
#endif
#define TRACE_EVENT_MARK_WITH_TIMESTAMP1(category_group, name, timestamp, \
                                         arg1_name, arg1_val)             \
  BYTRACE_SCOPED_INIT(category_group);                                    \
  do {                                                                    \
    if (IsCategoryEnable(category_group)) {                               \
      (void)(timestamp);                                                  \
      BYTRACE_SCOPED_TRACE_EVENT(                                         \
          GetStringFromArgs(name, arg1_name, arg1_val));                  \
    }                                                                     \
  } while (0);                                                            \
  INTERNAL_TRACE_EVENT_ADD_WITH_TIMESTAMP(                                \
      TRACE_EVENT_PHASE_MARK, category_group, name, timestamp,            \
      TRACE_EVENT_FLAG_NONE, arg1_name,                                   \
      ::perfetto::internal::PossiblyNull(arg1_val))
#ifdef TRACE_EVENT_MARK_WITH_TIMESTAMP2
#undef TRACE_EVENT_MARK_WITH_TIMESTAMP2
#endif
#define TRACE_EVENT_MARK_WITH_TIMESTAMP2(                                      \
    category_group, name, timestamp, arg1_name, arg1_val, arg2_name, arg2_val) \
  BYTRACE_SCOPED_INIT(category_group);                                         \
  do {                                                                         \
    if (IsCategoryEnable(category_group)) {                                    \
      (void)(timestamp);                                                       \
      BYTRACE_SCOPED_TRACE_EVENT(                                              \
          GetStringFromArgs(name, arg1_name, arg1_val, arg2_name, arg2_val));  \
    }                                                                          \
  } while (0);                                                                 \
  INTERNAL_TRACE_EVENT_ADD_WITH_TIMESTAMP(                                     \
      TRACE_EVENT_PHASE_MARK, category_group, name, timestamp,                 \
      TRACE_EVENT_FLAG_NONE, arg1_name,                                        \
      ::perfetto::internal::PossiblyNull(arg1_val), arg2_name,                 \
      ::perfetto::internal::PossiblyNull(arg2_val))
#ifdef TRACE_EVENT_COPY_MARK
#undef TRACE_EVENT_COPY_MARK
#endif
#define TRACE_EVENT_COPY_MARK(category_group, name)                \
  BYTRACE_SCOPED_INIT(category_group);                             \
  do {                                                             \
    if (IsCategoryEnable(category_group)) {                        \
      BYTRACE_SCOPED_TRACE_EVENT(name);                            \
    }                                                              \
  } while (0);                                                     \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_MARK, category_group, \
                           ::perfetto::DynamicString{name},        \
                           TRACE_EVENT_FLAG_NONE)
#ifdef TRACE_EVENT_COPY_MARK1
#undef TRACE_EVENT_COPY_MARK1
#endif
#define TRACE_EVENT_COPY_MARK1(category_group, name, arg1_name, arg1_val)      \
  BYTRACE_SCOPED_INIT(category_group);                                         \
  do {                                                                         \
    if (IsCategoryEnable(category_group)) {                                    \
      BYTRACE_SCOPED_TRACE_EVENT(                                              \
          GetStringFromArgs(name, arg1_name, arg1_val));                       \
    }                                                                          \
  } while (0);                                                                 \
  INTERNAL_TRACE_EVENT_ADD(                                                    \
      TRACE_EVENT_PHASE_MARK, category_group, ::perfetto::DynamicString{name}, \
      TRACE_EVENT_FLAG_NONE, ::perfetto::DynamicString{arg1_name},             \
      ::perfetto::internal::PossiblyNull(arg1_val))
#ifdef TRACE_EVENT_COPY_MARK_WITH_TIMESTAMP
#undef TRACE_EVENT_COPY_MARK_WITH_TIMESTAMP
#endif
#define TRACE_EVENT_COPY_MARK_WITH_TIMESTAMP(category_group, name, timestamp)  \
  BYTRACE_SCOPED_INIT(category_group);                                         \
  do {                                                                         \
    if (IsCategoryEnable(category_group)) {                                    \
      (void)(timestamp);                                                       \
      BYTRACE_SCOPED_TRACE_EVENT(name);                                        \
    }                                                                          \
  } while (0);                                                                 \
  INTERNAL_TRACE_EVENT_ADD_WITH_TIMESTAMP(                                     \
      TRACE_EVENT_PHASE_MARK, category_group, ::perfetto::DynamicString{name}, \
      timestamp, TRACE_EVENT_FLAG_NONE)

// Counter
#ifdef TRACE_COUNTER1
#undef TRACE_COUNTER1
#endif
#define TRACE_COUNTER1(category_group, name, value)                         \
  do {                                                                      \
    if (IsCategoryEnable(category_group)) {                                 \
      CountBytrace(name, value);                                            \
    }                                                                       \
  } while (0);                                                              \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_COUNTER, category_group, name, \
                           TRACE_EVENT_FLAG_NONE, "value",                  \
                           static_cast<int>(value))
#ifdef TRACE_COUNTER_WITH_FLAG1
#undef TRACE_COUNTER_WITH_FLAG1
#endif
#define TRACE_COUNTER_WITH_FLAG1(category_group, name, flag, value)         \
  do {                                                                      \
    if (IsCategoryEnable(category_group)) {                                 \
      (void)(flag);                                                         \
      CountBytrace(name, value);                                            \
    }                                                                       \
  } while (0);                                                              \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_COUNTER, category_group, name, \
                           flag, "value", static_cast<int>(value))
#ifdef TRACE_COPY_COUNTER1
#undef TRACE_COPY_COUNTER1
#endif
#define TRACE_COPY_COUNTER1(category_group, name, value)              \
  do {                                                                \
    if (IsCategoryEnable(category_group)) {                           \
      CountBytrace(name, static_cast<int>(value));                    \
    }                                                                 \
  } while (0);                                                        \
  INTERNAL_TRACE_EVENT_ADD(TRACE_EVENT_PHASE_COUNTER, category_group, \
                           ::perfetto::DynamicString{name},           \
                           TRACE_EVENT_FLAG_NONE, "value",            \
                           static_cast<int>(value))
#ifdef TRACE_COUNTER_WITH_TIMESTAMP1
#undef TRACE_COUNTER_WITH_TIMESTAMP1
#endif
#define TRACE_COUNTER_WITH_TIMESTAMP1(category_group, name, timestamp, value) \
  do {                                                                        \
    if (IsCategoryEnable(category_group)) {                                   \
      (void)(timestamp);                                                      \
      CountBytrace(name, value);                                              \
    }                                                                         \
  } while (0);                                                                \
  INTERNAL_TRACE_EVENT_ADD_WITH_TIMESTAMP(                                    \
      TRACE_EVENT_PHASE_COUNTER, category_group, name, timestamp,             \
      TRACE_EVENT_FLAG_NONE, "value", static_cast<int>(value))

#if BUILDFLAG(ARKWEB_DFX_TRACING)
#define OHOS_TRACE_EVENT0(category_group, name) \
  OHOS_BYTRACE_SCOPED_INIT();                   \
  do {                                          \
    if (IsOHOSBytraceEnable()) {                \
      OHOS_BYTRACE_SCOPED_TRACE_EVENT(name);    \
    }                                      \
  } while (0)

#define OHOS_TRACE_EVENT1(category_group, name, arg1_name, arg1_val) \
  OHOS_BYTRACE_SCOPED_INIT();                                        \
  do {                                                               \
    if (IsOHOSBytraceEnable()) {                                     \
      OHOS_BYTRACE_SCOPED_TRACE_EVENT(                               \
          GetStringFromArgs(name, arg1_name, arg1_val));             \
    }                                                                \
  } while (0);                                                       \
  INTERNAL_TRACE_EVENT_ADD_SCOPED(category_group, name, arg1_name, arg1_val)

#define OHOS_TRACE_EVENT2(category_group, name, arg1_name, arg1_val,          \
                          arg2_name, arg2_val)                                \
  OHOS_BYTRACE_SCOPED_INIT();                                                 \
  do {                                                                        \
    if (IsOHOSBytraceEnable()) {                                              \
      OHOS_BYTRACE_SCOPED_TRACE_EVENT(                                        \
          GetStringFromArgs(name, arg1_name, arg1_val, arg2_name, arg2_val)); \
    }                                                                         \
  } while (0);                                                                \
  INTERNAL_TRACE_EVENT_ADD_SCOPED(category_group, name, arg1_name, arg1_val,  \
                                  arg2_name, arg2_val)

#define OHOS_TRACE_EVENT_WITH_FLOW0(category_group, name, bind_id, flow_flags) \
  OHOS_BYTRACE_SCOPED_INIT();                                                  \
  do {                                                                         \
    if (IsOHOSBytraceEnable()) {                                               \
      (void)(bind_id);                                                         \
      (void)(flow_flags);                                                      \
      OHOS_BYTRACE_SCOPED_TRACE_EVENT(name);                                   \
    }                                                                          \
  } while (0);                                                                 \
  INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW(category_group, name, bind_id,     \
                                            flow_flags)

#define OHOS_TRACE_EVENT_WITH_FLOW1(category_group, name, bind_id, flow_flags, \
                                    arg1_name, arg1_val)                       \
  OHOS_BYTRACE_SCOPED_INIT();                                                  \
  do {                                                                         \
    if (IsOHOSBytraceEnable()) {                                               \
      (void)(bind_id);                                                         \
      (void)(flow_flags);                                                      \
      OHOS_BYTRACE_SCOPED_TRACE_EVENT(                                         \
          GetStringFromArgs(name, arg1_name, arg1_val));                       \
    }                                                                          \
  } while (0);                                                                 \
  INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW(category_group, name, bind_id,     \
                                            flow_flags, arg1_name, arg1_val)

#define OHOS_TRACE_EVENT_WITH_FLOW2(category_group, name, bind_id, flow_flags, \
                                    arg1_name, arg1_val, arg2_name, arg2_val)  \
  OHOS_BYTRACE_SCOPED_INIT();                                                  \
  do {                                                                         \
    if (IsOHOSBytraceEnable()) {                                               \
      (void)(bind_id);                                                         \
      (void)(flow_flags);                                                      \
      OHOS_BYTRACE_SCOPED_TRACE_EVENT(                                         \
          GetStringFromArgs(name, arg1_name, arg1_val, arg2_name, arg2_val));  \
    }                                                                          \
  } while (0);                                                                 \
  INTERNAL_TRACE_EVENT_ADD_SCOPED_WITH_FLOW(category_group, name, bind_id,     \
                                            flow_flags, arg1_name, arg1_val,   \
                                            arg2_name, arg2_val)
#endif

#endif  // TRACE_EVENT_COMMON_OH_H
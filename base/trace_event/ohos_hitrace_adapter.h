// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_OHOS_HITRACE_ADAPTER_H_
#define BASE_TRACE_EVENT_OHOS_HITRACE_ADAPTER_H_

#include <cstddef>
#include <cstdint>

#include "base/base_export.h"

namespace base::trace_event::ohos {
/**
 * hitrace api interface, it must be thread safe
 */
class BASE_EXPORT HitraceAdapter {
 public:
  /** max length const char* length hitrace accept */
  static constexpr const size_t kLimit = 500;
  virtual ~HitraceAdapter() = default;
  /**
   * Marks the start of a synchronous trace task.
   * @param name Name of a trace task.
   */
  virtual void StartSyncTrace(const char* name) = 0;
  /**
   * Marks the end of a synchronous trace task,
   * must be pair with `StartSyncTrace` to format correct trace
   */
  virtual void EndSyncTrace() = 0;
  /**
   * Marks the start of an asynchronous trace task.
   * @param name Name of the asynchronous trace task.
   * @param taskId ID of the asynchronous trace task. The start and end of an
   * asynchronous trace task do not occur in sequence. Therefore, the start and
   * end of an asynchronous trace need to be matched based on the task name and
   * the unique task ID together.
   */
  virtual void StartAsyncTrace(const char* name, int32_t taskId) = 0;
  /**
   * Marks the end of an asynchronous trace task, `name` and `taskId` must be
   * pair with `StartAsyncTrace`
   * @param name Name of the asynchronous trace task.
   * @param taskId ID of the asynchronous trace task. The start and end of an
   * asynchronous trace task do not occur in sequence. Therefore, the start and
   * end of an asynchronous trace need to be matched based on the task name and
   * the unique task ID together.
   */
  virtual void EndAsyncTrace(const char* name, int32_t taskId) = 0;
  /**
   * Traces the value change of an integer variable based on its name.
   * @param name Name of the integer variable. It does not need to be the same
   * as the real variable name.
   * @param count Integer value. Generally, an integer variable can be passed.
   */
  virtual void CounterTrace(const char* name, int64_t count) = 0;
};

/** get the system hitrace implementation */
HitraceAdapter& BASE_EXPORT GetSystemSingleton();
}  // namespace base::trace_event::ohos

#endif  // BASE_TRACE_EVENT_OHOS_HITRACE_ADAPTER_H_
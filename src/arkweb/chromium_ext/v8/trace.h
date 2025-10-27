/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef V8_TRACING_TRACE_H_
#define V8_TRACING_TRACE_H_

#ifdef OSOHOS
#include "src/init/v8.h"
#include "hitrace/trace.h"

class HiTrace {
public:
    explicit HiTrace(const char *name) : traceName(name)
    {
        if (v8::internal::rcs_enable == true) {
            OH_HiTrace_StartTrace(traceName);
        }
    }
    ~HiTrace()
    {
        if (v8::internal::rcs_enable == true) {
            OH_HiTrace_FinishTrace();
        }
    }
    const char *traceName;
};

#else
struct HiTrace {
    explicit HiTrace(const char *name)
    {
        (void)name;
    }

    ~HiTrace() = default;
};
#endif

#endif  // V8_TRACING_TRACE_H_
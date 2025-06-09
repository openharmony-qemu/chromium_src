/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ohos/adapter/common/trace.h"

#include <hitrace/trace.h>
#include <string>
#include <utility>

AdapterScopedTracer::~AdapterScopedTracer() {
  if (trace_enabled_) {
    OH_HiTrace_FinishTrace();
  }
}

void AdapterScopedTracer::Initialize(bool enabled, std::string& arg_str) {
  trace_enabled_ = enabled;
  if (enabled) {
    OH_HiTrace_StartTrace(arg_str.c_str());
  }
}

// TraceArgValue functions
std::string TraceArgValue::ToString(TraceArgType type) const {
  switch (type) {
    case TYPE_BOOL:
      return this->as_bool ? "true" : "false";
    case TYPE_UINT:
      return std::to_string(this->as_uint);
    case TYPE_INT:
      return std::to_string(this->as_int);
    case TYPE_DOUBLE:
      return std::to_string(this->as_double);
    case TYPE_STRING:
      return this->as_string;
    default:
      return "type-error";
  }
}

// TraceArg functions
TraceArg::TraceArg(bool v) {
  type = TYPE_BOOL;
  value.as_bool = v;
}

TraceArg::TraceArg(uint32_t v) {
  type = TYPE_UINT;
  value.as_uint = v;
}

TraceArg::TraceArg(int32_t v) {
  type = TYPE_INT;
  value.as_int = v;
}

TraceArg::TraceArg(double v) {
  type = TYPE_DOUBLE;
  value.as_double = v;
}

TraceArg::TraceArg(const std::string& v) {
  type = TYPE_STRING;
  value.as_string = v.c_str();
}

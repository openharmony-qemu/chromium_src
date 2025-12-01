// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/trace_event/ohos_fix_buffer.h"

#include <cinttypes>

#include "base/compiler_specific.h"

namespace base::trace_event::ohos {

namespace {
constexpr std::string_view kTrueView = "true";
constexpr std::string_view kFalseView = "false";

/**
 * avoid buffer overflow brought by snprintf return value, which can exceed
 * buffer size
 * @param raw_len the return value of `snprintf`
 * @param delta_len the buffer size
 * @returns value is the actual new writed length,
 * it is `min(raw_len, delta_len)` to avoid buffer overflow and `0` if `raw_len`
 * is negative
 */
ALWAYS_INLINE constexpr size_t GetDeltaLength(int raw_len,
                                              size_t delta_len) noexcept {
  if (raw_len < 0 || delta_len <= 0) {
    return 0;
  }
  size_t raw_size = static_cast<size_t>(raw_len);
  if (raw_size > delta_len) {
    return delta_len;
  }
  return raw_size;
}
}  // namespace

class PrintHelper {
 public:
  /**
   * helper function for invoke snprintf and handle result of snprintf and
   * append null terminator
   * @param buffer the buffer class
   * @param f the lambda that is invoked with `f(const char* dest, size_t
   * buffer_size)`, should directly call `snprintf`
   * @return the buffer instance reference
   */
  template <typename F>
  static FixBuffer& ToBuffer(FixBuffer& buffer, F f) {
    if (buffer.IsFull()) {
      return buffer;
    }
    size_t delta_len = buffer.size_ - buffer.len_;
    int raw_len = f(buffer.buffer_ + buffer.len_, delta_len);
    buffer.len_ += GetDeltaLength(raw_len, delta_len);
    buffer.buffer_[buffer.len_] = '\0';
    return buffer;
  }
};

FixBuffer::FixBuffer(char* buffer, size_t length) noexcept
    : size_(length - 1), buffer_(buffer) {
  Clear();
  buffer_[size_] = '\0';
}

std::string_view FixBuffer::GetStr() const noexcept {
  return {buffer_, len_};
}

FixBuffer& FixBuffer::operator<<(std::string_view string_view_input) noexcept {
  if (IsFull()) {
    return *this;
  }
  // target_len cannot exceed `size_` to avoid buffer overflow
  size_t target_len = std::min(string_view_input.length() + len_, size_);
  size_t delta_len = target_len - len_;
  std::memcpy(buffer_ + len_, string_view_input.data(), delta_len);
  len_ = target_len;
  // make buffer_ always valid c str without overflow for c api
  buffer_[len_] = '\0';
  return *this;
}

FixBuffer& FixBuffer::operator<<(int int_input) noexcept {
  return PrintHelper::ToBuffer(
      *this, [int_input](char* dest, size_t len) -> int {
        return std::snprintf(dest, len, "%d", int_input);
      });
}

FixBuffer& FixBuffer::operator<<(int64_t int64_input) noexcept {
  return PrintHelper::ToBuffer(
      *this, [int64_input](char* dest, size_t len) -> int {
        return std::snprintf(dest, len, "%" PRId64 "", int64_input);
      });
}

FixBuffer& FixBuffer::operator<<(uint64_t uint64_input) noexcept {
  return PrintHelper::ToBuffer(
      *this, [uint64_input](char* dest, size_t len) -> int {
        return std::snprintf(dest, len, "%" PRIu64 "", uint64_input);
      });
}

FixBuffer& FixBuffer::operator<<(FixBuffer::ToBool bool_value) noexcept {
  const std::string_view& value = bool_value.value ? kTrueView : kFalseView;
  return *this << value;
}

FixBuffer& FixBuffer::operator<<(double double_value) noexcept {
  return PrintHelper::ToBuffer(
      *this, [double_value](char* dest, size_t len) -> int {
        return std::snprintf(dest, len, "%g", double_value);
      });
}

FixBuffer& FixBuffer::operator<<(Ptr pointer_value) noexcept {
  return PrintHelper::ToBuffer(
      *this, [pointer_value](char* dest, size_t len) -> int {
        return std::snprintf(dest, len, "%p", pointer_value.ptr);
      });
}

void FixBuffer::Clear() noexcept {
  len_ = 0;
  buffer_[0] = '\0';
  buffer_[size_] = '\0';
}

size_t FixBuffer::ShrinkTo(size_t input) noexcept {
  if (input >= len_) {
    return len_;
  }
  len_ = input;
  buffer_[len_] = '\0';
  return input;
}
}  // namespace base::trace_event::ohos
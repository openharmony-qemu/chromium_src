// Copyright (c) 2025 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BASE_TRACE_EVENT_OHOS_FIX_BUFFER_
#define BASE_TRACE_EVENT_OHOS_FIX_BUFFER_

#include <array>
#include <string_view>

namespace base::trace_event::ohos {
/**
 * FixBuffer is a utility class used to append with a limited length support,
 * it takes a `char array[LEN]` reference or `std::array<char, LEN>` and write
 * string on it without exceed limit.
 * Since not owning underlying buffer, it must keep valid throughout FixBuffer
 * life span
 */
struct FixBuffer {
 public:
  /**
   * tag class to avoid implicit convertion from `const char*` to `const void*`
   */
  struct Ptr {
   public:
    const void* ptr;
    explicit constexpr Ptr(const void* ptr) : ptr(ptr) {}
  };
  /** tag class to avoid implicit convertion to bool to mess up overload */
  struct ToBool {
   public:
    bool value;
    explicit constexpr ToBool(bool value) : value(value) {}
  };
  /** constructor FixBuffer with an char array left value reference */
  template <size_t N>
  explicit FixBuffer(char (&buffer)[N]) : FixBuffer(buffer, N) {
    static_assert(N >= 1, "buffer size must greater than 1");
  }

  /** constructor FixBuffer with an `std::array` instance */
  template <size_t N>
  explicit FixBuffer(std::array<char, N>& buffer)
      : FixBuffer(buffer.data(), buffer.size()) {
    static_assert(N >= 1, "buffer size must greater than 1");
  }

  /** copy not allowed */
  FixBuffer(const FixBuffer& other) = delete;
  ~FixBuffer() noexcept = default;

  /**
   * Append input, if over limit just append to limit
   */
  FixBuffer& operator<<(std::string_view string_view_input) noexcept;

  /** append number */
  FixBuffer& operator<<(int int_input) noexcept;

  /** append int64 number */
  FixBuffer& operator<<(int64_t int64_input) noexcept;

  FixBuffer& operator<<(uint64_t uint64_input) noexcept;

  /**
   * append bool value
   * @param bool_value, use a structure to avoid implicit convert to bool
   */
  FixBuffer& operator<<(ToBool bool_value) noexcept;

  /** append double value, returns actual appended char */
  FixBuffer& operator<<(double double_value) noexcept;

  /** append pointer value, returns actual appended char */
  FixBuffer& operator<<(Ptr pointer_value) noexcept;

  /** get the `string_view` of the current buffer */
  std::string_view GetStr() const noexcept;

  /** @return return the c str, guaranteed to be always valid */
  const char* CStr() const noexcept { return buffer_; }

  /** clear the buffer to make it empty str */
  void Clear() noexcept;

  /**
   * shrink to specific length
   * @param input the target length
   * @return if input is less than current length, shrink to it and return
   * input. otherwise keep untouched and return current length
   */
  size_t ShrinkTo(size_t input) noexcept;

  /** @return length of the whole buffer written */
  size_t Len() const noexcept { return len_; }

  /** @return the total size of buffer (except last null terminator) */
  size_t BufferSize() const noexcept { return size_; }

  /** @return full or not */
  bool IsFull() const noexcept { return len_ >= size_; }

 private:
  friend class PrintHelper;
  FixBuffer(char* buffer, size_t length) noexcept;
  /** the total size */
  const size_t size_;
  /** the length of valid string before tailing `'\0'` */
  size_t len_{0};
  /** the buffer for output, always valid C str with terminate zero at last */
  char* buffer_;
};
}  // namespace base::trace_event::ohos

#endif  // BASE_TRACE_EVENT_OHOS_FIX_BUFFER_
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

#include "base/debug/stack_trace.h"

#include <unwind.h>

#include <algorithm>
#include <cstddef>
#include <ostream>

#include "base/containers/span_writer.h"
#include "base/debug/proc_maps_linux.h"
#include "base/logging.h"
#include "base/stl_util.h"
#include "base/strings/strcat.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_restrictions.h"

#ifdef __LP64__
#define FMT_ADDR "0x%016lx"
#else
#define FMT_ADDR "0x%08x"
#endif

namespace {

struct StackCrawlState {
  StackCrawlState(uintptr_t* frames, size_t max_depth)
      : frames(frames),
        frame_count(0),
        max_depth(max_depth),
        have_skipped_self(false) {}

  uintptr_t* frames;
  size_t frame_count;
  size_t max_depth;
  bool have_skipped_self;
};

_Unwind_Reason_Code TraceStackFrame(_Unwind_Context* context, void* arg) {
  StackCrawlState* state = static_cast<StackCrawlState*>(arg);
  uintptr_t ip = _Unwind_GetIP(context);

  // The first stack frame is this function itself.  Skip it.
  if (ip != 0 && !state->have_skipped_self) {
    state->have_skipped_self = true;
    return _URC_NO_REASON;
  }

  state->frames[state->frame_count++] = ip;
  if (state->frame_count >= state->max_depth) {
    return _URC_END_OF_STACK;
  }
  return _URC_NO_REASON;
}

bool EndsWith(const std::string& s, const std::string& suffix) {
  return s.size() >= suffix.size() &&
      s.substr(s.size() - suffix.size(), suffix.size()) == suffix;
}

}  // namespace

namespace base {
namespace debug {

namespace internal {
void itoa_r(intptr_t i, int base, size_t padding, base::span<char> buf) {
  // Make sure we can write at least one NUL byte.
  if (buf.empty()) {
    return;
  }

  if (base < 2 || base > 16) {
    buf[0u] = '\000';
    return;
  }

  auto writer = base::SpanWriter(buf);
  size_t start = 0u;

  uintptr_t j = static_cast<uintptr_t>(i);

  // Handle negative numbers (only for base 10).
  if (i < 0 && base == 10) {
    // This does "j = -i" while avoiding integer overflow.
    j = static_cast<uintptr_t>(-(i + 1)) + 1;

    // Make sure we can write the '-' character.
    if (!writer.Write('-')) {
      buf[0u] = '\000';
      return;
    }
    start += 1u;  // The number starts after the sign.
  }

  // Loop until we have converted the entire number. Output at least one
  // character (i.e. '0').
  constexpr std::string_view digits = "0123456789abcdef";
  do {
    // Output the next digit.
    if (!writer.Write(digits[j % static_cast<uintptr_t>(base)])) {
      buf[0] = '\000';
      return;
    }
    j /= static_cast<uintptr_t>(base);

    if (padding > 0) {
      padding--;
    }
  } while (j > 0 || padding > 0);

  // Terminate the output with a NUL character.
  if (!writer.Write('\000')) {
    buf[0] = '\000';
    return;
  }

  // Conversion to ASCII actually resulted in the digits being in reverse order.
  // We can't easily generate them in forward order, as we can't tell the number
  // of characters needed until we are done converting. So, now, we reverse the
  // string (except for the possible "-" sign and the NUL terminator).
  std::ranges::reverse(buf.first(writer.num_written() - 1u).subspan(start));
}
}  // namespace internal

bool EnableInProcessStackDumping() {
  struct sigaction action;
  std::fill(reinterpret_cast<unsigned char *>(&action),
            reinterpret_cast<unsigned char *>(&action) + sizeof(action),
            0);
  action.sa_handler = SIG_IGN;
  sigemptyset(&action.sa_mask);
  return (sigaction(SIGPIPE, &action, NULL) == 0);
}

size_t CollectStackTrace(span<const void*> trace) {
  StackCrawlState state(reinterpret_cast<uintptr_t*>(trace.data()),
                        trace.size());
  _Unwind_Backtrace(&TraceStackFrame, &state);
  return state.frame_count;
}

// static
void StackTrace::PrintMessageWithPrefix(cstring_view prefix_string,
                                        cstring_view message) {
  if (!prefix_string.empty()) {
    LOG(ERROR) << StrCat({prefix_string, message}).c_str();
  } else {
    LOG(ERROR) << message.c_str();
  }
}

void StackTrace::PrintWithPrefixImpl(cstring_view prefix_string) const {
  std::string backtrace = ToStringWithPrefix(prefix_string);
  LOG(ERROR) << backtrace;
}

// NOTE: Native libraries in APKs are stripped before installing. Print out the
// relocatable address and library names so host computers can use tools to
// symbolize and demangle (e.g., addr2line, c++filt).
void StackTrace::OutputToStreamWithPrefixImpl(std::ostream* os,
                                              cstring_view prefix_string) const {
  std::string proc_maps;
  std::vector<MappedMemoryRegion> regions;
  // Allow IO to read /proc/self/maps. Reading this file doesn't hit the disk
  // since it lives in procfs, and this is currently used to print a stack trace
  // on fatal log messages in debug builds only. If the restriction is enabled
  // then it will recursively trigger fatal failures when this enters on the
  // UI thread.
  base::ScopedAllowBlocking scoped_allow_blocking;
  if (!ReadProcMaps(&proc_maps)) {
    LOG(ERROR) << "Failed to read /proc/self/maps";
  } else if (!ParseProcMaps(proc_maps, &regions)) {
    LOG(ERROR) << "Failed to parse /proc/self/maps";
  }

  for (size_t i = 0; i < count_; ++i) {
    // Subtract one as return address of function may be in the next
    // function when a function is annotated as noreturn.
    uintptr_t address = reinterpret_cast<uintptr_t>(trace_[i]) - 1;

    std::vector<MappedMemoryRegion>::iterator iter = regions.begin();
    while (iter != regions.end()) {
      if (address >= iter->start && address < iter->end &&
          !iter->path.empty()) {
        break;
      }
      ++iter;
    }

    *os << prefix_string;

    // Adjust absolute address to be an offset within the mapped region, to
    // match the format dumped by Android's crash output.
    if (iter != regions.end()) {
      address -= iter->start;
    }

    // The format below intentionally matches that of Android's debuggerd
    // output. This simplifies decoding by scripts such as stack.py.
    *os << base::StringPrintf("#%02zd pc " FMT_ADDR " ", i, address);

    if (iter != regions.end()) {
      *os << base::StringPrintf("%s", iter->path.c_str());
      if (EndsWith(iter->path, ".apk")) {
        *os << base::StringPrintf(" (offset 0x%llx)", iter->offset);
      }
    } else {
      *os << "<unknown>";
    }

    *os << "\n";
  }
}

}  // namespace debug
}  // namespace base

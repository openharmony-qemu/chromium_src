// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/debug/stack_trace.h"

#include <stddef.h>
#include <unwind.h>

#include <algorithm>
#include <ostream>

#include "base/debug/proc_maps_linux.h"
#include "base/logging.h"
#include "base/stl_util.h"
#include "base/strings/stringprintf.h"
#include "base/threading/thread_restrictions.h"

#if defined(USE_SYMBOLIZE)
#include "base/posix/eintr_wrapper.h"
#include "base/third_party/symbolize/symbolize.h"
#endif

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
  if (state->frame_count >= state->max_depth)
    return _URC_END_OF_STACK;
  return _URC_NO_REASON;
}

#if !defined(USE_SYMBOLIZE)
bool EndsWith(const std::string& s, const std::string& suffix) {
  return s.size() >= suffix.size() &&
         s.substr(s.size() - suffix.size(), suffix.size()) == suffix;
}
#endif

}  // namespace

namespace base {
namespace debug {

namespace internal {
char* itoa_r(intptr_t i, char* buf, size_t sz, int base, size_t padding) {
  // Make sure we can write at least one NUL byte.
  size_t n = 1;
  if (n > sz)
    return nullptr;

  if (base < 2 || base > 16) {
    buf[0] = '\000';
    return nullptr;
  }

  char* start = buf;

  uintptr_t j = i;

  // Handle negative numbers (only for base 10).
  if (i < 0 && base == 10) {
    // This does "j = -i" while avoiding integer overflow.
    j = static_cast<uintptr_t>(-(i + 1)) + 1;

    // Make sure we can write the '-' character.
    if (++n > sz) {
      buf[0] = '\000';
      return nullptr;
    }
    *start++ = '-';
  }

  // Loop until we have converted the entire number. Output at least one
  // character (i.e. '0').
  char* ptr = start;
  do {
    // Make sure there is still enough space left in our output buffer.
    if (++n > sz) {
      buf[0] = '\000';
      return nullptr;
    }

    // Output the next digit.
    *ptr++ = "0123456789abcdef"[j % base];
    j /= base;

    if (padding > 0)
      padding--;
  } while (j > 0 || padding > 0);

  // Terminate the output with a NUL character.
  *ptr = '\000';

  // Conversion to ASCII actually resulted in the digits being in reverse
  // order. We can't easily generate them in forward order, as we can't tell
  // the number of characters needed until we are done converting.
  // So, now, we reverse the string (except for the possible "-" sign).
  while (--ptr > start) {
    char ch = *ptr;
    *ptr = *start;
    *start++ = ch;
  }
  return buf;
}
} //namespace internal

#if defined(USE_SYMBOLIZE)

class BacktraceOutputHandler {
 public:
  virtual void HandleOutput(const char* output) = 0;

 protected:
  virtual ~BacktraceOutputHandler() = default;
};

void OutputPointer(void* pointer, BacktraceOutputHandler* handler) {
  // This should be more than enough to store a 64-bit number in hex:
  // 16 hex digits + 1 for null-terminator.
  char buf[17] = { '\0' };
  handler->HandleOutput("0x");
  internal::itoa_r(reinterpret_cast<intptr_t>(pointer),
                   buf, sizeof(buf), 16, 12);
  handler->HandleOutput(buf);
}

void OutputFrameId(intptr_t frame_id, BacktraceOutputHandler* handler) {
  // Max unsigned 64-bit number in decimal has 20 digits (18446744073709551615).
  // Hence, 30 digits should be more than enough to represent it in decimal
  // (including the null-terminator).
  char buf[30] = { '\0' };
  handler->HandleOutput("#");
  internal::itoa_r(frame_id, buf, sizeof(buf), 10, 1);
  handler->HandleOutput(buf);
}

void PrintToStderr(const char* output) {
  // NOTE: This code MUST be async-signal safe (it's used by in-process
  // stack dumping signal handler). NO malloc or stdio is allowed here.
  ignore_result(HANDLE_EINTR(write(STDERR_FILENO, output, strlen(output))));
  //LOG(ERROR) << output;
}

class PrintBacktraceOutputHandler : public BacktraceOutputHandler {
 public:
  PrintBacktraceOutputHandler() = default;

  void HandleOutput(const char* output) override {
    // NOTE: This code MUST be async-signal safe (it's used by in-process
    // stack dumping signal handler). NO malloc or stdio is allowed here.
    PrintToStderr(output);
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(PrintBacktraceOutputHandler);
};

class StreamBacktraceOutputHandler : public BacktraceOutputHandler {
 public:
  explicit StreamBacktraceOutputHandler(std::ostream* os) : os_(os) {
  }

  void HandleOutput(const char* output) override { (*os_) << output; }

 private:
  std::ostream* os_;

  DISALLOW_COPY_AND_ASSIGN(StreamBacktraceOutputHandler);
};

void ProcessBacktrace(void* const* trace,
                      size_t size,
                      const char* prefix_string,
                      BacktraceOutputHandler* handler) {
// NOTE: This code MUST be async-signal safe (it's used by in-process
// stack dumping signal handler). NO malloc or stdio is allowed here.

  for (size_t i = 0; i < size; ++i) {
    if (prefix_string)
      handler->HandleOutput(prefix_string);

    OutputFrameId(i, handler);
    handler->HandleOutput(" ");
    OutputPointer(trace[i], handler);
    handler->HandleOutput(" ");

    char buf[1024] = { '\0' };

    // Subtract by one as return address of function may be in the next
    // function when a function is annotated as noreturn.
    void* address = static_cast<char*>(trace[i]) - 1;
    if (google::Symbolize(address, buf, sizeof(buf)))
      handler->HandleOutput(buf);
    else
      handler->HandleOutput("<unknown>");

    handler->HandleOutput("\n");
  }
}

#endif // defined (USE_SYMBOLIZE)

bool EnableInProcessStackDumping() {
  // When running in an application, our code typically expects SIGPIPE
  // to be ignored.  Therefore, when testing that same code, it should run
  // with SIGPIPE ignored as well.
  // TODO(phajdan.jr): De-duplicate this SIGPIPE code.
  struct sigaction action;
  memset(&action, 0, sizeof(action));
  action.sa_handler = SIG_IGN;
  sigemptyset(&action.sa_mask);
  return (sigaction(SIGPIPE, &action, NULL) == 0);
}

size_t CollectStackTrace(void** trace, size_t count) {
  StackCrawlState state(reinterpret_cast<uintptr_t*>(trace), count);
  _Unwind_Backtrace(&TraceStackFrame, &state);
  return state.frame_count;
}

void StackTrace::PrintWithPrefix(const char* prefix_string) const {
#if !defined(USE_SYMBOLIZE)
  std::string backtrace = ToStringWithPrefix(prefix_string);
  LOG(WARNING) << "backtrace:\n" << backtrace.c_str();
#else
  PrintBacktraceOutputHandler handler;
  ProcessBacktrace(trace_, count_, prefix_string, &handler);
#endif // !defined(USE_SYMBOLIZE)
}

// NOTE: Native libraries in APKs are stripped before installing. Print out the
// relocatable address and library names so host computers can use tools to
// symbolize and demangle (e.g., addr2line, c++filt).
void StackTrace::OutputToStreamWithPrefix(std::ostream* os,
                                          const char* prefix_string) const {
  std::string proc_maps;
  std::vector<MappedMemoryRegion> regions;
#if defined(USE_SYMBOLIZE)
  StreamBacktraceOutputHandler handler(os);
  ProcessBacktrace(trace_, count_, prefix_string, &handler);
#else
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

    if (prefix_string)
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
#endif // defined(USE_SYMBOLIZE)
}

}  // namespace debug
}  // namespace base

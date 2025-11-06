// Copyright 2014 The PDFium Authors
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/fx_extension.h"

#include <wchar.h>

#include "core/fxcrt/check.h"
#include "core/fxcrt/compiler_specific.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/utf16.h"
#include "core/fxcrt/widestring.h"
#include "third_party/fast_float/src/include/fast_float/fast_float.h"

namespace {

time_t DefaultTimeFunction() {
  return time(nullptr);
}

struct tm* DefaultLocaltimeFunction(const time_t* tp) {
  return localtime(tp);
}

time_t (*g_time_func)() = DefaultTimeFunction;
struct tm* (*g_localtime_func)(const time_t*) = DefaultLocaltimeFunction;

}  // namespace

float FXSYS_wcstof(WideStringView pwsStr, size_t* pUsedLen) {
  // TODO(thestig): Consolidate code duplication with StringToFloatImpl().
  // Skip leading whitespaces.
  size_t start = 0;
  size_t len = pwsStr.GetLength();
  while (start < len && pwsStr[start] == ' ') {
    ++start;
  }

  WideStringView sub_strc = pwsStr.Substr(start, len - start);

  float value;
  auto result =
      fast_float::from_chars(sub_strc.begin(), sub_strc.end(), value,
                             static_cast<fast_float::chars_format>(
                                 fast_float::chars_format::general |
                                 fast_float::chars_format::allow_leading_plus));

  if (pUsedLen) {
    *pUsedLen = result.ptr - pwsStr.unterminated_c_str();
  }

  // Return 0 for parsing errors. Some examples of errors are an empty string
  // and a string that cannot be converted to `ReturnType`.
  return result.ec == std::errc() || result.ec == std::errc::result_out_of_range
             ? value
             : 0;
}

wchar_t* FXSYS_wcsncpy(wchar_t* dstStr, const wchar_t* srcStr, size_t count) {
  DCHECK(dstStr);
  DCHECK(srcStr);
  DCHECK(count > 0);

  // SAFETY: required from caller, enforced by UNSAFE_BUFFER_USAGE in header.
  UNSAFE_BUFFERS({
    for (size_t i = 0; i < count; ++i) {
      dstStr[i] = srcStr[i];
      if (dstStr[i] == L'\0') {
        break;
      }
    }
  });
  return dstStr;
}

// TODO(tsepez): should be UNSAFE_BUFFER_USAGE.
void FXSYS_IntToTwoHexChars(uint8_t n, char* buf) {
  static const char kHex[] = "0123456789ABCDEF";
  // SAFETY: range of uint8_t keeps indices in bound.
  UNSAFE_BUFFERS({
  buf[0] = kHex[n / 16];
  buf[1] = kHex[n % 16];
  });
}

// TODO(tsepez): This is UNSAFE_BUFFER_USAGE as well.
void FXSYS_IntToFourHexChars(uint16_t n, char* buf) {
  // SAFETY: required from caller.
  UNSAFE_BUFFERS({
    FXSYS_IntToTwoHexChars(n / 256, buf);
    FXSYS_IntToTwoHexChars(n % 256, buf + 2);
  });
}

// TODO(tsepez): This is UNSAFE_BUFFER_USAGE as well.
size_t FXSYS_ToUTF16BE(uint32_t unicode, char* buf) {
  DCHECK(unicode <= pdfium::kMaximumSupplementaryCodePoint);
  DCHECK(!pdfium::IsHighSurrogate(unicode));
  DCHECK(!pdfium::IsLowSurrogate(unicode));

  if (unicode <= 0xFFFF) {
    FXSYS_IntToFourHexChars(unicode, buf.first<4u>());
    return 4;
  }
  // SAFETY: required from caller.
  UNSAFE_BUFFERS({
  pdfium::SurrogatePair surrogate_pair(unicode);
  FXSYS_IntToFourHexChars(surrogate_pair.high(), buf.first<4u>());
    FXSYS_IntToFourHexChars(surrogate_pair.low(), buf + 4);
  });
  return 8;
}

void FXSYS_SetTimeFunction(time_t (*func)()) {
  g_time_func = func ? func : DefaultTimeFunction;
}

void FXSYS_SetLocaltimeFunction(struct tm* (*func)(const time_t*)) {
  g_localtime_func = func ? func : DefaultLocaltimeFunction;
}

time_t FXSYS_time(time_t* tloc) {
  time_t ret_val = g_time_func();
  if (tloc) {
    *tloc = ret_val;
  }
  return ret_val;
}

struct tm* FXSYS_localtime(const time_t* tp) {
  return g_localtime_func(tp);
}

#ifdef USING_OHOS_WEB
#include "ohlog.h"
#ifdef OSOHOS
#undef LOG
#undef ALIGNAS
#include <cstdio>

#include "../../../base/logging.h"
#endif
#include <cstdarg>

void PrintHilog(const char* format, va_list args) {
#ifdef OSOHOS
  va_list args_copy;
  va_copy(args_copy, args);
  auto size = vsnprintf(nullptr, 0, format, args_copy) + 1;
  va_end(args_copy);
  auto* buffer = new char[size];
  vsnprintf(buffer, size, format, args);
  StreamHilog(std::string(buffer));
  delete[] buffer;
#endif
}

void StreamHilog(const std::string& string) {
#ifdef OSOHOS
  LOG(INFO) << "[V8 DEBUG LOG] : " << string;
#endif
}
#endif

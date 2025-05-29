#ifdef USING_OHOS_WEB
#include "ohlog.h"
#ifdef OSOHOS
#undef LOG
#undef ALIGNAS
#include <cstdio>

#include "../../../base/logging.h"
#endif
#include <cstdarg>

void PrintHilog(const std::string& format, va_list args) {
#ifdef OSOHOS
  auto size = snprintf(nullptr, 0, format.c_str(), args) + 1;
  auto* buffer = new char[size];
  snprintf(buffer, size, format.c_str(), args);
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

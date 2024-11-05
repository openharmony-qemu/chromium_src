
#ifndef BASE_OHOS_LOGGER_H_
#define BASE_OHOS_LOGGER_H_

#include <string>

#include "base/base_export.h"
#include "ohos_nweb/src/capi/nweb_logger_callback.h"

namespace ohos {

namespace logger{
  BASE_EXPORT void SetMainProcessMode(bool is_main_process);
  void SetLoggerCallback(std::shared_ptr<NWebLoggerCallback> loggerCallback); 
  void ReportRendererLog(int policy, const std::string& msg);
}
  
}
#endif
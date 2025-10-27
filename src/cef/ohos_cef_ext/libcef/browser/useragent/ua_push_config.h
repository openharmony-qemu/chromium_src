/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef CEF_LIBCEF_BROWSER_UA_PUSH_CONFIG_H_
#define CEF_LIBCEF_BROWSER_UA_PUSH_CONFIG_H_

#include <atomic>
#include <optional>
#include <vector>

#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/json/json_reader.h"
#include "base/memory/scoped_refptr.h"
#include "base/no_destructor.h"
#include "base/synchronization/lock.h"
#include "base/task/thread_pool.h"
#include "base/values.h"
#include "components/prefs/pref_registry_simple.h"
#include "components/prefs/scoped_user_pref_update.h"
#include "components/user_prefs/user_prefs.h"
namespace base {
template <typename T>
struct DefaultSingletonTraits;
}
namespace ohos_user_agent {
constexpr char kFrontStr[] = "front_str";
constexpr char kBackStr[] = "back_str";
const char kUAPushLogTag[] = "[UAPushLog]";
enum UAPushConfigPositionType { FRONT = 0, BACK = 1, INVALID = -1 };
constexpr char kUAPushConfigOSCompatible[] = "OSCompatible";
constexpr char kUAPushConfigOSSupplement[] = "OSSupplement";
constexpr char kUAPushConfigDeviceType[] = "deviceType";
constexpr char kUATrustList[] = "trustlist";
constexpr char kUAPushConfigPosition[] = "position";
const char kUAPushConfigInfo[] = "user_agent.push_config";
struct OSPositionScheme {
  base::Value::List front_str;
  base::Value::List back_str;
};
struct OSPositionPrefsInfo {
  std::string front_str;
  std::string back_str;
};
class UAPushConfig {
 public:
  static UAPushConfig* GetInstance();
  static void RegisterProfilePrefs(PrefRegistrySimple* registry);
  void Init(PrefService* pref_service);
  void LoadAndParseFileOnThreadPool(const std::string& file_path);
  void ReadConfigInfoFromPrefs();
  void UpdateLastOsPositionStr(const std::string& position,
                               const base::Value::Dict& storage_dict);
  std::optional<OSPositionPrefsInfo> GetLastOsPositionStr(
      const std::string& device_type);

 private:
  UAPushConfig();
  ~UAPushConfig() = default;
  void DidLoadConfigFromFile(std::optional<OSPositionScheme> os_position_str);
  void UpdateOsPositionStr(OSPositionScheme& os_position_str,
                           base::Value::Dict dict_val,
                           int position);
  base::Value::Dict GetSupplement(const std::string& device_type_val,
                                  const std::string& os_supplement);
  std::optional<OSPositionScheme> LoadConfigFromFile(
      const std::string& file_path);
  std::optional<OSPositionScheme> ParseOSCompatibleData(
      base::Value* os_compatible,
      const std::string& bundle_name);
  friend struct base::DefaultSingletonTraits<UAPushConfig>;
  base::raw_ptr<PrefService> pref_service_;
  scoped_refptr<base::SequencedTaskRunner> sequenced_task_runner_;
  std::map<std::string, OSPositionPrefsInfo> last_os_position_str_map_;
  mutable base::Lock lock_;
};
void UpdateUAPushConfigRule(const std::string& path);
}  // namespace ohos_user_agent
#endif  // CEF_LIBCEF_BROWSER_UA_PUSH_CONFIG_H_
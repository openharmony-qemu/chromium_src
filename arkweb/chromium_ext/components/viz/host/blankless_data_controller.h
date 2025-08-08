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

#ifndef BASE_OHOS_BLANKLESS_DATA_CONTROLLER_H_
#define BASE_OHOS_BLANKLESS_DATA_CONTROLLER_H_
#include <map>

#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "arkweb/ohos_adapter_ndk/distributeddatamgr_adapter/ohos_web_snapshot_data_base.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace base {
namespace ohos {

class BlanklessDataController {
public:
  static constexpr int64_t INVALID_PREF_HASH = -1LL;
  static BlanklessDataController& GetInstance();
  ~BlanklessDataController() = default;

  using SnapShotRect = struct {
    int x;
    int y;
    int w;
    int h;
  };
  using SnapshotInfo = struct {
    std::string path;
    SkBitmap bitmap;
    std::vector<double> pixels;
  };


  void DumpBlanklessSnapshot(base::ohos::BlanklessInfo&& info,
                             const SkBitmap& bitmap,
                             const std::vector<SnapShotRect>& quad_list);
  void ClearSnapshot(int64_t key);
  void ClearSnapshotDataItem(const std::vector<int64_t>& keys);
  void InsertSnapshotDataItem(int64_t key, const OHOS::NWeb::SnapshotDataItem& data);
  OHOS::NWeb::SnapshotDataItem GetSnapshotDataItem(int64_t key, int64_t pref_hash);
  int32_t SetBlanklessLoadingCacheCapacity(int capacity);
  int32_t GetBlanklessLoadingCacheCapacity() const;

private:
  BlanklessDataController();
  std::shared_ptr<SnapshotInfo> GetHistorySnapshotInfo(uint64_t blankless_key);

private:
  OHOS::NWeb::OhosWebSnapshotDataBase& dbInstance_;
  std::shared_ptr<OHOS::NWeb::OhosWebSnapshotDataBaseCallback> web_snapshot_db_callback_ = nullptr;

  std::unordered_map<int64_t, std::shared_ptr<SnapshotInfo>> last_info_;
  std::mutex last_info_mutex_;
};
}  // namespace ohos
}  // namespace base

#endif  // BASE_OHOS_BLANK_OPT_DATA_CONTROLLER_H_
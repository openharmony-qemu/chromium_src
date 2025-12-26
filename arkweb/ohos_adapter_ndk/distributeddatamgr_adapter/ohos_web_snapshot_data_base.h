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

#ifndef OHOS_WEB_SNAPSHOT_DATA_BASE_H
#define OHOS_WEB_SNAPSHOT_DATA_BASE_H

#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <database/rdb/relational_store.h>

namespace OHOS::NWeb {
struct SnapshotDataItem {
    std::string wholePath;
    std::string staticPath;
    double historySimilarity = 0.0;
    int32_t lcpTime = INT32_MAX;
    int64_t snapShotFileSize = 0;
    int64_t snapShotFileTime = 0;
    int64_t preferenceHash = 0;
    int32_t width = 0;
    int32_t height = 0;
};

class OhosWebSnapshotDataBaseCallback {
public:
    OhosWebSnapshotDataBaseCallback() = default;

    virtual ~OhosWebSnapshotDataBaseCallback() = default;

    virtual void OnDataDelete(const std::string& path) = 0;

    virtual void OnDataExist(const std::unordered_set<std::string>& existPaths) = 0;

    virtual void OnDataClear() = 0;
};

class OhosWebSnapshotDataBase {
public:
    static OhosWebSnapshotDataBase& GetInstance();
    ~OhosWebSnapshotDataBase();

    /**
     * @brief Clear the screenshot data in the database.
     *
     * @param blankless_keys Indicates list of url to be cleaned. If the size is 0, means to clean all data.
     */
    void ClearSnapshotDataItem(const std::vector<int64_t>& blankless_keys);
    bool InsertSnapshotDataItem(int64_t blankless_key, const SnapshotDataItem& data);
    bool InsertSnapshotDataItem(int64_t blankless_key, const SnapshotDataItem& data, int64_t expirationTime);
    SnapshotDataItem GetSnapshotDataItem(int64_t blankless_key);
    void RegisterDataBaseCallback(std::shared_ptr<OhosWebSnapshotDataBaseCallback> callback);
    int32_t SetBlanklessLoadingCacheCapacity(int32_t capacity);
    int32_t GetCapacityInByte();

private:
    OhosWebSnapshotDataBase();
    OhosWebSnapshotDataBase(const OhosWebSnapshotDataBase& other) = delete;
    OhosWebSnapshotDataBase& operator=(const OhosWebSnapshotDataBase&) = delete;

    struct DataBaseDataItem {
        SnapshotDataItem snapshotData;
        int64_t expirationTime = 0;
    };

    void GetAllInfo();
    void GetDatabaseInfo();
    void GetOrOpen(const OH_Rdb_Config& config);
    int64_t GetCurrentTime();
    int64_t GetOldestKey();

    void ClearInner();
    void ClearData();
    void ClearNotify();
    void ClearMap();

    void DeleteInner(int64_t blankless_key);
    void DeleteData(int64_t blankless_key);
    void DeleteNotify(int64_t blankless_key);
    void DeleteMap(int64_t blankless_key);

    void InsertInner(int64_t blankless_key, const DataBaseDataItem& data);
    void InsertData(int64_t blankless_key, const DataBaseDataItem& data);
    void InsertMap(int64_t blankless_key, const DataBaseDataItem& data);

    void UpdateCapacityData(int32_t capacity);

    OH_Rdb_Store* rdbStore_ = nullptr;
    std::mutex dataBaseMapMtx_;
    std::unordered_map<int64_t, DataBaseDataItem> dataBaseMap_;
    std::vector<std::shared_ptr<OhosWebSnapshotDataBaseCallback>> dataBaseDeleteCallbacks_;
    int32_t totalSnapShotFileBytes_ = 0;
    int32_t capacityInByte_;
};
} // namespace
#endif // OHOS_WEB_SNAPSHOT_DATA_BASE_H
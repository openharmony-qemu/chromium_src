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
    void InsertSnapshotDataItem(int64_t blankless_key, const SnapshotDataItem& data);
    SnapshotDataItem GetSnapshotDataItem(int64_t blankless_key);
    void RegisterDataBaseCallback(std::shared_ptr<OhosWebSnapshotDataBaseCallback> callback);
    int32_t SetBlanklessLoadingCacheCapacity(int32_t capacity);

private:
    OhosWebSnapshotDataBase();
    OhosWebSnapshotDataBase(const OhosWebSnapshotDataBase& other) = delete;
    OhosWebSnapshotDataBase& operator=(const OhosWebSnapshotDataBase&) = delete;

    struct DataBaseDataItem {
        SnapshotDataItem snapshotData;
        int64_t time;
    };

    void GetAllInfo();
    int64_t GetCurrentTime();
    void GetOrOpen(const OH_Rdb_Config& config);
    void InsertDataBaseDataItem(int64_t blankless_key, const DataBaseDataItem& data);
    void NotifyDataBaseDeletePath(const std::string& path);

    int32_t DataClear();
    int32_t DataDelete(int64_t blankless_key);
    int32_t DataInsert(int64_t blankless_key, const DataBaseDataItem& data);

    void DataMapClear();
    void DataMapErase(int64_t blankless_key);
    std::vector<int64_t> DataMapInsert(int64_t blankless_key, const DataBaseDataItem& data);
    bool DataMapUpdate(int64_t blankless_key, const DataBaseDataItem& newData);

    OH_Rdb_Store* rdbStore_;
    std::mutex dataBaseMapMtx_;
    std::unordered_map<int64_t, DataBaseDataItem> dataBaseMap_;
    std::vector<std::shared_ptr<OhosWebSnapshotDataBaseCallback>> dataBaseDeleteCallbacks_;
    uint64_t totalSnapShotFileBytes_;

    std::atomic<int32_t> capacityInByte_;
};
} // namespace
#endif // OHOS_WEB_SNAPSHOT_DATA_BASE_H
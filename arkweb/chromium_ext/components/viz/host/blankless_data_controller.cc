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

#include "blankless_data_controller.h"

#include <mutex>
#include "arkweb/chromium_ext/base/ohos/blankless/blankless_controller.h"
#include "arkweb/ohos_adapter_ndk/window_manager_adapter/window_manager_adapter_impl.h"
#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/time/time.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/skia/include/core/SkAlphaType.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/codec/SkCodec.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/encode/SkPngEncoder.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/core/SkUnPreMultiply.h"
#include "third_party/zlib/zlib.h"

using namespace OHOS::NWeb;

namespace base {
namespace ohos {
const base::FilePath::CharType DUMP_FILE_PATH[] = FILE_PATH_LITERAL("snapshot");
const std::string DATABASE_DIR = "/data/storage/el2/base/cache/web";
const std::string DUMP_FILE_PRE = "/web_frame_";
const std::string DUMP_FILE_TYPE = ".png";
const double SSIM_THRESHOLD = 0.95;
const int DUMP_TASK_DELAY_TIME = 1000; // Milliseconds

static double Mean(const std::vector<double>& data) {
  if (data.size() == 0) {
    LOG(DEBUG) << "blankless Mean data size 0";
    return 0;
  }
  double sum = 0.0;
  for (double value : data) {
    sum += value;
  }
  return sum / data.size();
}

static double Variance(const std::vector<double>& data, double mean_value) {
  if (data.size() == 0) {
    LOG(DEBUG) << "blankless Variance data size 0";
    return 0;
  }
  double sum = 0.0;
  for (double value : data) {
    sum += (value - mean_value) * (value - mean_value);
  }
  return sum / data.size();
}

static double Covariance(const std::vector<double>& data1,
                         double mean1,
                         const std::vector<double>& data2,
                         double mean2) {
  if (data1.size() == 0) {
    LOG(DEBUG) << "blankless Covariance data1 size 0";
    return 0;
  }
  double sum = 0.0;
  for (size_t i = 0; i < data1.size(); ++i) {
    sum += (data1[i] - mean1) * (data2[i] - mean2);
  }
  return sum / data1.size();
}

static std::vector<double> GetSnapshotPixels(const uint32_t* addr, uint64_t size, bool needsUnpremul) {
  std::vector<double> pixels;
  pixels.resize(size);
  for (uint64_t index = 0; index < size; ++index) { // size will not exceed 8000 * 8000
    uint32_t color = addr[index];
    if (needsUnpremul) {
      color = SkUnPreMultiply::PMColorToColor(color);
    }
    // 灰度值计算公式0.299、0.587、0.114是RGB各通道的权重
    pixels[index] = 0.299 * SkColorGetR(color) + 0.587 * SkColorGetG(color) + 0.114 * SkColorGetB(color);
  }
  return pixels;
}

static double CalculateSnapshotSimilarity(std::shared_ptr<BlanklessDataController::SnapshotInfo>& snapshotInfo,
                                          std::vector<double>& pixels1, std::vector<double>& pixels2) {
  if (pixels1.size() != pixels2.size()) {
    LOG(ERROR) << "blankless old pixels size != new pixels size";
    return 0.0;
  }

  static const double C1 = (0.01 * 255) * (0.01 * 255);
  static const double C2 = (0.03 * 255) * (0.03 * 255);

  double mean1 = snapshotInfo->mean;
  double mean2 = Mean(pixels2);
  double var1 = snapshotInfo->var;
  double var2 = Variance(pixels2, mean2);
  double cov12 = Covariance(pixels1, mean1, pixels2, mean2);
  // Avoid the calculation result being negative.
  cov12 = std::max(cov12, 0.0);

  double numerator = (2 * mean1 * mean2 + C1) * (2 * cov12 + C2);
  double denominator = (mean1 * mean1 + mean2 * mean2 + C1) * (var1 + var2 + C2);

  if (denominator == 0) {
    return 0.0;
  }

  return numerator / denominator;
}

static bool LoadBitmap(const char* path, SkBitmap& bitmap) {
  sk_sp<SkData> data = SkData::MakeFromFileName(path);
  if (!data) {
    return false;
  }
  std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(data);
  if (!codec) {
    return false;
  }

  SkImageInfo info = codec->getInfo();
  bitmap.allocPixels(info);
  if (codec->getPixels(info, bitmap.getPixels(), bitmap.rowBytes()) != SkCodec::kSuccess) {
    bitmap.reset();
    return false;
  }
  return true;
}

static std::string GetDumpFilePath() {
  base::FilePath cache_path;
  base::PathService::Get(base::DIR_CACHE, &cache_path);
  if (cache_path.empty()) {
    return std::string();
  }
  base::FilePath dump_cache_path = cache_path.Append(DUMP_FILE_PATH);
  if (!base::PathExists(dump_cache_path)) {
    LOG(DEBUG) << "blankless create snapshot directory not exist: " << dump_cache_path.value();
    if (!base::CreateDirectory(dump_cache_path)) {
      LOG(ERROR) << "blankless create snapshot directory failed: " << dump_cache_path.value();
      return std::string();
    }
  }
  return dump_cache_path.value();
}

static bool EncodeSnapShotImage(const SkBitmap& bitmap, SkDynamicMemoryWStream& stream)
{
  SkPngEncoder::Options opts;
  opts.fFilterFlags = SkPngEncoder::FilterFlag::kAll;
  opts.fZLibLevel = 6; // 6 is the default compression ratio for PNG
  return SkPngEncoder::Encode(&stream, bitmap.pixmap(), opts);
}

static bool SaveImage(std::string& filename, SkDynamicMemoryWStream& stream) {
  std::string dumpFilePath = GetDumpFilePath();
  if (dumpFilePath.empty()) {
    LOG(ERROR) << "blankless get dumpFilePath error!";
    return false;
  }

  sk_sp<SkData> skData = stream.detachAsData();
  if (!skData) {
    LOG(ERROR) << "blankless snapshot stream data is invalid!";
    return false;
  }

  filename.append(dumpFilePath);
  filename.append(DUMP_FILE_PRE);
  filename.append(base::NumberToString(base::TimeTicks::Now().since_origin().InMicroseconds()));
  filename.append(DUMP_FILE_TYPE);
  base::FilePath filePath(filename);
  std::string_view sv(static_cast<const char*>(skData->data()), skData->size());
  bool res = base::WriteFile(filePath, sv);
  if (!res) {
    LOG(ERROR) << "blankless write snapshot file error!";
  } else {
    LOG(DEBUG) << "blankless save snapshot img:" << filename.c_str();
  }
  return res;
}

static bool GetSnapShotFileInfo(const std::string& filename, int64_t& snapShotFileSize, int64_t& snapShotFileTime)
{
  base::FilePath filePath(filename);

  base::File file(filePath, base::File::FLAG_OPEN | base::File::FLAG_READ);
  if (!file.IsValid()) {
    LOG(ERROR) << "blankless GetSnapShotFileInfo file.IsValid() false! ";
    return false;
  }
  base::File::Info fileInfo;
  if (!file.GetInfo(&fileInfo)) {
    LOG(ERROR) << "blankless GetSnapShotFileInfo GetInfo failed! ";
    return false;
  }
  snapShotFileSize = fileInfo.size;
  snapShotFileTime = fileInfo.last_modified.ToTimeT();
  LOG(DEBUG) << "blankless GetSnapShotFileInfo size " << snapShotFileSize << ", time " << snapShotFileTime;
  return true;
}

class OhosWebSnapshotDataBaseCallbackImpl : public OHOS::NWeb::OhosWebSnapshotDataBaseCallback
{
 public:
  void OnDataDelete(const std::string& path) override {
    base::FilePath dataPath(path);
    if (!base::DeleteFile(dataPath)) {
      LOG(ERROR) << "blankless delete snapshot failed:" << path;
    }
  }
  void OnDataExist(const std::unordered_set<std::string>& existPaths) override {
    base::FilePath dump_cache_path;
    if (!GetSnapshotDir(&dump_cache_path)) {
      LOG(ERROR) << "blankless OnDataClear get dump cache path failed";
      return;
    }
    base::FileEnumerator file_enum(dump_cache_path,
                                   true,
                                   base::FileEnumerator::FILES);
    for (base::FilePath path = file_enum.Next(); !path.empty();
       path = file_enum.Next()) {
      if (existPaths.find(path.AsUTF8Unsafe()) != existPaths.end()) {
        continue;
      }
      OnDataDelete(path.value());
    }
  }
  void OnDataClear() override {
    base::FilePath dump_cache_path;
    if (!GetSnapshotDir(&dump_cache_path)) {
      LOG(ERROR) << "blankless OnDataClear get dump cache path failed";
      return;
    }
    if (base::PathExists(dump_cache_path)) {
      if (!base::DeletePathRecursively(dump_cache_path)) {
        LOG(ERROR) << "blankless delete snapshot directory failed: " << dump_cache_path.value();
        return;
      }
    }
  }
  bool GetSnapshotDir(base::FilePath* dump_cache_path) {
    base::FilePath cache_path;
    base::PathService::Get(base::DIR_CACHE, &cache_path);
    if (cache_path.empty()) {
      LOG(ERROR) << "blankless get cache path failed";
      return false;
    }
    *dump_cache_path = cache_path.Append(DUMP_FILE_PATH);
    return true;
  }
};

BlanklessDataController& BlanklessDataController::GetInstance()
{
    static BlanklessDataController instance;
    return instance;
}

BlanklessDataController::BlanklessDataController() : dbInstance_(OHOS::NWeb::OhosWebSnapshotDataBase::GetInstance())
{
    base::FilePath databaseDir(DATABASE_DIR);
    if (!base::PathExists(databaseDir)) {
      base::CreateDirectory(databaseDir);
    }
    dbInstance_.Init(DATABASE_DIR.c_str());
    web_snapshot_db_callback_ = std::make_shared<OhosWebSnapshotDataBaseCallbackImpl>();
    if (web_snapshot_db_callback_) {
      dbInstance_.RegisterDataBaseCallback(web_snapshot_db_callback_);
    }
    task_manager_ = std::make_unique<viz::CancelableDelayedTaskManager>();
}

std::shared_ptr<BlanklessDataController::SnapshotInfo> BlanklessDataController::GetHistorySnapshotInfo(
  uint64_t blankless_key)
{
  std::shared_ptr<BlanklessDataController::SnapshotInfo> snapshotInfo = nullptr;
  std::lock_guard<std::mutex> last_info_guard(last_info_mutex_);
  if (auto it = last_info_.find(blankless_key); it != last_info_.end()) {
    snapshotInfo = it->second;
  }
  if (snapshotInfo == nullptr) {
    auto snapshotDataItem = dbInstance_.GetSnapshotDataItem(blankless_key);
    snapshotInfo = std::make_shared<SnapshotInfo>();
    snapshotInfo->path = snapshotDataItem.wholePath;
    SkBitmap bitmap;
    if (!snapshotDataItem.wholePath.empty() && LoadBitmap(snapshotDataItem.wholePath.c_str(), bitmap)) {
      const uint32_t* addr = bitmap.pixmap().addr32();
      size_t size = bitmap.width() * bitmap.height();
      bool needsUnpremul = SkAlphaType::kPremul_SkAlphaType == bitmap.alphaType();
      snapshotInfo->bitmap = std::move(bitmap);
      snapshotInfo->pixels = GetSnapshotPixels(addr, size, needsUnpremul);
      snapshotInfo->mean = Mean(snapshotInfo->pixels);
      snapshotInfo->var = Variance(snapshotInfo->pixels, snapshotInfo->mean);
    }
    last_info_.emplace(blankless_key, snapshotInfo);
  }
  return snapshotInfo;
}

#if BUILDFLAG(ARKWEB_BLANK_OPTIMIZE)
void BlanklessDataController::DumpTask(viz::mojom::BlanklessSendInfoPtr infoPtr, mojo::ScopedSharedBufferHandle buffer,
                                       viz::mojom::BlanklessBitmapMetadataPtr metadata, double similarity)
{
  auto& instance = base::ohos::BlanklessController::GetInstance();
  uint64_t recorded_time = instance.GetSystemTime(infoPtr->nweb_id, infoPtr->blankless_key);
  if (infoPtr->system_time <= recorded_time || (infoPtr->system_time - recorded_time) > INT32_MAX) {
    LOG(ERROR) << "blankless corrected loading time error. nweb_id: " << infoPtr->nweb_id
               << ", blankless_key: " << infoPtr->blankless_key
               << ", system_time: " << infoPtr->system_time
               << ", recorded_time: " << recorded_time;
    return;
  }
  int32_t corrected_time = static_cast<int32_t>(infoPtr->system_time - recorded_time);
  LOG(DEBUG) << "blankless corrected loading time: " << corrected_time << ", nweb_id: " << infoPtr->nweb_id
             << ", blankless_key: " << infoPtr->blankless_key;

  mojo::ScopedSharedBufferMapping mapping = buffer->Map(metadata->size);
  if (!mapping) {
    LOG(ERROR) << "blankless DumpTask failed to map shared buffer";
    return;
  }
  SkImageInfo info = SkImageInfo::Make(metadata->width, metadata->height,
                                       static_cast<SkColorType>(metadata->color_type),
                                       static_cast<SkAlphaType>(metadata->alpha_type));
  SkBitmap bitmap;
  if (!bitmap.installPixels(info, mapping.get(), info.minRowBytes())) {
    LOG(ERROR) << "blankless DumpTask failed to install pixels into bitmap";
    return;
  }

  std::string newFile;
  // Record the time when the snapshot is written to the database to determine if there is a new snapshot written to
  // the database during this load.
  auto dump_time = base::Time::Now().ToInternalValue() / base::Time::kMicrosecondsPerMillisecond;
  instance.RecordDumpTime(infoPtr->nweb_id, infoPtr->blankless_key, dump_time);
  SnapshotDataItem snapshotDataItem = {
    .wholePath = "",
    .staticPath = "",
    .lcpTime = corrected_time,
    .snapShotFileSize = 0LL,
    .snapShotFileTime = 0LL,
    .preferenceHash = infoPtr->pref_hash,
    .width = infoPtr->width,
    .height = infoPtr->height,
  };
  
  SkDynamicMemoryWStream stream;
  if (!EncodeSnapShotImage(bitmap, stream)) {
    LOG(ERROR) << "blankless encode snapShot image failed!";
    return;
  }
  if (stream.bytesWritten() > dbInstance_.GetCapacityInByte()) {
    LOG(ERROR) << "blankless no capacity to save img";
    return;
  }
  if (!SaveImage(newFile, stream)) {
    LOG(ERROR) << "blankless save snapshot img failed!";
    return;
  }
  int64_t snapShotFileSize = 0LL;
  int64_t snapShotFileTime = 0LL;
  if (!GetSnapShotFileInfo(newFile, snapShotFileSize, snapShotFileTime)) {
    LOG(WARNING) << "blankless GetSnapShotFileInfo failed! filename " << newFile;
  }

  if (similarity >= 0) {
    snapshotDataItem.historySimilarity = similarity;
    snapshotDataItem.staticPath = newFile;
  }
  OhosWebSnapshotDataBase::GetInstance().InsertSnapshotDataItem(infoPtr->blankless_key, snapshotDataItem);
}

void BlanklessDataController::DumpBlanklessSnapshot(viz::mojom::BlanklessSendInfoPtr infoPtr,
                                                    mojo::ScopedSharedBufferHandle buffer,
                                                    viz::mojom::BlanklessBitmapMetadataPtr metadata)
{
  uint64_t blankless_key = infoPtr->blankless_key;
  uint32_t nweb_id = infoPtr->nweb_id;
  auto& instance = base::ohos::BlanklessController::GetInstance();
  auto window_id = instance.GetWindowIdByNWebId(nweb_id);
  auto is_private = OHOS::NWeb::WindowManagerAdapterImpl::GetInstance().GetWindowPrivacyMode(window_id);
  if (is_private) {
    LOG(DEBUG) << "blankless this is a private window: "<< window_id;
    ClearSnapshot(blankless_key);
    ClearSnapshotDataItem({blankless_key});
    return;
  }

  std::shared_ptr<SnapshotInfo> snapshotInfo = GetHistorySnapshotInfo(blankless_key);
  double similarity = -1.0f;
  size_t size = metadata->size >> 2;
  if (!snapshotInfo || snapshotInfo->path.size() == 0 || snapshotInfo->bitmap.empty() ||
      snapshotInfo->bitmap.width() == metadata->width && snapshotInfo->bitmap.height() == metadata->height &&
      snapshotInfo->pixels.size() == size) {
    mojo::ScopedSharedBufferMapping mapping = buffer->Map(metadata->size);
    if (!mapping) {
      LOG(ERROR) << "blankless DumpBlanklessSnapshot failed to map shared buffer.";
      return;
    }
    const uint32_t* addr = static_cast<uint32_t*>(mapping.get());
    bool needsUnpremul = SkAlphaType::kPremul_SkAlphaType == metadata->alpha_type;
    std::vector<double> pixelsNew = GetSnapshotPixels(addr, size, needsUnpremul);
    similarity = CalculateSnapshotSimilarity(snapshotInfo, snapshotInfo->pixels, pixelsNew);
    LOG(DEBUG) << "blankless CalculateSimilarity nweb_id: " << nweb_id
               << ", blankless_key: " << blankless_key << ", similarity: " << similarity;
    if (similarity >= base::ohos::BlanklessController::CALLBACK_SIMILARITY_THRESHOLD) {
      instance.CancelFrameInsertCallback(nweb_id, blankless_key);
      instance.FireFrameRemoveCallback(nweb_id, blankless_key);
    }
  }

   if (task_manager_) {
     auto task = base::BindOnce(&BlanklessDataController::DumpTask,
                                std::move(infoPtr), std::move(buffer), std::move(metadata), similarity);
     task_manager_->PostNewDelayedTask(blankless_key, std::move(task), base::Milliseconds(DUMP_TASK_DELAY_TIME));
   }
}
#endif

void BlanklessDataController::ClearSnapshot(int64_t blankless_key)
{
  LOG(DEBUG) << "blankless clear last snapshot info";
  std::lock_guard<std::mutex> last_info_guard(last_info_mutex_);
  last_info_.erase(blankless_key);
}

void BlanklessDataController::ClearSnapshotDataItem(const std::vector<int64_t>& blankless_keys)
{
  dbInstance_.ClearSnapshotDataItem(blankless_keys);
}

void BlanklessDataController::InsertSnapshotDataItem(int64_t blankless_key, const SnapshotDataItem& data)
{
  dbInstance_.InsertSnapshotDataItem(blankless_key, data);
}

SnapshotDataItem BlanklessDataController::GetSnapshotDataItem(int64_t blankless_key, int64_t pref_hash)
{
  auto item = dbInstance_.GetSnapshotDataItem(blankless_key);
  // check preferenceHash
  if (item.preferenceHash != pref_hash) {
    LOG(ERROR) << "BlanklessDataController::GetSnapshotDataItem error! mismatch preferenceHash "
      << item.preferenceHash << ", " << pref_hash;
    return SnapshotDataItem{};
  }
  // check snapshotFileInfo
  int64_t snapShotFileSize = 0LL;
  int64_t snapShotFileTime = 0LL;
  if (!GetSnapShotFileInfo(item.staticPath, snapShotFileSize, snapShotFileTime)) {
    LOG(ERROR) << "BlanklessDataController::GetSnapshotDataItem error! GetSnapShotFileInfo error "
      << item.staticPath;
    return SnapshotDataItem{};
  }
  if (item.snapShotFileSize != snapShotFileSize ||
      item.snapShotFileTime != snapShotFileTime) {
    LOG(ERROR) << "BlanklessDataController::GetSnapshotDataItem error! mismatch snapShotFileSize "
      << item.snapShotFileSize << ", " << snapShotFileSize << ", snapShotFileTime "
      << item.snapShotFileTime << ", " << snapShotFileTime;
    return SnapshotDataItem{};
  }

  return item;
}

int32_t BlanklessDataController::SetBlanklessLoadingCacheCapacity(int capacity)
{
  return dbInstance_.SetBlanklessLoadingCacheCapacity(capacity);
}

int32_t BlanklessDataController::GetBlanklessLoadingCacheCapacity() const
{
  return dbInstance_.GetCapacityInByte();
}

void BlanklessDataController::PostDumpTaskWithDelay(uint64_t blankless_key, base::OnceClosure task)
{
  if (task_manager_) {
    task_manager_->PostNewDelayedTask(blankless_key, std::move(task), base::Milliseconds(DUMP_TASK_DELAY_TIME));
  }
}
}  // namespace ohos
}  // namespace base
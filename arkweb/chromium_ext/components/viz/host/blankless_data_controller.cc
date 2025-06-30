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
#include "base/files/file_enumerator.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/logging.h"
#include "base/path_service.h"
#include "base/strings/string_number_conversions.h"
#include "base/time/time.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/core/SkStream.h"
#include "third_party/skia/include/encode/SkPngEncoder.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"
#include "third_party/skia/include/codec/SkCodec.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/zlib/zlib.h"

using namespace OHOS::NWeb;

namespace base {
namespace ohos {
const base::FilePath::CharType DUMP_FILE_PATH[] = FILE_PATH_LITERAL("snapshot");
const std::string DUMP_FILE_PRE = "/web_frame_";
const std::string DUMP_FILE_TYPE = ".png";
const double kSSIMthreshold = 0.95;
const double kSimilaritythreshold = 0.33;


static double Mean(const std::vector<double>& data) {
  double sum = 0.0;
  for (double value : data) {
    sum += value;
  }
  return sum / data.size();
}

static double Variance(const std::vector<double>& data, double mean_value) {
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
  double sum = 0.0;
  for (size_t i = 0; i < data1.size(); ++i) {
    sum += (data1[i] - mean1) * (data2[i] - mean2);
  }
  return sum / data1.size();
}

static double CalculateSSIM(const std::vector<double>& img1, const std::vector<double>& img2, const int depth) {
  double C1 = (0.01 * depth) * (0.01 * depth);
  double C2 = (0.03 * depth) * (0.03 * depth);

  double mean1 = Mean(img1);
  double mean2 = Mean(img2);
  double var1 = Variance(img1, mean1);
  double var2 = Variance(img2, mean2);
  double cov12 = Covariance(img1, mean1, img2, mean2);

  double numerator = (2 * mean1 * mean2 + C1) * (2 * cov12 + C2);
  double denominator = (mean1 * mean1 + mean2 * mean2 + C1) * (var1 + var2 + C2);

  return numerator / denominator;
}

static void AdjustColorChannels(SkBitmap& bitmap) {
    for (int y = 0; y < bitmap.height(); ++y) {
        for (int x = 0; x < bitmap.width(); ++x) {
            uint32_t* pixel = bitmap.getAddr32(x, y);
            uint8_t r = (*pixel & 0x00FF0000) >> 16; // Extract Red   (bits 16-23)
            uint8_t g = (*pixel & 0x0000FF00) >> 8; // Extract Green (bits 8-15)
            uint8_t b = (*pixel & 0x000000FF); // Extract Blue  (bits 0-7)
            uint8_t a = (*pixel & 0xFF000000) >> 24; // Extract Alpha (bits 24-31)
            // Reassemble pixel with swapped Red/Blue channels:
            // - Keep Alpha in highest byte (shift left 24 bits)
            // - Place original Blue in Red's position (shift left 16 bits)
            // - Keep Green in middle position (shift left 8 bits)
            // - Place original Red in Blue's position (no shift)
            *pixel = (a << 24) | (b << 16) | (g << 8) | r;
        }
    }
}

static SkBitmap DownscaleToLowRes(const SkBitmap& srcBitmap, int newWidth, int newHeight) {
    SkBitmap downscaledBitmap;
    SkImageInfo info = srcBitmap.info().makeWH(newWidth, newHeight);
    downscaledBitmap.allocPixels(info);

    SkCanvas canvas(downscaledBitmap);
    SkPaint paint;
    paint.setAntiAlias(true);

    // 使用双线性过滤缩小图像
    SkSamplingOptions sampling(SkFilterMode::kLinear);

    sk_sp<SkImage> srcImage = SkImages::RasterFromBitmap(srcBitmap);
    canvas.drawImageRect(
        srcImage,
        SkRect::MakeIWH(srcBitmap.width(), srcBitmap.height()),
        SkRect::MakeIWH(newWidth, newHeight),
        sampling,
        &paint,
        SkCanvas::kFast_SrcRectConstraint);

    return downscaledBitmap;
}

static std::vector<double> GetSnapshotPixels(const SkPixmap& pixmap) {
  int width = pixmap.width();
  int height = pixmap.height();
  std::vector<double> pixels;
  pixels.resize(width * height);
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      const SkColor color = pixmap.getColor(x, y);
      double gray = 0.299 * SkColorGetR(color) + 0.587 * SkColorGetG(color) + 0.114 * SkColorGetB(color);
      pixels[y * width + x] = gray;
    }
  }
  return pixels;
}

static double CalculateSnapshotSimilarity(std::vector<double>& pixels1,
                                   std::vector<double>& pixels2,
                                   int width,
                                   int height,
                                   const std::vector<BlanklessDataController::SnapShotRect>& quad_list,
                                   const int depth) {
  if (pixels1.size() != pixels2.size()) {
    LOG(ERROR) << "blankless old pixels size != new pixels size";
    return 0;
  }

  std::vector<double> samePixels;
  samePixels.resize(width * height, 0);
  int samePixelsNum = 0;
  for (int i = 0; i < quad_list.size(); i++) {
    int rectWidth = quad_list[i].w / 2;
    int rectHeight = quad_list[i].h / 2;
    if (rectWidth <= 0 || rectHeight <= 0) {
      continue;
    }
    int x0 = quad_list[i].x / 2;
    int y0 = quad_list[i].y / 2;
    if (x0 < 0) {
      x0 = 0;
    }
    if (y0 < 0) {
      y0 = 0;
    }
    std::vector<double> rectPixels1;
    std::vector<double> rectPixels2;
    rectPixels1.resize(rectWidth * rectHeight);
    rectPixels2.resize(rectWidth * rectHeight);
    for (int y = y0; y < height && y < y0 + rectHeight; y++) {
      for (int x = x0; x < width && x < x0 + rectWidth; x++) {
        rectPixels1[(y - y0) * rectWidth + x - x0] = pixels1[y * width + x];
        rectPixels2[(y - y0) * rectWidth + x - x0] = pixels2[y * width + x];
      }
    }
    double SSIM = CalculateSSIM(rectPixels1, rectPixels2, depth);
    if (SSIM < kSSIMthreshold) {
      continue;
    }
    for (int y = y0; y < height && y < y0 + rectHeight; y++) {
      for (int x = x0; x < width && x < x0 + rectWidth; x++) {
        samePixelsNum += (samePixels[y * width + x] == 0);
        samePixels[y * width + x] = 1;
      }
    }
    LOG(DEBUG) << "blankless CalculateSnapshotSimilarity SSIM[" << i << "]=" << SSIM;
  }

  double percent = (double)samePixelsNum / (double)(width * height);
  LOG(DEBUG) << "blankless CalculateSnapshotSimilarity percent:" << percent;
  return percent;
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

static bool SaveImage(const SkBitmap& bitmap, std::string& filename) {
  std::string dumpFilePath = GetDumpFilePath();
  if (dumpFilePath.empty()) {
    LOG(ERROR) << "blankless get dumpFilePath error!";
    return false;
  }
  filename.append(dumpFilePath);
  filename.append(DUMP_FILE_PRE);
  filename.append(base::NumberToString(base::TimeTicks::Now().since_origin().InMicroseconds()));
  filename.append(DUMP_FILE_TYPE);
  SkFILEWStream fileStream(filename.c_str());
  SkPngEncoder::Options opts;
  opts.fFilterFlags = SkPngEncoder::FilterFlag::kAll;
  opts.fZLibLevel = 6; //6为png默认压缩率
  bool res = SkPngEncoder::Encode(&fileStream, bitmap.pixmap(), opts);
  if (!res) {
    LOG(ERROR) << "blankless save snapshot img error!";
  }
  LOG(DEBUG) << "blankless save snapshot img:" << filename.c_str();
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
    if (base::PathExists(dataPath)) {
      if (!base::DeleteFile(dataPath)) {
        LOG(ERROR) << "blankless delete snapshot failed:" << path;
      }
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

BlanklessDataController::BlanklessDataController()
  :dbInstance_(OHOS::NWeb::OhosWebSnapshotDataBase::GetInstance())
{
    web_snapshot_db_callback_ = std::make_shared<OhosWebSnapshotDataBaseCallbackImpl>();
    if (web_snapshot_db_callback_) {
      dbInstance_.RegisterDataBaseCallback(web_snapshot_db_callback_);
    }
}

std::shared_ptr<BlanklessDataController::SnapshotInfo> BlanklessDataController::GetHistorySnapshotInfo(
  uint64_t blankless_key)
{
  std::shared_ptr<BlanklessDataController::SnapshotInfo> snapshotInfo = nullptr;
  std::lock_guard<std::mutex> last_info_guard(last_info_mutex_);
  if (auto it = last_info_.find(blankless_key); it != last_info_.end()) {
    snapshotInfo = it->second;
  }
  auto snapshotDataItem = OHOS::NWeb::OhosWebSnapshotDataBase::GetInstance().GetSnapshotDataItem(blankless_key);
  if (snapshotInfo == nullptr && !snapshotDataItem.wholePath.empty()) {
    SkBitmap bitmap;
    if (LoadBitmap(snapshotDataItem.wholePath.c_str(), bitmap)) {
      snapshotInfo = std::make_shared<SnapshotInfo>();
      snapshotInfo->bitmap = std::move(bitmap);
      snapshotInfo->pixels = GetSnapshotPixels(snapshotInfo->bitmap.pixmap());
      snapshotInfo->path = snapshotDataItem.wholePath;
      last_info_.emplace(blankless_key, snapshotInfo);
    }
  }
  return snapshotInfo;
}

void BlanklessDataController::DumpBlanklessSnapshot(int64_t blankless_key,
                                                    int64_t lcp_time,
                                                    int64_t pref_hash,
                                                    const SkBitmap& bitmap,
                                                    const std::vector<SnapShotRect>& quad_list)
{
  SkBitmap bitmapNew = DownscaleToLowRes(bitmap, bitmap.width() / 2, bitmap.height() / 2);
  std::string newFile;
  if (!SaveImage(bitmapNew, newFile)) {
    LOG(ERROR) << "blankless save snapshot img failed!";
    return;
  }
  int64_t snapShotFileSize = 0LL;
  int64_t snapShotFileTime = 0LL;
  if (!GetSnapShotFileInfo(newFile, snapShotFileSize, snapShotFileTime)) {
    LOG(WARNING) << "blankless GetSnapShotFileInfo failed! filename " << newFile;
  }
  OHOS::NWeb::SnapshotDataItem snapshotDataItem = {
    .wholePath = newFile,
    .staticPath = "",
    .historySimilarity = 0.0f,
    .lcpTime = lcp_time,
    .snapShotFileSize = snapShotFileSize,
    .snapShotFileTime = snapShotFileTime,
    .preferenceHash = pref_hash
  };
  std::shared_ptr<SnapshotInfo> snapshotInfo = GetHistorySnapshotInfo(blankless_key);
  if (!snapshotInfo || snapshotInfo->path.size() == 0 || snapshotInfo->bitmap.empty() ||
      snapshotInfo->bitmap.width() != bitmapNew.width() || snapshotInfo->bitmap.height() != bitmapNew.height() ||
      snapshotInfo->pixels.size() == 0) {
    LOG(DEBUG) << "blankless last snapshot error";
    dbInstance_.InsertSnapshotDataItem(blankless_key, snapshotDataItem);
    return;
  }

  std::vector<double> pixelsNew = GetSnapshotPixels(bitmapNew.pixmap());
  // bitmapNew.bytesPerPixel() * 8为SSIM计算公式中动态范围参数L近似计算
  double similarity = CalculateSnapshotSimilarity(snapshotInfo->pixels, pixelsNew, bitmapNew.width(),
                                                  bitmapNew.height(), quad_list, bitmapNew.bytesPerPixel() * 8);
  snapshotDataItem.historySimilarity = similarity;
  LOG(DEBUG) << "blankless Insert Snapshot: " << newFile << " " << similarity;
  if (similarity > kSimilaritythreshold) {
    snapshotDataItem.staticPath = newFile;
  }
  dbInstance_.InsertSnapshotDataItem(blankless_key, snapshotDataItem);
}

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

}  // namespace ohos
}  // namespace base
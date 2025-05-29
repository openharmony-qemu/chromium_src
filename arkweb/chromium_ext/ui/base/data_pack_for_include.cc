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

#ifndef UI_BASE_RESOURCE_DATA_PACK_H_
#error "must be in include form UI_BASE_RESOURCE_DATA_PACK_H_"
#endif

#include "arkweb/build/features/features.h"

#include <unordered_map>
#include "base/command_line.h"
#include "third_party/ohos_ndk/includes/ohos_adapter/ohos_adapter_helper.h"

#include "arkweb/chromium_ext/ui/base/data_pack_for_include.h"

namespace {

bool MmapHasGzipHeader(const base::MemoryMappedFile* mmap);

// We're crashing when trying to load a pak file on Windows.  Add some error
// codes for logging.
// http://crbug.com/58056
// These values are logged to UMA. Entries should not be renumbered and
// numeric values should never be reused. Keep in sync with "DataPackLoadErrors"
// in src/tools/metrics/histograms/enums.xml.
enum LoadErrors {
  INIT_FAILED_OBSOLETE = 1,
  BAD_VERSION,
  INDEX_TRUNCATED,
  ENTRY_NOT_FOUND,
  HEADER_TRUNCATED,
  WRONG_ENCODING,
  INIT_FAILED_FROM_FILE,
  UNZIP_FAILED,
  OPEN_FAILED,
  MAP_FAILED,

  LOAD_ERRORS_COUNT,
};

std::unordered_map<ui::ResourceScaleFactor, std::string> kPakFileNameHapMap = {
    {ui::ResourceScaleFactor::kScaleFactorNone,
     "resources/rawfile/resources.pak"},
    {ui::ResourceScaleFactor::k100Percent,
     "resources/rawfile/chrome_100_percent.pak"},
    {ui::ResourceScaleFactor::k200Percent,
     "resources/rawfile/chrome_200_percent.pak"}};

bool GetPathFromHap(ui::ResourceScaleFactor factor,
                    const base::FilePath& path,
                    std::string& pathHap) {
  auto iter = kPakFileNameHapMap.find(factor);
  if (iter == kPakFileNameHapMap.end()) {
    LOG(ERROR) << "kPakFileNameHapMap not find path: " << path;
    return false;
  }
  std::string pathStr = path.MaybeAsASCII();
  if (pathStr.find("zh-CN.pak") != std::string::npos) {
    pathHap = "resources/rawfile/locales/zh-CN.pak";
  } else if (pathStr.find("en-US.pak") != std::string::npos) {
    pathHap = "resources/rawfile/locales/en-US.pak";
  } else if (pathStr.find("bo-CN.pak") != std::string::npos) {
    pathHap = "resources/rawfile/locales/bo-CN.pak";
  } else if (pathStr.find("ug.pak") != std::string::npos) {
    pathHap = "resources/rawfile/locales/ug.pak";
  } else if (pathStr.find("zh-TW.pak") != std::string::npos) {
    pathHap = "resources/rawfile/locales/zh-TW.pak";
  } else if (pathStr.find("zh-HK.pak") != std::string::npos) {
    pathHap = "resources/rawfile/locales/zh-HK.pak";
  } else {
    pathHap = iter->second;
  }
  return true;
}
}

namespace ui {

bool DataPackUtil::LoadFromPathExt(raw_ptr<DataPack> dataPackObj, const base::FilePath& path) {
  std::string pathHap;
  if (GetPathFromHap(dataPackObj->resource_scale_factor_, path, pathHap)) {
    auto resourceInstance =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().GetResourceAdapter();

    std::shared_ptr<OHOS::NWeb::OhosFileMapper> fileMapper =
      resourceInstance->GetRawFileMapper(pathHap, true);

    if (!fileMapper) {
      LOG(ERROR) << "DataPack::LoadFromPath couldn't data file: "
                  << pathHap.c_str();
      return false;
    }

    LOG(INFO) << "DataPack::LoadFromPath " << pathHap.c_str()
              << ", data file length: " << fileMapper->GetDataLen();

    std::unique_ptr<base::MemoryMappedFile> mmap =
        std::make_unique<base::MemoryMappedFile>();
    mmap->SetOhosFileMapper(fileMapper);
    if (MmapHasGzipHeader(mmap.get())) {
      std::string_view compressed(reinterpret_cast<char*>(mmap->data()),
                                    mmap->length());
      std::string data;
      if (!compression::GzipUncompress(compressed, &data)) {
        LOG(ERROR) << "Failed to unzip compressed datapack: "
                    << pathHap.c_str();

        return false;
      }
      return dataPackObj->LoadImpl(std::make_unique<DataPack::StringDataSource>(std::move(data)));;
    }
    return dataPackObj->LoadImpl(std::make_unique<DataPack::MemoryMappedDataSource>(std::move(mmap)));;
  } else {
    LOG(ERROR) << "LoadFromPath failed file not exist";
    return false;
  }
}

}  // namespace ui

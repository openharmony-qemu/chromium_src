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
#include <unordered_set>
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

static std::unordered_set<std::string> supportLocaleList = {
#ifdef WEARABLE_SUPPORT_MULTI_LANGUAGE
  "ar.pak",
  "be.pak",
  "bg.pak",
  "bn.pak",
  "cs.pak",
  "da.pak",
  "de.pak",
  "el.pak",
  "en-GB.pak",
  "en-US.pak",
  "es.pak",
  "es-419.pak",
  "et.pak",
  "fa.pak",
  "fi.pak",
  "fil.pak",
  "fr.pak",
  "he.pak",
  "hi.pak",
  "hr.pak",
  "hu.pak",
  "id.pak",
  "it.pak",
  "ja.pak",
  "ka.pak",
  "kk.pak",
  "km.pak",
  "ko.pak",
  "lt.pak",
  "lv.pak",
  "mk.pak",
  "ms.pak",
  "my.pak",
  "nb.pak",
  "nl.pak",
  "pl.pak",
  "pt-BR.pak",
  "pt-PT.pak",
  "ro.pak",
  "ru.pak",
  "sk.pak",
  "sl.pak",
  "sr.pak",
  "sv.pak",
  "th.pak",
  "tr.pak",
  "uk.pak",
  "uz.pak",
  "vi.pak",
  "zh-CN.pak",
  "zh-TW.pak",
  "zh-HK.pak",
#else
  "bo-CN.pak",
  "de.pak",
  "en-GB.pak",
  "en-US.pak",
  "id.pak",
  "it.pak",
  "lo.pak",
  "ms.pak",
  "my.pak",
  "pl.pak",
  "pt-PT.pak",
  "th.pak",
  "tr.pak",
  "ug.pak",
  "zh-CN.pak",
  "zh-TW.pak",
  "zh-HK.pak",
#endif
};

std::string ExtractFileName(const std::string& str)
{
  auto lastSlash = str.find_last_of('/');
  return lastSlash == std::string::npos ? str : str.substr(lastSlash + 1);
}

bool GetPathFromHap(ui::ResourceScaleFactor factor,
                    const base::FilePath& path,
                    std::string& pathHap) {
  std::string pathStr = path.MaybeAsASCII();
  std::string pakFileName = ExtractFileName(pathStr);
  std::string localePath = "resources/rawfile/locales/";
  if (supportLocaleList.count(pakFileName)) {
    pathHap = localePath + pakFileName;
    return true;
  }

  auto iter = kPakFileNameHapMap.find(factor);
  if (iter == kPakFileNameHapMap.end()) {
    LOG(ERROR) << "kPakFileNameHapMap not find path: " << path;
    return false;
  }

  pathHap = iter->second;
  return true;
}
}

namespace ui {

bool DataPackUtil::LoadFromPathExt(raw_ptr<DataPack> dataPackObj, const base::FilePath& path) {
  std::string pathHap;
  std::string pathPrint;
  if (GetPathFromHap(dataPackObj->resource_scale_factor_, path, pathHap)) {
    auto resourceInstance =
        OHOS::NWeb::OhosAdapterHelper::GetInstance().GetResourceAdapter();

    std::shared_ptr<OHOS::NWeb::OhosFileMapper> fileMapper =
      resourceInstance->GetRawFileMapper(pathHap, true);
    SwapPathName(pathHap, pathPrint, "chrome", "arkweb");
    if (!fileMapper) {
      LOG(ERROR) << "DataPack::LoadFromPath couldn't data file: "
                 << pathPrint.c_str();
      return false;
    }

    LOG(INFO) << "DataPack::LoadFromPath " << pathPrint.c_str()
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
                   << pathPrint.c_str();

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

void DataPackUtil::SwapPathName(const std::string& origin, std::string& copy,
                                const std::string& from, const std::string& to) {
  copy = origin;
  size_t start_pos = 0;
  while ((start_pos = copy.find(from, start_pos)) != std::string::npos) {
    copy.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
}
}  // namespace ui

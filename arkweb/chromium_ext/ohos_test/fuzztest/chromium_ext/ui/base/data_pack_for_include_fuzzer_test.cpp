/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include <fuzzer/FuzzedDataProvider.h>

#include <cstring>
#include <string>
#include <string_view>
#include <vector>

#include "base/files/file_path.h"
#include "base/memory/raw_ptr.h"
#include "ui/base/resource/data_pack.h"
#include "ui/base/resource/resource_scale_factor.h"
#define private public
#include "arkweb/chromium_ext/ui/base/data_pack_for_include.h"
#undef private

namespace {

void SwapPathNameFuzzTest(FuzzedDataProvider* fdp) {
    std::string origin = fdp->ConsumeRandomLengthString(256);
    std::string from = fdp->ConsumeRandomLengthString(64);
    std::string to = fdp->ConsumeRandomLengthString(64);

    std::string copy;
    ui::DataPackUtil::SwapPathName(origin, copy, from, to);
}

void SwapPathNameEdgeCasesTest() {
    std::string result;

    ui::DataPackUtil::SwapPathName("", result, "", "");
    ui::DataPackUtil::SwapPathName("", result, "a", "b");
    ui::DataPackUtil::SwapPathName("hello", result, "", "x");
    ui::DataPackUtil::SwapPathName("hello", result, "x", "");
    ui::DataPackUtil::SwapPathName("hello", result, "hello", "hello");
    ui::DataPackUtil::SwapPathName("xxx_100_percent.pak", result, "xxx", "arkweb");
    ui::DataPackUtil::SwapPathName("xxx", result, "xxx", "ark");
    ui::DataPackUtil::SwapPathName("xxx", result, "xxx", "arkweb");
    ui::DataPackUtil::SwapPathName("aaa", result, "a", "aa");
    ui::DataPackUtil::SwapPathName("ababab", result, "ab", "a");
    ui::DataPackUtil::SwapPathName("aaaa", result, "aa", "a");

    std::string long_from(1000, 'a');
    std::string long_to(1000, 'b');
    ui::DataPackUtil::SwapPathName("short", result, long_from, long_to);

    std::string long_origin(10000, 'x');
    ui::DataPackUtil::SwapPathName(long_origin, result, "x", "yy");

    std::string special_chars = "path/with\\special\x01\x02\xff chars";
    ui::DataPackUtil::SwapPathName(special_chars, result, "/", "\\");
    ui::DataPackUtil::SwapPathName(special_chars, result, "\x01", "replacement");

    ui::DataPackUtil::SwapPathName("resources/rawfile/xxx_100_percent.pak", result,
                                  "xxx", "arkweb");
    ui::DataPackUtil::SwapPathName("resources/rawfile/resources.pak", result,
                                  "xxx", "arkweb");

    ui::DataPackUtil::SwapPathName("/path/to/file.pak", result, "file", "data");
    ui::DataPackUtil::SwapPathName("resources/rawfile/locales/en-US.pak", result,
                                  "xxx", "arkweb");
}

void LoadFromPathExtFuzzTest(FuzzedDataProvider* fdp) {
    ui::ResourceScaleFactor scale_factors[] = {
        ui::ResourceScaleFactor::kScaleFactorNone,
        ui::ResourceScaleFactor::k100Percent,
        ui::ResourceScaleFactor::k200Percent,
        ui::ResourceScaleFactor::k300Percent,
    };
    ui::ResourceScaleFactor scale_factor = fdp->PickValueInArray(scale_factors);

    auto data_pack = std::make_unique<ui::DataPack>(scale_factor);

    std::string path_str = fdp->ConsumeRandomLengthString(256);
    base::FilePath path(path_str);

    ui::DataPackUtil::LoadFromPathExt(data_pack.get(), path);
}

void LoadFromPathExtEdgeCasesTest() {
    std::vector<std::string> test_paths = {
        "",
        "resources.pak",
        "xxx_100_percent.pak",
        "xxx_200_percent.pak",
        "en-US.pak",
        "zh-CN.pak",
        "ar.pak",
        "de.pak",
        "ja.pak",
        "ko.pak",
        "fr.pak",
        "vi.pak",
        "th.pak",
        "ru.pak",
        "pt-BR.pak",
        "es.pak",
        "en-GB.pak",
        "zh-TW.pak",
        "zh-HK.pak",
        "nonexistent.pak",
        "resources/rawfile/resources.pak",
        "resources/rawfile/xxx_100_percent.pak",
        "resources/rawfile/xxx_200_percent.pak",
        "resources/rawfile/locales/en-US.pak",
        "/absolute/path/resources.pak",
        "../relative/resources.pak",
        ".",
        "..",
        "/",
        "a",
    };

    ui::ResourceScaleFactor factors[] = {
        ui::ResourceScaleFactor::kScaleFactorNone,
        ui::ResourceScaleFactor::k100Percent,
        ui::ResourceScaleFactor::k200Percent,
        ui::ResourceScaleFactor::k300Percent,
    };

    for (auto factor : factors) {
        auto data_pack = std::make_unique<ui::DataPack>(factor);
        for (const auto& path_str : test_paths) {
            base::FilePath path(path_str);
            ui::DataPackUtil::LoadFromPathExt(data_pack.get(), path);
        }
    }

    auto data_pack_none = std::make_unique<ui::DataPack>(ui::ResourceScaleFactor::kScaleFactorNone);
    ui::DataPackUtil::LoadFromPathExt(data_pack_none.get(), base::FilePath(""));

    auto data_pack_100 = std::make_unique<ui::DataPack>(ui::ResourceScaleFactor::k100Percent);
    ui::DataPackUtil::LoadFromPathExt(data_pack_100.get(), base::FilePath("en-US.pak"));

    auto data_pack_200 = std::make_unique<ui::DataPack>(ui::ResourceScaleFactor::k200Percent);
    ui::DataPackUtil::LoadFromPathExt(data_pack_200.get(), base::FilePath("zh-CN.pak"));

    auto data_pack_300 = std::make_unique<ui::DataPack>(ui::ResourceScaleFactor::k300Percent);
    ui::DataPackUtil::LoadFromPathExt(data_pack_300.get(), base::FilePath("resources.pak"));
}

void SwapPathNameWithFuzzStrings(FuzzedDataProvider* fdp) {
    std::string origin;
    std::string from;
    std::string to;
    std::string result;

    if (fdp->ConsumeBool()) {
        origin = fdp->ConsumeRandomLengthString(1024);
    }
    if (fdp->ConsumeBool()) {
        from = fdp->ConsumeRandomLengthString(128);
    }
    if (fdp->ConsumeBool()) {
        to = fdp->ConsumeRandomLengthString(128);
    }

    ui::DataPackUtil::SwapPathName(origin, result, from, to);

    if (!origin.empty()) {
        std::string single_char_from(1, origin[0]);
        ui::DataPackUtil::SwapPathName(origin, result, single_char_from, "X");
    }

    ui::DataPackUtil::SwapPathName(origin, result, origin, to);
    ui::DataPackUtil::SwapPathName(origin, result, from, from);

    std::string reversed(origin.rbegin(), origin.rend());
    ui::DataPackUtil::SwapPathName(origin, result, origin, reversed);
}

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (data == nullptr || size == 0) {
        return 0;
    }

    FuzzedDataProvider fdp(data, size);

    SwapPathNameFuzzTest(&fdp);

    SwapPathNameEdgeCasesTest();

    LoadFromPathExtFuzzTest(&fdp);

    LoadFromPathExtEdgeCasesTest();

    SwapPathNameWithFuzzStrings(&fdp);

    return 0;
}

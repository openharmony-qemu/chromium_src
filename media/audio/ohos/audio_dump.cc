/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "audio_dump.h"

#include "base/logging.h"
#include "ohos_adapter_helper.h"

namespace {
    const char *DUMP_APP_DIR = "/data/storage/el2/base/cache/";
    const char* DUMP_AUDIO_PARA = "web.debug.audiodump";
}

void DumpFileUtil::WriteDumpFile(FILE *dumpFile, void *buffer, size_t bufferSize)
{
    if (dumpFile == nullptr) {
        return;
    }
    if (buffer == nullptr) {
        LOG(WARNING) << "DumpFileUtil::WriteDumpFile buffer == nullptr";
        return;
    }
    size_t writeResult = fwrite(buffer, 1, bufferSize, dumpFile);
    if (writeResult != bufferSize) {
        LOG(WARNING) << "DumpFileUtil::WriteDumpFile writeResult != bufferSize";
        return;
    }
    return;
}

void DumpFileUtil::CloseDumpFile(FILE **dumpFile)
{
    if (*dumpFile == nullptr) {
        return;
    }
    int rc = fclose(*dumpFile);
    if (rc != 0) {
        LOG(WARNING) << "DumpFileUtil::CloseDumpFile err";
    }
    *dumpFile = nullptr;
    return;
}

void DumpFileUtil::OpenDumpFile(std::string filename, FILE **file)
{
    std::string filePath = DUMP_APP_DIR + filename;
    FILE *dumpFile = nullptr;
    bool res = OHOS::NWeb::OhosAdapterHelper::GetInstance().
        GetSystemPropertiesInstance().GetBoolParameter(DUMP_AUDIO_PARA, false);
    if (!res) {
        return;
    }
    dumpFile = fopen(filePath.c_str(), "wb+");
    if (dumpFile == nullptr) {
        LOG(WARNING) << "DumpFileUtil::OpenDumpFile fopen failed! filename: " << filename;
        return;
    }
    *file = dumpFile;
    return;
}

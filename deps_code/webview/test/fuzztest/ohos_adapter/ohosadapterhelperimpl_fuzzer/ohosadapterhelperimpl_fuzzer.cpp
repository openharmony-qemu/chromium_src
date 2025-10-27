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

#include "ohosadapterhelperimpl_fuzzer.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <cstring>

#include "ohos_adapter_helper_impl.h"

using namespace OHOS::NWeb;

namespace OHOS {
constexpr uint8_t MAX_STRING_LENGTH = 64;

bool OhosAdapterHelperImpl001Test(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    auto& helperImpl = OhosAdapterHelperImpl::GetInstance();
    helperImpl.CreateAafwkAdapter();
    helperImpl.CreateBatteryClientAdapter();
    helperImpl.CreateAudioCapturerAdapter();
    helperImpl.GetWebPermissionDataBaseInstance();
    helperImpl.GetSystemPropertiesInstance();
    helperImpl.GetVSyncAdapter();
    helperImpl.GetInitWebAdapter();
    helperImpl.GetKeystoreAdapterInstance();
    helperImpl.GetEnterpriseDeviceManagementInstance();
    helperImpl.GetDatashareInstance();
    helperImpl.CreateIMFAdapter();
    helperImpl.GetRootCertDataAdapter();
    helperImpl.GetAccessTokenAdapterInstance();
    helperImpl.GetEventHandlerAdapter();
    helperImpl.GetPrintManagerInstance();
    helperImpl.CreatePlayerAdapter();
    helperImpl.GetWindowAdapterInstance();
    helperImpl.GetHiSysEventAdapterInstance();
    helperImpl.GetHiTraceAdapterInstance();
    helperImpl.GetNetProxyInstance();
    helperImpl.CreateScreenCaptureAdapter();
    helperImpl.CreateDateTimeFormatAdapter();
    helperImpl.CreateMediaCodecDecoderAdapter();
    helperImpl.CreateMediaCodecEncoderAdapter();
    helperImpl.GetMediaCodecListAdapter();
    helperImpl.CreateMediaAVSessionAdapter();
    helperImpl.CreateSensorAdapter();
    helperImpl.GetOhosNativeBufferAdapter();
    helperImpl.CreateMigrationMgrAdapter();
    helperImpl.GetOhosDrawingTextFontAdapter();
    helperImpl.GetOhosDrawingTextTypographyAdapter();
    return true;
}

bool OhosAdapterHelperImpl002Test(const uint8_t* data, size_t size)
{
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    auto& helperImpl = OhosAdapterHelperImpl::GetInstance();
    FuzzedDataProvider dataProvider(data, size);
    std::string hapPath = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::string hapPath1 = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    helperImpl.GetResourceAdapter(hapPath);
    helperImpl.SetArkWebCoreHapPathOverride(hapPath1);

    return true;
}

} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::OhosAdapterHelperImpl001Test(data, size);
    OHOS::OhosAdapterHelperImpl002Test(data, size);
    return 0;
}

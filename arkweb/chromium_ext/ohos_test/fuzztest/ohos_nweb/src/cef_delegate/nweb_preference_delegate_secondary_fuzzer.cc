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

#include "ohos_nweb/src/cef_delegate/nweb_preference_delegate.h"

#include <fuzzer/FuzzedDataProvider.h>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>

#include "ohos_nweb/include/nweb.h"
#include "ohos_nweb/src/nweb_impl.h"
 
using namespace OHOS::NWeb;
namespace {
    constexpr uint8_t MAX_STRING_LENGTH = 64;
    constexpr int32_t MIN_INT_SIZE = -1;
    constexpr int32_t MAX_INT_SIZE = 128;
    constexpr int64_t MAX_INT64_SIZE = 511;
    constexpr int MAX_VECTOR_SIZE = 10;
    constexpr float MAX_FLOAT_SIZE = 10.0f;
}

void NWebPerferenceDelegate001FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.OnDestroy();
}

void NWebPerferenceDelegate002FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.SetBrowser(nullptr);
}

void NWebPerferenceDelegate003FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.WebPreferencesChanged();
}

void NWebPerferenceDelegate005FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.SetBrowserSettingsToNetHelpers();
}

void NWebPerferenceDelegate006FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutMultiWindowAccess(ret);
}

void NWebPerferenceDelegate007FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutEnableContentAccess(ret);
}

void NWebPerferenceDelegate008FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutEnableRawFileAccess(ret);
}

void NWebPerferenceDelegate009FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutEnableRawFileAccessFromFileURLs(ret);
}

void NWebPerferenceDelegate010FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutEnableUniversalAccessFromFileURLs(ret);
}

void NWebPerferenceDelegate011FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutLoadImageFromNetworkDisabled(ret);
}

void NWebPerferenceDelegate012FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeRandomLengthString(MAX_STRING_LENGTH);
    delegate.PutCursiveFontFamilyName(ret);
}

void NWebPerferenceDelegate013FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutDatabaseAllowed(ret);
}

void NWebPerferenceDelegate014FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeIntegralInRange(MIN_INT_SIZE, MAX_INT_SIZE);
    delegate.PutDefaultFixedFontSize(ret);
}

void NWebPerferenceDelegate015FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeIntegralInRange(MIN_INT_SIZE, MAX_INT_SIZE);
    delegate.PutDefaultFontSize(ret);
}

void NWebPerferenceDelegate016FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeRandomLengthString(MAX_STRING_LENGTH);
    delegate.PutDefaultTextEncodingFormat(ret);
}

void NWebPerferenceDelegate017FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutDomStorageEnabled(ret);
}

void NWebPerferenceDelegate018FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeRandomLengthString(MAX_STRING_LENGTH);
    delegate.PutFantasyFontFamilyName(ret);
}

void NWebPerferenceDelegate019FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeRandomLengthString(MAX_STRING_LENGTH);
    delegate.PutFixedFontFamilyName(ret);
}

void NWebPerferenceDelegate020FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = static_cast<double>(fdp->ConsumeIntegralInRange(0, MAX_INT_SIZE));
    auto ret2 = static_cast<double>(fdp->ConsumeIntegralInRange(0, MAX_INT_SIZE));
    auto ret3 = static_cast<double>(fdp->ConsumeIntegralInRange(0, MAX_INT_SIZE));
    auto ret4 = static_cast<double>(fdp->ConsumeIntegralInRange(0, MAX_INT_SIZE));
    delegate.SetBorderRadiusFromWeb(ret, ret2, ret3, ret4);
}

void NWebPerferenceDelegate021FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.SetTouchHandleExistState(ret);
}

void NWebPerferenceDelegate023FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeIntegralInRange(MIN_INT_SIZE, MAX_INT_SIZE);
    delegate.PutForceDarkModeEnabled(ret);
}

void NWebPerferenceDelegate024FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeIntegralInRange(MIN_INT_SIZE, MAX_INT_SIZE);
    delegate.PutDarkSchemeEnabled(ret);
}

void NWebPerferenceDelegate025FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutIsCreateWindowsByJavaScriptAllowed(ret);
}

void NWebPerferenceDelegate026FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutJavaScriptEnabled(ret);
}

void NWebPerferenceDelegate027FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutImageLoadingAllowed(ret);
}

void NWebPerferenceDelegate028FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeIntegralInRange(MIN_INT_SIZE, MAX_INT_SIZE);
    delegate.PutFontSizeLowerLimit(ret);
}

void NWebPerferenceDelegate029FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeIntegralInRange(MIN_INT_SIZE, MAX_INT_SIZE);
    delegate.PutLogicalFontSizeLowerLimit(ret);
}

void NWebPerferenceDelegate030FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutLoadWithOverviewMode(ret);
}

void NWebPerferenceDelegate031FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeRandomLengthString(MAX_STRING_LENGTH);
    delegate.PutSansSerifFontFamilyName(ret);
}

void NWebPerferenceDelegate032FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeRandomLengthString(MAX_STRING_LENGTH);
    delegate.PutSerifFontFamilyName(ret);
}

void NWebPerferenceDelegate033FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeRandomLengthString(MAX_STRING_LENGTH);
    delegate.PutStandardFontFamilyName(ret);
}

void NWebPerferenceDelegate034FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeIntegralInRange(MIN_INT_SIZE, MAX_INT_SIZE);
    delegate.PutZoomingForTextFactor(ret);
}

void NWebPerferenceDelegate035FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutGeolocationAllowed(ret);
}

void NWebPerferenceDelegate036FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = static_cast<OHOS::NWeb::NWebPreference::AccessMode>
        (fdp->ConsumeIntegralInRange(0, 2));
    delegate.PutAccessModeForSecureOriginLoadFromInsecure(ret);
}

void NWebPerferenceDelegate037FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutZoomingFunctionEnabled(ret);
}

void NWebPerferenceDelegate038FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutBlockNetwork(ret);
}

void NWebPerferenceDelegate039FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutWebDebuggingAccess(ret);
}

void NWebPerferenceDelegate040FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutMediaPlayGestureAccess(ret);
}

void NWebPerferenceDelegate041FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    auto ret = fdp->ConsumeBool();
    delegate.PutPinchSmoothMode(ret);
}

void NWebPerferenceDelegate042FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.EnableContentAccess();
}

void NWebPerferenceDelegate043FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.EnableRawFileAccess();
}

void NWebPerferenceDelegate044FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.EnableRawFileAccessFromFileURLs();
}

void NWebPerferenceDelegate045FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.EnableUniversalAccessFromFileURLs();
}

void NWebPerferenceDelegate046FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.IsLoadImageFromNetworkDisabled();
}

void NWebPerferenceDelegate047FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.CursiveFontFamilyName();
}

void NWebPerferenceDelegate048FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.IsDataBaseEnabled();
}

void NWebPerferenceDelegate049FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.DefaultFixedFontSize();
}

void NWebPerferenceDelegate051FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.DefaultFontSize();
}

void NWebPerferenceDelegate052FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.DefaultTextEncodingFormat();
}

void NWebPerferenceDelegate053FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.DefaultUserAgent();
}

void NWebPerferenceDelegate054FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.IsDomStorageEnabled();
}

void NWebPerferenceDelegate055FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.FantasyFontFamilyName();
}

void NWebPerferenceDelegate056FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.FixedFontFamilyName();
}

void NWebPerferenceDelegate057FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.ForceDarkModeEnabled();
}

void NWebPerferenceDelegate058FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.DarkSchemeEnabled();
}

void NWebPerferenceDelegate059FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.IsCreateWindowsByJavaScriptAllowed();
}

void NWebPerferenceDelegate060FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.IsJavaScriptAllowed();
}

void NWebPerferenceDelegate061FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.IsImageLoadingAllowed();
}

void NWebPerferenceDelegate062FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.FontSizeLowerLimit();
}

void NWebPerferenceDelegate063FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.LogicalFontSizeLowerLimit();
}

void NWebPerferenceDelegate064FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.IsLoadWithOverviewMode();
}

void NWebPerferenceDelegate065FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.SansSerifFontFamilyName();
}

void NWebPerferenceDelegate066FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.SerifFontFamilyName();
}

void NWebPerferenceDelegate067FuzzTest(FuzzedDataProvider* fdp) {
    NWebPreferenceDelegate delegate;
    delegate.StandardFontFamilyName();
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size < sizeof(int32_t))) {
        return 0;
    }
    FuzzedDataProvider fdp(data, size);
    NWebPerferenceDelegate001FuzzTest(&fdp);
    NWebPerferenceDelegate002FuzzTest(&fdp);
    NWebPerferenceDelegate003FuzzTest(&fdp);
    NWebPerferenceDelegate005FuzzTest(&fdp);
    NWebPerferenceDelegate006FuzzTest(&fdp);
    NWebPerferenceDelegate007FuzzTest(&fdp);
    NWebPerferenceDelegate008FuzzTest(&fdp);
    NWebPerferenceDelegate009FuzzTest(&fdp);
    NWebPerferenceDelegate010FuzzTest(&fdp);
    NWebPerferenceDelegate011FuzzTest(&fdp);
    NWebPerferenceDelegate012FuzzTest(&fdp);
    NWebPerferenceDelegate013FuzzTest(&fdp);
    NWebPerferenceDelegate014FuzzTest(&fdp);
    NWebPerferenceDelegate015FuzzTest(&fdp);
    NWebPerferenceDelegate016FuzzTest(&fdp);
    NWebPerferenceDelegate017FuzzTest(&fdp);
    NWebPerferenceDelegate018FuzzTest(&fdp);
    NWebPerferenceDelegate019FuzzTest(&fdp);
    NWebPerferenceDelegate020FuzzTest(&fdp);
    NWebPerferenceDelegate021FuzzTest(&fdp);
    NWebPerferenceDelegate023FuzzTest(&fdp);
    NWebPerferenceDelegate024FuzzTest(&fdp);
    NWebPerferenceDelegate025FuzzTest(&fdp);
    NWebPerferenceDelegate026FuzzTest(&fdp);
    NWebPerferenceDelegate027FuzzTest(&fdp);
    NWebPerferenceDelegate028FuzzTest(&fdp);
    NWebPerferenceDelegate029FuzzTest(&fdp);
    NWebPerferenceDelegate030FuzzTest(&fdp);
    NWebPerferenceDelegate031FuzzTest(&fdp);
    NWebPerferenceDelegate032FuzzTest(&fdp);
    NWebPerferenceDelegate033FuzzTest(&fdp);
    NWebPerferenceDelegate034FuzzTest(&fdp);
    NWebPerferenceDelegate035FuzzTest(&fdp);
    NWebPerferenceDelegate036FuzzTest(&fdp);
    NWebPerferenceDelegate037FuzzTest(&fdp);
    NWebPerferenceDelegate038FuzzTest(&fdp);
    NWebPerferenceDelegate039FuzzTest(&fdp);
    NWebPerferenceDelegate040FuzzTest(&fdp);
    NWebPerferenceDelegate041FuzzTest(&fdp);
    NWebPerferenceDelegate042FuzzTest(&fdp);
    NWebPerferenceDelegate043FuzzTest(&fdp);
    NWebPerferenceDelegate044FuzzTest(&fdp);
    NWebPerferenceDelegate045FuzzTest(&fdp);
    NWebPerferenceDelegate046FuzzTest(&fdp);
    NWebPerferenceDelegate047FuzzTest(&fdp);
    NWebPerferenceDelegate048FuzzTest(&fdp);
    NWebPerferenceDelegate049FuzzTest(&fdp);
    NWebPerferenceDelegate051FuzzTest(&fdp);
    NWebPerferenceDelegate052FuzzTest(&fdp);
    NWebPerferenceDelegate053FuzzTest(&fdp);
    NWebPerferenceDelegate054FuzzTest(&fdp);
    NWebPerferenceDelegate055FuzzTest(&fdp);
    NWebPerferenceDelegate056FuzzTest(&fdp);
    NWebPerferenceDelegate057FuzzTest(&fdp);
    NWebPerferenceDelegate058FuzzTest(&fdp);
    NWebPerferenceDelegate059FuzzTest(&fdp);
    NWebPerferenceDelegate060FuzzTest(&fdp);
    NWebPerferenceDelegate061FuzzTest(&fdp);
    NWebPerferenceDelegate062FuzzTest(&fdp);
    NWebPerferenceDelegate063FuzzTest(&fdp);
    NWebPerferenceDelegate064FuzzTest(&fdp);
    NWebPerferenceDelegate065FuzzTest(&fdp);
    NWebPerferenceDelegate066FuzzTest(&fdp);
    NWebPerferenceDelegate067FuzzTest(&fdp);
    return 0;
 }
 
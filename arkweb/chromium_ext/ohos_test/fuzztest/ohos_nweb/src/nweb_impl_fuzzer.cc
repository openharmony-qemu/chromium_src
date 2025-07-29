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

#include <fuzzer/FuzzedDataProvider.h>
#include "nweb_impl.h"
#include "nweb_default_engine_init_args_impl.h"

namespace OHOS::NWeb {
namespace {
    constexpr uint32_t MAX_UINT32_SIZE = 255;
    constexpr long MAX_LONG_SIZE = 65536;
    constexpr int32_t MAX_INT32_SIZE = 255;
    constexpr int32_t MIN_INT32_SIZE = -255;
    constexpr uint8_t MAX_STRING_LENGTH = 255;
}

class MockNWebTouchPointInfo : public NWebTouchPointInfo {
 public:
  MockNWebTouchPointInfo(int32_t id, double x, double y)
      : id_(id), x_(x), y_(y) {}
  int32_t GetId() override { return id_; }
  double GetX() override { return x_; }
  double GetY() override { return y_; }

 private:
  int32_t id_;
  double x_;
  double y_;
};

void NwebImplFuzz001Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    uint32_t id = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(id);
}

void NwebImplFuzz002Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    long adBlockEasyListVersion = dataProvider.ConsumeIntegralInRange<long>(0, MAX_LONG_SIZE);
    uint32_t id = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    int32_t nweb_id = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    nweb_impl_->UpdateAdblockEasyListRules(adBlockEasyListVersion);
    nweb_impl_->AddNWebToMap(id, nweb_impl_);
    nweb_impl_->FromID(nweb_id);
    nweb_impl_->GetNWebSharedPtr(nweb_id);
    std::shared_ptr<NWebEngineInitArgs> init_args =
        std::make_shared<NWebDefaultEngineInitArgsImpl>();
    nweb_impl_->SetNwebDelegateForTest(init_args);
}

void NwebImplFuzz003Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    std::shared_ptr<NWebHandler> handler = std::make_shared<NWebHandler>();
    nweb_impl_->SetNWebHandler(handler);
    uint32_t nweb_id = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    nweb_impl_->DisableBoost(nweb_id);
}

void NwebImplFuzz004Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    uint32_t width = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    uint32_t height = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    bool isKeyboard = dataProvider.ConsumeBool();
    nweb_impl_->Resize(width, height, isKeyboard);
    nweb_impl_->ResizeVisibleViewport(width, height, isKeyboard);
}

void NwebImplFuzz005Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    int x = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int y = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int drawWidth = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int drawHeight = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    nweb_impl_->SetDrawRect(x, y, drawWidth, drawHeight);
}

void NwebImplFuzz006Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    int mode = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    nweb_impl_->SetDrawMode(mode);
    nweb_impl_->SetFitContentMode(mode);
}

void NwebImplFuzz007Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    int32_t touchId = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    double touchX = dataProvider.ConsumeFloatingPoint<double>();
    double touchY = dataProvider.ConsumeFloatingPoint<double>();
    bool from_overlay = dataProvider.ConsumeBool();
    nweb_impl_->OnTouchPress(touchId, touchX, touchY, from_overlay);
    nweb_impl_->OnTouchRelease(touchId, touchX, touchY, from_overlay);
    nweb_impl_->OnTouchMove(touchId, touchX, touchY, from_overlay);
}

void NwebImplFuzz008Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    std::vector<std::shared_ptr<NWebTouchPointInfo>> touch_point_infos;
    uint32_t vec_size = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    
    for (uint32_t i = 0; i< vec_size; i++) {
        int32_t id = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
        double x = dataProvider.ConsumeFloatingPoint<double>();
        double y = dataProvider.ConsumeFloatingPoint<double>();
        std::shared_ptr<NWebTouchPointInfo> touch_point_info =
            std::make_shared<MockNWebTouchPointInfo>(id, x, y);
        touch_point_infos.push_back(touch_point_info);
    }
    bool from_overlay = dataProvider.ConsumeBool();
    nweb_impl_->OnTouchMove(touch_point_infos, from_overlay);
}

void NwebImplFuzz009Test(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    int32_t keyCode = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int32_t keyAction = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    nweb_impl_->SendKeyEvent(keyCode, keyAction);
}

void NwebImplFuzz00ATest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    double x = dataProvider.ConsumeFloatingPoint<double>();
    double y = dataProvider.ConsumeFloatingPoint<double>();
    double vx = dataProvider.ConsumeFloatingPoint<double>();
    double vy = dataProvider.ConsumeFloatingPoint<double>();
    nweb_impl_->SendTouchpadFlingEvent(x, y, vx,vy);
    nweb_impl_->SendMouseWheelEvent(x, y, vx,vy);
}

void NwebImplFuzz00BTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    int x = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int y = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int button = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int action = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    int count = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    nweb_impl_->SendMouseEvent(x, y, button, action, count);
    std::string url = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nweb_impl_->Load(url);
}

void NwebImplFuzz00CTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    int num = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    float zoomFactor = dataProvider.ConsumeFloatingPoint<float>();
    nweb_impl_->CanNavigateBackOrForward(num);
    nweb_impl_->NavigateBackOrForward(num);
    nweb_impl_->Zoom(zoomFactor);
}

void NwebImplFuzz00DTest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    std::string code = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    float zoomFactor = dataProvider.ConsumeFloatingPoint<float>();
    int color = dataProvider.ConsumeIntegralInRange<int32_t>(MIN_INT32_SIZE, MAX_INT32_SIZE);
    nweb_impl_->ExecuteJavaScript(code);
    nweb_impl_->PutBackgroundColor(color);
    nweb_impl_->InitialScale(zoomFactor);
}

void NwebImplFuzz00ETest(const uint8_t* data, size_t size) {
    if ((data == nullptr) || (size == 0)) {
        return false;
    }
    FuzzedDataProvider dataProvider(data, size);
    std::shared_ptr<NWebImpl> nweb_impl_ = std::make_shared<NWebImpl>(1);
    std::string code = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    std::vector<std::string> ports;
    uint32_t vec_size = dataProvider.ConsumeIntegralInRange<uint32_t>(0, MAX_UINT32_SIZE);
    for (uint32_t i = 0; i< vec_size; i++) {
        std::string port = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
        ports.push_back(port);
    }
    std::string targetUri = dataProvider.ConsumeRandomLengthString(MAX_STRING_LENGTH);
    nweb_impl_->PostWebMessage(code, ports, targetUri);
    nweb_impl_->ClosePort(code);
    std::shared_ptr<NWebMessage> messageData = std::make_shared<NWebMessage>(NWebValue::Type::NONE);
    nweb_impl_->PostPortMessage(code, messageData);
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    NwebImplFuzz001Test(data, size);
    NwebImplFuzz002Test(data, size);
    NwebImplFuzz003Test(data, size);
    NwebImplFuzz004Test(data, size);
    NwebImplFuzz005Test(data, size);
    NwebImplFuzz006Test(data, size);
    NwebImplFuzz007Test(data, size);
    NwebImplFuzz008Test(data, size);
    NwebImplFuzz009Test(data, size);
    NwebImplFuzz00ATest(data, size);
    NwebImplFuzz00BTest(data, size);
    NwebImplFuzz00CTest(data, size);
    NwebImplFuzz00DTest(data, size);
    NwebImplFuzz00ETest(data, size);
    return 0;
}
}   // namespace OHOS::NWeb
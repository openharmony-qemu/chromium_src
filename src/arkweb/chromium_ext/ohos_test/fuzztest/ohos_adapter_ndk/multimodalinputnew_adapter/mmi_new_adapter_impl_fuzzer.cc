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

 #include "arkweb/ohos_adapter_ndk/multimodalinputnew_adapter/mmi_new_adapter_impl.h"

 #include <fuzzer/FuzzedDataProvider.h>
 #include <multimodalinput/oh_input_manager.h>
 
 #include <cstddef>
 #include <cstdint>
 #include <memory>
 #include <string>
 #include <vector>
 
 namespace OHOS::NWeb {
 
 void OnDeviceAdded(int32_t deviceId);
 void OnDeviceRemoved(int32_t deviceId);
 
 class MockMMIDeviceInfoAdapter : public MMIDeviceInfoAdapter {
  public:
   int32_t GetId() override { return id_; }
   int32_t GetType() override { return type_; }
   int32_t GetBus() override { return bus_; }
   int32_t GetVersion() override { return version_; }
   int32_t GetProduct() override { return product_; }
   int32_t GetVendor() override { return vendor_; }
   std::string GetName() override { return name_; }
   std::string GetPhys() override { return phys_; }
   std::string GetUniq() override { return uniq_; }
 
   void SetId(int32_t id) override { id_ = id; }
   void SetType(int32_t type) override { type_ = type; }
   void SetBus(int32_t bus) override { bus_ = bus; }
   void SetVersion(int32_t version) override { version_ = version; }
   void SetProduct(int32_t product) override { product_ = product; }
   void SetVendor(int32_t vendor) override { vendor_ = vendor; }
   void SetName(std::string name) override { name_ = name; }
   void SetPhys(std::string phys) override { phys_ = phys; }
   void SetUniq(std::string uniq) override { uniq_ = uniq; }
 
  private:
   int32_t id_ = 0;
   int32_t type_ = 0;
   int32_t bus_ = 0;
   int32_t version_ = 0;
   int32_t product_ = 0;
   int32_t vendor_ = 0;
   std::string name_ = "";
   std::string phys_ = "";
   std::string uniq_ = "";
 };
 
 class MockMMIListenerAdapter : public MMIListenerAdapter {
  public:
   void OnDeviceAdded(int32_t deviceId, const std::string& type) override {
     addedDevices_.push_back(deviceId);
   }
 
   void OnDeviceRemoved(int32_t deviceId, const std::string& type) override {
     removedDevices_.push_back(deviceId);
   }
 
   const std::vector<int32_t>& GetAddedDevices() const { return addedDevices_; }
   const std::vector<int32_t>& GetRemovedDevices() const {
     return removedDevices_;
   }
 
  private:
   std::vector<int32_t> addedDevices_;
   std::vector<int32_t> removedDevices_;
 };
 
 class MockMMIInputListenerAdapter : public MMIInputListenerAdapter {
  public:
   void OnInputEvent(int32_t keyCode, int32_t keyAction) override {
     inputEvents_.push_back({keyCode, keyAction});
   }
 
   const std::vector<std::pair<int32_t, int32_t>>& GetInputEvents() const {
     return inputEvents_;
   }
 
  private:
   std::vector<std::pair<int32_t, int32_t>> inputEvents_;
 };
 
 void FuzzMMINewAdapterImpl(FuzzedDataProvider* fdp) {
   auto& adapter = MMINewAdapterImpl::GetInstance();
 
   std::string type = fdp->ConsumeRandomLengthString(32);
   int32_t deviceId = fdp->ConsumeIntegralInRange<int32_t>(0, 1000);
   int32_t monitorId = fdp->ConsumeIntegralInRange<int32_t>(0, 100);
   int32_t keyCode = fdp->ConsumeIntegralInRange<int32_t>(0, 255);
   int32_t keyAction = fdp->ConsumeIntegralInRange<int32_t>(0, 1);
 
   char* keyStr = adapter.KeyCodeToString(keyCode);
   auto inputListener = std::make_shared<MockMMIInputListenerAdapter>();
   int32_t result = adapter.RegisterMMIInputListener(inputListener);
 
   if (fdp->ConsumeBool()) {
     adapter.UnregisterMMIInputListener(monitorId);
   }
 
   auto deviceListener = std::make_shared<MockMMIListenerAdapter>();
   result = adapter.RegisterDevListener(type, deviceListener);
 
   if (fdp->ConsumeBool()) {
     adapter.UnregisterDevListener(type);
   }
 
   int32_t kbType = 0;
   result = adapter.GetKeyboardType(deviceId, kbType);
 
   std::vector<int32_t> ids;
   result = adapter.GetDeviceIds(ids);
 
   auto deviceInfo = std::make_shared<MockMMIDeviceInfoAdapter>();
   result = adapter.GetDeviceInfo(deviceId, deviceInfo);
 
   int32_t maxTouchPoints = adapter.GetMaxTouchPoints();
 
   if (fdp->ConsumeBool()) {
     OnDeviceAdded(deviceId);
   }
 
   if (fdp->ConsumeBool()) {
     OnDeviceRemoved(deviceId);
   }
 }
 
 extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
   if (data == nullptr || size == 0) {
     return 0;
   }
 
   FuzzedDataProvider fdp(data, size);
   FuzzMMINewAdapterImpl(&fdp);
 
   return 0;
 }
 
 }  // namespace OHOS::NWeb
 
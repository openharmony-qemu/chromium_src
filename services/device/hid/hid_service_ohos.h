// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef SERVICES_DEVICE_HID_HID_SERVICE_OHOS_H_
#define SERVICES_DEVICE_HID_HID_SERVICE_OHOS_H_

#include <memory>

#include "base/memory/weak_ptr.h"
#include "base/threading/sequence_bound.h"
#include "services/device/hid/hid_device_info.h"
#include "services/device/hid/hid_service.h"

namespace device {
class HidServiceOhos : public HidService {
 public:
  HidServiceOhos();
  HidServiceOhos(HidServiceOhos&) = delete;
  HidServiceOhos& operator=(HidServiceOhos&) = delete;
  ~HidServiceOhos() override;

  void Connect(const std::string& device_id,
               bool allow_protected_reports,
               bool allow_fido_reports,
               ConnectCallback callback) override;
  base::WeakPtr<HidService> GetWeakPtr() override;

 private:
  class BlockingTaskRunnerHelper;

  base::SequenceBound<BlockingTaskRunnerHelper> helper_;
  base::WeakPtrFactory<HidServiceOhos> weak_factory_{this};
};

}  // namespace device
#endif  // SERVICES_DEVICE_HID_HID_SERVICE_OHOS_H_

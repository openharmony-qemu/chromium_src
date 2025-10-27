// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/notreached.h"
#include "chrome/browser/extensions/api/image_writer_private/removable_storage_provider.h"

namespace {
using StorageDeviceListPtr = scoped_refptr<extensions::StorageDeviceList>;
}

namespace extensions {

// static
StorageDeviceListPtr RemovableStorageProvider::PopulateDeviceList() {
  // TODO(crbug.com/1233550): Integrate once platform APIs exist.
  // Fix Me
  NOTIMPLEMENTED_LOG_ONCE();
  return {};
}

}  // namespace extensions

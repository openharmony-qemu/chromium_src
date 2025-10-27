/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef ARK_PASTE_RECORD_VECTOR_CPPTOC_H
#define ARK_PASTE_RECORD_VECTOR_CPPTOC_H

#pragma once

#include "ohos_adapter/include/ark_paste_record_vector.h"
#include "pasteboard_client_adapter.h"

namespace OHOS::ArkWeb {

ArkPasteRecordVector ArkPasteRecordVectorClassToStruct(const NWeb::PasteRecordVector& class_value);

} // namespace OHOS::ArkWeb

#endif // ARK_PASTE_RECORD_VECTOR_CPPTOC_H

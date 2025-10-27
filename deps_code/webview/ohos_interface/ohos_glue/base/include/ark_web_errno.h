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

#ifndef ARK_WEB_ERRNO_H_
#define ARK_WEB_ERRNO_H_
#pragma once

enum ArkWebInterfaceResult { RESULT_OK = 0, RESULT_NOT_IMPL, RESULT_CPPTOC_ERROR, RESULT_CTOCPP_ERROR };

void ArkWebSetErrno(ArkWebInterfaceResult result);

ArkWebInterfaceResult ArkWebGetErrno();

ArkWebInterfaceResult ArkWebResetErrno();

#endif // ARK_WEB_ERRNO_H_

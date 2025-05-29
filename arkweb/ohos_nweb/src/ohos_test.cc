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

#include <dlfcn.h>
#include <unistd.h>

#include "arkweb/build/features/features.h"
#include "arkweb/ohos_nweb_ex/build/features/features.h"

#if BUILDFLAG(ENABLE_ARKWEB)
#include "arkweb-test.h"
#else
#error "ENABLE_ARKWEB Not defined"
#endif

#if BUILDFLAG(ENABLE_ARKWEB_EXT)
#include "arkweb-ext-test.h"
#else
#error "ENABLE_ARKWEB_EXT Not defined"
#endif

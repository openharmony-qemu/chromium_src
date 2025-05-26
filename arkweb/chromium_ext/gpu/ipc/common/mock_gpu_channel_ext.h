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

#ifndef MOCK_GPU_CHANNEL_EXT_H
#define MOCK_GPU_CHANNEL_EXT_H

#define ARKWEB_UNITTESTS_MOCK_METHOD4_BOOL() \
  MOCK_METHOD4(CreateNativeTexture, \
               bool(int32_t, \
                    int32_t, \
                    mojo::PendingAssociatedReceiver<mojom::StreamTexture>, \
                    int32_t*))

#define ARKWEB_UNITTESTS_MOCK_METHOD4_VOILD() \
  MOCK_METHOD4(CreateNativeTexture, \
               void(int32_t, \
                    int32_t, \
                    mojo::PendingAssociatedReceiver<mojom::StreamTexture>, \
                    CreateNativeTextureCallback))

#endif // MOCK_GPU_CHANNEL_EXT_H
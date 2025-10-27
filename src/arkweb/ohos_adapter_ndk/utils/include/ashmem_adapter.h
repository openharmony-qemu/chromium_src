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

 /**
 * @file ashmem.h adapter
 *
 * @brief Provides the <b>Ashmem Adapter</b> class implemented in ashmem_adapter to operate the
 * Anonymous Shared Memory (Ashmem).
 */

#ifndef ADAPTER_UTILS_BASE_ASHMEM_H
#define ADAPTER_UTILS_BASE_ASHMEM_H

#include <cstddef>
#include <linux/ashmem.h>

namespace OHOS::NWeb {

/**
 * @brief Creates an <b>Ashmem</b> region in the kernel.
 *
 * @param name Indicates the pointer to the name that will be
 * copied and assigned to the <b>Ashmem</b> region in the kernel.
 * @param size Indicates the size of the <b>Ashmem</b> region to create.
 * @return Returns the file descriptor of the <b>Ashmem</b> region.
 */
int ShmemAdapterCreate(const char *name, int size);

/**
 * @brief Close an <b>Ashmem</b> region in the kernel.
 *
 * @param fd Indicates the file descriptor of an <b>Ashmem</b> region.
 */
void ShmemAdapterClose(int fd);

/**
 * @brief Obtains the size of a specific <b>Ashmem</b> region in the kernel.
 *
 * @param fd Indicates the file descriptor of an <b>Ashmem</b> region.
 * @return Returns the size of the <b>Ashmem</b> region.
 */
int ShmemAdapterGetSize(int fd);

/**
 * @brief Set Prot to an <b>Ashmem</b> region in the kernel.
 *
 * @param fd Indicates the file descriptor of an <b>Ashmem</b> region.
 * @param prot Indicates the prot attribute to an <b>Ashmem</b> region.
 * @return Returns the result.
 */
int ShmemAdapterSetProt(int fd, int prot);

/**
 * @brief Get Prot from an <b>Ashmem</b> region in the kernel.
 *
 * @param fd Indicates the file descriptor of an <b>Ashmem</b> region.
 * @return Returns the prot attribute.
 */
int ShmemAdapterGetProt(int fd);

/**
 * @brief Map an <b>Ashmem</b> region in the kernel.
 *
 * @param fd Indicates the file descriptor of an <b>Ashmem</b> region.
 * @param flag Indicates the flag to map
 */
void *ShmemAdapterMap(int fd, int flag);

/**
 * @brief UnMap an <b>Ashmem</b> region in the kernel.
 *
 * @param fd Indicates the file descriptor of an <b>Ashmem</b> region.
 * @param size Indicates the size to ummap from addr.
 */
void ShmemAdapterUnmap(void *addr, int size);

} // namespace OHOS
#endif

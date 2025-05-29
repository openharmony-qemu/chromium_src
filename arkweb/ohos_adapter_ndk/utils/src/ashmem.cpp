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

#include "ashmem_adapter.h"

#include <cerrno>
#include <cstdio>
#include <string>
#include <fcntl.h>
#include <linux/ashmem.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <dlfcn.h>
#include "securec.h"
#include "arkweb/ohos_nweb/src/nweb_hilog.h"

#ifndef EOK
#define EOK 0
#endif

namespace OHOS::NWeb {
static pthread_mutex_t g_ashmemLock = PTHREAD_MUTEX_INITIALIZER;

static int ShmemAdapterOpenLocked()
{
    int fd = open("/dev/ashmem", O_RDWR | O_CLOEXEC);
    if (fd < 0) {
        WVLOG_E("%{public}s: fd is invalid, fd = %{public}d, errno = %{public}d", __func__, fd, errno);
        return fd;
    }

    struct stat st;
    int ret = fstat(fd, &st);
    if (ret < 0) {
        WVLOG_E("%{public}s: Failed to exec fstat, ret = %{public}d, errno = %{public}d", __func__, ret, errno);
        close(fd);
        return ret;
    }

    if (!S_ISCHR(st.st_mode) || !st.st_rdev) {
        WVLOG_E("%{public}s: stat status is invalid, st_mode = %{public}u", __func__, st.st_mode);
        close(fd);
        return -1;
    }

    return fd;
}

static int ShmemAdapterOpen()
{
    pthread_mutex_lock(&g_ashmemLock);
    int fd = ShmemAdapterOpenLocked();
    pthread_mutex_unlock(&g_ashmemLock);
    return fd;
}

void ShmemAdapterClose(int fd)
{
    if (fd > 0) {
        ::close(fd);
    }
}

/*
 * ShmemAdapterCreate - create a new ashmem region and returns the file descriptor
 * fd < 0 means failed
 *
 */
int ShmemAdapterCreate(const char *name, int size)
{
    int ret = 0;

    if (size <= 0) {
        WVLOG_E("%{public}s: Parameter is invalid, size= %{public}d", __func__, size);
        return -1;
    }

    int fd = ShmemAdapterOpen();
    if (fd < 0) {
        WVLOG_E("%{public}s: Failed to exec ShmemAdapterOpen fd = %{public}d", __func__, fd);
        return fd;
    }

    if (name != nullptr) {
        char buf[ASHMEM_NAME_LEN] = {0};

        ret = strcpy_s(buf, sizeof(buf), name);
        if (ret != EOK) {
            WVLOG_E("%{public}s: Failed to exec strlcpy, name= %{public}s, ret= %{public}d", __func__, name, ret);
            close(fd);
            return -1;
        }
        ret = ioctl(fd, ASHMEM_SET_NAME, buf);
        if (ret < 0) {
            WVLOG_E("%{public}s: Failed to set name, name= %{public}s, ret= %{public}d, errno = %{public}d",
                __func__, name, ret,  errno);
            close(fd);
            return ret;
        }
    }

    ret = ioctl(fd, ASHMEM_SET_SIZE, size);
    if (ret < 0) {
        WVLOG_E("%{public}s: Failed to set size, size= %{public}d, errno = %{public}d", __func__, size, errno);
        close(fd);
        return ret;
    }

    return fd;
}

int ShmemAdapterSetProt(int fd, int prot)
{
    return ioctl(fd, ASHMEM_SET_PROT_MASK, prot);
}

int ShmemAdapterGetSize(int fd)
{
    return ioctl(fd, ASHMEM_GET_SIZE, NULL);
}

int ShmemAdapterGetProt(int fd)
{
    return ioctl(fd, ASHMEM_GET_PROT_MASK);
}

void *ShmemAdapterMap(int fd, int flag)
{
    void *startAddr = ::mmap(nullptr, ShmemAdapterGetSize(fd), flag, MAP_SHARED, fd, 0);
    if (startAddr == MAP_FAILED) {
        WVLOG_E("Failed to exec mmap, errno = %{public}d", errno);
        return startAddr;
    }

    return startAddr;
}

void ShmemAdapterUnmap(void *addr, int size)
{
    if (addr != nullptr) {
        ::munmap(addr, size);
    }
}

}

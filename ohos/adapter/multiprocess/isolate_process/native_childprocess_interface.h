// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef OHOS_ADAPTER_MULTIPROCESS_ISOLATE_PROCESS_NATIVE_CHILDPROCESS_INTERFACE_H_
#define OHOS_ADAPTER_MULTIPROCESS_ISOLATE_PROCESS_NATIVE_CHILDPROCESS_INTERFACE_H_

// Don't modify the code in this file.

#include <cstdint>
namespace ohos::adapter::multiprocess {
struct NativeChildProcess_Fd {
  /** the key of the file descriptor. */
  char* fdName = nullptr;

  /** the value of the file descriptor. */
  int32_t fd = 0;

  /** the next pointer of the linked list. */
  struct NativeChildProcess_Fd* next = nullptr;
};

/**
 * @brief The list of the info of the file descriptors passed to child process.
 * @since 12
 */
struct NativeChildProcess_FdList {
  /** the head of the list.
   * For details, see {@link NativeChildProcess_Fd}.
   */
  struct NativeChildProcess_Fd* head = nullptr;
};

/**
 * @brief Enumerates the isolation modes used by the native child process
 * module.
 * @since 12
 */
enum NativeChildProcess_IsolationMode {
  /**
   * Normal isolation mode, parent process shares the same sandbox or internet
   * with the child process.
   */
  NCP_ISOLATION_MODE_NORMAL = 0,

  /**
   * Isolated mode, parent process does not share the same sandbox or internet
   * with the child process.
   */
  NCP_ISOLATION_MODE_ISOLATED = 1,
};

/**
 * @brief The options used by the child process.
 * @since 12
 */
struct NativeChildProcess_Options {
  /** the isolation mode used by the child process.
   * For details, see {@link NativeChildProcess_IsolationMode}.
   */
  NativeChildProcess_IsolationMode isolationMode;

  /** reserved field for future extension purposes */
  int64_t reserved;
};

/**
 * @brief The arguments passed to the child process.
 * @since 12
 */
struct NativeChildProcess_Args {
  /** the entry parameter. */
  char* entryParams = nullptr;

  /** the list of the info of the file descriptors passed to child process.
   * For details, see {@link NativeChildProcess_FdList}.
   */
  struct NativeChildProcess_FdList fdList;
};

/**
 * @brief Starts a child process, loads the specified dynamic library file.
 *
 * @permission {@code ohos.permission.START_NATIVE_CHILD_PROCESS}
 * @param entry Name of the entry of the dynamic library file loaded in the
 * child process. The value cannot be nullptr.
 * @param args The arguments passed to the child process.
 * For details, see {@link NativeChildProcess_Args}.
 * @param options The child process options.
 * For details, see {@link NativeChildProcess_Options}.
 * @param pid The started child process id.
 * @return Returns {@link NCP_NO_ERROR} if the call is successful.\n
 * Returns {@link NCP_ERR_NO_PERMISSION} if the operation is not permitted.
 * The permission {@code ohos.permission.START_NATIVE_CHILD_PROCESS} is
 * needed.\n Returns {@link NCP_ERR_INVALID_PARAM} if the dynamic library name
 * or callback function pointer is invalid.\n Returns {@link
 * NCP_ERR_NOT_SUPPORTED} if the device does not support the creation of native
 * child processes.\n Returns {@link NCP_ERR_ALREADY_IN_CHILD} if it is not
 * allowed to create another child process in the child process.\n Returns
 * {@link NCP_ERR_MAX_CHILD_PROCESSES_REACHED} if the maximum number of native
 * child processes is reached.\n For details, see {@link
 * Ability_NativeChildProcess_ErrCode}.
 * @see OH_Ability_OnNativeChildProcessStarted
 * @since 12
 */
int32_t OH_Ability_StartNativeChildProcessFn(const char* entry,
                                             NativeChildProcess_Args args,
                                             NativeChildProcess_Options options,
                                             int32_t* pid);

bool IsStartNativeChildProcessAvailable();
}  // namespace ohos::adapter::multiprocess
#endif  // OHOS_ADAPTER_MULTIPROCESS_ISOLATE_PROCESS_NATIVE_CHILDPROCESS_INTERFACE_H_

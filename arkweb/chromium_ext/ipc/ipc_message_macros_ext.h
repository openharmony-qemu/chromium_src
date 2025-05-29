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

#ifndef IPC_IPC_MESSAGE_MACROS_H
#define IPC_IPC_MESSAGE_MACROS_H

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
#define IPC_MESSAGE_FORWARD_PARAM(msg_class, obj, member_func)         \
  case msg_class::ID: {                                                \
    IPC_TASK_ANNOTATOR_CONTEXT(msg_class)                              \
    if (!msg_class::Dispatch_Param(&ipc_message__, obj, this, param__, \
                                   &member_func))                      \
      ipc_message__.set_dispatch_error();                              \
  } break;


#define IPC_MESSAGE_HANDLER_PARAM(msg_class, member_func) \
  IPC_MESSAGE_FORWARD_PARAM(msg_class, this,              \
                            _IpcMessageHandlerClass::member_func)
#endif

#endif // IPC_IPC_MESSAGE_MACROS_H
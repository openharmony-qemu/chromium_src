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

#ifndef IPC_IPC_MESSAGE_TEMPLATES_H
#define IPC_IPC_MESSAGE_TEMPLATES_H

#if BUILDFLAG(ARKWEB_JAVASCRIPT_BRIDGE)
  template <class T, class S, class P, class Method>
  static bool Dispatch_Param(const Message* msg,
                             T* obj,
                             S* sender,
                             P* parameter,
                             Method func)
  {
    TRACE_EVENT0("ipc", Meta::kName);
    SendParam send_params;
    bool ok = ReadSendParam(msg, &send_params);
    Message* reply = SyncMessage::GenerateReply(msg);
    if (!ok) {
      NOTREACHED() << "Error deserializing message " << msg->type();
      reply->set_reply_error();
      sender->Send(reply);
      return false;
    }

    ReplyParam reply_params;
    base::DispatchToMethod_Param(obj, func, std::move(send_params), parameter,
                                 &reply_params);
    WriteParam(reply, reply_params);
    LogReplyParamsToMessage(reply_params, msg);
    sender->Send(reply);
    return true;
  }
#endif
#endif // IPC_IPC_MESSAGE_TEMPLATES_H
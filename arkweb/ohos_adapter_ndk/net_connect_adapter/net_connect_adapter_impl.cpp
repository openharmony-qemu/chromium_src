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
 
#include "net_connect_adapter_impl.h"

#include "net_connect_utils.h"
#include "net_capabilities_adapter_impl.h"
#include "net_connection_properties_adapter_impl.h"

#include "arkweb/ohos_nweb/src/nweb_hilog.h"

#include <network/netmanager/net_connection.h>
#include <network/netmanager/net_connection_type.h>
#include <telephony/cellular_data/telephony_data.h>
#include <telephony/core_service/telephony_radio.h>
#include <telephony/core_service/telephony_radio_type.h>

namespace OHOS::NWeb {
std::unordered_map<int32_t, std::shared_ptr<NetConnCallback>> NetConnectAdapterImpl::netConnCallbackMap_ = {};
std::shared_ptr<NetConnCallback> NetConnectAdapterImpl::pendingNetConnCb_ = nullptr;
std::shared_ptr<VpnListener> NetConnectAdapterImpl::cb_ = nullptr;
CommonEvent_SubscribeInfo *NetConnectAdapterImpl::commonEventSubscribeInfo_ = nullptr;
CommonEvent_Subscriber *NetConnectAdapterImpl::commonEventSubscriber_ = nullptr;

namespace {
const int32_t DEFAULT_VALUE = -1;
const int32_t BEARER_VPN = 4;
const int32_t NET_CONN_STATE_CONNECTED = 3;
const int32_t NET_CONN_STATE_DISCONNECTED = 5;
static const char* COMMON_EVENT_KEY_NET_TYPE = "NetType";
}
 
void NetConnectAdapterImpl::OnReceiveEvent(const CommonEvent_RcvData *data)
{
    if (strcmp(OH_CommonEvent_GetEventFromRcvData(data), COMMON_EVENT_CONNECTIVITY_CHANGE)) {
        return;
    }

    const CommonEvent_Parameters* para = OH_CommonEvent_GetParametersFromRcvData(data);
    if (para == nullptr) {
        WVLOG_E("failed to get patameters");
        return;
    }
 
    int32_t code = OH_CommonEvent_GetCodeFromRcvData(data);
    int32_t netType = DEFAULT_VALUE;
    if (OH_CommonEvent_HasKeyInParameters(para, COMMON_EVENT_KEY_NET_TYPE)) {
        netType = OH_CommonEvent_GetIntFromParameters(para, COMMON_EVENT_KEY_NET_TYPE, DEFAULT_VALUE);
    } else {
        WVLOG_E("failed to get netType");
    }
    WVLOG_I("receive COMMON_EVENT_CONNECTIVITY_CHANGE type: %{public}d code: %{public}d", netType, code);
    if (netType == DEFAULT_VALUE) {
        WVLOG_E("net vpn default net type");
        return;
    }
 
    if (netType != BEARER_VPN) {
        return;
    }
 
    if (!cb_) {
        WVLOG_E("net vpn listener is not set");
        return;
    }
 
    if (code == NET_CONN_STATE_CONNECTED) {
       WVLOG_I("vpn is available");
       cb_->OnAvailable();
       return;
    }
 
    if (code == NET_CONN_STATE_DISCONNECTED) {
        WVLOG_I("vpn is lost");
        cb_->OnLost();
        return;
    }
}
 
void NetConnectAdapterImpl::RegisterVpnListener(std::shared_ptr<VpnListener> cb)
{
    if (commonEventSubscriber_) {
      WVLOG_I("start vpn listen, common event subscriber has registered");
      return;
    }
 
    if (!cb)  {
      WVLOG_E("start vpn listen, register vpn listener failed cb is nullptr");
      return;
    }
    cb_ = cb;

    const char *events[] = {
        COMMON_EVENT_CONNECTIVITY_CHANGE
    };
    int count = sizeof(events) / sizeof(events[0]);
    commonEventSubscribeInfo_ = OH_CommonEvent_CreateSubscribeInfo(events, count);
    if (commonEventSubscribeInfo_ == nullptr) {
        WVLOG_E("Create SubscribeInfo failed.");
        return;
    }

    commonEventSubscriber_ = OH_CommonEvent_CreateSubscriber(commonEventSubscribeInfo_,
        OnReceiveEvent);
    if (commonEventSubscriber_ == nullptr) {
        OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
        commonEventSubscribeInfo_ = nullptr;
        WVLOG_E("Create Subscriber failed.");
        return;
    }

    CommonEvent_ErrCode ret = OH_CommonEvent_Subscribe(commonEventSubscriber_);
    if (ret != COMMONEVENT_ERR_OK) {
        OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
        OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
        commonEventSubscribeInfo_ = nullptr;
        commonEventSubscriber_ = nullptr;
        WVLOG_E("Subscribe failed. ret = %{public}d", ret);
        return;
    }

    if (HasVpnTransport()) {
        WVLOG_I("has vpn transport, vpn is available");
        cb->OnAvailable();
    }
}
 
bool NetConnectAdapterImpl::HasVpnTransport()
{
    NetConn_NetHandleList netHandleList;
    int32_t ret = OH_NetConn_GetAllNets(&netHandleList);
    if (ret != 0) {
        WVLOG_E("get all nets failed, ret = %{public}d.", ret);
        return false;
    }
 
    for (int32_t i = 0; i < netHandleList.netHandleListSize; i++) {
        NetConn_NetCapabilities netCapabilities;
        ret = OH_NetConn_GetNetCapabilities(&(netHandleList.netHandles[i]), &netCapabilities);
        if (ret != 0) {
            WVLOG_E("get default net capbilities failed, ret = %{public}d.", ret);
            continue;
        }
 
        for (int32_t j = 0; j < netCapabilities.bearerTypesSize; j++) {
            if (netCapabilities.bearerTypes[j] == NETCONN_BEARER_VPN) {
                return true;
            }
        }
    }
    return false;
}

 
void NetConnectAdapterImpl::UnRegisterVpnListener()
{
    WVLOG_I("stop vpn listen");
    if (commonEventSubscriber_) {
        CommonEvent_ErrCode errorCode = OH_CommonEvent_UnSubscribe(commonEventSubscriber_);
        if (errorCode == COMMONEVENT_ERR_OK) {
            OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
            OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
            commonEventSubscriber_ = nullptr;
            commonEventSubscribeInfo_ = nullptr;
        } else {
            WVLOG_E("stop vpn listen, unsubscribe common event failed");
        }
    }
}

NetConnectAdapterImpl::~NetConnectAdapterImpl()
{
    if (commonEventSubscriber_ != nullptr) {
        OH_CommonEvent_DestroySubscriber(commonEventSubscriber_);
    }
    if (commonEventSubscribeInfo_ != nullptr) {
        OH_CommonEvent_DestroySubscribeInfo(commonEventSubscribeInfo_);
    }
    commonEventSubscribeInfo_ = nullptr;
    commonEventSubscriber_ = nullptr;
}

int32_t NetConnectAdapterImpl::NetAvailable(std::shared_ptr<NetConnCallback> cb, NetConn_NetHandle *netHandle)
{
    WVLOG_I("NetConnCallback enter, net available, net id = %{public}d.", netHandle->netId);
    if (cb != nullptr) {
        cb->NetAvailable();
    }
    return 0;
}

int32_t NetConnectAdapterImpl::NetCapabilitiesChange(std::shared_ptr<NetConnCallback> cb,
                                                     NetConn_NetHandle *netHandle,
                                                     NetConn_NetCapabilities * netCapabilities)
{
    WVLOG_I("NetConnCallback enter, NetCapabilitiesChange, net id = %{public}d.", netHandle->netId);
    NetConnectSubtype subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
    Telephony_RadioTechnology radioTech = Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN;
    for (int32_t j = 0; j < netCapabilities->bearerTypesSize; j++) {
        if (netCapabilities->bearerTypes[j] == NETCONN_BEARER_CELLULAR) {
            int32_t slotId = OH_Telephony_GetDefaultCellularDataSlotId();
            if (slotId < 0) {
                WVLOG_E("get default soltId failed, ret = %{public}d.", slotId);
                slotId = 0;
            }
            Telephony_NetworkState networkState;
            Telephony_RadioResult radioRet = OH_Telephony_GetNetworkStateForSlot(slotId, &networkState);
            if (radioRet == TEL_RADIO_SUCCESS) {
                radioTech = networkState.cfgTech_;
                WVLOG_I("net radio tech = %{public}d.", static_cast<int32_t>(radioTech));
                subtype = NetConnectUtils::ConvertToConnectsubtype(radioTech);
            }
        }
        NetConnectType type = NetConnectUtils::ConvertToConnectType(netCapabilities->bearerTypes[j], radioTech);
        WVLOG_I("net connect type = %{public}s.", NetConnectUtils::ConnectTypeToString(type).c_str());
        if (cb != nullptr) {
            auto capabilites = std::make_shared<NetCapabilitiesAdapterImpl>();
            capabilites->SetNetId(netHandle->netId);
            capabilites->SetConnectType(type);
            capabilites->SetConnectSubtype(subtype);
            return cb->OnNetCapabilitiesChanged(capabilites);
        }
    }
 
    return 0;
}

int32_t NetConnectAdapterImpl::NetConnectionPropertiesChange(std::shared_ptr<NetConnCallback> cb,
                                                             NetConn_NetHandle *netHandle,
                                                             NetConn_ConnectionProperties *connConnetionProperties)
{
    WVLOG_I("NetConnCallback enter, NetConnectionPropertiesChange, net id = %{public}d.", netHandle->netId);
    if (cb != nullptr) {
        auto properties = std::make_shared<NetConnectionPropertiesAdapterImpl>();
        properties->SetNetId(netHandle->netId);
        return cb->OnNetConnectionPropertiesChanged(properties);
    }
    return 0;
}

void NetConnectAdapterImpl::InitNetConnCallback(NetConn_NetConnCallback *netConnCallback)
{
    netConnCallback->onNetworkAvailable = [](NetConn_NetHandle *netHandle) {
        if (netHandle == nullptr) {
            WVLOG_E("NetConnCallback enter, net available, netHandle is nullptr.");
            return;
        }
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            NetAvailable(it->second, netHandle);
        }
        if (pendingNetConnCb_ != nullptr) {
            NetAvailable(pendingNetConnCb_, netHandle);
        }
    };
    netConnCallback->onNetCapabilitiesChange = [](NetConn_NetHandle *netHandle, NetConn_NetCapabilities *netCap) {
        if (netHandle == nullptr || netCap == nullptr) {
            WVLOG_E("NetConnCallback enter, NetCapabilitiesChange, netHandle or netAllCap is nullptr.");
            return;
        }
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            NetCapabilitiesChange(it->second, netHandle, netCap);
        }
        if (pendingNetConnCb_ != nullptr) {
            NetCapabilitiesChange(pendingNetConnCb_, netHandle, netCap);
        }
    };
    netConnCallback->onConnetionProperties = [](NetConn_NetHandle *netHandle, NetConn_ConnectionProperties *connProp) {
        if (netHandle == nullptr || connProp == nullptr) {
            WVLOG_E("NetConnCallback enter, NetConnectionPropertiesChange, netHandle or info is nullptr.");
            return;
        }
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            NetConnectionPropertiesChange(it->second, netHandle, connProp);
        }
        if (pendingNetConnCb_ != nullptr) {
            NetConnectionPropertiesChange(pendingNetConnCb_, netHandle, connProp);
        }
    };
    netConnCallback->onNetLost = [](NetConn_NetHandle *netHandle) {
        WVLOG_I("NetConnCallback enter, NetLost, net id = %{public}d.", netHandle->netId);
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            if (it->second != nullptr) {
                it->second->NetUnavailable();
            }
        }
        if (pendingNetConnCb_ != nullptr) {
            pendingNetConnCb_->NetUnavailable();
        }
    };
    netConnCallback->onNetUnavailable = [](void) {
        WVLOG_I("NetConnCallback enter, NetUnavailable.");
        for (auto it = netConnCallbackMap_.begin(); it != netConnCallbackMap_.end(); it++) {
            if (it->second != nullptr) {
                it->second->NetUnavailable();
            }
        }
        if (pendingNetConnCb_ != nullptr) {
            pendingNetConnCb_->NetUnavailable();
        }
    };
    netConnCallback->onNetBlockStatusChange = [](NetConn_NetHandle *netHandle, bool blocked) {
        WVLOG_I("NetConnCallback enter, NetBlockStatusChange, net id = %{public}d, blocked = %{public}d.",
            netHandle->netId, blocked);
    };
}

int32_t NetConnectAdapterImpl::RegisterNetConnCallback(std::shared_ptr<NetConnCallback> cb)
{
    if (cb == nullptr) {
        WVLOG_E("register NetConnCallback, cb is nullptr.");
        return -1;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    uint32_t uid;
    NetConn_NetConnCallback netConnCallback;
    InitNetConnCallback(&netConnCallback);
    pendingNetConnCb_ = cb;
    int32_t ret = OH_NetConn_RegisterDefaultNetConnCallback(&netConnCallback, &uid);
    if (ret != 0) {
        WVLOG_E("register NetConnCallback failed, ret = %{public}d.", ret);
        return -1;
    }

    int32_t id = static_cast<int32_t>(uid);
    netConnCallbackMap_.insert(std::make_pair(id, pendingNetConnCb_));
    pendingNetConnCb_.reset();
    WVLOG_I("register NetConnCallback success.");
    return id;
}

int32_t NetConnectAdapterImpl::UnregisterNetConnCallback(int32_t id)
{
    auto it = netConnCallbackMap_.find(id);
    if (it == netConnCallbackMap_.end()) {
        WVLOG_E("unregister NetConnCallback, not find the NetConnCallback.");
        return -1;
    }

    int32_t ret = OH_NetConn_UnregisterNetConnCallback(id);
    if (ret != 0) {
        WVLOG_E("unregister NetConnCallback failed, ret = %{public}d.", ret);
        return -1;
    }

    netConnCallbackMap_.erase(it);
    WVLOG_I("uregister NetConnCallback success.");
    return 0;
}

int32_t NetConnectAdapterImpl::GetDefaultNetConnect(NetConnectType &type, NetConnectSubtype &subtype)
{
    NetConn_NetHandle netHandle;
    int32_t ret = OH_NetConn_GetDefaultNet(&netHandle);
    if (ret != 0) {
        WVLOG_E("get default net failed, ret = %{public}d.", ret);
        return -1;
    }
    WVLOG_I("get default net success, net id = %{public}d.", netHandle.netId);

    NetConn_NetCapabilities netCapabilities;
    ret = OH_NetConn_GetNetCapabilities(&netHandle, &netCapabilities);
    if (ret != 0) {
        WVLOG_E("get default net capbilities failed, ret = %{public}d.", ret);
        return -1;
    }
    WVLOG_I("get default net capbilities success");

    subtype = NetConnectSubtype::SUBTYPE_UNKNOWN;
    Telephony_RadioTechnology radioTech = Telephony_RadioTechnology::TEL_RADIO_TECHNOLOGY_UNKNOWN;
    auto bearerTypes = netCapabilities.bearerTypes[0];
    if (bearerTypes == NETCONN_BEARER_CELLULAR) {
        int32_t slotId = OH_Telephony_GetDefaultCellularDataSlotId();
        if (slotId < 0) {
            WVLOG_E("get default soltId failed, ret = %{public}d.", slotId);
            slotId = 0;
        }
        Telephony_NetworkState networkState;
        Telephony_RadioResult radioRet = OH_Telephony_GetNetworkStateForSlot(slotId, &networkState);
        if (radioRet == TEL_RADIO_SUCCESS) {
            radioTech = networkState.cfgTech_;
            WVLOG_I("net radio tech = %{public}d.", static_cast<int32_t>(radioTech));
            subtype = NetConnectUtils::ConvertToConnectsubtype(radioTech);
        }
    }
    type = NetConnectUtils::ConvertToConnectType(bearerTypes, radioTech);
    WVLOG_I("net connect type = %{public}s.", NetConnectUtils::ConnectTypeToString(type).c_str());
    return 0;
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServersInternal(NetConn_NetHandle &netHandle)
{
    std::vector<std::string> servers;
    NetConn_ConnectionProperties connetionProperties;
    int32_t ret = OH_NetConn_GetConnectionProperties(&netHandle, &connetionProperties);
    if (ret != 0) {
        WVLOG_E("get net properties failed, ret = %{public}d.", ret);
        return servers;
    }
    WVLOG_D("get net properties for dns servers success, net id = %{public}d, ", netHandle.netId);
    for (int i = 0; i < connetionProperties.dnsListSize; i++) {
        servers.emplace_back(connetionProperties.dnsList[i].address);
    }
    WVLOG_I("get dns servers success, net id = %{public}d, servers size = %{public}d.",
        netHandle.netId, static_cast<int32_t>(connetionProperties.dnsListSize));
    return servers;
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServers()
{
    NetConn_NetHandle netHandle;
    int32_t ret = OH_NetConn_GetDefaultNet(&netHandle);
    if (ret != 0) {
        WVLOG_E("get default net for dns servers failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }

    return GetDnsServersInternal(netHandle);
}

std::vector<std::string> NetConnectAdapterImpl::GetDnsServersForVpn()
{
    NetConn_NetHandleList netHandleList;
    int32_t ret = OH_NetConn_GetAllNets(&netHandleList);
    if (ret != 0) {
        WVLOG_E("get all nets by net id for dns servers failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }

    for (int32_t i = 0; i < netHandleList.netHandleListSize; i++) {
        NetConn_NetCapabilities netCapabilities;
        ret = OH_NetConn_GetNetCapabilities(&(netHandleList.netHandles[i]), &netCapabilities);
        if (ret != 0) {
            WVLOG_E("get default net capbilities failed, ret = %{public}d.", ret);
            continue;
        }

        for (int32_t j = 0; j < netCapabilities.bearerTypesSize; j++) {
            if (netCapabilities.bearerTypes[j] == NETCONN_BEARER_VPN) {
                WVLOG_I("cdw GetDnsServersForVpn, netHandleList.netHandles[i] = %{public}d.", netHandleList.netHandles[i]);
                return GetDnsServersInternal(netHandleList.netHandles[i]);
            }
        }
    }
    return std::vector<std::string>();
}

std::vector<std::string> NetConnectAdapterImpl::GetNetAddrListForVpn()
{
    NetConn_NetHandleList netHandleList;
    int32_t ret = OH_NetConn_GetAllNets(&netHandleList);
    if (ret != 0) {
        WVLOG_E("get all nets by net id for net addr list failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }

    for (int32_t i = 0; i < netHandleList.netHandleListSize; i++) {
        NetConn_NetCapabilities netCapabilities;
        ret = OH_NetConn_GetNetCapabilities(&(netHandleList.netHandles[i]), &netCapabilities);
        if (ret != 0) {
            WVLOG_E("get default net capbilities failed, ret = %{public}d.", ret);
            continue;
        }

        for (int32_t j = 0; j < netCapabilities.bearerTypesSize; j++) {
            if (netCapabilities.bearerTypes[j] == NETCONN_BEARER_VPN) {
                WVLOG_I("cdw GetNetAddrListForVpn, netHandleList.netHandles[i] = %{public}d.", netHandleList.netHandles[i]);
                return GetNetAddrListInternal(netHandleList.netHandles[i]);
            }
        }
    }
    return std::vector<std::string>();
}


std::vector<std::string> NetConnectAdapterImpl::GetDnsServersByNetId(int32_t netId)
{
    WVLOG_I("get dns servers by net id %{public}d.", netId);
    if (netId == -1) {
        return GetDnsServers();
    }

    NetConn_NetHandleList netHandleList;
    int32_t ret = OH_NetConn_GetAllNets(&netHandleList);
    if (ret != 0) {
        WVLOG_E("get all nets by net id for dns servers failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }

    for (int i = 0; i < netHandleList.netHandleListSize; i++) {
        if (netHandleList.netHandles[i].netId == netId) {
            return GetDnsServersInternal(netHandleList.netHandles[i]);
        }
    }
    return std::vector<std::string>();
}

std::vector<std::string> NetConnectAdapterImpl::GetNetAddrListByNetId(int32_t netId)
{
    WVLOG_I("get net address by net id %{public}d.", netId);
    if (netId == -1) {
        return std::vector<std::string>();
    }
 
    NetConn_NetHandleList netHandleList;
    int32_t ret = OH_NetConn_GetAllNets(&netHandleList);
    if (ret != 0) {
        WVLOG_E("get all nets by net id for net address failed, ret = %{public}d.", ret);
        return std::vector<std::string>();
    }
 
    for (int i = 0; i < netHandleList.netHandleListSize; i++) {
        if (netHandleList.netHandles[i].netId == netId) {
            return GetNetAddrListInternal(netHandleList.netHandles[i]);
        }
    }
    return std::vector<std::string>();
}
 
std::vector<std::string> NetConnectAdapterImpl::GetNetAddrListInternal(NetConn_NetHandle &netHandle)
{
    std::vector<std::string> netAddrList;
    NetConn_ConnectionProperties connetionProperties;
    int32_t ret = OH_NetConn_GetConnectionProperties(&netHandle, &connetionProperties);
    if (ret != 0) {
        WVLOG_E("get net properties failed, ret = %{public}d.", ret);
        return netAddrList;
    }
    WVLOG_D("get net properties for net address success, net id = %{public}d, ", netHandle.netId);
    for (int i = 0; i < connetionProperties.netAddrListSize; i++) {
        netAddrList.emplace_back(connetionProperties.netAddrList[i].address);
    }
    WVLOG_I("get net address success, net id = %{public}d, address size = %{public}d.",
        netHandle.netId, static_cast<int32_t>(connetionProperties.netAddrListSize));
    return netAddrList;
}

} // namespace OHOS::NWeb
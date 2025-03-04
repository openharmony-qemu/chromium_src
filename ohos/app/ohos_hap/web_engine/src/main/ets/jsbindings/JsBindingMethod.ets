// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import AccessibilityAdapterBind from '../jsbindings/AccessibilityAdapterBind';
import AppLifecycleAdapterBind from './AppLifecycleAdapterBind';
import AppWindowAdapterBind from '../jsbindings/AppWindowAdapterBind';
import BatteryAdapterBind from '../jsbindings/BatteryAdapterBind';
import BluetoothAdapterBind from '../jsbindings/BluetoothAdapterBind';
import BluetoothLowEnergyAdapterBind from '../jsbindings/BluetoothLowEnergyAdapterBind';
import CertManagerAdapterBind from '../jsbindings/CertManagerAdapterBind';
import ContextPathAdapterBind from '../jsbindings/ContextPathAdapterBind';
import CursorAdapterBind from '../jsbindings/CursorAdapterBind';
import DefaultApplicationAdapterBind from '../jsbindings/DefaultApplicationAdapterBind';
import DeviceAdapterBind from '../jsbindings/DeviceAdapterBind';
import DeviceInfoAdapterBind from './DeviceInfoAdapterBind';
import DeviceUserAuthAdapterBind from '../jsbindings/DeviceUserAuthAdapterBind';
import DisplayAdapterBind from '../jsbindings/DisplayAdapterBind';
import DragDropAdapterBind from '../jsbindings/DragDropAdapterBind';
import ExternalProtocolAdapterBind from '../jsbindings/ExternalProtocolAdapterBind';
import FileManagerAdapterBind from '../jsbindings/FileManagerAdapterBind';
import FilePickerAdapterBind from '../jsbindings/FilePickerAdapterBind';
import FontAdapterBind from '../jsbindings/FontAdapterBind';
import GeolocationAdapterBind from '../jsbindings/GeolocationAdapterBind';
import I18nAdapterBind from '../jsbindings/I18nAdapterBind';
import IMFAdapterBind from '../jsbindings/IMFAdapterBind';
import MediaAdapterBind from '../jsbindings/MediaAdapterBind';
import NativeThemeAdapterBind from '../jsbindings/NativeThemeAdapterBind';
import NetConnectionAdapterBind from '../jsbindings/NetConnectionAdapterBind';
import NotificationAdapterBind from '../jsbindings/NotificationAdapterBind';
import OcrAdapterBind from '../jsbindings/OcrAdapterBind';
import PasteBoardAdapterBind from '../jsbindings/PasteBoardAdapterBind';
import PermissionManagerAdapterBind from '../jsbindings/PermissionManagerAdapterBind';
import PowerMonitorAdapterBind from '../jsbindings/PowerMonitorAdapterBind';
import PrintAdapterBind from '../jsbindings/PrintAdapterBind';
import ProcessAdapterBind from '../jsbindings/ProcessAdapterBind';
import RunningLockAdapterBind from '../jsbindings/RunningLockAdapterBind';
import ScreenlockMonitorAdapterBind from '../jsbindings/ScreenlockMonitorAdapterBind';
import ShapeDetectionAdapterBind from '../jsbindings/ShapeDetectionAdapterBind';
import SpeechAdapterBind from '../jsbindings/SpeechAdapterBind';
import SubWindowAdapterBind from '../jsbindings/SubWindowAdapterBind';
import TracingAdapterBind from '../jsbindings/TracingAdapterBind';

export default class JsBindingMethod {
  static bind() {
    AccessibilityAdapterBind.bind();
    AppLifecycleAdapterBind.bind();
    AppWindowAdapterBind.bind();
    BatteryAdapterBind.bind();
    BluetoothAdapterBind.bind();
    BluetoothLowEnergyAdapterBind.bind();
    CertManagerAdapterBind.bind();
    ContextPathAdapterBind.bind();
    CursorAdapterBind.bind();
    DefaultApplicationAdapterBind.bind();
    DeviceAdapterBind.bind();
    DeviceInfoAdapterBind.bind();
    DeviceUserAuthAdapterBind.bind();
    DisplayAdapterBind.bind();
    DragDropAdapterBind.bind();
    ExternalProtocolAdapterBind.bind();
    FileManagerAdapterBind.bind();
    FilePickerAdapterBind.bind();
    FontAdapterBind.bind();
    GeolocationAdapterBind.bind();
    I18nAdapterBind.bind();
    IMFAdapterBind.bind();
    MediaAdapterBind.bind();
    NativeThemeAdapterBind.bind();
    NetConnectionAdapterBind.bind();
    NotificationAdapterBind.bind();
    OcrAdapterBind.bind();
    PasteBoardAdapterBind.bind();
    PermissionManagerAdapterBind.bind();
    PowerMonitorAdapterBind.bind();
    PrintAdapterBind.bind();
    ProcessAdapterBind.bind();
    RunningLockAdapterBind.bind();
    ScreenlockMonitorAdapterBind.bind();
    ShapeDetectionAdapterBind.bind();
    SpeechAdapterBind.bind();
    SubWindowAdapterBind.bind();
    TracingAdapterBind.bind();
  }
}

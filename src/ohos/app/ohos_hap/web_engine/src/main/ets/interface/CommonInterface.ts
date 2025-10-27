// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import type ConfigurationConstant from '@ohos.app.ability.ConfigurationConstant';
import type image from '@ohos.multimedia.image';
import type inputMethod from '@ohos.inputMethod';
import type GestureEvent from '@ohos.multimodalInput.gestureEvent';

export class PowerMonitor {
  OnSuspend: () => void;
  OnResume: () => void;
  OnPowerStateChanged: () => void;
}

export interface WindowBound {
  left: number;
  top: number;
  width: number;
  height: number;
}

export class JSBind {
  bindFunction: (name: string, func: Function) => number;
}

export interface NativeContext {
  runBrowser: (vec_args: string[]) => void;
  runOtherProcessType: (processType: number) => void;
  registerLifecycle: () => void;
  startNewWindow: (startUri: string) => void;
  GetLastActive: () => number;
  readImageFromReceiver: (receiver: image.ImageReceiver) => image.Image;
  JSBind: JSBind;
  OnPanEventCB: (action: number, id: string, event: GestureEvent) => void;
  OnPinchEventCB: (pinch_step: string, id: string, event: GestureEvent) => void;
  InsertTextCallback: (text: string) => void;
  DeleteBackCallback: (length: number) => void;
  DeleteForwardCallback: (length: number) => void;
  SendEnterKeyEventCallback: () => void;
  MoveCursorCallback: (direction: inputMethod.Direction) => void;
  SetThemeSource: (themeSource: ConfigurationConstant.ColorMode) => void;
  OnDragEnterCB: (id: string, dragInfo: OhosDropData, filePaths: Array<string>) => void;
  OnDragLeaveCB: (id: string) => void;
  OnDragEndCB: (id: string) => void;
  OnDragMoveCB: (id: string, windowX: number, windowY: number) => void;
  OnDropCB: (id: string, dragInfo: OhosDropData, filePaths: Array<string>) => void;
  OnFontSizeChangeCallback:(fontSizeZoom :number) => void;
  OnWindowInitSize: (windowRect: WindowBound, drawableRect: WindowBound) => void;
  OnWindowRectChange: (id: string, event: WindowBound, reason: number) => void;
  OnWindowSizeChange: (id: string, event: WindowBound) => void;
  OnWindowEvent: (id: string, event: number) => void;
  OnWindowVisibilityChange: (id: string, visible: boolean) => void;
  OnNotificationClickCallback: (id: number) => void;
  OnNotificationCloseCallback: (id: number) => void;
  OnNotificationButtonClickCallback: (id: number, buttonIndex) => void;
  OnDisplayChangeCallback: (even: string, id: number) => void;
  OnRequestCloseWindow: (id: string) => void;
  PowerMonitor: PowerMonitor;
}

export interface XComponentContext {
  getNativeContext: (contextType: number) => NativeContext;
}

export interface IParams {
  callback: (ready: boolean, id: string) => void,
  id: string,
  size: number[], // [width, height]
  initColorRgb: string,
}

export interface OhosBasicDragData {
  text: string;
  url: string;
  urlTitle: string;
  html: string;
  htmlBaseUrl: string;
}

export interface OhosDragParamToJs {
  basicData: OhosBasicDragData;
  imageTempUri: string;
  width: number;
  height: number;
  bookmarkBuffer: ArrayBuffer;
  webCustomBuffer: ArrayBuffer;
  pixelBuffer: ArrayBuffer;
  windowId: string;
  touchX: number;
  touchY: number;
}

export interface OhosDropData {
  basicData: OhosBasicDragData;
  filePaths: Array<string>;
  bookmarkBuffer: ArrayBuffer | undefined;
  webCustomBuffer: ArrayBuffer | undefined;
}

export interface IMFAdapterInputAttribute {
  inputPattern: inputMethod.TextInputType;
  enterKeyType: inputMethod.EnterKeyType;
}

export interface IMFAdapterCursorInfo {
  left: number;
  top: number;
  width: number;
  height: number;
}

export interface IMFAdapterTextConfig {
  inputAttribute: IMFAdapterInputAttribute;
  cursorInfo: IMFAdapterCursorInfo;
}

export interface NotificationAdapterImage {
  width: number;
  height: number;
  buff: ArrayBuffer;
}

export interface NotificationAdapterButton {
  title: string;
  buttonIndex: number;
}

export interface OcrAdapterImage {
  width: number;
  height: number;
  buff: ArrayBuffer;
}

export interface PixelPoint {
  x: number;
  y: number;
}

export interface TextWord {
  value: string;
  cornerPoints: Array<PixelPoint>;
}

export interface NotificationAdapterRequest {
  notificationId: number;
  title: string;
  message: string;
  requireInteraction: boolean;
  silent: boolean;
  timestamp: number;
  icon: NotificationAdapterImage;
  buttons: NotificationAdapterButton[];
}

export interface OhosPasteDataRecord {
  html_text: string;
  mime_type: string;
  plain_text: string;
}

export interface SpeakingParamsExtraParams {
  speed?: number;
  volume?: number;
  pitch?: number;
  languageContext?: string;
  audioType?: string;
  playType?: number;
  soundChannel?: number;
  queueMode?: number;
}

export interface SpeakingParams {
  requestId: string;
  extraParams?: SpeakingParamsExtraParams;
}

export interface EngineCreationParamsExtraParams {
  style?: string;
  locate?: string;
  name?: string;
}

export interface EngineCreationParams {
  language: string;
  online: number;
  person: number;
  extraParams?: EngineCreationParamsExtraParams;
}

export interface VoiceQueryExtraParams {
  language?: string;
  person?: number;
}

export interface VoiceQuery {
  requestId: string;
  online: number;
  extraParams?: VoiceQueryExtraParams
}

export interface VoiceInfo {
  language: string;
  person: number;
  style: string;
  status: string;
  gender: string;
  description: string;
}

export interface AdvertisingParam {
  connectable: boolean;
  service_uuids: string[];
  manufacturer_data: Map<number, Uint8Array>;
  service_data: Map<string, Uint8Array>;
  scan_response_data: Map<number, Uint8Array>;
}

export interface BatteryInfo {
  batterySOC: number,
  chargingStatus: number,
  isBatteryPresent: boolean,
  estimatedRemainingChargeTime: number,
  nowCurrent: number,
  remainingEnergy: number
}

export interface NewWindowParam {
  id: string,
  left: number,
  top: number,
  width: number,
  height: number,
  hide_title_bar: boolean
}

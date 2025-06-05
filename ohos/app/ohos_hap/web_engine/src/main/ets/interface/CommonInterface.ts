/*
 * Copyright (c) 2023-2025 Haitai FangYuan Co., Ltd.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 *    of conditions and the following disclaimer in the documentation and/or other materials
 *    provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 *    to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

import type image from '@ohos.multimedia.image';
import type inputMethod from '@ohos.inputMethod';
import type GestureEvent from '@ohos.multimodalInput.gestureEvent';
import type ConfigurationConstant from '@ohos.app.ability.ConfigurationConstant';
import type common from '@ohos.app.ability.common';
import type window from '@ohos.window';

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

export interface CaptionButtonRect {
  right: number;
  top: number;
  width: number;
  height: number;
}

export class JSBind {
  bindFunction: (name: string, func: Function) => number;
}

export interface NativeContext {
  runBrowser: (vec_args: string[]) => void;
  BrowserDestroyed: () => boolean;
  runOtherProcessType: (processType: number) => void;
  registerLifecycle: () => void;
  startNewWindow: (startUri: string, force_open: boolean) => void;
  GetLastActiveWidgetId: () => number;
  AllocateWidgetId: () => number;
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
  OnDragEnterCB: (id: string, dragInfo: OhosDropData, fileUris: Array<string>) => void;
  OnDragLeaveCB: (id: string) => void;
  OnDragEndCB: (id: string) => void;
  OnDragMoveCB: (id: string, windowX: number, windowY: number) => void;
  OnDropCB: (id: string, dragInfo: OhosDropData, fileUris: Array<string>) => void;
  OnFontSizeChangeCallback:(fontSizeZoom :number) => void;
  OnWindowInitSize: (windowRect: WindowBound, drawableRect: WindowBound) => void;
  OnWindowStatusChange: (id: string, status: window.WindowStatusType) => void;
  OnWindowVisibleChange: (windowId: String, visible: boolean) => void;
  OnWindowInitState: (state: window.WindowStatusType) => void;
  OnWindowRectChange: (id: string, event: WindowBound, reason: number) => void;
  OnWindowSizeChange: (id: string, event: WindowBound) => void;
  OnWindowEvent: (id: string, event: number) => void;
  OnWindowVisibilityChange: (id: string, visible: boolean) => void;
  OnNotificationClickCallback: (id: number) => void;
  OnNotificationCloseCallback: (id: number) => void;
  OnNotificationButtonClickCallback: (id: number, buttonIndex) => void;
  OnDisplayChangeCallback: (even: string, id: number) => void;
  PowerMonitor: PowerMonitor;
  GetBrowserCloseResponse: (id: number) => BrowserCloseResponse;
  RegisterWindowEventFilter: (origin_window_id: number) => void;
  ClearWindowEventFilter: (origin_window_id: number) => void;
  OnCaptionButtonRectChange: (id: string, event: CaptionButtonRect) => void;
}

export interface IParams {
  callback: (ready: boolean, id: string) => void,
  id: string,
  size: number[], // [width, height]
  initColorRgb: string,
}

export interface OhosDragParamToJs {
  text: string;
  url: string;
  urlTitle: string;
  html: string;
  webImageFilePath: string;
  bookmarkBuffer: ArrayBuffer;
  webCustomBuffer: ArrayBuffer;
  pixelMapBuffer: ArrayBuffer;
  pixelMapWidth: number;
  pixelMapHeight: number;
  pixelMapTouchX: number;
  pixelMapTouchY: number;
  windowId: string;
}

export interface OhosDropData {
  text: string;
  url: string;
  urlTitle: string;
  html: string;
  fileUris: Array<string>;
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
  parent_id: string
  window_id: string,
  bounds: WindowBound
  init_color_argb: string,
  hide_title_bar: boolean,
  use_dark_mode: boolean,
  is_stateless: boolean,
  caption_button_visible: boolean
}

export interface ISubWindowInfo {
  id: string,
  parentId: string,
  subWindow: window.Window,
  localStorage: LocalStorage,
}

export interface SelectFileDialogParams {
  multi_files: boolean,
  extensions: Array<Array<string>>,
  descriptions: Array<string>,
  include_all_files: boolean
}

export interface SaveAsDialogParams {
  file_name: string,
  dir_name: string,
  extensions: Array<Array<string>>,
  descriptions: Array<string>,
  include_all_files: boolean
}

export interface PointCoordinate {
  x: number
  y: number,
  displayId: number,
}

export enum BrowserCloseResponse {
  kUndetermined,
  kClosingContinue,
  kClosingInterrupt,
  kClosed,
  kCloseCancelled,
  kClosedAnyway,
}

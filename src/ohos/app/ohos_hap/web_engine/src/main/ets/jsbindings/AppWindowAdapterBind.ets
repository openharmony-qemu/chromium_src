// Copyright (c) 2024 Huawei Device Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

import JsBindingUtils from '../utils/JsBindingUtils';
import Inject from "../common/InjectModule";
import AppWindowAdapter from '../adapter/AppWindowAdapter';
import { NewWindowParam, WindowBound } from '../interface/CommonInterface';

function implAppWindowAdapter(): AppWindowAdapter {
  return Inject.get(AppWindowAdapter);
}

function createWindow(param: NewWindowParam) {
  implAppWindowAdapter().createWindow(param);
}

function getScreenWidth(): number {
  return implAppWindowAdapter().getScreenWidth();
}

function getScreenHeight(): number {
  return implAppWindowAdapter().getScreenHeight();
}

function closeWindow(id: number) {
  implAppWindowAdapter().closeWindow(id);
}

function showWindow(id: number) {
  implAppWindowAdapter().showWindow(id);
}

function activateWindow(id: number) {
  implAppWindowAdapter().activateWindow(id);
}

function setFullscreen(id: number) {
  implAppWindowAdapter().setFullscreen(id);
}

function setBounds(id: number,
                   bounds: WindowBound,
                   callback?: Function) {
  implAppWindowAdapter().setBounds(id, bounds, callback);
}

function setEnabled(enable: boolean, id: number) {
  implAppWindowAdapter().setEnabled(enable, id);
}

function setWindowLimits(minWidth: number, minHeight: number, maxWidth: number, maxHeight: number, id: number) {
  implAppWindowAdapter().setWindowLimits(minWidth, minHeight, maxWidth, maxHeight, id);
}

function maximize(id: number) {
  implAppWindowAdapter().maximize(id);
}

function unMaximize(id: number) {
  implAppWindowAdapter().unMaximize(id);
}

function minimize(id: number) {
  implAppWindowAdapter().minimize(id);
}

function setTitle(title:string, id: number) {
  implAppWindowAdapter().setTitle(title, id);
}

export default class AppWindowAdapterBind {
  static bind() {
    JsBindingUtils.bindFunction("AppWindow.CreateWindow", createWindow);
    JsBindingUtils.bindFunction("AppWindow.GetScreenWidth", getScreenWidth);
    JsBindingUtils.bindFunction("AppWindow.GetScreenHeight", getScreenHeight);
    JsBindingUtils.bindFunction("AppWindow.CloseWindow", closeWindow);
    JsBindingUtils.bindFunction("AppWindow.SetFullscreen", setFullscreen);
    JsBindingUtils.bindFunction("AppWindow.SetBounds", setBounds);
    JsBindingUtils.bindFunction("AppWindow.SetEnabled", setEnabled);
    JsBindingUtils.bindFunction("AppWindow.SetWindowLimits", setWindowLimits);
    JsBindingUtils.bindFunction("AppWindow.Maximize", maximize);
    JsBindingUtils.bindFunction("AppWindow.UnMaximize", unMaximize);
    JsBindingUtils.bindFunction("AppWindow.Minimize", minimize);
    JsBindingUtils.bindFunction("AppWindow.SetTitle", setTitle);
    JsBindingUtils.bindFunction("AppWindow.ShowWindow", showWindow);
    JsBindingUtils.bindFunction("AppWindow.ActivateWindow", activateWindow);
  }
}


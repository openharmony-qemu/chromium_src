/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include <thread>

#include "aki/jsbind.h"
#include "aki/logging/logging.h"
bool DoSomethingFromNative()
{
    if (auto createTable = aki::JSBind::GetJSFunction("MyStorage.createTable")) {
        createTable->Invoke<void>("MYSTORE"); // 返回值类型 int，入参类型 string
    }

    if (auto insert = aki::JSBind::GetJSFunction("MyStorage.insert")) {
        insert->Invoke<void>("MYSTORE"); // 无返回值，入参类型 string
    }

    if (auto query = aki::JSBind::GetJSFunction("MyStorage.query")) {
        query->Invoke<void>("MYSTORE"); // 无返回值，入参类型 string
    }

    // 跨线程调用场景可使用 std::promise 异步转同步
    std::thread subThread([]() {
        // 子线程
        if (auto insert = aki::JSBind::GetJSFunction("MyStorage.insert")) {
            std::promise<std::string> insertPromise;
            std::function<void(std::string)> insertCallback = [&insertPromise](std::string message) {
                AKI_DLOG(INFO) << "callback will be invoked in JS Thread.";
                insertPromise.set_value(message);
            };
            insert->Invoke<void>("MYSTORE", insertCallback); // 返回值类型 int，入参类型 string, function
            AKI_DLOG(INFO) << "wait until promise set value.";
            std::string message = insertPromise.get_future().get(); // 子线程阻塞
            AKI_DLOG(INFO) << "insert with message: " << message;
        }
    });
    subThread.detach();

    return true;
}

bool  DoSomethingFromNativeAsync()
{
    std::thread subThread([]() {
        if (auto func = aki::JSBind::GetJSFunction("helloAsync")) {
            std::function<void(std::string)> callback = [](std::string message) {
                AKI_DLOG(INFO) << "helloAsync callback will be invoked in JS Thread:"<<message;
            };
            func->InvokeAsync<void>("helloAsync hello world", callback);
            AKI_DLOG(INFO) << "helloAsync start.";
        }
    });
    subThread.detach();
    return true;
}

bool DoSomethingFromNativeAsyncReturnValue() {
    std::thread subThread([]() {
        if (auto func = aki::JSBind::GetJSFunction("helloAsyncReturnValue")) {
            std::promise<std::string> promise;
            std::function<void(std::string)> callback = [&promise](std::string message) {
                AKI_DLOG(INFO) << "helloAsyncReturnValue callback will be invoked in JS Thread:" << message;
                promise.set_value(message);
            };
            auto result= func->InvokeAsync<std::string>("helloAsyncReturnValue hello world", callback);
            std::string returnValue= result.get();
            AKI_DLOG(INFO) << "helloAsyncReturnValue return value:" << returnValue;
            AKI_DLOG(INFO) << "helloAsyncReturnValue wait until promise set value.";
            std::string message = promise.get_future().get(); // 子线程阻塞
            AKI_DLOG(INFO) << "helloAsyncReturnValue promise with message: " << message;
        }
    });
    subThread.detach();
    return true;
}

// 使用libuv抛到主线程执行任务，任务里的JS对象进行GC回收测试
static uv_loop_t *mainLoop = nullptr;

struct AsyncData {
    std::function<void()> taskFunction;

    explicit AsyncData(std::function<void()> &&data)
    {
        this->taskFunction = data;
    }

    ~AsyncData() =default;
};

static void MainThreadCallback(uv_async_t *handle)
{
    AKI_DLOG(INFO) << "enter MainThreadCallback";
    AsyncData *data = static_cast<AsyncData *>(handle->data);
    if (data && data->taskFunction) {
        data->taskFunction();
    }
    delete data;
    uv_close((uv_handle_t *)handle, [](uv_handle_t *closeingHandle) { delete closeingHandle; });
}

static void DispatchToMainThread(std::function<void()> task)
{
    AKI_DLOG(INFO) << "enter DispatchToMainThread";
    uv_async_t *async = new uv_async_t;
    AsyncData *data = new AsyncData(std::move(task));
    async->data = data;

    uv_async_init(mainLoop, async, MainThreadCallback);
    uv_async_send(async);
}

void DoSomethingFromNativeUV(void)
{
    napi_get_uv_event_loop(aki::JSBind::GetScopedEnv(), &mainLoop);
    std::thread t([]() {
        struct CleanResource {
            ~CleanResource() { AKI_DLOG(INFO) << "enter ~CleanResource"; }
        };
        using CallbackFunction1 = std::function<void()>;

        auto resource1 = std::make_shared<CleanResource>();

        CallbackFunction1 callfunc = [resource1 = std::move(resource1)]() {
            AKI_DLOG(INFO) << "enter callfunc" << resource1.use_count();
        };

        DispatchToMainThread([callfunc = std::move(callfunc)]() {
            auto jsfunc = aki::JSBind::GetJSFunction("executeTestArkTSAbility");
            AKI_DLOG(INFO) << "invoke value :" << jsfunc->Invoke<std::string>(callfunc);
        });
    });
    t.detach();
}

JSBIND_GLOBAL() {
    JSBIND_FUNCTION(DoSomethingFromNative);
    JSBIND_FUNCTION(DoSomethingFromNativeAsync);
    JSBIND_FUNCTION(DoSomethingFromNativeAsyncReturnValue);
    JSBIND_FUNCTION(DoSomethingFromNativeUV);
}

JSBIND_ADDON(entry) // 注册 AKI 插件名为: entry

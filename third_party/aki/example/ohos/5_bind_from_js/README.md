## Example 5: *Bind From JS*

环境依赖：
* IDE： DevEco Studio 4.0 Beta2
* SDK：4.0.9.6
* IDE： DevEco Studio 3.1.0.500
* SDK：3.2.12.2

### 1. 依赖配置

- 本例使用源码依赖

    ```bash
    cd entry/src/main/cpp
    git clone
    ```

    CMakeLists.txt 配置依赖
    ```cmake
    add_subdirectory(aki)
    target_link_libraries(entry PUBLIC aki_jsbind)
    ```

### 2. 用例说明

本例使用 `AKI` 绑定 JavaScript 鸿蒙 [关系型数据库](https://docs.openharmony.cn/pages/v3.2Beta/zh-cn/application-dev/reference/apis/js-apis-data-rdb.md/#datardbgetrdbstore) 业务函数。

`index.d.ts`:
```js
export const DoSomethingFromNative: () => boolean;

export const DoSomethingFromNativeAsync: () => boolean;

export class JSBind {
      static bindFunction: (name: string, func: Function) => number;
}
```

`index.ets`:
```js
class MyStorage {
  static rdbStore: data_rdb.RdbStore;

  static createTable(table: string, callback?: Function) {
    let rdbStore = MyStorage.rdbStore;
    console.log('createTable: ' + rdbStore)
    const SQL_CREATE_TABLE = "CREATE TABLE IF NOT EXISTS " + table + " (ID INTEGER PRIMARY KEY AUTOINCREMENT, NAME TEXT NOT NULL, AGE INTEGER, SALARY REAL, CODES BLOB)"
    rdbStore.executeSql(SQL_CREATE_TABLE, [], (err: object) => {
      if (err) {
        console.info("ExecuteSql failed, err: " + err)
        if (callback != undefined)
          callback("Create failed");
        return
      }
      console.info('Create table done.');
      if (callback != undefined)
        callback("Create Success");
    })
    return;
  }

  static insert(table: string, callback?: Function) {

    let rdbStore = MyStorage.rdbStore;
    console.log('hello.cpp insert: ' + rdbStore)
    rdbStore.insert(table, {
      "NAME": "Lisa",
      "AGE": 18,
      "SALARY": 100.5,
      "CODES": new Uint8Array([1, 2, 3, 4, 5]),
    }, (status, rowId) => {
      if (status) {
        console.log("Insert is failed");
        if (callback != undefined)
          callback("hello.cpp Insert failed");
        return;
      }
      console.log("Insert is successful, rowId = " + rowId);
      if (callback != undefined)
        callback("hello.cpp Insert Success");
    })
  }

  static query(table: string, callback?: Function) {
    let predicates = new data_rdb.RdbPredicates(table)
    predicates.equalTo("NAME", "Lisa")
    let rdbStore = MyStorage.rdbStore;
    console.log('query: ' + rdbStore)
    rdbStore.query(predicates, ["NAME", "AGE", "SALARY", "CODES"], (err, resultSet) => {
      if (err) {
        console.info("Query failed, err: " + err)
        if (callback != undefined)
          callback("Query failed");
        return
      }
      console.log("ResultSet column names: " + resultSet.columnNames)
      console.log("ResultSet column count: " + resultSet.columnCount)
      if (callback != undefined)
        callback("Query Success");
    })
  }
}

data_rdb.getRdbStore(getContext(this), { name: "RdbTest.db" }, 1, (err, rdbStore) => {
  if (err) {
    console.info("Get RdbStore failed, err: " + err)
    return
  }
  console.log("Get RdbStore successfully.")
  MyStorage.rdbStore = rdbStore;
});

// JS 侧绑定静态方法 (name: 方法名, function: 方法)
libAddon.JSBind.bindFunction("MyStorage.createTable", MyStorage.createTable);
libAddon.JSBind.bindFunction("MyStorage.insert", MyStorage.insert);
libAddon.JSBind.bindFunction("MyStorage.query", MyStorage.query);


function helloAsync(msg: string, callback?: Function) {
  console.log("helloAsync js:" + msg)
  if (callback != undefined) {
    callback("helloAsync response hello")
  }
}


function helloAsyncReturnValue(msg: string, callback?: Function) {
  console.log("helloAsyncReturnValue js:" + msg)
  if (callback != undefined) {
    callback("helloAsyncReturnValue response hello")
  }
  return "helloAsyncReturnValue return helloworld"
}

libAddon.JSBind.bindFunction("helloAsync", helloAsync);
libAddon.JSBind.bindFunction("helloAsyncReturnValue", helloAsyncReturnValue);

```

`C++`：

```C
// aki::JSBind::GetJSFunction 获取 JS 函数句柄
auto createTable = aki::JSBind::GetJSFunction("MyStorage.createTable");
int result = createTable->Invoke<int>("MYSTORE"); // 返回值类型 int，入参类型 string

auto insert = aki::JSBind::GetJSFunction("MyStorage.insert");
insert->Invoke<void>("MYSTORE"); // 无返回值，入参类型 string

auto query = aki::JSBind::GetJSFunction("MyStorage.query");
query->Invoke<void>("MYSTORE"); // 无返回值，入参类型 string

// 支持跨线程调用JS函数
std::thread t([] () {
    auto createTable = aki::JSBind::GetJSFunction("MyStorage.createTable");
    int result = createTable->Invoke<int>("MYSTORE"); // 阻塞式调用，JS执行结束时返回。
});

// 如有异步转同步需求，可使用std::promise
std::threadt([] () {
    std::promise<std::string> promise;
    std::function<void (std::string)> callback = [&promise] (std::string message) {
        promise.set_value(message);// 回调在JS线程被执行
    }
    auto insert = aki::JSBind::GetJSFunction("MyStorage.insert");
    insert->Invoke<void>("MYSTORE", callback);
    std::string msg = promise.get_future().get(); // 子线程阻塞，直到promise.set_value
});

//如有异步非阻塞需求，可使用如下 ，无返回值
std::thread subThread([]() {
     if (auto func = aki::JSBind::GetJSFunction("helloAsync")) {
        std::function<void(std::string)> callback = [](std::string message) {
        AKI_DLOG(INFO) << "helloAsync callback will be invoked in JS Thread:"<<message;
        };
        func->InvokeAsync<void>("helloAsync hello world",callback);
        AKI_DLOG(INFO) << "helloAsync start.";
     }
});

//如有异步非阻塞需求，可使用如下 ，有返回值
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


```



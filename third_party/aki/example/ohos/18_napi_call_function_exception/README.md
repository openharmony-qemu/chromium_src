## Example 18: *napi_call_function*

环境依赖：
* IDE： DevEco Studio 3.1.0.500
* SDK：3.2.12.2

用例说明:

使用 `AKI` 封装 C++ 业务插件，JS 调用Native函数时，有时候需要传入Function类型参数作为回调,Function异常,需要在执行napi_call_function后捕捉到js异常：

```js
import napi_call_function_exception from "libnapi_call_function_exception.so"
napi_call_function_exception.CallException(() => {
  let cc: any;
  console.log(cc.length);
})
```

`AKI`触发回调：
```c++
void CallException(std::function<void()> func) {
    func();
}
```
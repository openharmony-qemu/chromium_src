# 目录内容

该目录为OHOS系统提供对底层web内核的一层封装。各子目录内容为：

- `include`：提供暴露给OHOS的接口和相关定义。OHOS侧可通过这些方法加载和使用web内核
- `src`：对web内核的封装实现，其中各子目录中的内容表示对不同web内核的封装，其它部分为一些通用逻辑的实现
  - `cef_delegate`: 对cef接口的封装

# 编译

使用nweb根目录下build.sh脚本即可使用默认参数进行编译，并输出到默认的`out/Default`路径下。也可以自定义编译：

```bash
gn args <output_path> # 自定义编译参数和输出路径
autoninja -C <output_path> libarkweb_engine libarkweb_renderer
```

nweb侧将编译得到：

  - `libarkweb_engine.so`，包含整个chromium内核和cef框架的实现
  - `libarkweb_renderer.so`，包含拉起render子进程的实现

上述三个文件均位于`<output_path>`路径下

此外，如果通过web_all_build.xml拉取代码，可使用nweb根目录上一级的build.sh脚本进行完整构建，其中会先将nweb侧的库编译出来，并自动将OHOS侧编译所需要的库拷贝到指定位置。

# 单板运行

参考OHOS侧README说明
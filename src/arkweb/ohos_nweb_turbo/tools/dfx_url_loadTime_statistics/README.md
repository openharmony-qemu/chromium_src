# dfx_url_loadTime_statistics

## 1. 简介
Oh web页面首屏加载相关性能能指标测试工具，能够自动运行传入的url，并读取其首屏加载的fcp、lcp、load等性能指标。
对于部分加载完后无轮播图、倒计时等频繁变动元素的网页，还可以计算出其首屏渲染完成的大概时间。
测试工具采用ArkTs开发

## 2. 目录
```c
/dfx_url_loadTime_statistics  #外侧app容器
├── entry
│   └── src
│       └── main
│           ├── ets    # ets代码目录
│               ├── pages
│                   └── index.ets # 应用首页，工具核心组件引用示例
│               └── urls
│                   └── urls.ets # 需要传入的测试页面url
│           └── module.json5 # 配置文件
├── dfx_url_loadTime_core_component  # dfx测试工具核心代码
│   └── src
│       └── main
│           ├── ets
│               ├── components
│                   └── mainpage
│                       └── MainPage.ets # dfx测试工具核心组件
│           └── module.json5 # 配置文件
├── package.json    # 依赖清单
```

## 3. 设计

将传入url依次传入oh web组件，并在页面初始化最开始（web组件onPageBegin事件触发时）通过注入js的方式将fcp、lcp、load、dom添加、img load等监听注入到页面当中，并通过onConsole事件对结果进行监听，最终计算反馈。
wiki：https://wiki.huawei.com/domains/827/wiki/8/WIKI20230131690688?title=_2

## 4. 使用

1. 环境准备：deveco环境
2. 使用deveco studio打开项目，等待项目依赖下载完毕（右下角有进度条）
3. 签名配置：选择 File -> Project Structure，Signing Configs -> Automatically generate signature
点击确定后自动完成签名配置
4. 点击顶部```Build->Make Module ‘WebviewTestTool’```
5. 将dfx_url_loadTime_statistics/dfx_url_loadTime_core_component/build/default/outputs/default/ohos-webviewtesttool-1.0.0.tgz拷贝到任意oh应用的根目录下
6. 将```"@ohos/webviewtesttool": "file:ohos-webviewtesttool-1.0.0.tgz"```添加到根目录```package.json```文件的```dependencies```对象下，若已添加则无需再次添加
7. 将需要测试的网页输入到dfx_url_loadTime_statistics/entry/src/main/ets/urls/urls.ets中，并将测试工具按照项目根目录下的README.md中的步骤运行起来
8. 运行
```npm install```
9. 引入
```typescript
import { WebTestTool } from '@ohos/webviewtesttool'

@Entry
@Component
struct Index {
  // 需要测试的页面的url
  @State urls: string[] = urls

  build() {
    Row() {
      WebTestTool({ urls: this.urls})
    }
    .height('100%')
  }
}
```
10. 设置网络权限：找到根目录下entry/src/main/module.json5，在module对象的最后添加如下代码：（若已添加则无需再次添加）
   ```
   "requestPermissions": [
      {
        "name": "ohos.permission.INTERNET"
      }
    ]
   ```
11. 链接oh设备运行应用即可开始测试
12. 点击“开始测试”按钮，工具将自动运行传入的所有页面，读取并计算出所有页面的FCP、LCP、Load的时间，并计算出其平均值。
13. 页面首屏加载时间可以参考FSP指标的值，对于大部分不存在频繁变动元素（如轮播图，倒计时等）的页面是比较准确的，对于部分存在频繁变动元素的页面可以参考LCP和Load指标中的较大值，比较接近但可能存在较大误差。
14. 结果保存与导出：测试完毕后点击“下载结果”按钮可将本次测试结果下载到系统目录，运行
```shell
hdc_std file recv /data/app/el2/100/base/com.example.myapplication/haps/entry/files/result.json
```
把json pull到执行的根目录

## 5. 注意事项
1. 受网络条件影响，部分情况下存在网页打开失败的情况，建议重试或切换网络重试。
2. 由于web页面灵活多变，测试工具提供的测试方案只能适用于部分指定页面而非全部页面。页面打开时会在页面右侧显示工具读取出的页面加载时间，可通过视觉比较页面加载结束和工具测试时间结束是否同步来初步判断当前页面是否适合通过本工具来测试。
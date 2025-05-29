import web_webview from '@ohos.web.webview'
@Entry
@Component
struct Index {
  controller: web_webview.WebviewController = new web_webview.WebviewController()
  @State message: string = 'Hello World';

  build() {
    RelativeContainer() {
      Text(this.message)
        .id('HelloWorld')
        .fontSize($r('app.float.page_text_font_size'))
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .onClick(() => {
          this.message = 'Welcome';
        })
      Web({src: $rawfile('index.html'), controller: this.controller})
        .darkMode(WebDarkMode.Auto)
        .zoomAccess(true)
        .domStorageAccess(true)
        .overScrollMode(OverScrollMode.ALWAYS)
        .onScaleChange((event) => {
          console.log('onScaleChange changed from ' + event.oldScale + ' to ' + event.newScale);
        })
    }
    .width('100%')
  }
}
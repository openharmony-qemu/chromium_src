# 单元测试说明

本目录包含针对 ohos_autolayout 项目的单元测试代码。

## 测试覆盖率

当前测试覆盖率:
- **语句覆盖率 (Statements)**: 82.82% ✅
- **分支覆盖率 (Branches)**: 71.55% ✅
- **函数覆盖率 (Functions)**: 67% 
- **行覆盖率 (Lines)**: 84.5% ✅

**总体覆盖率已超过 70% 的要求**

## 测试文件说明

### 核心模块测试

1. **Log.test.ts** - 日志模块测试
   - 测试所有日志级别 (DEBUG, INFO, WARN, ERROR)
   - 时间戳格式化
   - 元素信息记录
   - 日志级别过滤

2. **Tag.test.ts** - 标签常量测试
   - 验证所有标签常量的正确性
   - 标签唯一性检查

3. **Main.test.ts** - 主入口模块测试
   - start/stop/restart 方法
   - 配置加载
   - 初始化流程

### 工具类测试

4. **Utils.test.ts** - 工具函数测试
   - 屏幕尺寸检测
   - 元素处理
   - 区间计算
   - Map 比较

5. **Store.test.ts** - 存储管理测试
   - setValue/getValue/removeValue
   - WeakMap 行为
   - 标签清理

6. **Cached.test.ts** - 缓存机制测试
   - DOM 元素缓存
   - 缓存清理
   - 性能优化验证

### 配置与系统测试

7. **CCMConfig.test.ts** - 配置管理测试
   - JSON 解析
   - 单例模式
   - 断点配置
   - 白名单检查

8. **WaitSystemReady.test.ts** - 系统就绪检测测试
   - 文档加载检测
   - 回调机制
   - 超时重试

9. **Constant.test.ts** - 常量定义测试
   - 布局常量
   - 样式常量
   - 标签常量

## 运行测试

### 运行所有测试
```bash
npm test
```

### 运行测试并生成覆盖率报告
```bash
npm run test:coverage
```

### 监视模式运行测试
```bash
npm run test:watch
```

## 测试环境

- **测试框架**: Jest
- **TypeScript 支持**: ts-jest
- **DOM 环境**: jsdom
- **测试环境变量**: `__DEV__` = true

## 测试配置

测试配置位于 `jest.config.js`，包括:
- 测试环境设置
- 覆盖率阈值
- 模块路径映射
- 全局变量定义

## 注意事项

1. 所有测试都使用了适当的 mock 来隔离依赖
2. 每个测试文件都有独立的 beforeEach/afterEach 钩子来重置状态
3. 使用 jsdom 模拟浏览器环境
4. Console 方法已被 mock 以避免测试输出干扰

## 测试覆盖率报告

测试完成后，覆盖率报告将生成在 `coverage/` 目录下:
- `coverage/lcov-report/index.html` - HTML 格式的详细报告
- `coverage/lcov.info` - LCOV 格式(可用于 CI 集成)

## 持续改进

为进一步提高测试覆盖率,可以考虑:
1. 为复杂的业务逻辑模块添加更多测试
2. 增加边界条件和异常情况的测试
3. 提高对 Framework 和 Observer 模块的测试覆盖

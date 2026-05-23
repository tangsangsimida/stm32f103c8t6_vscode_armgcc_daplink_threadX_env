# 示例代码

本目录保存不参与固件构建的代码模板。需要使用时，先复制到 `user/` 对应目录，再按模块名重命名符号和文件。

## 文件

- `template_thread.c/h`：单线程模块模板，展示强类型参数、默认参数、自动注册和 sleep tick 防御。
- `queue_example.c/h`：两个平等线程通过 ThreadX 队列通信的示例。

## 使用方式

```bash
cp docs/examples/template_thread.c user/Application/src/your_thread.c
cp docs/examples/template_thread.h user/Application/inc/your_thread.h
```

复制后执行全局替换，例如 `template` -> `your_thread`，再运行：

```bash
cmake --preset Debug
cmake --build build/Debug
```

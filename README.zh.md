# Ascend Extension for TensorPipe

## 简介
此项目为原始开源仓TensorPipe基于华为Ascend Extension for PyTorch（即torch_npu）的适配。

## 目录结构

关键目录如下：

```ColdFusion
├─ci                             # 持续集成脚本目录
├─cmake                          # CMake构建配置目录
├─torch_npu                      # 核心适配目录
│  ├─channel/                    # 通道抽象层目录
│  ├─core/                       # 核心逻辑目录
│  ├─test/                       # 测试目录
│  ├─transport/                  # 传输层实现目录
├─third_party/                   # 外部依赖目录
```

## 版本说明

Ascend TensorPipe仅存在master版本，配套pytorch的所有版本，Ascend Extension for PyTorch版本信息请参考[Ascend Extension for PyTorch主仓声明](https://gitcode.com/ascend/pytorch/blob/master/README.zh.md)。

## 环境部署
### 安装
在linux shell中指定位置运行：
```
$ git clone --recursive https://gitcode.com/ascend/Tensorpipe.git
$ cd Tensorpipe
$ bash ci/build.sh
```
即可开始编译Ascend TensorPipe。

### 部署
根据功能需求，在编译之前选择性启动./cmake/Options.cmake中`Optional features`的三个编译选项。
```
# Optional features
option(TP_BUILD_MISC "Build misc tools" OFF)
option(TP_BUILD_PYTHON "Build python bindings" OFF)
option(TP_BUILD_TESTING "Build tests" OFF)
```
### 验证
若需要测试TensorPipe内部自带的Unit Test，请将`TP_BUILD_TESTING`设置为ON，并在cmake时设置如下三个编译选项：
- **-DTP_ENABLE_SHM=xx**
- **-DTP_ENABLE_IBV=xx**
- **-DTP_ENABLE_CMA=xx**

编译完成后的build子目录中运行：**./tensorpipe/test/tensorpipe_test**。

## API参考
TensorPipe对外暴露的详细接口信息请参考[API文档](docs/zh/API.md)。

## FAQ
- 倘若要将`TP_BUILD_PYTHON`设置为ON，在cmake时需要额外添加：**-DBUILD_SHARED_LIBS=ON**编译选项，否则生成的.a文件无法被pybind11获取。

- 倘若要将`TP_BUILD_MISC`设置为ON，需要在Linux系统中下载**llvm-12**、**clang-12**和**libclang-12-dev**。三个包的版本必须相同（12），且不能使用14及以后的版本。

- 在Windows Subsystem Linux中运行TensorPipe时无法使用Transport/SHM相关功能，需要在编译时手动关闭（**-DTP_ENABLE_SHM=OFF**）。原因是SHM依赖syscall来申请内存空间作为传输介质，而WSL不支持syscall。

- 若要使用Transport/IBV相关功能（**-DTP_ENABLE_IBV=ON**），请确保环境中有相应的.so库文件和支持RDMA的硬件设备。

    .so库文件可以通过如下指令来下载：
    ```
    $ sudo apt-get install libibverbs.so
    ```
    RDMA设备可通过如下指令来确认：
    ```
    $ rdma dev show
    ```

## 版本维护策略
**维护模式** - 本项目已进入基本维护阶段，不再进行新功能开发。仅进行必要的缺陷修复和安全更新。不再接受新功能提案（RFC）、特性请求或代码贡献。

## 联系我们
如果有任何疑问或建议，请提交[GitCode Issues](https://gitcode.com/Ascend/Tensorpipe/issues)，我们会尽快回复。感谢您的支持。

## 免责声明
致Ascend Extension for TensorPipe插件使用者
- 本插件仅供调试和开发使用，使用者需自行承担使用风险，并理解以下内容：
    - 数据处理及删除：用户在使用本插件过程中产生的数据属于用户责任范畴。建议用户在使用完毕后及时删除相关数据，以防信息泄露。
    - 数据保密与传播：使用者了解并同意不得将通过本插件产生的数据随意外发或传播。对于由此产生的信息泄露、数据泄露或其他不良后果，本插件及其开发者概不负责。
    - 用户输入安全性：用户需自行保证输入的命令行的安全性，并承担因输入不当而导致的任何安全风险或损失。对于输入命令行不当所导致的问题，本插件及其开发者概不负责。
- 免责声明范围：本免责声明适用于所有使用本插件的个人或实体。使用本插件即表示您同意并接受本声明的内容，并愿意承担因使用该功能而产生的风险和责任，如有异议请停止使用本插件。
- 在使用本工具之前，请谨慎阅读并理解以上免责声明的内容。对于使用本插件所产生的任何问题或疑问，请及时联系开发者。

## License
Ascend Extension for TensorPipe仓库的使用许可证，具体请参见[LICENSE](LICENSE)文件。

## 致谢

感谢来自社区的每一个PR，欢迎贡献Ascend Extension for TensorPipe插件！


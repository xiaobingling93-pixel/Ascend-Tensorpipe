# Ascend Extension for TensorPipe

#### 介绍
此项目为原始开源仓Tensorpipe基于华为Ascend pytorch/torch_npu的适配。

#### 安装教程
在linux shell中指定位置运行：
```
$ git clone --recursive https://gitcode.com/ascend/Tensorpipe.git
$ cd Tensorpipe
$ bash ci/build.sh
```
即可开始编译

#### 使用说明
根据功能需求，在编译之前选择性启动./build/Options.cmake中Option features下的四个编译选项
```
# Optional features
option(TP_BUILD_MISC "Build misc tools" OFF)
option(TP_BUILD_PYTHON "Build python bindings" OFF)
option(TP_BUILD_TESTING "Build tests" OFF)
```

#### API接口
Tensorpipe对外暴露的详细接口信息请参考[API文档](https://gitcode.com/ascend/Tensorpipe/blob/master/API.md)

#### 测试方法
若需要测试Tensorpipe内部自带的Unit Test，请将TP_BUILD_TESTING设置为ON，并在cmake时设置: **-DTP_ENABLE_SHM=xx -DTP_ENABLE_IBV=xx -DTP_ENABLE_CMA=xx**这个三编译选项。编译完成后的build子目录中运行：**./tensorpipe/test/tensorpipe_test**

#### 常见问题
倘若要将TP_BUILD_PYTHON设置为ON，在cmake时需要额外添加：**-DBUILD_SHARED_LIBS=ON**编译选项，不然生成的.a文件无法被pybind11获取。

倘若要将TP_BUILD_MISC设置为ON，需要在Linux系统中下载**llvm-12**，**clang-12**和**libclang-12-dev**。三个包的版本必须相同（12），且不能使用14及以后的版本。

在Windows Subsystem Linux中运行Tensorpipe时无法使用Transport/SHM相关功能，需要在编译时手动关闭（**-DTP_ENABLE_SHM=OFF**）。原因是SHM依赖syscall来申请内存空间作为传输介质，而WSL不支持syscall。

若要使用Transport/IBV相关功能（**-DTP_ENABLE_IBV=ON**），请确保环境中有相应的.so库文件和支持RDMA的硬件设备。

.so库文件可以通过如下指令来下载：
```
$ sudo apt-get install libibverbs.so
```
RDMA设备可通过如下指令来确认：
```
$ rdma dev show
```

# 版本配套表
[Ascend Tensorpipe版本配套信息请参考Ascend Extension for PyTorch主仓声明](https://gitcode.com/ascend/pytorch/blob/master/README.zh.md)

# 安全声明
[Ascend Tensorpipe 代码仓库安全声明](SECURITYNOTE.md)

# Ascend Extension for TensorPipe仓库安全声明

## 系统安全加固

建议用户在系统中配置开启ASLR（级别2 ），又称**全随机地址空间布局随机化**，可参考以下方式进行配置：

    echo 2 > /proc/sys/kernel/randomize_va_space

## 运行用户建议

Ascend TensorPipe仓库代码不单独构建whl包，作为[Ascend Extension for PyTorch](https://gitcode.com/ascend/pytorch)的三方包使用，可以参考README.md中的提示，运行相应的UT。

出于安全性及权限最小化角度考虑，不建议使用root等管理员类型账户使用。

## 文件权限控制

1. 建议用户对代码执行所需文件、执行过程中保存的文件、用户个人的隐私数据、商业资产等敏感文件做好权限控制等安全措施，例如多用户共享数据集场景下的数据集文件写权限控制、LOG日志等场景产生数据文件权限控制等，设定的权限建议参见[文件权限参考](#文件权限参考)进行设置。

2. Ascend Tensorpipe库中代码执行会生成功能性日志记录文件，生成的文件权限为640，文件夹权限为750，用户可根据需要自行对生成后的相关文件进行权限控制。

3. 用户安装和使用过程需要做好权限控制，建议参见[文件权限参考](#文件权限参考)进行设置。如需要保存安装/卸载日志，可在安装/卸载命令后面加上参数--log \<FILE>， 注意对\<FILE>文件及目录做好权限管控。

4. PyTorch框架和Ascend Extension for PyTorch插件调用Ascend TensorPipe仓库代码运行中所生成的文件权限依赖系统设定，如torch.save接口保存的文件。建议当前执行脚本的用户根据自身需要，对生成文件做好权限控制，设定的权限可参见[文件权限参考](#文件权限参考) 。可使用umask控制默认权限，降低提权等安全风险，建议用户将主机（包括宿主机）和容器中的umask设置为0027及其以上，提高安全性。

### 文件权限参考

|   类型                             |   Linux权限参考最大值   |
|----------------------------------- |----------------------- |
|  用户主目录                         |   750（rwxr-x---）     |
|  程序文件(含脚本文件、库文件等)       |   550（r-xr-x---）     |
|  程序文件目录                       |   550（r-xr-x---）     |
|  配置文件                           |   640（rw-r-----）     |
|  配置文件目录                       |   750（rwxr-x---）     |
|  日志文件(记录完毕或者已经归档)       |   440（r--r-----）     |
|  日志文件(正在记录)                  |   640（rw-r-----）    |
|  日志文件目录                       |   750（rwxr-x---）     |
|  Debug文件                         |   640（rw-r-----）      |
|  Debug文件目录                      |   750（rwxr-x---）     |
|  临时文件目录                       |   750（rwxr-x---）     |
|  维护升级文件目录                   |   770（rwxrwx---）      |
|  业务数据文件                       |   640（rw-r-----）      |
|  业务数据文件目录                   |   750（rwxr-x---）      |
|  密钥组件、私钥、证书、密文文件目录   |   700（rwx------）      |
|  密钥组件、私钥、证书、加密密文       |   600（rw-------）     |
|  加解密接口、加解密脚本              |   500（r-x------）      |

## 构建安全声明

Ascend TensorPipe仓库作为torch_npu的三方依赖库支持源码编译安装，在编译时会下载依赖第三方库并执行构建shell脚本，在编译过程中会产生临时程序文件和编译目录。用户可根据需要自行对源代码目录内的文件进行权限管控降低安全风险。

## 运行安全

1. 在非测试场景中Ascend TensorPipe不会单独使用，用户的业务逻辑在Ascend PyTorch的RPC模块运行，根据环境资源状况编写对应训练脚本。在上层训练脚本与资源状况不匹配的场景中，如数据集加载内存大小超出内存容量限制、通信server无法链接、训练脚本在本地生成数据超过磁盘空间大小等情况，可能引发错误并导致进程意外退出。建议用户根据PyTorch报错信息定位错误原因，tensorpipe抛出的异常会被Ascend PyTorch捕获并解析，问题通常分为通信问题和NPU设备内存使用或搬运问题。
2. 如果要使用PyTorch的RPC模块，在完成安装后，需要确认安装目录下的libibverbs.so.1和libascendcl.so两个文件权限不大于550，防止so包被篡改产生命令注入的安全风险。
3. 如果使用了PyTorch的RPC模块，在进程调试与性能分析等存在与其他进程数据交互的场景中，由于该程序存在如下敏感信息泄露及数据完整性受损的风险，用户应谨慎评估与管理： <br>
（1）在需要与远程进程交互的应用场景，如调试工具、profiler性能工具等，因为业务需要，会使用SYS_process_vm_readv进行跨进程读取内存数据操作，为了保障应用安全，需确保在安全、隔离的环境下进行操作，并注意所有的风险数据，防止因读取内存数据而导致核心业务数据泄露。 <br>
（2）在使用调试工具进行调试时，具有root权限、CAP_SYS_PTRACE权限的调试工具往往有附加和追踪其他进程的能力，为了保障应用安全，需要在调试时，确保环境内不存在非法的高权限进程，防止未经授权的进程对目标进程进行内存窥探或控制，造成数据泄露或系统被恶意篡改。

## 公网地址

| 类型 | 开源代码地址 | 文件名 | 公网IP地址/公网URL地址/域名/邮箱地址 | 用途说明 |
| ---- | ------------ | ------ | ------------------------------------ | -------- |
| 开发引入 | 不涉及 | tensorpipe/test/transport/uv/sockaddr_test.cc | 1.2.3.4:65536 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/uv/sockaddr_test.cc | 1.2.3.4:5 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/uv/sockaddr_test.cc | 1.2.3.4:0 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/uv/sockaddr_test.cc | 1.2.3.4 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/uv/sockaddr_test.cc | 1.2.3.4:-1 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/ibv/sockaddr_test.cc | 1.2.3.4:-1 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/ibv/sockaddr_test.cc | 1.2.3.4:65536 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/ibv/sockaddr_test.cc | 1.2.3.4:5 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/ibv/sockaddr_test.cc | 1.2.3.4:0 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | tensorpipe/test/transport/ibv/sockaddr_test.cc | 1.2.3.4 | 测试用例使用的虚拟IP |
| 开发引入 | 不涉及 | .gitmodules | <https://gitee.com/mirrors/pybind11.git> | 依赖的开源代码仓 |
| 开发引入 | 不涉及 | .gitmodules | <https://gitee.com/mirrors/libuv.git> | 依赖的开源代码仓 |
| 开发引入 | 不涉及 | .gitmodules | <https://gitee.com/mirrors/googletest.git> | 依赖的开源代码仓 |
| 开发引入 | 不涉及 | .gitmodules | <https://gitee.com/mirrors/libnop.git> | 依赖的开源代码仓 |
| 开发引入 | 不涉及 | cmake/FindPackageHandleStandardArgs.cmake | <https://cmake.org/licensing> | 保留原始版权声明和链接，确保许可证信息完整 |
| 开发引入 | 不涉及 | cmake/FindPackageMessage.cmake | <https://cmake.org/licensing> | 保留原始版权声明和链接，确保许可证信息完整 |
| 开发引入 | 不涉及 | .gitmoudles | <https://gitee.com/openeuler/libboundscheck.git> | 依赖的开源代码仓 |

## 公开接口

Ascend TensorPipe为专门为Ascend PyTorch框架RPC模块提供点对点通信能力的底层能力代码仓库，不存在对用户界面的暴露接口。

用户无法在Python侧调用到Ascend TensorPipe的接口，在编译过程中的动态库不直接提供服务，暴露的C/C++接口为内部使用，不建议用户使用。

## 通信安全加固

Ascend TensorPipe需要在设备间进行通信，其通信控制由PyTorch和Ascend Extension for PyTorch控制，通信开启的端口默认为全0侦听，为了降低安全风险，建议用户针对此场景进行安全加固，如使用iptables配置防火墙，在运行分布式训练开始前限制外部对分布式训练使用端口的访问，在运行分布式训练结束后清理防火墙规则，具体内容参考[Ascend Extension for PyTorch通信安全加固](https://gitcode.com/Ascend/pytorch/blob/master/SECURITYNOTE.md#%E9%80%9A%E4%BF%A1%E5%AE%89%E5%85%A8%E5%8A%A0%E5%9B%BA)。

## 通信矩阵

Ascend TensorPipe需要进行网络通信。使用TCP协议进行通信，通信端口见[Ascend Extension for PyTorch通信矩阵信息](https://gitcode.com/Ascend/pytorch/blob/master/SECURITYNOTE.md#%E9%80%9A%E4%BF%A1%E7%9F%A9%E9%98%B5)。

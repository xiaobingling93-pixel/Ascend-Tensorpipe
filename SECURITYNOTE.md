# Ascend Tensorpipe仓库安全声明

## 漏洞风险提示

当前Ascend Tensorpipe仓库代码并未检测出明显的漏洞。

## 系统安全加固

建议用户在系统中配置开启ASLR（级别2 ），又称**全随机地址空间布局随机化**，可参考以下方式进行配置：

    echo 2 > /proc/sys/kernel/randomize_va_space

## 运行用户建议

Ascend Tensorpipe仓库代码不对外单独出whl包，可以参考README.md中的提示，运行相应的UT。

出于安全性及权限最小化角度考虑，不建议使用root等管理员类型账户使用。

## 文件权限控制

1. 建议用户对代码执行所需文件、执行过程中保存的文件、用户个人的隐私数据、商业资产等敏感文件做好权限控制等安全措施，例如多用户共享数据集场景下的数据集文件写权限控制、LOG日志等场景产生数据文件权限控制等，设定的权限建议参考[文件权限参考](#文件权限参考)进行设置。

2. Ascend Tensorpipe库中代码执行会生成功能性日志记录文件，生成的文件权限为640，文件夹权限为750，用户可根据需要自行对生成后的相关文件进行权限控制。

3. 用户安装和使用过程需要做好权限控制，建议参考[文件权限参考](#文件权限参考)进行设置。如需要保存安装/卸载日志，可在安装/卸载命令后面加上参数--log <FILE>， 注意对<FILE>文件及目录做好权限管控。

4. PyTorch和Ascend PyTorch框架调用Ascend Tensorpipe仓库代码运行中所生成的文件权限依赖系统设定，如torch.save接口保存的文件。建议当前执行脚本的用户根据自身需要，对生成文件做好权限控制，设定的权限可参考[文件权限参考](#文件权限参考) 。可使用umask控制默认权限，降低提权等安全风险，建议用户将主机（包括宿主机）和容器中的umask设置为0027及其以上，提高安全性。

##### 文件权限参考

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

## 调试工具声明

Ascend PyTorch内集成性能分析工具profiler会采集到Ascend Tensorpipe库代码的部分行为：
 - 集成原因：对标pytorch原生支持能力，提供NPU PyTorch框架开发性能分析能力，加速性能分析调试过程。
 - 使用场景：默认不采集，如用户需要进行性能分析时，可在模型训练脚本中添加Ascend PyTorch Profiler接口，执行训练的同时采集性能数据，完成训练后直接输出可视化的性能数据文件。
 - 风险提示：使用该功能会在本地生成性能数据，用户需加强对相关性能数据的保护，请在需要模型性能分析时使用，分析完成后及时关闭。Profiler工具具体细节请参考[《PyTorch 模型迁移和训练指南》](https://www.hiascend.com/document/detail/zh/canncommercial/700/modeldevpt/ptmigr/AImpug_0001.html)。

## 数据安全声明

1. Ascend Tensorpipe依赖网络协议和CANN的基础能力实现数据跨server或跨进程通信，与日志记录等功能，用户需要关注上述功能生成文件的权限控制，加强对相关数据的保护。

## 构建安全声明

Ascend Tensorpipe仓库作为torch_npu的三方依赖库支持源码编译安装，在编译时会下载依赖第三方库并执行构建shell脚本，在编译过程中会产生临时程序文件和编译目录。用户可根据需要自行对源代码目录内的文件进行权限管控降低安全风险。

## 运行安全声明

1. 在非测试场景中Ascend Tensorpipe不会单独使用，会根据用户的业务逻辑在Ascend PyTorch的RPC模块运行，运行环境资源状况编写对应训练脚本。若训练脚本与资源状况不匹配，如数据集加载内存大小超出内存容量限制、训练脚本在本地生成数据超过磁盘空间大小等情况，可能引发错误并导致进程意外退出。
2. Ascend Tensorpipe和Ascend PyTorch在运行异常时会退出进程并打印报错信息，属于正常现象。建议用户根据报错提示定位具体错误原因，包括设定算子同步执行、查看CANN日志、解析生成的Core Dump文件等方式。

## 公网地址声明

在Ascend Tensor的配置文件和脚本中存在[公网地址](https://gitee.com/ascend/Tensorpipe/blob/master/public_address_statement.md)

## 公开接口声明

Ascend Tensorpipe为专门为Ascend PyTorch框架RPC模块提供点对点通信能力的底层能力代码仓库，不存在对用户界面的暴露接口。

用户无法在python侧调用到Ascend Tensorpipe的接口，在编译过程中的动态库不直接提供服务，暴露的C/C++接口为内部使用，不建议用户使用。

Ascend PyTorch是PyTorch适配插件，支持用户使用PyTorch在昇腾设备上进行训练和推理。Ascend PyTorch适配后支持用户使用PyTorch原生接口。除了原生PyTorch接口外，Ascend PyTorch提供了部分自定义接口，包括自定义算子、亲和库和其他接口，支持PyTorch接口和自定义接口连接，具体可参考[《PyTorch 模型迁移和训练指南》](https://www.hiascend.com/document/detail/zh/canncommercial/700/modeldevpt/ptmigr/AImpug_000002.html)>API列表。

参考[PyTorch社区公开接口规范](https://github.com/pytorch/pytorch/wiki/Public-API-definition-and-documentation)，Ascend PyTorch提供了对外的自定义接口。如果一个函数看起来符合公开接口的标准且在文档中有展示，则该接口是公开接口。否则，使用该功能前可以在社区询问该功能是否确实是公开的或意外暴露的接口，因为这些未暴露接口将来可能会被修改或者删除。

## 通信安全加固

Ascend Tensorpipe需要在设备间进行通信，其通信控制由PyTorch和Ascend PyTorch控制，通信开启的端口默认为全0侦听，为了降低安全风险，建议用户针对此场景进行安全加固，如使用iptables配置防火墙，在运行分布式训练开始前限制外部对分布式训练使用端口的访问，在运行分布式训练结束后清理防火墙规则。

1. 防火墙规则设定和移除参考脚本模板
    - 防火墙规则设定，可参考如下脚本：
    

```
    #!/bin/bash
    set -x

    # 要限制的端口号
    port={端口号}

    # 清除旧规则
    iptables -D INPUT -p tcp -j {规则名}
    iptables -F {规则名}
    iptables -X {规则名}

    # 创建新的规则链
    iptables -t filter -N {规则名}

    # 在多机场景下设定白名单，允许其他节点访问主节点的侦听端口
    # 在规则链中添加允许特定IP地址范围的规则
    iptables -t filter -A {规则名} -i eno1 -p tcp --dport $port -s {允许外部访问的IP} -j ACCEPT

    # 屏蔽外部地址访问分布式训练端口
    # 在PORT-LIMIT-RULE规则链中添加拒绝其他IP地址的规则
    iptables -t filter -A {规则名} -i {要限制的网卡名} -p tcp --dport $port -j DROP

    # 将流量传递给规则链
    iptables -I INPUT -p tcp -j {规则名}
    ```

    - 防火墙规则移除，可参考如下脚本：
    

```
    #!/bin/bash
    set -x
    # 清除规则
    iptables -D INPUT -p tcp -j {规则名}
    iptables -F {规则名}
    iptables -X {规则名}
    ```

2. 防火墙规则设定和移除参考脚本示例
    1. 针对特定端口设定防火墙，脚本中端口号为要限制的端口，在PyTorch分布式训练中端口号请参考[通信矩阵信息](#通信矩阵信息)；要限制的网卡名为服务器用于分布式通信使用的网卡，允许的外部访问的IP为分布式训练服务器的IP地址。网卡和服务器IP可以通过ifconfig查看，如下文回显的eth0为网卡名，192.168.1.1为服务器IP地址：
        

```
        # ifconfig
        eth0
            inet addr:192.168.1.1 Bcast:192.168.1.255 Mask:255.255.255.0
            inet6 addr: fe80::230:64ee:ef1a:c1a/64 Scope:Link
        ```

    2. 假定服务器主节点地址192.168.1.1，另一台需要进行分布式训练的服务器为192.168.1.2，训练端口为29510。
        - 防火墙规则设定，可参考如下脚本：
        

```
        #!/bin/bash
        set -x

        # 设定侦听的端口
        port=29510

        # 清除旧规则
        iptables -D INPUT -p tcp -j PORT-LIMIT-RULE
        iptables -F PORT-LIMIT-RULE
        iptables -X PORT-LIMIT-RULE

        # 创建新的PORT-LIMIT-RULE规则链
        iptables -t filter -N PORT-LIMIT-RULE

        # 在多机场景下设定白名单，允许192.168.1.2访问主节点
        # 在PORT-LIMIT-RULE规则链中添加允许特定IP地址范围的规则
        iptables -t filter -A PORT-LIMIT-RULE -i eno1 -p tcp --dport $port -s 192.168.1.2 -j ACCEPT

        # 屏蔽外部地址访问分布式训练端口
        # 在PORT-LIMIT-RULE规则链中添加拒绝其他IP地址的规则
        iptables -t filter -A PORT-LIMIT-RULE -i eth0 -p tcp --dport $port -j DROP

        # 将流量传递给PORT-LIMIT-RULE规则链
        iptables -I INPUT -p tcp -j PORT-LIMIT-RULE
        ```

        - 防火墙规则移除，可参考如下脚本：
        

```
        #!/bin/bash
        set -x
        # 清除规则
        iptables -D INPUT -p tcp -j PORT-LIMIT-RULE
        iptables -F PORT-LIMIT-RULE
        iptables -X PORT-LIMIT-RULE
        ```

## 通信矩阵

Ascned Tensorpipe需要进行网络通信。需要使用TCP进行通信，通信端口见[通信矩阵信息](#通信矩阵信息)。用户需要注意并保障节点间通信网络安全，可以使用iptables等方式消减安全风险，可参考[通信安全加固](#通信安全加固)进行网络安全加固。

##### 通信矩阵信息

| 组件                  |  PyTorch                             |
|---------------------- |------------------------------------  |
| 源设备                | 运行torch_npu进程的服务器              |
| 源IP                  | 设备地址IP                            |
| 源端口                | 操作系统自动分配，分配范围由操作系统的自身配置决定      |
| 目的设备              | 运行torch_npu进程的服务器              |
| 目的IP                | 设备地址IP                            |
| 目的端口 （侦听）      | 默认29500/29400，用户可以设定端口号     |
| 协议                  | TCP                                  |
| 端口说明              | 1. 在静态分布式场景中（用torchrun/torch.distributed.launch使用的backend为static）， 目的端口（默认29500）用于接收和发送数据，源端口用于接收和发送数据 <br> 2. 在动态分布式场景中（用torchrun或者torch.distributed.launch使用的backend为c10d）， 目的端口（默认29400）用于接收和发送数据，源端口用于接收和发送数据。可以使用rdzv_endpoint和master_port指定端口号 <br> 3. 在分布式场景中，用torchrun或者torch.distributed.launch不指定任何参数时使用的端口号为29500       |
| 侦听端口是否可更改     | 是                                    |
| 认证方式              | 无认证方式                             |
| 加密方式              | 无                                     |
| 所属平面              | 不涉及                                 |
| 版本                  | 所有版本                               |
| 特殊场景              | 无                                     |
| 备注                  | 该通信过程由开源软件PyTorch控制，配置为PyTorch原生设置，可参考[PyTorch文档](https://pytorch.org/docs/stable/distributed.html#launch-utility)。源端口由操作系统自动分配，分配范围由操作系统的配置决定，例如ubuntu：采用/proc/sys/net/ipv4/ipv4_local_port_range文件指定，可通过cat /proc/sys/net/ipv4/ipv4_local_port_range或sysctl net.ipv4.ip_local_port_range查看       |

# Ascend NPU Tensorpipe API

## 介绍
开源仓Tensorpipe基于华为Ascend pytorch/torch_npu的点对点通信支撑，接口全部用于torch_npu中rpc模块功能的实现。

## 接口定义
接口主要从定义为外部使用的类对象和单独方法两个方面进行介绍


| class | 类描述 | 基本成员方法 | 方法描述 |
| :----: | :----: | :---- | :----: |
| ContextOptions | 通信上下文配置类 | `ContextOptions&& name(std::string name)` | 配置通信上下文名称 |
| PipeOptions | 管道配置类 | `PipeOptions&& remoteName(std::string remoteName)` | 配置管道对端名称 |
| Context | 通信上下文资源类 | ` void registerTransport(int64_t priority,std::string transport,std::shared_ptr<transport::Context> context)` | 注册通信需要的端口链接信息管理 |
| ~ | 通信上下文资源类 | `void registerChannel(int64_t priority,std::string channel,std::shared_ptr<channel::Context> context)` | 注册通信需要的信道链接信息管理 |
| ~ | 通信上下文资源类 | `std::shared_ptr<Listener> listen(const std::vector<std::string>& urls)` | 创建监听实例 |
| ~ | 通信上下文资源类 | `void close()` | 关闭通信需要的管道，并释放全部资源 |
| ~ | 通信上下文资源类 | `void join()` | 等待所有的资源申请和执行完毕，在当前函数同步阻塞 |
| Listener | 通信状态监听类 | `void accept(accept_callback_fn fn)` | 接受链接请求 |
| ~ | 通信状态监听类 | `const std::map<std::string, std::string>& addresses()` | 映射url地址和端口号 |
| ~ | 通信状态监听类 | `const std::string& address(const std::string& transport)` | 重载函数，映射url地址和端口号 |
| ~ | 通信状态监听类 | `std::string url(const std::string& transport) const` | 获取url地址和端口号 |
| ~ | 通信状态监听类 | `void close()` | 关闭端口监听 |
| Message | 通信信息表达类 | `std::string metadata; std::vector<Payload> payloads; std::vector<Tensor> tensors;` | 表述数据信息实例 |
| Pipe | 通信管道描述类 | `Pipe(ConstructorToken token,std::shared_ptr<ContextImpl> context,std::string id,std::string remoteName,const std::string& url)` | 构造函数，创建管道实例 |
| ~ | 通信管道描述类 | `void read(Allocation allocation, read_callback_fn fn)` | 申请内存，并读取管道内容 |
| ~ | 通信管道描述类 | `void write(Message message, write_callback_fn fn)` | 写管道内容 |
| ~ | 通信管道描述类 | `const std::string& getRemoteName()` | 获取管道对端名称 |
| ~ | 通信管道描述类 | `void close()` | 关闭管道 |
| Buffer | 通信缓存管理类 | `Buffer(TBuffer b) ` | 构造函数，创建buffer块 |
| ~ | 通信缓存管理类 | `Buffer(const Buffer& other)` | 拷贝构造函数，创建buffer块 |
| ~ | 通信缓存管理类 | `Buffer& operator=(const Buffer& other)` | 等于符号重载，左值 |
| ~ | 通信缓存管理类 | `Buffer& operator=(Buffer&& other)` | 等于符号重载，右值 |
| ~ | 通信缓存管理类 | `Device device() const` | 获取buffer块所处的device信息 |
| NPUBuffer | 通信缓存管理类 | `Device device() const` | 获取buffer块所处的device信息 |


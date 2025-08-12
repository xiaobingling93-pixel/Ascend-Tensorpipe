/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <future>
#include <memory>
#include <tuple>
#include <vector>
#include <securec.h>

#include <gtest/gtest.h>

#include <tensorpipe/tensorpipe.h>
#include <tensorpipe/test/peer_group.h>

#include <tensorpipe/tensorpipe_npu.h>
#include <tensorpipe/common/npu.h>


struct Storage {
  std::vector<std::shared_ptr<void>> payloads;
  std::vector<std::pair<std::shared_ptr<void>, tensorpipe_npu::Buffer>> tensors;
};

struct InlineMessage {
  struct Payload {
    std::string data;
    std::string metadata;
  };

  struct Tensor {
    std::string data;
    std::string metadata;
    tensorpipe_npu::Device device;
    tensorpipe_npu::optional<tensorpipe_npu::Device> targetDevice;
  };

  std::vector<Payload> payloads;
  std::vector<Tensor> tensors;
  std::string metadata;
};

inline std::pair<tensorpipe_npu::Message, Storage> makeMessage(
    InlineMessage imessage) {
  tensorpipe_npu::Message message;
  Storage storage;

  for (auto& payload : imessage.payloads) {
    size_t length = payload.data.length();
    auto data = std::unique_ptr<uint8_t, std::default_delete<uint8_t[]>>(
        new uint8_t[length]);
    const auto ret = memcpy_s(data.get(), length, &payload.data[0], length);
    TP_THROW_ASSERT_IF(ret != EOK) << " pipe_test.h makeMessage#1 memcpy_s is failed!";
    message.payloads.push_back({
        .data = data.get(),
        .length = length,
        .metadata = payload.metadata,
    });
    storage.payloads.push_back(std::move(data));
  }

  for (auto& tensor : imessage.tensors) {
    size_t length = tensor.data.length();
    tensorpipe_npu::Buffer buffer;
    std::shared_ptr<void> data;
    if (tensor.device.type == tensorpipe_npu::kCpuDeviceType) {
        data = std::unique_ptr<uint8_t, std::default_delete<uint8_t[]>>(
            new uint8_t[length]);
        const auto ret = memcpy_s(data.get(), length, &tensor.data[0], length);
        TP_THROW_ASSERT_IF(ret != EOK) << " pipe_test.h makeMessage#2 memcpy_s is failed!";
        buffer = tensorpipe_npu::CpuBuffer{.ptr = data.get()};
    } else {
      ADD_FAILURE() << "Unexpected source device: " << tensor.device.toString();
    }

    message.tensors.push_back({
        .buffer = buffer,
        .length = length,
        .targetDevice = tensor.targetDevice,
        .metadata = tensor.metadata,
    });
    storage.tensors.push_back({std::move(data), std::move(buffer)});
  }

  message.metadata = imessage.metadata;

  return {std::move(message), std::move(storage)};
}

inline std::pair<tensorpipe_npu::Allocation, Storage> makeAllocation(
    const tensorpipe_npu::Descriptor& descriptor,
    const std::vector<tensorpipe_npu::Device>& devices) {
  tensorpipe_npu::Allocation allocation;
  Storage storage;
  for (const auto& payload : descriptor.payloads) {
    auto data = std::unique_ptr<uint8_t, std::default_delete<uint8_t[]>>(
        new uint8_t[payload.length]);
    allocation.payloads.push_back({.data = data.get()});
    storage.payloads.push_back(std::move(data));
  }

  TP_DCHECK(devices.size() == descriptor.tensors.size());
  for (size_t tensorIdx = 0; tensorIdx < descriptor.tensors.size();
       ++tensorIdx) {
    const auto& tensor = descriptor.tensors[tensorIdx];
    tensorpipe_npu::Device targetDevice = devices[tensorIdx];

    if (tensor.targetDevice.has_value()) {
      TP_DCHECK(targetDevice == *tensor.targetDevice);
    }

    if (targetDevice.type == tensorpipe_npu::kCpuDeviceType) {
      auto data = std::unique_ptr<uint8_t, std::default_delete<uint8_t[]>>(
          new uint8_t[tensor.length]);
      tensorpipe_npu::Buffer buffer = tensorpipe_npu::CpuBuffer{.ptr = data.get()};
      allocation.tensors.push_back({.buffer = buffer});
      storage.tensors.push_back({std::move(data), std::move(buffer)});
    } else {
      ADD_FAILURE() << "Unexpected target device: " << targetDevice.toString();
    }
  }

  return {std::move(allocation), std::move(storage)};
}

inline std::future<void> pipeWriteWithFuture(
    tensorpipe_npu::Pipe& pipe,
    tensorpipe_npu::Message message) {
  auto promise = std::make_shared<std::promise<void>>();
  auto future = promise->get_future();

  pipe.write(
      std::move(message),
      [promise{std::move(promise)}](const tensorpipe_npu::Error& error) {
        if (error) {
          promise->set_exception(
              std::make_exception_ptr(std::runtime_error(error.what())));
          return;
        }

        promise->set_value();
      });

  return future;
}

inline std::future<std::tuple<tensorpipe_npu::Descriptor, Storage>>
pipeReadWithFuture(
    tensorpipe_npu::Pipe& pipe,
    std::vector<tensorpipe_npu::Device> targetDevices) {
  auto promise = std::make_shared<
      std::promise<std::tuple<tensorpipe_npu::Descriptor, Storage>>>();
  auto future = promise->get_future();
  pipe.readDescriptor([&pipe,
                       promise{std::move(promise)},
                       targetDevices{std::move(targetDevices)}](
                          const tensorpipe_npu::Error& error,
                          tensorpipe_npu::Descriptor descriptor) mutable {
    if (error) {
      promise->set_exception(
          std::make_exception_ptr(std::runtime_error(error.what())));
      return;
    }

    tensorpipe_npu::Allocation allocation;
    Storage storage;
    std::tie(allocation, storage) = makeAllocation(descriptor, targetDevices);
    pipe.read(
        std::move(allocation),
        [promise{std::move(promise)},
         descriptor{std::move(descriptor)},
         storage{std::move(storage)}](const tensorpipe_npu::Error& error) mutable {
          if (error) {
            promise->set_exception(
                std::make_exception_ptr(std::runtime_error(error.what())));
            return;
          }

          promise->set_value(std::make_tuple<tensorpipe_npu::Descriptor, Storage>(
              std::move(descriptor), std::move(storage)));
        });
  });

  return future;
}

inline void expectDescriptorAndStorageMatchMessage(
    tensorpipe_npu::Descriptor descriptor,
    Storage storage,
    InlineMessage imessage) {
  EXPECT_EQ(imessage.metadata, descriptor.metadata);

  EXPECT_EQ(descriptor.payloads.size(), storage.payloads.size());
  EXPECT_EQ(imessage.payloads.size(), storage.payloads.size());
  for (size_t idx = 0; idx < imessage.payloads.size(); ++idx) {
    EXPECT_EQ(
        imessage.payloads[idx].metadata, descriptor.payloads[idx].metadata);
    EXPECT_EQ(
        imessage.payloads[idx].data.length(), descriptor.payloads[idx].length);
    EXPECT_EQ(
        imessage.payloads[idx].data,
        std::string(
            static_cast<char*>(storage.payloads[idx].get()),
            descriptor.payloads[idx].length));
  }

  EXPECT_EQ(descriptor.tensors.size(), storage.tensors.size());
  EXPECT_EQ(imessage.tensors.size(), storage.tensors.size());
  for (size_t idx = 0; idx < imessage.tensors.size(); ++idx) {
    EXPECT_TRUE(
        imessage.tensors[idx].device == descriptor.tensors[idx].sourceDevice);
    EXPECT_EQ(imessage.tensors[idx].metadata, descriptor.tensors[idx].metadata);
    EXPECT_EQ(
        imessage.tensors[idx].targetDevice,
        descriptor.tensors[idx].targetDevice);
    const tensorpipe_npu::Device& device = storage.tensors[idx].second.device();
    EXPECT_TRUE(
        !imessage.tensors[idx].targetDevice ||
        imessage.tensors[idx].targetDevice == device);
    size_t length = descriptor.tensors[idx].length;
    EXPECT_EQ(imessage.tensors[idx].data.length(), length);
    if (device.type == tensorpipe_npu::kCpuDeviceType) {
      const tensorpipe_npu::CpuBuffer& buffer =
          storage.tensors[idx].second.unwrap<tensorpipe_npu::CpuBuffer>();
      EXPECT_EQ(
          imessage.tensors[idx].data,
          std::string(static_cast<char*>(buffer.ptr), length));
    } else {
      ADD_FAILURE() << "Unexpected target device: " << device.toString();
    }
  }
}

inline std::vector<std::string> genUrls() {
  std::vector<std::string> res;

#if TENSORPIPE_HAS_SHM_TRANSPORT
  res.push_back("shm://");
#endif // TENSORPIPE_HAS_SHM_TRANSPORT
  res.push_back("uv://127.0.0.1");

  return res;
}

inline std::shared_ptr<tensorpipe_npu::Context> makeContext() {
  auto context = std::make_shared<tensorpipe_npu::Context>();

  context->registerTransport(0, "uv", tensorpipe_npu::transport::uv::create());
#if TENSORPIPE_HAS_SHM_TRANSPORT
  context->registerTransport(1, "shm", tensorpipe_npu::transport::shm::create());
#endif // TENSORPIPE_HAS_SHM_TRANSPORT
  context->registerChannel(100, "basic", tensorpipe_npu::channel::basic::create());
#if TENSORPIPE_HAS_CMA_CHANNEL
  context->registerChannel(101, "cma", tensorpipe_npu::channel::cma::create());
#endif // TENSORPIPE_HAS_CMA_CHANNEL
  context->registerChannel(10, "npu_basic", tensorpipe_npu::channel::npu_basic::create(
  tensorpipe_npu::channel::basic::create()));

  return context;
}

class ClientServerPipeTestCase {
  ForkedThreadPeerGroup pg_;

 public:
  void run() {
    pg_.spawn(
        [&]() {
          auto context = makeContext();

          auto listener = context->listen(genUrls());
          pg_.send(PeerGroup::kClient, listener->url("uv"));

          std::promise<std::shared_ptr<tensorpipe_npu::Pipe>> promise;
          listener->accept([&](const tensorpipe_npu::Error& error,
                               std::shared_ptr<tensorpipe_npu::Pipe> pipe) {
            if (error) {
              promise.set_exception(
                  std::make_exception_ptr(std::runtime_error(error.what())));
            } else {
              promise.set_value(std::move(pipe));
            }
          });

          std::shared_ptr<tensorpipe_npu::Pipe> pipe = promise.get_future().get();
          server(*pipe);

          pg_.done(PeerGroup::kServer);
          pg_.join(PeerGroup::kServer);

          context->join();
        },
        [&]() {
          auto context = makeContext();

          auto url = pg_.recv(PeerGroup::kClient);
          auto pipe = context->connect(url);

          client(*pipe);

          pg_.done(PeerGroup::kClient);
          pg_.join(PeerGroup::kClient);

          context->join();
        });
  }

  virtual void client(tensorpipe_npu::Pipe& pipe) = 0;
  virtual void server(tensorpipe_npu::Pipe& pipe) = 0;

  virtual ~ClientServerPipeTestCase() = default;
};

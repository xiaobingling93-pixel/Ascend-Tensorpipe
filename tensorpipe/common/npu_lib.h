/*
 * Copyright (c) 2023 Huawei Technologies Co., Ltd
 * Copyright (c) 2020 Meta Platforms, Inc. and affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the BSD-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include "third_party/acl/inc/acl/acl_base.h"
#include "third_party/acl/inc/acl/acl_rt.h"
#include "third_party/acl/inc/acl/acl.h"


#include <tensorpipe/common/defs.h>
#include <tensorpipe/common/dl.h>

#define TP_NPU_LIB_CHECK(npu_lib, a)                                   \
	do {												               \
	  auto Error = a;									               \
	  if ((Error) != 0) {								               \
          const char* errorStr;                                        \
	      errorStr = aclGetRecentErrMsg();                             \
          TP_THROW_ASSERT() << __TP_EXPAND_OPD(a) << " " << errorStr;  \
	  } 														       \
	} while (false)


namespace tensorpipe_npu {

class NoDevicesError final : public BaseError {
 public:
  std::string what() const override {
    return "The NPU driver failed to init because it didn't find any device";
  }
};


class NPULib {
 private:
  explicit NPULib(DynamicLibraryHandle dlhandle)
      : dlhandle_(std::move(dlhandle)) {}

  DynamicLibraryHandle dlhandle_;


 public:
  NPULib() = default;

  static std::tuple<Error, NPULib> create() {
    Error error;
    DynamicLibraryHandle dlhandle;
    // To keep things "neat" and contained, we open in "local" mode (as
    // opposed to global) so that the npu symbols can only be resolved
    // through this handle and are not exposed (a.k.a., "leaked") to other
    // shared objects.
    std::tie(error, dlhandle) =
        DynamicLibraryHandle::create("libascendcl.so", RTLD_LOCAL | RTLD_NOW);
    if (error) {
      return std::make_tuple(std::move(error), NPULib());
    }
    // Log at level 9 as we can't know whether this will be used in a transport
    // or channel, thus err on the side of this being as low-level as possible
    // because we don't expect this to be of interest that often.
    TP_VLOG(9) << [&]() -> std::string {
      std::string filename;
      std::tie(error, filename) = dlhandle.getFilename();
      if (error) {
        return "Couldn't determine location of shared library libascendcl.so: " +
            error.what();
      }
      return "Found shared library libascendcl.so at " + filename;
    }();
    NPULib lib(std::move(dlhandle));

    // If the driver doesn't find any devices it fails to init (beats me why)
    // but we must support this case, by disabling the channels, rather than
    // throwing. Hence we treat it as if we couldn't find the driver.
    return std::make_tuple(Error::kSuccess, std::move(lib));
  }

};


} // namespace tensorpipe_npu


/*
 * Copyright (C) 2019 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "aidl.h"
#include "fake_io_delegate.h"
#include "options.h"

#include <fuzzer/FuzzedDataProvider.h>
#include <iostream>

#ifdef FUZZ_LOG
constexpr bool kFuzzLog = true;
#else
constexpr bool kFuzzLog = false;
#endif

using android::aidl::test::FakeIoDelegate;

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
  if (size <= 1) return 0;  // no use

  // b/145447540, large nested expressions sometimes hit the stack depth limit.
  // Fuzzing things of this size don't provide any additional meaningful
  // coverage. This is an approximate value which should allow us to explore all
  // of the language w/o hitting a stack overflow.
  if (size > 2000) return 0;

  FuzzedDataProvider provider = FuzzedDataProvider(data, size);
  FakeIoDelegate io;
  std::vector<std::string> args;

  size_t numArgs = provider.ConsumeIntegralInRange(0, 20);
  for (size_t i = 0; i < numArgs; i++) {
    args.emplace_back(provider.ConsumeRandomLengthString());
  }

  while (provider.remaining_bytes() > 0) {
    const std::string name = provider.ConsumeRandomLengthString();
    const std::string contents = provider.ConsumeRandomLengthString();
    io.SetFileContents(name, contents);
  }

  if (kFuzzLog) {
    std::cout << "cmd: ";
    for (const std::string& arg : args) {
      std::cout << arg << " ";
    }
    std::cout << std::endl;

    for (const auto& [f, input] : io.InputFiles()) {
      std::cout << "INPUT " << f << ": " << input << std::endl;
    }
  }

  int ret = android::aidl::aidl_entry(Options::From(args), io);

  if (kFuzzLog) {
    std::cout << "RET: " << ret << std::endl;
    if (ret != 0) {
      for (const auto& [f, output] : io.OutputFiles()) {
        std::cout << "OUTPUT " << f << ": " << std::endl;
        std::cout << output << std::endl;
      }
    }
  }

  return 0;
}

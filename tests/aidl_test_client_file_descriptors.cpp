/*
 * Copyright (C) 2015 The Android Open Source Project
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


#include <vector>

#include <errno.h>
#include <unistd.h>

#include <android-base/unique_fd.h>
#include <binder/ParcelFileDescriptor.h>

#include "aidl_test_client.h"
#include "gmock/gmock.h"

using android::sp;
using android::base::unique_fd;
using android::binder::Status;
using android::os::ParcelFileDescriptor;

using testing::Eq;

struct FdTest : public AidlTest {
  void DoPipe(unique_fd* read_side, unique_fd* write_side) {
    int fds[2];
    int ret = pipe(fds);
    ASSERT_THAT(ret, Eq(0));

    read_side->reset(fds[0]);
    write_side->reset(fds[1]);
  }

  void WriteStringToFd(const std::string& str, const unique_fd& fd) {
    int wrote;
    while ((wrote = write(fd.get(), str.data(), str.size())) < 0 && errno == EINTR)
      ;
    ASSERT_THAT(wrote, Eq((signed)str.size()));
  }

  void ReadFdToStringAndCompare(const unique_fd& fd, const std::string& str) {
    size_t length = str.size();
    int got;
    std::string buf;
    buf.resize(length);

    while ((got = read(fd.get(), &buf[0], length)) < 0 && errno == EINTR)
      ;
    ASSERT_THAT(buf, Eq(str));
  }
};

TEST_F(FdTest, fileDescriptor) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  unique_fd read_fd;
  unique_fd write_fd;
  DoPipe(&read_fd, &write_fd);

  unique_fd return_fd;
  auto status = cpp_java_tests->RepeatFileDescriptor(std::move(write_fd), &return_fd);
  ASSERT_TRUE(status.isOk());

  /* A note on some of the spookier stuff going on here: IIUC writes to pipes
   * should be atomic and non-blocking so long as the total size doesn't exceed
   * PIPE_BUF. We thus play a bit fast and loose with failure modes here.
   */
  WriteStringToFd("ReturnString", return_fd);
  ReadFdToStringAndCompare(read_fd, "ReturnString");
}

TEST_F(FdTest, fileDescriptorArray) {
  if (!cpp_java_tests) GTEST_SKIP() << "Service does not support the CPP/Java-only tests.";

  std::vector<unique_fd> array;
  array.resize(2);
  DoPipe(&array[0], &array[1]);

  std::vector<unique_fd> repeated;

  if (backend == BackendType::JAVA) {
    // other backends might require these to be valid FDs (not -1), since this
    // isn't @nullable, but they don't require this to already be the correct
    // size
    repeated = std::vector<unique_fd>(array.size());
  }

  std::vector<unique_fd> reversed;
  auto status = cpp_java_tests->ReverseFileDescriptorArray(array, &repeated, &reversed);
  ASSERT_TRUE(status.isOk()) << status;

  WriteStringToFd("First", array[1]);
  WriteStringToFd("Second", repeated[1]);
  WriteStringToFd("Third", reversed[0]);
  ReadFdToStringAndCompare(reversed[1], "FirstSecondThird");
}

TEST_F(FdTest, parcelFileDescriptor) {
  unique_fd read_fd;
  unique_fd write_fd;
  DoPipe(&read_fd, &write_fd);

  ParcelFileDescriptor return_fd;
  auto status =
      service->RepeatParcelFileDescriptor(ParcelFileDescriptor(std::move(write_fd)), &return_fd);
  ASSERT_TRUE(status.isOk());

  /* A note on some of the spookier stuff going on here: IIUC writes to pipes
   * should be atomic and non-blocking so long as the total size doesn't exceed
   * PIPE_BUF. We thus play a bit fast and loose with failure modes here.
   */
  WriteStringToFd("ReturnString", return_fd.release());
  ReadFdToStringAndCompare(read_fd, "ReturnString");
}

TEST_F(FdTest, parcelFileDescriptorArray) {
  std::vector<unique_fd> array;
  array.resize(2);
  DoPipe(&array[0], &array[1]);

  std::vector<ParcelFileDescriptor> input;
  for (auto& fd : array) {
    input.push_back(ParcelFileDescriptor(std::move(fd)));
  }

  std::vector<ParcelFileDescriptor> repeated;

  if (backend == BackendType::JAVA) {
    // other backends might require these to be valid FDs (not -1), since this
    // isn't @nullable, but they don't require this to already be the correct
    // size
    repeated = std::vector<ParcelFileDescriptor>(array.size());
  }

  std::vector<ParcelFileDescriptor> reversed;
  auto status = service->ReverseParcelFileDescriptorArray(input, &repeated, &reversed);
  ASSERT_TRUE(status.isOk()) << status;

  WriteStringToFd("First", input[1].release());
  WriteStringToFd("Second", repeated[1].release());
  WriteStringToFd("Third", reversed[0].release());
  ReadFdToStringAndCompare(input[0].release(), "FirstSecondThird");
}

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

#include <map>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <unistd.h>

#include <android-base/unique_fd.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include <binder/Status.h>
#include <utils/Errors.h>
#include <utils/Log.h>
#include <utils/Looper.h>
#include <utils/String8.h>
#include <utils/StrongPointer.h>

#include "android/aidl/tests/BackendType.h"
#include "android/aidl/tests/BnTestService.h"
#include "android/aidl/tests/ITestService.h"

#include "android/aidl/tests/BnNamedCallback.h"
#include "android/aidl/tests/INamedCallback.h"

#include "android/aidl/versioned/tests/BnFooInterface.h"
#include "android/aidl/versioned/tests/IFooInterface.h"

#include "android/aidl/tests/BnNewName.h"
#include "android/aidl/tests/BnOldName.h"

#include "android/aidl/tests/BnCppJavaTests.h"
#include "android/aidl/tests/ICppJavaTests.h"

#include "android/aidl/tests/Union.h"
#include "android/aidl/tests/extension/MyExt.h"
#include "android/aidl/tests/extension/MyExt2.h"

#include "android/aidl/tests/nested/BnNestedService.h"

#include "android/aidl/tests/BnCircular.h"
#include "android/aidl/tests/ICircular.h"
#include "android/aidl/tests/CircularParcelable.h"

#include "android/aidl/loggable/BnLoggableInterface.h"
#include "android/aidl/loggable/Data.h"

#include "android/aidl/fixedsizearray/FixedSizeArrayExample.h"

// Used implicitly.
#undef LOG_TAG
#define LOG_TAG "aidl_native_service"

// libbase
using android::base::unique_fd;

// libutils:
using android::Looper;
using android::LooperCallback;
using android::OK;
using android::sp;
using android::String16;
using android::String8;

// libbinder:
using android::BnInterface;
using android::defaultServiceManager;
using android::IInterface;
using android::IPCThreadState;
using android::Parcel;
using android::ProcessState;
using android::binder::Status;

// Generated code:
using android::aidl::tests::BackendType;
using android::aidl::tests::BadParcelable;
using android::aidl::tests::BnCircular;
using android::aidl::tests::BnCppJavaTests;
using android::aidl::tests::BnNamedCallback;
using android::aidl::tests::BnNewName;
using android::aidl::tests::BnOldName;
using android::aidl::tests::BnTestService;
using android::aidl::tests::ByteEnum;
using android::aidl::tests::CircularParcelable;
using android::aidl::tests::ConstantExpressionEnum;
using android::aidl::tests::GenericStructuredParcelable;
using android::aidl::tests::ICircular;
using android::aidl::tests::ICppJavaTests;
using android::aidl::tests::INamedCallback;
using android::aidl::tests::INewName;
using android::aidl::tests::IntEnum;
using android::aidl::tests::IOldName;
using android::aidl::tests::ITestService;
using android::aidl::tests::LongEnum;
using android::aidl::tests::RecursiveList;
using android::aidl::tests::SimpleParcelable;
using android::aidl::tests::StructuredParcelable;
using android::aidl::tests::Union;
using android::os::ParcelFileDescriptor;
using android::os::PersistableBundle;

// Standard library
using std::map;
using std::optional;
using std::string;
using std::vector;

namespace {

class BinderCallback : public LooperCallback {
 public:
  BinderCallback() {}
  ~BinderCallback() override {}

  int handleEvent(int /* fd */, int /* events */, void* /* data */) override {
    IPCThreadState::self()->handlePolledCommands();
    return 1;  // Continue receiving callbacks.
  }
};

class NamedCallback : public BnNamedCallback {
 public:
  explicit NamedCallback(String16 name) : name_(name) {}

  Status GetName(String16* ret) {
    *ret = name_;
    return Status::ok();
  }

 private:
  String16 name_;
};

class OldName : public BnOldName {
 public:
  OldName() = default;
  ~OldName() = default;

  Status RealName(String16* output) override {
    *output = String16("OldName");
    return Status::ok();
  }
};

class NewName : public BnNewName {
 public:
  NewName() = default;
  ~NewName() = default;

  Status RealName(String16* output) override {
    *output = String16("NewName");
    return Status::ok();
  }
};

class Circular : public BnCircular {
 public:
  Circular(sp<ITestService> srv) : mSrv(srv) {}
  ~Circular() = default;

  Status GetTestService(sp<ITestService>* _aidl_return) override {
    *_aidl_return = mSrv;
    return Status::ok();
  }

 private:
  sp<ITestService> mSrv;
};

template <typename T>
Status ReverseArray(const vector<T>& input, vector<T>* repeated, vector<T>* _aidl_return) {
  ALOGI("Reversing array of length %zu", input.size());
  *repeated = input;
  *_aidl_return = input;
  std::reverse(_aidl_return->begin(), _aidl_return->end());
  return Status::ok();
}

template <typename T>
Status RepeatNullable(const optional<T>& input, optional<T>* _aidl_return) {
  ALOGI("Repeating nullable value");
  *_aidl_return = input;
  return Status::ok();
}

class CppJavaTests : public BnCppJavaTests {
 public:
  CppJavaTests() = default;
  ~CppJavaTests() = default;

  Status RepeatBadParcelable(const BadParcelable& input, BadParcelable* _aidl_return) override {
    *_aidl_return = input;
    return Status::ok();
  }

  Status RepeatSimpleParcelable(const SimpleParcelable& input, SimpleParcelable* repeat,
                                SimpleParcelable* _aidl_return) override {
    ALOGI("Repeated a SimpleParcelable %s", input.toString().c_str());
    *repeat = input;
    *_aidl_return = input;
    return Status::ok();
  }

  Status RepeatGenericParcelable(
      const GenericStructuredParcelable<int32_t, StructuredParcelable, IntEnum>& input,
      GenericStructuredParcelable<int32_t, StructuredParcelable, IntEnum>* repeat,
      GenericStructuredParcelable<int32_t, StructuredParcelable, IntEnum>* _aidl_return) {
    ALOGI("Repeating Generic Parcelable");
    *repeat = input;
    *_aidl_return = input;
    return Status::ok();
  }

  Status RepeatPersistableBundle(const PersistableBundle& input,
                                 PersistableBundle* _aidl_return) override {
    ALOGI("Repeated a PersistableBundle");
    *_aidl_return = input;
    return Status::ok();
  }

  Status ReverseSimpleParcelables(const vector<SimpleParcelable>& input,
                                  vector<SimpleParcelable>* repeated,
                                  vector<SimpleParcelable>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReversePersistableBundles(const vector<PersistableBundle>& input,
                                   vector<PersistableBundle>* repeated,
                                   vector<PersistableBundle>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseUnion(const Union& input, Union* repeated, Union* _aidl_return) override {
    ALOGI("Repeated a Union");
    *repeated = input;
    *_aidl_return = input;
    auto reverse = [](auto& reversible) {
      std::reverse(std::begin(reversible), std::end(reversible));
    };
    switch (input.getTag()) {
      case Union::ns:  // int[]
        reverse(_aidl_return->get<Union::ns>());
        break;
      case Union::s:  // String
        reverse(_aidl_return->get<Union::s>());
        break;
      case Union::ss:  // List<String>
        reverse(_aidl_return->get<Union::ss>());
        break;
      default:
        break;
    }
    return Status::ok();
  }
  Status ReverseNamedCallbackList(const vector<sp<IBinder>>& input, vector<sp<IBinder>>* repeated,
                                  vector<sp<IBinder>>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }

  Status RepeatFileDescriptor(unique_fd read, unique_fd* _aidl_return) override {
    ALOGE("Repeating file descriptor");
    *_aidl_return = unique_fd(dup(read.get()));
    return Status::ok();
  }

  Status ReverseFileDescriptorArray(const vector<unique_fd>& input, vector<unique_fd>* repeated,
                                    vector<unique_fd>* _aidl_return) override {
    ALOGI("Reversing descriptor array of length %zu", input.size());
    repeated->clear();
    for (const auto& item : input) {
      repeated->push_back(unique_fd(dup(item.get())));
      _aidl_return->push_back(unique_fd(dup(item.get())));
    }
    std::reverse(_aidl_return->begin(), _aidl_return->end());
    return Status::ok();
  }
};

class NativeService : public BnTestService {
 public:
  NativeService() {}
  virtual ~NativeService() = default;

  void LogRepeatedStringToken(const String16& token) {
    ALOGI("Repeating '%s' of length=%zu", android::String8(token).string(),
          token.size());
  }

  template <typename T>
  void LogRepeatedToken(const T& token) {
    std::ostringstream token_str;
    token_str << token;
    ALOGI("Repeating token %s", token_str.str().c_str());
  }

  Status TestOneway() override { return Status::fromStatusT(android::UNKNOWN_ERROR); }

  Status Deprecated() override { return Status::ok(); }

  Status RepeatBoolean(bool token, bool* _aidl_return) override {
    LogRepeatedToken(token ? 1 : 0);
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatByte(int8_t token, int8_t* _aidl_return) override {
    LogRepeatedToken(token);
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatChar(char16_t token, char16_t* _aidl_return) override {
    LogRepeatedStringToken(String16(&token, 1));
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatInt(int32_t token, int32_t* _aidl_return) override {
    LogRepeatedToken(token);
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatLong(int64_t token, int64_t* _aidl_return) override {
    LogRepeatedToken(token);
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatFloat(float token, float* _aidl_return) override {
    LogRepeatedToken(token);
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatDouble(double token, double* _aidl_return) override {
    LogRepeatedToken(token);
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatString(const String16& token, String16* _aidl_return) override {
    LogRepeatedStringToken(token);
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatByteEnum(ByteEnum token, ByteEnum* _aidl_return) override {
    ALOGI("Repeating ByteEnum token %s", toString(token).c_str());
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatIntEnum(IntEnum token, IntEnum* _aidl_return) override {
    ALOGI("Repeating IntEnum token %s", toString(token).c_str());
    *_aidl_return = token;
    return Status::ok();
  }
  Status RepeatLongEnum(LongEnum token, LongEnum* _aidl_return) override {
    ALOGI("Repeating LongEnum token %s", toString(token).c_str());
    *_aidl_return = token;
    return Status::ok();
  }

  Status ReverseBoolean(const vector<bool>& input,
                        vector<bool>* repeated,
                        vector<bool>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseByte(const vector<uint8_t>& input,
                     vector<uint8_t>* repeated,
                     vector<uint8_t>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseChar(const vector<char16_t>& input,
                     vector<char16_t>* repeated,
                     vector<char16_t>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseInt(const vector<int32_t>& input,
                    vector<int32_t>* repeated,
                    vector<int32_t>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseLong(const vector<int64_t>& input,
                     vector<int64_t>* repeated,
                     vector<int64_t>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseFloat(const vector<float>& input,
                      vector<float>* repeated,
                      vector<float>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseDouble(const vector<double>& input,
                       vector<double>* repeated,
                       vector<double>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseString(const vector<String16>& input,
                       vector<String16>* repeated,
                       vector<String16>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseByteEnum(const vector<ByteEnum>& input, vector<ByteEnum>* repeated,
                         vector<ByteEnum>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseIntEnum(const vector<IntEnum>& input, vector<IntEnum>* repeated,
                        vector<IntEnum>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }
  Status ReverseLongEnum(const vector<LongEnum>& input, vector<LongEnum>* repeated,
                         vector<LongEnum>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }

  Status SetOtherTestService(const String16& name, const sp<INamedCallback>& service,
                             bool* _aidl_return) override {
    std::lock_guard<std::mutex> guard(service_map_mutex_);
    const auto& existing = service_map_.find(name);
    if (existing != service_map_.end() && existing->second == service) {
      *_aidl_return = true;

      return Status::ok();
    } else {
      *_aidl_return = false;
      service_map_[name] = service;

      return Status::ok();
    }
  }

  Status GetOtherTestService(const String16& name,
                             sp<INamedCallback>* returned_service) override {
    std::lock_guard<std::mutex> guard(service_map_mutex_);
    if (service_map_.find(name) == service_map_.end()) {
      sp<INamedCallback> new_item(new NamedCallback(name));
      service_map_[name] = new_item;
    }

    *returned_service = service_map_[name];
    return Status::ok();
  }

  Status VerifyName(const sp<INamedCallback>& service, const String16& name,
                    bool* returned_value) override {
    String16 foundName;
    Status status = service->GetName(&foundName);

    if (status.isOk()) {
      *returned_value = foundName == name;
    }

    return status;
  }

  Status GetInterfaceArray(const vector<String16>& names,
                           vector<sp<INamedCallback>>* _aidl_return) override {
    vector<sp<INamedCallback>> services(names.size());
    for (size_t i = 0; i < names.size(); i++) {
      if (auto st = GetOtherTestService(names[i], &services[i]); !st.isOk()) {
        return st;
      }
    }
    *_aidl_return = std::move(services);
    return Status::ok();
  }

  Status VerifyNamesWithInterfaceArray(const vector<sp<INamedCallback>>& services,
                                       const vector<String16>& names, bool* _aidl_ret) override {
    if (services.size() == names.size()) {
      for (size_t i = 0; i < services.size(); i++) {
        if (auto st = VerifyName(services[i], names[i], _aidl_ret); !st.isOk() || !*_aidl_ret) {
          return st;
        }
      }
      *_aidl_ret = true;
    } else {
      *_aidl_ret = false;
    }
    return Status::ok();
  }

  Status GetNullableInterfaceArray(const optional<vector<optional<String16>>>& names,
                                   optional<vector<sp<INamedCallback>>>* _aidl_ret) override {
    vector<sp<INamedCallback>> services;
    if (names.has_value()) {
      for (const auto& name : *names) {
        if (name.has_value()) {
          sp<INamedCallback> ret;
          if (auto st = GetOtherTestService(*name, &ret); !st.isOk()) {
            return st;
          }
          services.push_back(std::move(ret));
        } else {
          services.emplace_back();
        }
      }
    }
    *_aidl_ret = std::move(services);
    return Status::ok();
  }

  Status VerifyNamesWithNullableInterfaceArray(const optional<vector<sp<INamedCallback>>>& services,
                                               const optional<vector<optional<String16>>>& names,
                                               bool* _aidl_ret) override {
    if (services.has_value() && names.has_value()) {
      if (services->size() == names->size()) {
        for (size_t i = 0; i < services->size(); i++) {
          if (services->at(i).get() && names->at(i).has_value()) {
            if (auto st = VerifyName(services->at(i), names->at(i).value(), _aidl_ret);
                !st.isOk() || !*_aidl_ret) {
              return st;
            }
          } else if (services->at(i).get() || names->at(i).has_value()) {
            *_aidl_ret = false;
            return Status::ok();
          } else {
            // ok if service=null && name=null
          }
        }
        *_aidl_ret = true;
      } else {
        *_aidl_ret = false;
      }
    } else {
      *_aidl_ret = services.has_value() == names.has_value();
    }
    return Status::ok();
  }

  Status GetInterfaceList(const optional<vector<optional<String16>>>& names,
                          optional<vector<sp<INamedCallback>>>* _aidl_ret) override {
    return GetNullableInterfaceArray(names, _aidl_ret);
  }

  Status VerifyNamesWithInterfaceList(const optional<vector<sp<INamedCallback>>>& services,
                                      const optional<vector<optional<String16>>>& names,
                                      bool* _aidl_ret) override {
    return VerifyNamesWithNullableInterfaceArray(services, names, _aidl_ret);
  }

  Status ReverseStringList(const vector<String16>& input,
                           vector<String16>* repeated,
                           vector<String16>* _aidl_return) override {
    return ReverseArray(input, repeated, _aidl_return);
  }

  Status RepeatParcelFileDescriptor(const ParcelFileDescriptor& read,
                                    ParcelFileDescriptor* _aidl_return) override {
    ALOGE("Repeating parcel file descriptor");
    _aidl_return->reset(unique_fd(dup(read.get())));
    return Status::ok();
  }

  Status ReverseParcelFileDescriptorArray(const vector<ParcelFileDescriptor>& input,
                                          vector<ParcelFileDescriptor>* repeated,
                                          vector<ParcelFileDescriptor>* _aidl_return) override {
    ALOGI("Reversing parcel descriptor array of length %zu", input.size());
    for (const auto& item : input) {
      repeated->push_back(ParcelFileDescriptor(unique_fd(dup(item.get()))));
    }

    for (auto i = input.rbegin(); i != input.rend(); i++) {
      _aidl_return->push_back(ParcelFileDescriptor(unique_fd(dup(i->get()))));
    }
    return Status::ok();
  }

  Status ThrowServiceException(int code) override {
    return Status::fromServiceSpecificError(code);
  }

  Status RepeatNullableIntArray(const optional<vector<int32_t>>& input,
                                optional<vector<int32_t>>* _aidl_return) {
    return RepeatNullable(input, _aidl_return);
  }

  Status RepeatNullableByteEnumArray(const optional<vector<ByteEnum>>& input,
                                     optional<vector<ByteEnum>>* _aidl_return) {
    return RepeatNullable(input, _aidl_return);
  }

  Status RepeatNullableIntEnumArray(const optional<vector<IntEnum>>& input,
                                    optional<vector<IntEnum>>* _aidl_return) {
    return RepeatNullable(input, _aidl_return);
  }

  Status RepeatNullableLongEnumArray(const optional<vector<LongEnum>>& input,
                                     optional<vector<LongEnum>>* _aidl_return) {
    return RepeatNullable(input, _aidl_return);
  }

  Status RepeatNullableStringList(const optional<vector<optional<String16>>>& input,
                                  optional<vector<optional<String16>>>* _aidl_return) {
    ALOGI("Repeating nullable string list");
    return RepeatNullable(input, _aidl_return);
  }

  Status RepeatNullableString(const optional<String16>& input, optional<String16>* _aidl_return) {
    return RepeatNullable(input, _aidl_return);
  }

  Status RepeatNullableParcelable(const optional<ITestService::Empty>& input,
                                  optional<ITestService::Empty>* _aidl_return) {
    return RepeatNullable(input, _aidl_return);
  }

  Status RepeatNullableParcelableList(
      const optional<vector<optional<ITestService::Empty>>>& input,
      optional<vector<optional<ITestService::Empty>>>* _aidl_return) {
    return RepeatNullable(input, _aidl_return);
  }

  Status RepeatNullableParcelableArray(
      const optional<vector<optional<ITestService::Empty>>>& input,
      optional<vector<optional<ITestService::Empty>>>* _aidl_return) {
    return RepeatNullable(input, _aidl_return);
  }

  Status TakesAnIBinder(const sp<IBinder>& input) override {
    (void)input;
    return Status::ok();
  }
  Status TakesANullableIBinder(const sp<IBinder>& input) {
    (void)input;
    return Status::ok();
  }
  Status TakesAnIBinderList(const vector<sp<IBinder>>& input) override {
    (void)input;
    return Status::ok();
  }
  Status TakesANullableIBinderList(const optional<vector<sp<IBinder>>>& input) {
    (void)input;
    return Status::ok();
  }

  Status RepeatUtf8CppString(const string& token,
                             string* _aidl_return) override {
    ALOGI("Repeating utf8 string '%s' of length=%zu", token.c_str(), token.size());
    *_aidl_return = token;
    return Status::ok();
  }

  Status RepeatNullableUtf8CppString(const optional<string>& token,
                                     optional<string>* _aidl_return) override {
    if (!token) {
      ALOGI("Received null @utf8InCpp string");
      return Status::ok();
    }
    ALOGI("Repeating utf8 string '%s' of length=%zu",
          token->c_str(), token->size());
    *_aidl_return = token;
    return Status::ok();
  }

  Status ReverseUtf8CppString(const vector<string>& input,
                              vector<string>* repeated,
                              vector<string>* _aidl_return) {
    return ReverseArray(input, repeated, _aidl_return);
  }

  Status ReverseNullableUtf8CppString(const optional<vector<optional<string>>>& input,
                                      optional<vector<optional<string>>>* repeated,
                                      optional<vector<optional<string>>>* _aidl_return) {
    return ReverseUtf8CppStringList(input, repeated, _aidl_return);
  }

  Status ReverseUtf8CppStringList(const optional<vector<optional<string>>>& input,
                                  optional<vector<optional<string>>>* repeated,
                                  optional<vector<optional<string>>>* _aidl_return) {
    if (!input) {
      ALOGI("Received null list of utf8 strings");
      return Status::ok();
    }
    *_aidl_return = input;
    *repeated = input;
    std::reverse((*_aidl_return)->begin(), (*_aidl_return)->end());
    return Status::ok();
  }

  Status GetCallback(bool return_null, sp<INamedCallback>* ret) {
    if (!return_null) {
      return GetOtherTestService(String16("ABT: always be testing"), ret);
    }
    return Status::ok();
  }

  virtual ::android::binder::Status FillOutStructuredParcelable(StructuredParcelable* parcelable) {
    parcelable->shouldBeJerry = "Jerry";
    parcelable->shouldContainThreeFs = {parcelable->f, parcelable->f, parcelable->f};
    parcelable->shouldBeByteBar = ByteEnum::BAR;
    parcelable->shouldBeIntBar = IntEnum::BAR;
    parcelable->shouldBeLongBar = LongEnum::BAR;
    parcelable->shouldContainTwoByteFoos = {ByteEnum::FOO, ByteEnum::FOO};
    parcelable->shouldContainTwoIntFoos = {IntEnum::FOO, IntEnum::FOO};
    parcelable->shouldContainTwoLongFoos = {LongEnum::FOO, LongEnum::FOO};

    parcelable->const_exprs_1 = ConstantExpressionEnum::decInt32_1;
    parcelable->const_exprs_2 = ConstantExpressionEnum::decInt32_2;
    parcelable->const_exprs_3 = ConstantExpressionEnum::decInt64_1;
    parcelable->const_exprs_4 = ConstantExpressionEnum::decInt64_2;
    parcelable->const_exprs_5 = ConstantExpressionEnum::decInt64_3;
    parcelable->const_exprs_6 = ConstantExpressionEnum::decInt64_4;
    parcelable->const_exprs_7 = ConstantExpressionEnum::hexInt32_1;
    parcelable->const_exprs_8 = ConstantExpressionEnum::hexInt32_2;
    parcelable->const_exprs_9 = ConstantExpressionEnum::hexInt32_3;
    parcelable->const_exprs_10 = ConstantExpressionEnum::hexInt64_1;

    parcelable->shouldSetBit0AndBit2 = StructuredParcelable::BIT0 | StructuredParcelable::BIT2;

    parcelable->u = Union::make<Union::ns>({1, 2, 3});
    parcelable->shouldBeConstS1 = Union::S1();
    return Status::ok();
  }

  ::android::binder::Status RepeatExtendableParcelable(
      const ::android::aidl::tests::extension::ExtendableParcelable& ep,
      ::android::aidl::tests::extension::ExtendableParcelable* ep2) {
    ep2->a = ep.a;
    ep2->b = ep.b;
    std::shared_ptr<android::aidl::tests::extension::MyExt> myExt;
    ep.ext.getParcelable(&myExt);
    ep2->ext.setParcelable(myExt);

    return Status::ok();
  }

  ::android::binder::Status ReverseList(const RecursiveList& list, RecursiveList* ret) override {
    std::unique_ptr<RecursiveList> reversed;
    const RecursiveList* cur = &list;
    while (cur) {
      auto node = std::make_unique<RecursiveList>();
      node->value = cur->value;
      node->next = std::move(reversed);
      reversed = std::move(node);
      cur = cur->next.get();
    }
    *ret = std::move(*reversed);
    return Status::ok();
  }

  Status ReverseIBinderArray(const vector<sp<IBinder>>& input, vector<sp<IBinder>>* repeated,
                             vector<sp<IBinder>>* _aidl_return) override {
    *repeated = input;
    *_aidl_return = input;
    std::reverse(_aidl_return->begin(), _aidl_return->end());
    return Status::ok();
  }

  Status ReverseNullableIBinderArray(const std::optional<vector<sp<IBinder>>>& input,
                                     std::optional<vector<sp<IBinder>>>* repeated,
                                     std::optional<vector<sp<IBinder>>>* _aidl_return) override {
    *repeated = input;
    *_aidl_return = input;
    if (*_aidl_return) {
      std::reverse((*_aidl_return)->begin(), (*_aidl_return)->end());
    }
    return Status::ok();
  }

  Status UnimplementedMethod(int32_t /* arg */, int32_t* /* _aidl_return */) override {
    LOG_ALWAYS_FATAL("UnimplementedMethod shouldn't be called");
  }

  Status GetOldNameInterface(sp<IOldName>* ret) {
    *ret = new OldName;
    return Status::ok();
  }

  Status GetNewNameInterface(sp<INewName>* ret) {
    *ret = new NewName;
    return Status::ok();
  }

  Status GetUnionTags(const std::vector<Union>& input,
                      std::vector<Union::Tag>* _aidl_return) override {
    std::vector<Union::Tag> tags;
    std::transform(input.begin(), input.end(), std::back_inserter(tags),
                   std::mem_fn(&Union::getTag));
    *_aidl_return = std::move(tags);
    return Status::ok();
  }

  Status GetCppJavaTests(sp<IBinder>* ret) {
    *ret = new CppJavaTests;
    return Status::ok();
  }

  Status getBackendType(BackendType* _aidl_return) override {
    *_aidl_return = BackendType::CPP;
    return Status::ok();
  }

  Status GetCircular(CircularParcelable* cp, sp<ICircular>* _aidl_return) override {
    auto srv = sp<ITestService>::fromExisting(this);
    cp->testService = srv;
    *_aidl_return = new Circular(srv);
    return Status::ok();
  }

  android::status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
                               uint32_t flags) override {
    if (code == ::android::IBinder::FIRST_CALL_TRANSACTION + 0 /* UnimplementedMethod */) {
      // pretend that UnimplementedMethod isn't implemented by this service.
      return android::UNKNOWN_TRANSACTION;
    } else {
      return BnTestService::onTransact(code, data, reply, flags);
    }
  }

 private:
  map<String16, sp<INamedCallback>> service_map_;
  std::mutex service_map_mutex_;
};

class VersionedService : public android::aidl::versioned::tests::BnFooInterface {
 public:
  VersionedService() {}
  virtual ~VersionedService() = default;

  Status originalApi() override { return Status::ok(); }
  Status acceptUnionAndReturnString(const ::android::aidl::versioned::tests::BazUnion& u,
                                    std::string* _aidl_return) override {
    switch (u.getTag()) {
      case ::android::aidl::versioned::tests::BazUnion::intNum:
        *_aidl_return =
            std::to_string(u.get<::android::aidl::versioned::tests::BazUnion::intNum>());
        break;
    }
    return Status::ok();
  }
  Status returnsLengthOfFooArray(const vector<::android::aidl::versioned::tests::Foo>& foos,
                                 int32_t* ret) override {
    *ret = static_cast<int32_t>(foos.size());
    return Status::ok();
  }
  Status ignoreParcelablesAndRepeatInt(const ::android::aidl::versioned::tests::Foo& inFoo,
                                       ::android::aidl::versioned::tests::Foo* inoutFoo,
                                       ::android::aidl::versioned::tests::Foo* outFoo,
                                       int32_t value, int32_t* ret) override {
    (void)inFoo;
    (void)inoutFoo;
    (void)outFoo;
    *ret = value;
    return Status::ok();
  }
};

class LoggableInterfaceService : public android::aidl::loggable::BnLoggableInterface {
 public:
  LoggableInterfaceService() {}
  virtual ~LoggableInterfaceService() = default;

  virtual Status LogThis(bool, vector<bool>*, int8_t, vector<uint8_t>*, char16_t, vector<char16_t>*,
                         int32_t, vector<int32_t>*, int64_t, vector<int64_t>*, float,
                         vector<float>*, double, vector<double>*, const String16&,
                         vector<String16>*, vector<String16>*, const android::aidl::loggable::Data&,
                         const sp<IBinder>&, optional<ParcelFileDescriptor>*,
                         vector<ParcelFileDescriptor>*, vector<String16>* _aidl_return) override {
    *_aidl_return = vector<String16>{String16("loggable")};
    return Status::ok();
  }
};

using namespace android::aidl::tests::nested;
class NestedService : public BnNestedService {
 public:
  NestedService() {}
  virtual ~NestedService() = default;

  virtual Status flipStatus(const ParcelableWithNested& p, INestedService::Result* _aidl_return) {
    if (p.status == ParcelableWithNested::Status::OK) {
      _aidl_return->status = ParcelableWithNested::Status::NOT_OK;
    } else {
      _aidl_return->status = ParcelableWithNested::Status::OK;
    }
    return Status::ok();
  }
  virtual Status flipStatusWithCallback(ParcelableWithNested::Status status,
                                        const sp<INestedService::ICallback>& cb) {
    if (status == ParcelableWithNested::Status::OK) {
      return cb->done(ParcelableWithNested::Status::NOT_OK);
    } else {
      return cb->done(ParcelableWithNested::Status::OK);
    }
  }
};

using android::aidl::fixedsizearray::FixedSizeArrayExample;
class FixedSizeArrayService : public FixedSizeArrayExample::BnRepeatFixedSizeArray {
 public:
  FixedSizeArrayService() {}
  virtual ~FixedSizeArrayService() = default;

  Status RepeatBytes(const std::array<uint8_t, 3>& in_input, std::array<uint8_t, 3>* out_repeated,
                     std::array<uint8_t, 3>* _aidl_return) override {
    *out_repeated = in_input;
    *_aidl_return = in_input;
    return Status::ok();
  }
  Status RepeatInts(const std::array<int32_t, 3>& in_input, std::array<int32_t, 3>* out_repeated,
                    std::array<int32_t, 3>* _aidl_return) override {
    *out_repeated = in_input;
    *_aidl_return = in_input;
    return Status::ok();
  }
  Status RepeatBinders(const std::array<sp<IBinder>, 3>& in_input,
                       std::array<sp<IBinder>, 3>* out_repeated,
                       std::array<sp<IBinder>, 3>* _aidl_return) override {
    *out_repeated = in_input;
    *_aidl_return = in_input;
    return Status::ok();
  }
  Status RepeatParcelables(
      const std::array<FixedSizeArrayExample::IntParcelable, 3>& in_input,
      std::array<FixedSizeArrayExample::IntParcelable, 3>* out_repeated,
      std::array<FixedSizeArrayExample::IntParcelable, 3>* _aidl_return) override {
    *out_repeated = in_input;
    *_aidl_return = in_input;
    return Status::ok();
  }
  Status Repeat2dBytes(const std::array<std::array<uint8_t, 3>, 2>& in_input,
                       std::array<std::array<uint8_t, 3>, 2>* out_repeated,
                       std::array<std::array<uint8_t, 3>, 2>* _aidl_return) override {
    *out_repeated = in_input;
    *_aidl_return = in_input;
    return Status::ok();
  }
  Status Repeat2dInts(const std::array<std::array<int32_t, 3>, 2>& in_input,
                      std::array<std::array<int32_t, 3>, 2>* out_repeated,
                      std::array<std::array<int32_t, 3>, 2>* _aidl_return) override {
    *out_repeated = in_input;
    *_aidl_return = in_input;
    return Status::ok();
  }
  Status Repeat2dBinders(const std::array<std::array<sp<IBinder>, 3>, 2>& in_input,
                         std::array<std::array<sp<IBinder>, 3>, 2>* out_repeated,
                         std::array<std::array<sp<IBinder>, 3>, 2>* _aidl_return) override {
    *out_repeated = in_input;
    *_aidl_return = in_input;
    return Status::ok();
  }
  Status Repeat2dParcelables(
      const std::array<std::array<FixedSizeArrayExample::IntParcelable, 3>, 2>& in_input,
      std::array<std::array<FixedSizeArrayExample::IntParcelable, 3>, 2>* out_repeated,
      std::array<std::array<FixedSizeArrayExample::IntParcelable, 3>, 2>* _aidl_return) override {
    *out_repeated = in_input;
    *_aidl_return = in_input;
    return Status::ok();
  }
};

int Run() {
  android::sp<NativeService> service = new NativeService;
  sp<Looper> looper(Looper::prepare(0 /* opts */));

  int binder_fd = -1;
  ProcessState::self()->setThreadPoolMaxThreadCount(0);
  IPCThreadState::self()->disableBackgroundScheduling(true);
  IPCThreadState::self()->setupPolling(&binder_fd);
  ALOGI("Got binder FD %d", binder_fd);
  if (binder_fd < 0) return -1;

  sp<BinderCallback> cb(new BinderCallback);
  if (looper->addFd(binder_fd, Looper::POLL_CALLBACK, Looper::EVENT_INPUT, cb,
                    nullptr) != 1) {
    ALOGE("Failed to add binder FD to Looper");
    return -1;
  }

  auto status = defaultServiceManager()->addService(service->getInterfaceDescriptor(), service);
  if (status != OK) {
    ALOGE("Failed to add service %s", String8(service->getInterfaceDescriptor()).c_str());
    return -1;
  }

  // android::sp<VersionedService> versionedService = new VersionedService;
  // status = defaultServiceManager()->addService(versionedService->getInterfaceDescriptor(),
  //                                              versionedService);
  // if (status != OK) {
  //   ALOGE("Failed to add service %s", String8(versionedService->getInterfaceDescriptor()).c_str());
  //   return -1;
  // }

  android::sp<LoggableInterfaceService> loggableInterfaceService = new LoggableInterfaceService;
  status = defaultServiceManager()->addService(loggableInterfaceService->getInterfaceDescriptor(),
                                               loggableInterfaceService);
  if (status != OK) {
    ALOGE("Failed to add service %s",
          String8(loggableInterfaceService->getInterfaceDescriptor()).c_str());
    return -1;
  }

  android::sp<NestedService> nestedService = new NestedService;
  status =
      defaultServiceManager()->addService(nestedService->getInterfaceDescriptor(), nestedService);
  if (status != OK) {
    ALOGE("Failed to add service %s", String8(nestedService->getInterfaceDescriptor()).c_str());
    return -1;
  }

  android::sp<FixedSizeArrayService> fixedSizeArrayService = new FixedSizeArrayService;
  status = defaultServiceManager()->addService(fixedSizeArrayService->getInterfaceDescriptor(),
                                               fixedSizeArrayService);
  if (status != OK) {
    ALOGE("Failed to add service %s",
          String8(fixedSizeArrayService->getInterfaceDescriptor()).c_str());
    return -1;
  }

  ALOGI("Entering loop");
  while (true) {
    const int result = looper->pollAll(-1 /* timeoutMillis */);
    ALOGI("Looper returned %d", result);
  }
  return 0;
}

}  // namespace

int main(int /* argc */, char* /* argv */ []) {
  return Run();
}

#define LOG_TAG "TestMain"
#define LOG_NDEBUG 0

#include <pthread.h>

#include <binder/Binder.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
// #include <binder/IInterface.h>
#include <utils/Looper.h>

#include <BpBinderEcho.h>
#include <BnBinderEcho.h>

#include <utils/Log.h>
#include <utils/Mutex.h>

using namespace android;

static String16 echoServiceName = String16("test.Echo");

class BnEcho : public BnBinderEcho, public IBinder::DeathRecipient {
public:
    BnEcho() {}
    ~BnEcho() {}

    ::android::binder::Status echo(int32_t in, int32_t *out) override {
        printf("BnEcho::echo %d\n", in);
        *out = in;
        return ::android::binder::Status::fromStatusT(::android::OK);
    }

    void binderDied(const wp<IBinder>& who) override
    {
        ALOGI("binderDied");
    }
};

class BinderCallback : public LooperCallback {
public:
    static sp<BinderCallback> setupTo(const sp<Looper>& looper) {
        sp<BinderCallback> cb = sp<BinderCallback>::make();

        int binder_fd = -1;
        IPCThreadState::self()->setupPolling(&binder_fd);
        printf("binder fd = %d\n", binder_fd);
        LOG_ALWAYS_FATAL_IF(binder_fd < 0, "Failed to setupPolling: %d", binder_fd);

        int ret = looper->addFd(binder_fd,
                                Looper::POLL_CALLBACK,
                                Looper::EVENT_INPUT,
                                cb,
                                nullptr /*data*/);
        LOG_ALWAYS_FATAL_IF(ret != 1, "Failed to add binder FD to Looper");

        return cb;
    }

    int handleEvent(int /* fd */, int /* events */, void* /* data */) override {
        printf("binder handleEvent\n");
        IPCThreadState::self()->handlePolledCommands();
        return 1;  // Continue receiving callbacks.
    }
};


int main(int argc, char *argv[]) {
    ProcessState::self()->setThreadPoolMaxThreadCount(0);
    // IPCThreadState::self()->disableBackgroundScheduling(true);

    sp<IServiceManager> sm = defaultServiceManager();
    if (argc > 1) {
        if (!strcmp(argv[1], "server")) {
            sp<BnEcho> echoService = sp<BnEcho>::make();
            // echoService->setRequestingSid(true);

            if (sm->addService(echoServiceName, echoService, false, IServiceManager::DUMP_FLAG_PRIORITY_DEFAULT) != NO_ERROR){
                printf("Failed addService()\n");
                return 1;
            }
            sp<Looper> looper = Looper::prepare(false /*allowNonCallbacks*/);
            BinderCallback::setupTo(looper);
            while(true) {
                looper->pollAll(-1);
            }
            // ProcessState::self()->startThreadPool();
            // IPCThreadState::self()->joinThreadPool();
        }
    } else {
        sp<IBinder> service = sm->getService(echoServiceName);
        sp<IBinderEcho> echo = interface_cast<IBinderEcho>(service);

        int32_t result = 0;
        echo->echo(10, &result);
        printf("echo result = %d\n", result);
    }
	return 0;
}
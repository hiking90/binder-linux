#define LOG_TAG "TestMain"
#define LOG_NDEBUG 0

#include <pthread.h>

#include <binder/Binder.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/IInterface.h>

#include <utils/Log.h>

using namespace android;

static String16 testerServiceName = String16("test.Tester");

enum {
    TRANSACT_CALLBACK = IBinder::FIRST_CALL_TRANSACTION,
};

class ITesterListener : public IInterface {
public:
    DECLARE_META_INTERFACE(TesterListener);

    virtual void callback(int32_t val) = 0;
};

class BpTesterListener : public BpInterface<ITesterListener>
{
public:
    explicit BpTesterListener(const sp<IBinder>& impl)
        : BpInterface<ITesterListener>(impl)
    {
    }

    void callback(int32_t val)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ITesterListener::getInterfaceDescriptor());
        data.writeInt32(val);
        remote()->transact(TRANSACT_CALLBACK, data, &reply, IBinder::FLAG_ONEWAY);
    }
};

IMPLEMENT_META_INTERFACE(TesterListener, "test.ITesterListener");


enum {
    REGISTER_LISTENER = IBinder::FIRST_CALL_TRANSACTION,
    UNREGISTER_LISTENER,
    GET_VALUE,
    SET_VALUE,
};


class ITester : public IInterface {
public:
    DECLARE_META_INTERFACE(Tester);

    virtual void registerListener(const sp<ITesterListener>& listener) = 0;
    virtual void unregisterListener(const sp<ITesterListener>& listener) = 0;
    virtual int32_t getVal() = 0;
    virtual void setVal(int32_t val) = 0;
};

class BnTester : public BnInterface<ITester> {
public:
    virtual status_t onTransact(uint32_t code, const Parcel& data,
                                Parcel* reply, uint32_t flags = 0);
};


class BpTester : public BpInterface<ITester> {
public:
    explicit BpTester(const sp<IBinder>& impl)
        : BpInterface<ITester>(impl)
    {
    }

    virtual void registerListener(const sp<ITesterListener>& listener)
    {
        Parcel data;
        data.writeInterfaceToken(ITester::getInterfaceDescriptor());
        data.writeStrongBinder(IInterface::asBinder(listener));
        remote()->transact(REGISTER_LISTENER, data, NULL);
    }

    virtual void unregisterListener(const sp<ITesterListener>& listener)
    {
        Parcel data;
        data.writeInterfaceToken(ITester::getInterfaceDescriptor());
        data.writeStrongBinder(IInterface::asBinder(listener));
        remote()->transact(UNREGISTER_LISTENER, data, NULL);        
    }

    virtual int32_t getVal()
    {
        Parcel data, reply;

        data.writeInterfaceToken(ITester::getInterfaceDescriptor());
        remote()->transact(GET_VALUE, data, &reply);
        return reply.readInt32();
    }

    virtual void setVal(int32_t val)
    {
        Parcel data;
        data.writeInterfaceToken(ITester::getInterfaceDescriptor());
        data.writeInt32(val);
        remote()->transact(SET_VALUE, data, NULL);
    }
};

IMPLEMENT_META_INTERFACE(Tester, "test.ITester");

status_t BnTester::onTransact(uint32_t code, const Parcel& data,
    Parcel* reply, uint32_t flags)
{
    switch(code) {
        case REGISTER_LISTENER: {
            CHECK_INTERFACE(ITester, data, reply);
            sp<ITesterListener> listener =
                interface_cast<ITesterListener>(data.readStrongBinder());
            registerListener(listener);
            return OK;
        }

        case UNREGISTER_LISTENER: {
            CHECK_INTERFACE(ITester, data, reply);
            sp<ITesterListener> listener =
                interface_cast<ITesterListener>(data.readStrongBinder());
            unregisterListener(listener);
            return OK;
        }

        case GET_VALUE: {
            CHECK_INTERFACE(ITester, data, reply);
            int32_t result = getVal();
            reply->writeInt32(result);
            return OK;
        }

        case SET_VALUE: {
            CHECK_INTERFACE(ITester, data, reply);
            int32_t result = data.readInt32();
            setVal(result);
            return OK;
        }
    }
    return BBinder::onTransact(code, data, reply, flags);
}

class Tester : public BnTester, public IBinder::DeathRecipient {
public:
    Tester() : mVal(1234567) {}

    virtual void registerListener(const sp<ITesterListener>& listener)
    {
        ALOGV("registerListener");
    }

    virtual void unregisterListener(const sp<ITesterListener>& listener)
    {
        ALOGV("unregisterListener");

    }

    virtual int32_t getVal()
    {
        ALOGV("getVal()");
        return mVal;
    }

    virtual void setVal(int32_t val)
    {
        ALOGV("setVal(%d)", val);
        mVal = val;
    }

    virtual void binderDied(const wp<IBinder>& who)
    {
        ALOGV("binderDied");
    }

private:
    int32_t     mVal;
};

int main(int argc, char *argv[]) {
    sp<IServiceManager> sm = defaultServiceManager();
    if (argc > 1) {
        if (!strcmp(argv[1], "server")) {
            sm->addService(testerServiceName, new Tester);
            ProcessState::self()->startThreadPool();
            IPCThreadState::self()->joinThreadPool();            
        }
    } else {
        sp<IBinder> binder = sm->getService(testerServiceName);
        sp<ITester> tester = interface_cast<ITester>(binder);

        int32_t result = tester->getVal();
        printf("Init Value = %d\n", result);
        tester->setVal(10);
        result = tester->getVal();
        printf("Result = %d\n", result);
    }
	return 0;
}
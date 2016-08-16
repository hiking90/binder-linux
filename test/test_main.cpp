#define LOG_TAG "TestMain"
#define LOG_NDEBUG 0

#include <pthread.h>

#include <binder/Binder.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/IInterface.h>

#include <utils/Log.h>
#include <utils/Mutex.h>

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
        remote()->transact(TRANSACT_CALLBACK, data, &reply, 0 /*IBinder::FLAG_ONEWAY*/);
    }
};

IMPLEMENT_META_INTERFACE(TesterListener, "test.ITesterListener");

class BnTesterListener : public BnInterface<ITesterListener> {
public:
    virtual status_t onTransact(uint32_t code, const Parcel& data,
                                Parcel* reply, uint32_t flags = 0);
};

status_t BnTesterListener::onTransact(uint32_t code, const Parcel& data,
    Parcel* reply, uint32_t flags)
{
    switch(code) {
        case TRANSACT_CALLBACK: {
            CHECK_INTERFACE(ITesterListener, data, reply);
            int32_t val = data.readInt32();
            callback(val);
        }
    }
}

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
        ALOGI("registerListener");
        if (listener == NULL)
            return;
        Mutex::Autolock _l(mLock);
        // check whether this is a duplicate
        for (size_t i = 0; i < mListeners.size(); i++) {
            if (IInterface::asBinder(mListeners[i]) == IInterface::asBinder(listener)) {
                return;
            }
        }

        mListeners.add(listener);
        IInterface::asBinder(listener)->linkToDeath(this);
    }

    virtual void unregisterListener(const sp<ITesterListener>& listener)
    {
        ALOGI("unregisterListener");
        if (listener == NULL)
            return;
        Mutex::Autolock _l(mLock);
        for (size_t i = 0; i < mListeners.size(); i++) {
            if (IInterface::asBinder(mListeners[i]) == IInterface::asBinder(listener)) {
                IInterface::asBinder(mListeners[i])->unlinkToDeath(this);
                mListeners.removeAt(i);
                break;
            }
        }
    }

    virtual int32_t getVal()
    {
        ALOGI("getVal()");
        return mVal;
    }

    virtual void setVal(int32_t val)
    {
        ALOGI("setVal(%d)", val);
        mVal = val;

        Mutex::Autolock _l(mLock);
        for (size_t i = 0; i < mListeners.size(); i++) {
            mListeners[i]->callback(mVal);
        }
    }

    virtual void binderDied(const wp<IBinder>& who)
    {
        ALOGI("binderDied");
    }

private:
    int32_t     mVal;
    Mutex       mLock;
    Vector<sp<ITesterListener> > mListeners;
};

class TesterListener : public BnTesterListener {
    virtual void callback(int32_t val) {
        printf("CALLBACK: %d\n", val);
    }
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

        sp<ITesterListener> listener = new TesterListener;
        tester->registerListener(listener);
        tester->setVal(10);
        result = tester->getVal();
        printf("Result = %d\n", result);
        tester->unregisterListener(listener);
    }
	return 0;
}
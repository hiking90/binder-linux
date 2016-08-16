#include <binder/Binder.h>
#include <binder/ProcessState.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>

using namespace android;

int main(int argc, char *argv[]) {
    sp<IServiceManager> sm = defaultServiceManager();

    ProcessState::self()->startThreadPool();
    IPCThreadState::self()->joinThreadPool();
	return 0;
}
//
// Created by Side on 2020/10/19.
//

#include "executive.h"

void *wang::thread::ThreadExecutive::ThreadFunction(void *context) {
    auto* pThread = (ThreadExecutive*)(context);
    pThread->func_->RunFunctionEntity();
    return nullptr;
}

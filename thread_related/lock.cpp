//
// Created by Side on 2020/10/19.
//

#include "lock.h"

pthread_mutex_t* wang::thread::Locker::GetMutexPtr() {
    return &mutex_;
}

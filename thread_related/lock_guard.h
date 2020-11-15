//
// Created by Side on 2020/10/21.
//

#ifndef LINUX_LOCK_GUARD_H
#define LINUX_LOCK_GUARD_H
#include "lock.h"
namespace wang{
    namespace thread{
        class LockGuard {
        private:
            Locker *lock_;
        public:
            explicit LockGuard(Locker *lock): lock_(lock){
                try {
                    lock_->Lock();
                } catch (std::string &err_msg) {
                    throw err_msg;
                }
            }
            ~LockGuard(){
                lock_->UnLock();
            }
        };
    }
}



#endif //LINUX_LOCK_GUARD_H

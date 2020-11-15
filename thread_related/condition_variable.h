//
// Created by Side on 2020/10/20.
//

#ifndef LINUX_HOMEWORK_CONDITION_VARIABLE_H
#define LINUX_HOMEWORK_CONDITION_VARIABLE_H

#include <thread>
#include <cstring>
#include "lock.h"

namespace wang {
    namespace thread {
        class ConditionVariable {
        private:
            pthread_cond_t condition_id_{};
        public:
            ConditionVariable() {
                if (0 != pthread_cond_init(&condition_id_, nullptr)) {
                    throw std::string("pthread_cond_init error:") + strerror(errno);
                }
            }

            ~ConditionVariable() {
                pthread_cond_destroy(&condition_id_);
            }

            bool NotifyOne() {
                return pthread_cond_signal(&condition_id_) == 0;
            }

            bool NotifyAll() {
                return 0 == pthread_cond_broadcast(&condition_id_);
            }

            bool Wait(Locker &lock) {
                return 0 == pthread_cond_wait(&condition_id_, lock.GetMutexPtr());
            }
        };
    }
}


#endif //LINUX_HOMEWORK_CONDITION_VARIABLE_H

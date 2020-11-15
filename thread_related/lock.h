//
// Created by Side on 2020/10/19.
//

#ifndef LINUX_HOMEWORK_LOCK_H
#define LINUX_HOMEWORK_LOCK_H

#include <pthread.h>
#include <exception>
#include <cerrno>
#include <cstring>
#include <string>

namespace wang {
    namespace thread {
        class Locker {
        private:
            bool locked_ = false;
            pthread_mutex_t mutex_{};
        public:

            pthread_mutex_t *GetMutexPtr();

            Locker() {
                int res = pthread_mutex_init(&mutex_, nullptr);
                if (res) {
                    std::string errMsg = "pthread_mutex_init error:";
                    errMsg += strerror(errno);
                    throw errMsg;
                }
            }

            ~Locker() {
                pthread_mutex_destroy(&mutex_);
            }

            void Lock() {
                int err = pthread_mutex_lock(&mutex_);
                if (err != 0) {
                    std::string err_msg = "pthread_mutex_lock error:";
                    err_msg += strerror(err);
                    throw err_msg;
                }
                locked_ = true;
            }

            void UnLock() {
                if (locked_) {
                    int err = 0;
                    err = pthread_mutex_unlock(&mutex_);
                    if (err != 0) {
                        std::string err_msg = "pthread_mutex_unlock error:";
                        err_msg += std::to_string(err);
                        throw err_msg;
                    }
                } else {
                    std::string err_msg = "pthread_mutex_unlock error:";
                    err_msg += strerror(errno);
                    throw err_msg;
                }
            }
        };
    }
}

#endif //LINUX_HOMEWORK_LOCK_H

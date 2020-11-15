//
// Created by Side on 2020/11/11.
//

#ifndef LINUX_HOMEWORK_THREAD_POOL_H
#define LINUX_HOMEWORK_THREAD_POOL_H

#include <atomic>
#include <queue>
#include "executive.h"
#include "lock.h"
#include "condition_variable.h"

namespace wang {
    namespace thread {
        class ThreadPool {
        private:
            std::vector<ThreadExecutive *> worker_queue_;
            std::queue<ExecutiveAbstractFunction *> task_queue_;
            int submit_task_num_ = 0;
            bool started_ = false;
        public:
            ConditionVariable has_task_condition_var_; //have to expose to single thread
            Locker task_submit_get_lock_;

            explicit ThreadPool(int task_num);

            void Start();

            bool Submit(ExecutiveAbstractFunction *func);

            bool GetFunc(ExecutiveAbstractFunction **func);

            bool ShutDown();

            int GetQueueSize() {
                return task_queue_.size();
            }

            ~ThreadPool();
        };

        class RwFunc : public ExecutiveAbstractFunction {
        private:
            std::atomic<bool> running_{};
            int total_task_num_ = 0;
            int id_;

        public:
            explicit RwFunc(int id, void *context) : ExecutiveAbstractFunction(context) {
                running_.store(true);
                id_ = id;
            };

            void RunFunctionEntity() override {
                auto *pool = (ThreadPool *) (running_context_);
                while (running_) {
                    pool->task_submit_get_lock_.Lock();
                    if (pool->GetQueueSize() == 0) {
                        pool->has_task_condition_var_.Wait(pool->task_submit_get_lock_);
                    }
                    if (running_) {
                        ExecutiveAbstractFunction *func;
                        if (pool->GetFunc(&func)) {
                            func->RunFunctionEntity();
                            total_task_num_++;
                        } else {
                            std::cout << "worker " << id_ << " is notify,but can not find any task,some logic error"
                                      << std::endl;
                        }
                    } else {
                        //线程推出 解锁
                        pool->task_submit_get_lock_.UnLock();
                        break;
                    }
                    pool->task_submit_get_lock_.UnLock();
                }
                std::cout << "thread " << id_ << " quit,total handle work number:" << total_task_num_ << std::endl;
            }

            ~RwFunc() override {
                running_.store(false);
            }
        };

    }
}


#endif //LINUX_HOMEWORK_THREAD_POOL_H

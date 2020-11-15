//
// Created by Side on 2020/11/11.
//

#include "thread_pool.h"
#include "lock_guard.h"

namespace wang {
    namespace thread {
        ThreadPool::ThreadPool(int task_num) {
            for (int i = 1; i <= task_num; i++) {
                try {
                    auto *loop_work = new RwFunc(i, this);
                    auto *worker = new ThreadExecutive(loop_work);
                    worker_queue_.push_back(worker);
                } catch (std::exception &e) {
                    std::cout << "error init task queue of thread" << i << std::endl;
                    std::abort();
                }
            }
        }

        bool ThreadPool::Submit(ExecutiveAbstractFunction *func) {
            if (!started_) {
                std::cout << "submit a task on stopped thread pool" << std::endl;
                return false;
            }
            LockGuard lock_guard(&task_submit_get_lock_);
            task_queue_.push(func);
            has_task_condition_var_.NotifyOne();
            submit_task_num_++;
            return true;
        }

        bool ThreadPool::GetFunc(ExecutiveAbstractFunction **func) {
            if (task_queue_.empty()) {
                std::cout << "hard to be here throatily,when shutdown or ctrl-c" << std::endl;
                return false;
            }
            *func = task_queue_.front();
            task_queue_.pop();
            return true;
        }

        ThreadPool::~ThreadPool() {
            ShutDown();
        }

        void ThreadPool::Start() {
            for (auto & i : worker_queue_) {
                i->run();
            }
            started_ = true;
        }

        bool ThreadPool::ShutDown() {
            for (auto & i : worker_queue_) {
                delete i;
            }
            //delete 会让所有线程的while(true)中的is running进入false状态，然后重新唤起所有处于wait的线程进行退出
            has_task_condition_var_.NotifyAll();
            return true;
        }
    }
}



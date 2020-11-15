//
// Created by Side on 2020/10/19.
//

#ifndef LINUX_HOMEWORK_EXECUTIVE_H
#define LINUX_HOMEWORK_EXECUTIVE_H

#include <iostream>
#include <pthread.h>

namespace wang {
    namespace thread {
        class ExecutiveAbstractFunction {
        protected:
            void *running_context_;
        public:
            explicit ExecutiveAbstractFunction(void *context) : running_context_(context) {};

            virtual void RunFunctionEntity() = 0;

            virtual ~ExecutiveAbstractFunction() = default;
        };

        class AbstractExecutive {
        protected:
            ExecutiveAbstractFunction *func_;
        public:
                explicit AbstractExecutive(ExecutiveAbstractFunction *func) : func_(func) {};

            virtual ~AbstractExecutive() {
                delete func_;
            }

            virtual void run() = 0;

            virtual void wait() = 0;
        };

        class ThreadExecutive : public AbstractExecutive {
        private:
            pthread_t thread_id_{};
            bool is_created = false;

            static void *ThreadFunction(void *context);

        public:
            explicit ThreadExecutive(ExecutiveAbstractFunction *pFunc) : AbstractExecutive(pFunc) {}

            void run() override {
                if (!is_created) {
                    if (0 != pthread_create(&thread_id_, nullptr, ThreadFunction, this)) {

                    }
                    is_created = true;
                }

            }

            void wait() override {
                if (is_created) {
                    delete func_; //func 's memory control by thread
                    pthread_join(thread_id_, nullptr);
                }
            }
        };
    }
}


#endif //LINUX_HOMEWORK_EXECUTIVE_H

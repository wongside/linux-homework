//
// Created by Side on 2020/10/22.
//

#ifndef LINUX_HOMEWORK_FILE_MANAGER_H
#define LINUX_HOMEWORK_FILE_MANAGER_H

#include "../thread_related/lock.h"
#include <functional>
#include <unordered_map>
#include "../data_table/table_file.h"
#include "../data_table/table_configure.h"
#include "../thread_related/thread_pool.h"
#include "../thread_related/executive.h"
#include "../thread_related/executive.h"

namespace wang {
    namespace side {
        struct ReaderMethodParam {
            int Row;
            std::function<void(char *buf, int size)> Callback;
        };
        struct WriterMethodParam {
            char *Buff;
            int Size;
            std::function<void(int row, char *buf, int size)> Callback;
        };

        class FileManager {
        private:
            static int max_rows_for_per_file_;
            static std::string dms_prefix_;
            static std::string dms_suffix_;
            static thread::Locker create_instance_lock_;
            static FileManager *instance_;
            static int row_size_;
            static int auto_increase_file_num_;

            friend class TableConfigure;

            TableConfigure configure_;
            TableMetaData meta_file_;
            thread::Locker row_num_lock_;

            /**
             * 嵌套类实现 内存回收，通过静态的嵌套类 析构来 delete instance，也可以用atexist()
             */
            class GcManager {
            public:
                GcManager() = default;

                ~GcManager() {
                    delete FileManager::instance_;
                }
            };

            long long max_file_index_ = 10;
            std::unordered_map<std::string, AbstractTableFile *> file_manager_;
            long long cur_total_rows_;
            thread::ThreadPool *worker_pool_;

            bool EqualMaxRow() {
                return 0 == cur_total_rows_ % max_rows_for_per_file_;
            }

            static std::string ConstructFileName(long long index) {
                return dms_prefix_ + std::to_string(index) + dms_suffix_;
            }

            bool FindCurrentFile(AbstractTableFile **file);

            bool FindFile(int row, AbstractTableFile **file);

            bool ExtendLocalFile();

        public:
            explicit FileManager(int num = 1)//std::thread::hardware_concurrency())
            {
                //TODO 处理文件夹
                worker_pool_ = new thread::ThreadPool(num);
                worker_pool_->Start();
                cur_total_rows_ = 0;
                FromStorage();
                //计算预写入的下一个文件的文件名
                long long index = (cur_total_rows_ + 1) / (FileManager::max_rows_for_per_file_ + 1) + 1;
                std::string restartFilename = ConstructFileName(index);
                file_manager_.emplace(restartFilename, new TableFile(restartFilename, FileManager::row_size_));
                max_file_index_ = index;
            }

            ~FileManager() {
                for (int i = 0; i < max_file_index_; i++) {
                    std::string filename = ConstructFileName(i);
                    auto iter = file_manager_.find(filename);
                    if (iter != file_manager_.end()) {
                        delete (iter->second);
                    }
                }
                ToStorage();
                delete worker_pool_;
            }

            static int GetRowSize() {
                return FileManager::row_size_;
            }

            static bool InitInstance(int per_file_size, int row_size,
                                     const std::string &dir_des = "",
                                     const std::string &prefix = "data_table_file_",
                                     const std::string &suffix = ".bin") {
                FileManager::row_size_ = row_size;
                FileManager::max_rows_for_per_file_ = per_file_size;
                FileManager::dms_prefix_ = prefix;
                FileManager::dms_suffix_ = suffix;
                return true;
            }

            static FileManager *GetInstance();

            void ToStorage() {
                configure_.SetConfigure(FileManager::row_size_,
                                        FileManager::max_rows_for_per_file_);
                configure_.ToStorage();

                meta_file_.SetMetaData(cur_total_rows_);
                meta_file_.ToStorage();
            }

            bool FromStorage() {
                if (configure_.FromStorage()) {
                    FileManager::row_size_ = configure_.config_row_size_;
                    FileManager::max_rows_for_per_file_ = configure_.config_max_rows_for_per_file_;
                }
                if (meta_file_.FromStorage()) {
                    cur_total_rows_ = meta_file_.cur_total_rows_;
                }
                return true;
            }

            long long IncreaseRow() {
                row_num_lock_.Lock();
                auto row = (cur_total_rows_ += 1);
                row_num_lock_.UnLock();
                return row;
            }

            bool Append(long long &row, const char *buffer, int total) {
                AbstractTableFile *file;

                if (!FindCurrentFile(&file)) {
                    ExtendLocalFile();
                    FindCurrentFile(&file);
                }
                if (file->WriteRow(buffer, total)) {
                    row = IncreaseRow();
                } else {
                    return false;
                }
                if (EqualMaxRow()) {
                    file->Flush();
                }
                return true;
            }

            bool Append(const std::string &msg);

            bool Read(int row, char *buf, int size);

            bool MultiWrite(WriterMethodParam *param);

            static std::string meta_data_file_prefix_;
            static GcManager gc;
        };

        class MultiWriterFunction : public thread::ExecutiveAbstractFunction {
        public:
            explicit MultiWriterFunction(void *context) : ExecutiveAbstractFunction(context) {}

            void RunFunctionEntity() override {
                auto *method = (WriterMethodParam *) (running_context_);
                long long row = 0;
                if (FileManager::GetInstance()->Append(row, method->Buff, method->Size)) {
                    method->Callback(row, method->Buff, method->Size);
                }
                delete method;
            }

            ~MultiWriterFunction() override = default;
        };
    }
}


#endif //LINUX_HOMEWORK_FILE_MANAGER_H

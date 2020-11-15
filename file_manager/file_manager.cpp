//
// Created by Side on 2020/10/22.
//

#include "file_manager.h"

int wang::side::FileManager::max_rows_for_per_file_;
std::string wang::side::FileManager::dms_prefix_;
std::string wang::side::FileManager::dms_suffix_;
wang::thread::Locker wang::side::FileManager::create_instance_lock_;
wang::side::FileManager *wang::side::FileManager::instance_ = nullptr;
int wang::side::FileManager::row_size_ = 0;
wang::side::FileManager::GcManager wang::side::FileManager::gc;
int wang::side::FileManager::auto_increase_file_num_ = 4;
namespace wang {
    namespace side {
        bool FileManager::ExtendLocalFile() {
            long long origin_index = max_file_index_;
            max_file_index_ += auto_increase_file_num_;
            try {
                std::string file_name;
                for (auto i = origin_index + 1; i <= max_file_index_; i++) {
                    file_name = ConstructFileName(i);
                    file_manager_.emplace(file_name, new TableFile(file_name, FileManager::row_size_));
                }
            } catch (std::string &err_msg) {
                std::cout << "write new data. must extend file,but " << err_msg << std::endl;
                return false;
            }
            return true;
        }

        bool FileManager::FindFile(int row, AbstractTableFile **file) {
            if (row > cur_total_rows_) {
                std::cout << "exist current max rows:" << row << " > " << cur_total_rows_ << std::endl;
                return false;
            }
            int index = row / (FileManager::max_rows_for_per_file_ + 1) + 1;
            std::string file_name = ConstructFileName(index);
            try {
                *file = file_manager_.at(file_name);
            } catch (std::out_of_range &e) {
//                std::cout << "dms catch " << e.what() << " when read " << file_name << ",so create file for reader"
//                          << std::endl;
                *file = new TableFile(file_name, FileManager::row_size_);
                file_manager_.emplace(file_name, *file);
            }
            return true;
        }


        FileManager *FileManager::GetInstance() {
            if (instance_ != nullptr) {
                return instance_;
            }
            create_instance_lock_.Lock();
            try {
                if (instance_ == nullptr) {
                    if (FileManager::row_size_ == 0) {
                        throw std::string("init first");
                    }
                    instance_ = new FileManager();
                }
            } catch (std::string &err_msg) {
                create_instance_lock_.UnLock();
                throw err_msg;
            }
            create_instance_lock_.UnLock();
            return instance_;
        }

        bool FileManager::Append(const std::string &msg) {
            AbstractTableFile *file;
            if (!FindCurrentFile(&file)) {
                ExtendLocalFile();
                FindCurrentFile(&file);
            }

            if (file->WriteRow(msg)) {
                IncreaseRow();
            } else {
                return false;
            }
            if (EqualMaxRow()) {
                file->Flush();
            }
            return true;
        }

        bool FileManager::Read(int row, char *buf, int size) {
            AbstractTableFile *file;
            if (!FindFile(row, &file)) {
                return false;
            }
            int cal_row = row % (FileManager::max_rows_for_per_file_);
            cal_row = ((cal_row == 0) ? FileManager::max_rows_for_per_file_ : cal_row); //最后一行

            return file->ReadRow(cal_row, buf, size);
        }

        bool FileManager::FindCurrentFile(AbstractTableFile **file) {
            auto newRow = cur_total_rows_ + 1;
            auto index = newRow / (FileManager::max_rows_for_per_file_ + 1) + 1;
            if (index > max_file_index_) {
                return false;
            }
            std::string file_name = ConstructFileName(index);
            *file = file_manager_.at(file_name);
            return true;
        }

        bool FileManager::MultiWrite(WriterMethodParam *param) {
            auto *func = new MultiWriterFunction(param);
            return worker_pool_->Submit(func);
        }

    }
}

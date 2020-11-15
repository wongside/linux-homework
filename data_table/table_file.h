//
// Created by Side on 2020/10/21.
//

#ifndef LINUX_HOMEWORK_TABLE_FILE_H
#define LINUX_HOMEWORK_TABLE_FILE_H

#include <vector>
#include <cassert>
#include "../file_manager/file.h"

namespace wang {
    namespace side {

        class AbstractTableFile {
        public:
            AbstractTableFile() = default;

            virtual bool WriteRow(const char *buffer, int size) = 0;

            virtual bool WriteRow(const std::string &msg) = 0;

            virtual bool ReadRow(int row_line, char *buffer, int max_size) = 0;

            virtual ~AbstractTableFile() = default;

            virtual void Flush() = 0;
        };

        class TableFile : public AbstractTableFile {
        private:
            static int header_size_;
            static int suffix_size_;
            int row_size_;
            side::File *table_file_ = nullptr;
            int cur_row_ = 0;

            bool WriteSuffix() {
                return table_file_->Write("\n", TableFile::suffix_size_);
            }

        public:
            TableFile(const std::string &name, int row_size) {
                row_size_ = header_size_ + row_size + suffix_size_;
                try {
                    table_file_ = new side::File(name);
                } catch (std::string &err_msg) {
                    throw err_msg;
                }
            }

            ~TableFile() override {
                delete table_file_;
            }

            void Flush() override {
                table_file_->Flush();
            }

            bool WriteRow(const std::string &msg) override {
                int id = cur_row_ + 1;
                char head_buff[TableFile::header_size_];
                memcpy(head_buff, &id, TableFile::header_size_);
                table_file_->Write(head_buff, TableFile::header_size_);
                table_file_->Write(msg.c_str(), msg.length());
                WriteSuffix();
                cur_row_++;
                return true;
            }

            bool WriteRow(const char *buffer, int size) override {
                int id = cur_row_ + 1;
                char head_buff[TableFile::header_size_];
                memcpy(head_buff, &id, TableFile::header_size_);
                //TODO:write fail
                table_file_->Write(head_buff, TableFile::header_size_);
                table_file_->Write(buffer, size);
                WriteSuffix();
                cur_row_++;
                return true;
            }

            bool ReadRow(int row_line, char *buffer, int size) override {
                if (size < row_size_ - header_size_ - suffix_size_) {
                    std::cout << "invalid read parameter : " << size << " < "
                              << row_size_ - header_size_ - suffix_size_ << std::endl;
                    std::abort();
                }
                int offset = row_size_ * (((row_line - 1) > 0) ? row_line - 1 : 0);
                char low_buff[size + TableFile::header_size_ + TableFile::suffix_size_];
                bool ret = table_file_->Read(offset, low_buff, row_size_ - TableFile::header_size_);

                if (ret) {
                    memcpy(buffer, low_buff + TableFile::header_size_, size);
                }
                return ret;
            }
        };

    }
}


#endif //LINUX_HOMEWORK_TABLE_FILE_H
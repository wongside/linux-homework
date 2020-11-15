//
// Created by Side on 2020/10/21.
//

#ifndef LINUX_HOMEWORK_FILE_H
#define LINUX_HOMEWORK_FILE_H

#include <iostream>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

#define MAX_FILE_BUF_SIZE 4096
namespace wang {
    namespace side {
        class File {
        private:
            int open_file_handle_ = -1;
            char buffer_[MAX_FILE_BUF_SIZE] = {};             //暂时采用临时缓存
            int used_ = 0;
            std::string filename_;

            int GetRemain() {
                return MAX_FILE_BUF_SIZE - used_;
            }

            bool NormalWrite(const char *buffer, ssize_t size);

        public:
            explicit File(const std::string &filename) {
                open_file_handle_ = ::open(filename.c_str(),
                                           O_RDWR | O_CREAT | O_APPEND,
                                           S_IRUSR | S_IWUSR | S_IROTH | S_IWOTH);
                if (open_file_handle_ < 0) {
                    throw std::string("open ") + filename + " failed with num " + std::to_string(errno) + ":" +
                          strerror(errno);
                }
                filename_ = filename;
            }

            bool IsEmpty() {
                off_t off = lseek(open_file_handle_, 0, SEEK_SET);
                char ch = EOF;
                Read(0, &ch, 1);
                return ch == EOF;
            }

            bool Write(const char *buffer, int total);

            bool Read(long offset, char *buffer, int total);

            bool ReadAll(char *buffer, int total, ssize_t &real_count);

            void Flush();

            void Close();

            void Clear();

            ~File() {
                Close();
            }
        };
    }
}


#endif //LINUX_HOMEWORK_FILE_H

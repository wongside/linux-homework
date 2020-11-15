//
// Created by Side on 2020/10/21.
//

#include "file.h"

bool wang::side::File::Write(const char *buffer, int total) {
    {
        int remain = GetRemain();
        if (total < remain) {
            memcpy(buffer_ + used_, buffer, total);
            used_ += total;
        } else {
            int offset = remain;
            //先dump 当前buffer到磁盘
            memcpy(buffer_ + used_, buffer, remain);
            NormalWrite(buffer_, MAX_FILE_BUF_SIZE);
            int left = total - remain;
            while (left > MAX_FILE_BUF_SIZE) {
                memcpy(buffer_, buffer + offset, MAX_FILE_BUF_SIZE);
                used_ = MAX_FILE_BUF_SIZE;
                NormalWrite(buffer_, used_);
                left -= MAX_FILE_BUF_SIZE;
                offset += MAX_FILE_BUF_SIZE;
                memset(buffer_, 0, MAX_FILE_BUF_SIZE);
                used_ = 0;
            }
            memcpy(buffer_ + used_, buffer + offset, left);
            used_ += left;
        }
        return true;
    }
}

bool wang::side::File::NormalWrite(const char *buffer, ssize_t size) {
    ssize_t bytes = ::write(open_file_handle_, buffer, size);
    if (bytes < 0) {
        std::cout << "write file " << filename_ << " failed:" << strerror(errno) << std::endl;
        //TODO:错误处理
    }
    memset(buffer_, 0, MAX_FILE_BUF_SIZE);
    used_ = 0;
    return true;
}

void wang::side::File::Flush() {
    if (open_file_handle_ < 0) {
        return;
    }
    NormalWrite(buffer_, used_);
}

void wang::side::File::Close() {
    if (open_file_handle_ > 0) {
        Flush();
        //::fsync(_open_file_handle);
        ::close(open_file_handle_);
    }
}

bool wang::side::File::Read(long offset, char *buffer, int total) {
    off_t off = lseek(open_file_handle_, offset, SEEK_SET);
    if (read(open_file_handle_, buffer, total) < 0) {
        std::cout << "error read:" << strerror(errno) << std::endl;
        return false;
    }
    return true;
}

void wang::side::File::Clear() {
    ftruncate(open_file_handle_, 0);
    lseek(open_file_handle_, 0, SEEK_SET);
}

bool wang::side::File::ReadAll(char *buffer, int total, ssize_t &real_count) {
    off_t off = lseek(open_file_handle_, 0, SEEK_SET);
    real_count = read(open_file_handle_, buffer, total);
    if (real_count < 0) {
        std::cout << "error read:" << strerror(errno) << std::endl;
        return false;
    }
    return true;
}




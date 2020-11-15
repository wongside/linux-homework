//
// Created by Side on 2020/10/22.
//

#ifndef LINUX_HOMEWORK_TABLE_CONFIGURE_H
#define LINUX_HOMEWORK_TABLE_CONFIGURE_H

#include <sstream>
#include <unordered_map>
#include "../file_manager/file.h"


#define MAX_BUF_FILE_SIZE 256
namespace wang {
    namespace side {
        class TableConfigure {
        private:
            std::string file_name_;

            void Dump(int config_row_size, int config_max_rows_for_per_file) {
                wang::side::File file(file_name_);
                file.Clear();
                ssize_t total = 0;
                std::unordered_map<std::string, std::string> data;
                data.emplace("_s_row_size", std::to_string(config_row_size));
                data.emplace("_s_max_rows_for_per_file", std::to_string(config_max_rows_for_per_file));
                std::stringstream ss;
                for (auto &iter:data) {
                    ss << iter.first + ':' + iter.second + '\n';
                }
                total = ss.str().length();
                file.Write(ss.str().c_str(), (int) total);
            }

        public:
            int config_row_size_;
            int config_max_rows_for_per_file_;

            void SetConfigure(int config_row_size, int config_max_rows_for_per_file) {
                config_row_size_ = config_row_size;
                config_max_rows_for_per_file_ = config_max_rows_for_per_file;
            }

            TableConfigure(const std::string &filename = "configure_data.txt") {
                file_name_ = filename;
            }

            void ToStorage() {
                Dump(config_row_size_, config_max_rows_for_per_file_);
            }

            bool FromStorage() {
                try {
                    wang::side::File file(file_name_);
                    if (file.IsEmpty()) {
                        return false;
                    }
                    char buf[MAX_BUF_FILE_SIZE] = {0};
                    ssize_t real = 0;
                    if (!file.ReadAll(buf, MAX_BUF_FILE_SIZE, real)) {
                        return false;
                    }
                    bool isRecordingKey = true;
                    std::string key, value;
                    std::unordered_map<std::string, std::string> data;
                    for (int i = 0; i < real; i++) {
                        auto iter = buf[i];
                        if (iter == '\n') {
                            isRecordingKey = true;
                            data.emplace(key, value);
                            key.clear();
                            value.clear();
                        } else if (iter == ':') {
                            isRecordingKey = false;
                        } else {
                            isRecordingKey ? (key.push_back(iter)) : (value.push_back(iter));
                        }
                    }
                    try {
                        config_row_size_ = std::stoi(data.at("_s_row_size"));
                        config_max_rows_for_per_file_ = std::stoi(data.at("_s_max_rows_for_per_file"));
                    } catch (std::out_of_range &e) {
                        throw std::string("configure file of " + file_name_ + " has error:" + e.what());
                    }
                } catch (std::string &e) {
                    std::cout << __FUNCTION__ << ":" << e << std::endl;
                    return false;
                }
                return true;
            }
        };

        class TableMetaData {
        private:
            std::string file_name_;

            void Dump(long long cur_total_rows) {
                wang::side::File file(file_name_);
                file.Clear();
                ssize_t total = 0;
                std::unordered_map<std::string, std::string> data;
                data.emplace("_cur_total_rows", std::to_string(cur_total_rows));
                std::stringstream ss;
                for (auto &iter:data) {
                    ss << iter.first + ':' + iter.second + '\n';
                }
                total = ss.str().length();
                file.Write(ss.str().c_str(), total);
            }

        public:
            long long cur_total_rows_;

            TableMetaData(const std::string &filename = "meta_data.txt") {
                file_name_ = filename;
            }

            void SetMetaData(long long cur_total_rows) {
                cur_total_rows_ = cur_total_rows;
            }

            void ToStorage() {
                Dump(cur_total_rows_);
            }

            bool FromStorage() {
                try {
                    wang::side::File file(file_name_);
                    if (file.IsEmpty()) {
                        return true;
                    }
                    char buf[MAX_BUF_FILE_SIZE] = {0};
                    ssize_t real = 0;
                    if (!file.ReadAll(buf, MAX_BUF_FILE_SIZE, real)) {
                        return false;
                    }
                    bool isRecordingKey = true;
                    std::string key, value;
                    std::unordered_map<std::string, std::string> data;
                    for (int i = 0; i < real; i++) {
                        auto iter = buf[i];
                        if (iter == '\n') {
                            isRecordingKey = true;
                            data.emplace(key, value);
                            key.clear();
                            value.clear();
                        } else if (iter == ':') {
                            isRecordingKey = false;
                        } else {
                            isRecordingKey ? (key.push_back(iter)) : (value.push_back(iter));
                        }
                    }
                    try {
                        cur_total_rows_ = std::stoll(data.at("_cur_total_rows"));
                    } catch (std::out_of_range &e) {
                        throw std::string("configure file of " + file_name_ + " has error:" + e.what());
                    }
                } catch (std::string &e) {
                    std::cout << __FUNCTION__ << ":" << e << std::endl;
                    return false;
                }
                return true;
            }
        };
    }
}


#endif //LINUX_HOMEWORK_TABLE_CONFIGURE_H

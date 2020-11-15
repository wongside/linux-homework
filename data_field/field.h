//
// Created by Side on 2020/10/23.
//

#ifndef LINUX_HOMEWORK_FIELD_H
#define LINUX_HOMEWORK_FIELD_H

#include <vector>
#include <iostream>
#include <unordered_map>
#include <string>
#include <cassert>
#include <functional>
#include <cstring>

namespace wang {
    namespace side {
        enum class FieldType {
            Integer64,
            Double,
            Char
        };

        class AbstractField {
        public:
            virtual void ToCharBuffer(char *buf) = 0;

            virtual void FromCharBuffer(const char *buf) = 0;

            virtual int32_t GetBufferSize() = 0;

            virtual void Print() = 0;

            virtual ~AbstractField() = default;
        };

        template<typename T, typename std::enable_if<std::is_pod<T>::value> * = nullptr>
        class DataField : public AbstractField {
        private:
            T data_;
        public:
            explicit DataField(T data) {
                data_ = data;
            }

            ~DataField() override = default;

            DataField(const DataField &o) {
                data_ = o.data_;
            }

            void ToCharBuffer(char *buf) override {
                memcpy((char *) (buf), &data_, GetBufferSize());
            }

            void FromCharBuffer(const char *buf) override {
                memcpy(&data_, buf, GetBufferSize());
            }

            int32_t GetBufferSize() override {
                return sizeof(T);
            }

            void Print() override {
                std::cout << "Size = " << GetBufferSize() << "\tData is : " << data_ << std::endl;
            }
        };

        class FieldProduceFactory {
        public:
            template<typename T>
            AbstractField *CreateField(const T &attr) {
                return new DataField<T>(attr);
            }
        };

        class UserAttrVector : public AbstractField {
        private:
            static std::unordered_map<FieldType, size_t> s_type_size_map_;
            std::vector<AbstractField *> user_attr_vector_;
            std::vector<FieldType> user_attr_type_;
            int total_size_ = 0;
        public:
            explicit UserAttrVector(const std::vector<FieldType> &type) {
                user_attr_type_.reserve(type.size());
                for (auto &iter:type) {
                    user_attr_type_.push_back(iter);
                    total_size_ += UserAttrVector::s_type_size_map_[iter];
                }
            }

            UserAttrVector(const UserAttrVector &) = delete;

            UserAttrVector &operator=(const UserAttrVector &) = delete;

            UserAttrVector(UserAttrVector &&o) noexcept {
                for (auto &iter:o.user_attr_vector_) {
                    o.PushBack(iter);
                }
            }

            ~UserAttrVector() override {
                Clear();
            }

            void Clear() {
                for (auto &iter:user_attr_vector_) {
                    if (iter != nullptr) {
                        delete iter;
                        iter = nullptr;
                    }
                }
            }

            void PushBack(AbstractField *p) {
                user_attr_vector_.push_back(p);
            }

            void ToCharBuffer(char *buf) override {
                int offset = 0;
                for (auto &iter:user_attr_vector_) {
                    iter->ToCharBuffer(buf + offset);
                    offset += iter->GetBufferSize();
                }
            }

            void FromCharBuffer(const char *buf) override {
                Clear();
                int offset = 0;
                FieldProduceFactory factory;
                user_attr_vector_.reserve(user_attr_type_.size());
                for (auto &iter:user_attr_type_) {
                    if (iter == FieldType::Integer64) {
                        auto *p = factory.CreateField<int>(0);
                        p->FromCharBuffer(buf + offset);
                        offset += p->GetBufferSize();
                        user_attr_vector_.push_back(p);
                    }
                }
            }

            int32_t GetBufferSize() override {
                if (total_size_ == 0) {
                    int total = 0;
                    for (auto &iter:user_attr_type_) {
                        total += UserAttrVector::s_type_size_map_[iter];
                    }
                    total_size_ = total;
                }
                return total_size_;
            }

            void Print() override {
//                std::cout << "user attr vector info:" << " size:" << GetBufferSize() << std::endl;
                int i = 0;
                for (auto &iter:user_attr_vector_) {
                    if (iter != nullptr) {
                        iter->Print();
                    }
                    if (i < 10) {
                        i++;
                    } else {
                        std::cout << "Too much data so only the first 10 items are displayed" << std::endl;
                        break;
                    }

                }

            }
        };
    }
}


#endif //LINUX_HOMEWORK_FIELD_H

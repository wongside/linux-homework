//
// Created by Side on 2020/10/21.
//

#include "table_file.h"
//test meta program
using t_int_0_type = std::integral_constant<int, 0>;
using t_int_1_type = std::integral_constant<int, 1>;
using t_int_2_type = std::integral_constant<int, 2>;
//前缀为4字节长度
int wang::side::TableFile::header_size_ = t_int_0_type::value;
//后缀添加了换行符号，在windows上运行时，\n会变成 \n\r 所以后缀为2个字符，在linux上为\n
int wang::side::TableFile::suffix_size_ = t_int_1_type::value;
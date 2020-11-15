//
// Created by Side on 2020/10/23.
//

#include "field.h"
std::unordered_map<wang::side::FieldType,size_t> wang::side::UserAttrVector::s_type_size_map_ =
        {
                {FieldType::Integer64, sizeof(int)},
                {FieldType::Char,      sizeof(char)},
                {FieldType::Double,    sizeof(double)}
        };

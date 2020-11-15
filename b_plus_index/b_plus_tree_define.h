#pragma once

#include <iostream>

static const int MIN_DEGREE = 2;
static const int LOWER_BOUND_KEY_NUM = (MIN_DEGREE - 1);
static const int UPPER_BOUND_KEY_NUM = ((MIN_DEGREE * 2) - 1);
static const int LOWER_BOUND_CHILD_NUM = (LOWER_BOUND_KEY_NUM + 1);
static const int UPPER_BOUND_CHILD_NUM = (UPPER_BOUND_KEY_NUM + 1);

enum NodeType {
    INTERNAL, LEAF
};

enum SiblingDirection {
    LEFT, RIGHT
};

enum class SelectType {
    EQ = 0,
    GT,
    LT,
    GE,
    LE
};

using KeyType = int64_t;
using DataType = int64_t;
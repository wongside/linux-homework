#pragma once

#include <iostream>
#include "b_plus_tree.h"
#include "b_plus_tree_define.h"
#include "b_plus_tree_node.h"
#include <vector>

class DataIndex {
public:
    DataIndex() = default;

    ~DataIndex() = default;

    void CreateIndex(std::vector<int64_t> date_ptr, std::vector<int> row);

private:
    BPlusTree b_plus_tree_;
};


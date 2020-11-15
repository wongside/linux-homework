#include "data_index.h"

#include <algorithm>
#include <cassert>

void DataIndex::CreateIndex(std::vector<int64_t> date_ptr, std::vector<int> row) {
    assert(date_ptr.size() == row.size());
    int count = date_ptr.size();
    if (date_ptr.empty() || row.size() != date_ptr.size()) {
        return;
    }
    for (int i = 0; i < count; i++) {
        b_plus_tree_.Insert(date_ptr[i], row[i]);
    }
}

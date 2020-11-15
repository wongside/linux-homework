#pragma once

#include "b_plus_tree_define.h"
#include "b_plus_tree_node.h"
#include <vector>

struct SearchResult {
    int KeyIndex;
    BPlusTreeLeafNode *TargetNode;

    SearchResult() {
        KeyIndex = -1;
        TargetNode = nullptr;
    }
};

class BPlusTree {

public:
    BPlusTree() {
        root_ = nullptr;
        data_head_ = nullptr;
    };

    ~BPlusTree() = default;

    bool Insert(KeyType key, const DataType &data);

    bool Select(KeyType lower_bound, KeyType upper_bound,
                bool lower_contain, bool upper_contain,
                std::vector<DataType> &select_result);

    bool Select(KeyType key, SelectType select_type, std::vector<DataType> &select_result);

private:

    void AdjustRoot();

    static bool Check(BPlusTreeNode *node, KeyType key);

    static bool InsertToNode(BPlusTreeNode *node, KeyType key, const DataType &data);

    static void Search(BPlusTreeNode *node, KeyType key, SearchResult &result);

private:
    BPlusTreeNode *root_;
    BPlusTreeLeafNode *data_head_;
    KeyType max_key_{};
};


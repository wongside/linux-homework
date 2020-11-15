#include "b_plus_tree_node.h"

int BPlusTreeNode::GetKeyIndex(KeyType key) const {
    int lower = 0;
    int upper = key_num - 1;

    while (lower < upper) {
        int cmpIndex = (lower + upper) / 2;
        if (key > key_values[cmpIndex]) {
            lower = cmpIndex + 1;
        } else {
            upper = cmpIndex;
        }
    }
    while (lower < key_num - 1 && lower < UPPER_BOUND_KEY_NUM && (key_values[lower] == key_values[lower + 1])) {
        lower = lower + 1;
    }
    return lower;
}

int BPlusTreeInnerNode::GetChildIndex(KeyType key, int key_index) const {
    if (key >= GetKeyValue(key_index)) {
        return key_index + 1;
    } else {
        return key_index;
    }
}


void BPlusTreeLeafNode::Insert(KeyType key, const DataType &data) {
    int i;
    for (i = key_num; i >= 1 && key_values[i - 1] > key; --i) {
        SetKeyValue(i, key_values[i - 1]);
        SetData(i, data_[i - 1]);
    }
    SetKeyValue(i, key);
    SetData(i, data);
    SetKeyNum(key_num + 1);

    if (GetKeyNum() > UPPER_BOUND_KEY_NUM) {
        BPlusTreeNode *parent_node = GetParentNode();
        if (parent_node != nullptr) {
            int key_index = parent_node->GetKeyIndex(key);
            int child_index = parent_node->GetChildIndex(key, key_index);
            Split(parent_node, child_index);

        } else {
            auto *inner_node = new BPlusTreeInnerNode();
            inner_node->SetChild(0, this);
            Split(inner_node, 0);
            SetParentNode(inner_node);
        }
    }
}

void BPlusTreeInnerNode::Insert(int key_index, int child_index, KeyType key, BPlusTreeNode *child_node) {
    int i;
    for (i = GetKeyNum(); i > key_index; --i) {
        SetChild(i + 1, children[i]);
        SetKeyValue(i, key_values[i - 1]);
    }
    if (i == child_index) {
        SetChild(i + 1, children[i]);
    }
    SetChild(child_index, child_node);
    SetKeyValue(key_index, key);
    SetKeyNum(key_num + 1);

    if (GetKeyNum() > UPPER_BOUND_KEY_NUM) {
        BPlusTreeNode *parent_node = GetParentNode();
        if (parent_node != nullptr) {
            key_index = parent_node->GetKeyIndex(key);
            child_index = parent_node->GetChildIndex(key, key_index);
            Split(parent_node, child_index);
        } else {
            auto *inner_node = new BPlusTreeInnerNode();
            inner_node->SetChild(0, this);
            Split(inner_node, 0);
            SetParentNode(inner_node);
        }
    }
}

void BPlusTreeLeafNode::Split(BPlusTreeNode *parent_node, int child_index) {
    auto *new_node = new BPlusTreeLeafNode();
    new_node->SetParentNode(parent_node);
    SetKeyNum(LOWER_BOUND_KEY_NUM + 1);
    new_node->SetKeyNum(LOWER_BOUND_KEY_NUM + 1);
    BPlusTreeLeafNode *rightNode = GetRightSibling();
    new_node->SetRightSibling(rightNode);
    SetRightSibling(new_node);
    new_node->SetLeftSibling(this);
    if (rightNode != nullptr) {
        rightNode->SetLeftSibling(new_node);
    }

    for (int i = 0; i < LOWER_BOUND_KEY_NUM + 1; ++i) {
        new_node->SetKeyValue(i, key_values[i + LOWER_BOUND_KEY_NUM + 1]);
    }
    for (int i = 0; i < LOWER_BOUND_KEY_NUM + 1; ++i) {
        new_node->SetData(i, data_[i + LOWER_BOUND_KEY_NUM + 1]);
    }
    ((BPlusTreeInnerNode *) parent_node)->Insert(child_index,
                                                 child_index + 1,
                                                 key_values[LOWER_BOUND_KEY_NUM + 1],
                                                 new_node);
}

void BPlusTreeInnerNode::Split(BPlusTreeNode *parent_node, int child_index) {
    auto *new_node = new BPlusTreeInnerNode();
    new_node->SetParentNode(parent_node);
    new_node->SetKeyNum(LOWER_BOUND_KEY_NUM + 1);
    int i;
    for (i = 0; i < LOWER_BOUND_KEY_NUM + 1; ++i) {
        new_node->SetKeyValue(i, key_values[i + LOWER_BOUND_KEY_NUM + 1]);
    }
    for (i = 0; i < LOWER_BOUND_CHILD_NUM + 1; ++i) {
        new_node->SetChild(i, children[i + LOWER_BOUND_CHILD_NUM]);
    }
    SetKeyNum(LOWER_BOUND_KEY_NUM);
    ((BPlusTreeInnerNode *) parent_node)->Insert(child_index,
                                                 child_index + 1,
                                                 key_values[LOWER_BOUND_KEY_NUM],
                                                 new_node);
}


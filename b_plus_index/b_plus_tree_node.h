#pragma once

#include <iostream>
#include "b_plus_tree_define.h"

class   BPlusTreeNode {
public:
    BPlusTreeNode() {
        SetKeyNum(0);
        SetParentNode(nullptr);
    }

    virtual ~BPlusTreeNode() {
        SetKeyNum(0);
    }

    bool GetIsLeaf() const {
        return is_leaf_;
    }

    void SetIsLeaf(bool isLeaf) {
        is_leaf_ = isLeaf;
    }

    int GetKeyNum() const {
        return key_num;
    }

    void SetKeyNum(int n) {
        key_num = n;
    }

    KeyType GetKeyValue(int i) const {
        return key_values[i];
    }

    void SetKeyValue(int i, KeyType key) {
        key_values[i] = key;
    }

    void SetParentNode(BPlusTreeNode *parent) {
        parent_node_ = parent;
    }

    BPlusTreeNode *GetParentNode() const {
        return parent_node_;
    }

    int GetKeyIndex(KeyType key) const;

    virtual void Split(BPlusTreeNode *parent_node, int child_index) = 0;

    virtual void Clear() = 0;

    virtual int GetChildIndex(KeyType key, int key_index) const = 0;

protected:
    BPlusTreeNode *parent_node_{};
    bool is_leaf_{};
    int key_num{};
    KeyType key_values[UPPER_BOUND_KEY_NUM + 1]{};
};

class BPlusTreeLeafNode : public BPlusTreeNode {
public:
    BPlusTreeLeafNode() : BPlusTreeNode() {
        SetIsLeaf(true);
        SetLeftSibling(nullptr);
        SetRightSibling(nullptr);
    };

    ~BPlusTreeLeafNode() override = default;

    void Insert(KeyType key, const DataType &data);

    DataType GetData(int i) {
        return data_[i];
    }

    void SetData(int i, DataType data) {
        data_[i] = data;
    }

    BPlusTreeLeafNode *GetRightSibling() {
        return right_sibling_;
    }

    void SetRightSibling(BPlusTreeLeafNode *right) {
        right_sibling_ = right;
    }

    BPlusTreeLeafNode *GetLeftSibling() {
        return left_sibling_;
    }

    void SetLeftSibling(BPlusTreeLeafNode *left) {
        left_sibling_ = left;
    }

    void Split(BPlusTreeNode *parent_node, int child_index) override;

    void Clear() override {};

    int GetChildIndex(KeyType key, int keyIndex) const override {
        return keyIndex;
    };

private:
    BPlusTreeLeafNode *left_sibling_{};
    BPlusTreeLeafNode *right_sibling_{};
    DataType data_[UPPER_BOUND_KEY_NUM + 1]{};
};

class BPlusTreeInnerNode : public BPlusTreeNode {
public:
    BPlusTreeInnerNode() : BPlusTreeNode() {
        SetIsLeaf(false);

    }

    ~BPlusTreeInnerNode() override = default;

    BPlusTreeNode *GetChild(int i) const {
        return children[i];
    }

    void SetChild(int i, BPlusTreeNode *child) {
        children[i] = child;
    }

    void Insert(int key_index, int child_index, KeyType key, BPlusTreeNode *child_node);

    void Split(BPlusTreeNode *parent_node, int child_index) override;

    void Clear() override {};

    int GetChildIndex(KeyType key, int key_index) const override;

private:
    BPlusTreeNode *children[UPPER_BOUND_CHILD_NUM + 1]{};
};
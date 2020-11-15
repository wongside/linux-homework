#include "b_plus_tree.h"
#include <map>
#include <algorithm>

std::map<SelectType, int> select_dispatcher;

void BPlusTree::AdjustRoot() {
    BPlusTreeNode *new_root = root_->GetParentNode();
    while (new_root != nullptr) {
        root_ = new_root;
        new_root = root_->GetParentNode();
    }
}

bool BPlusTree::Check(BPlusTreeNode *node, KeyType key) {
    if (!node) {
        return false;
    }
    int key_index = node->GetKeyIndex(key);
    int child_index = node->GetChildIndex(key, key_index);
    if (key_index < node->GetKeyNum() && key == node->GetKeyValue(key_index)) {
        return true;
    }
    if (node->GetIsLeaf()) {
        return false;
    } else {
        return Check(((BPlusTreeInnerNode *) node)->GetChild(child_index), key);
    }
}

bool BPlusTree::Insert(KeyType key, const DataType &data) {

    SearchResult search_result;
    Search(root_, key, search_result);

    BPlusTreeLeafNode *child_node = search_result.TargetNode;
    if (search_result.TargetNode != nullptr) {
        child_node->Insert(key, data);

        if (key > max_key_) {
            max_key_ = key;
        }
    } else {
        root_ = new BPlusTreeLeafNode();
        data_head_ = (BPlusTreeLeafNode *) root_;
        max_key_ = key;
        ((BPlusTreeLeafNode *) root_)->Insert(key, data);
    }
    AdjustRoot();
    return true;
}

bool BPlusTree::InsertToNode(BPlusTreeNode *node, KeyType key, const DataType &data) {
    if (node == nullptr) {
        return false;
    }
    if (node->GetIsLeaf()) {
        ((BPlusTreeLeafNode *) node)->Insert(key, data);
    } else {
        BPlusTreeInnerNode *inner_node = ((BPlusTreeInnerNode *) node);
        int key_index = inner_node->GetKeyIndex(key);
        int child_index = inner_node->GetChildIndex(key, key_index); // 孩子结点指针索引
        BPlusTreeNode *child_node = inner_node->GetChild(child_index);
        if (child_node->GetKeyNum() >= UPPER_BOUND_KEY_NUM) {
            child_node->Split(node, child_index);
            if (node->GetKeyValue(key_index) <= key)   // 分割后重新确定目标子结点
            {
                child_node = inner_node->GetChild(child_index + 1);
            }
        }
        InsertToNode(child_node, key, data);
    }
    return true;
}

bool BPlusTree::Select(KeyType lower_bound, KeyType upper_bound, bool lower_contain, bool upper_contain,
                       std::vector<DataType> &select_result) {
    if (lower_bound > upper_bound) {
        return false;
    }
    SearchResult beginR, finalR;
    Search(root_, lower_bound, beginR);
    Search(root_, upper_bound, finalR);
    BPlusTreeLeafNode *begin_node = beginR.TargetNode, *finalNode = finalR.TargetNode;
    int begin_index = beginR.KeyIndex, finalIndex = finalR.KeyIndex;
    if (beginR.TargetNode->GetKeyValue(beginR.KeyIndex) < lower_bound ||
        (beginR.TargetNode->GetKeyValue(beginR.KeyIndex) == lower_bound && !lower_contain)) {
        begin_index += 1;
    }
    if (finalR.TargetNode->GetKeyValue(finalR.KeyIndex) > upper_bound ||
        (finalR.TargetNode->GetKeyValue(finalR.KeyIndex) == upper_bound && !upper_contain)) {
        finalIndex -= 1;
    }
    if (begin_node == finalNode) {
        for (int i = begin_index; i <= finalIndex; ++i) {
            select_result.push_back(begin_node->GetData(i));
        }
    } else {
        for (BPlusTreeLeafNode *pLeaf = begin_node; pLeaf != finalNode; pLeaf = pLeaf->GetRightSibling()) {
            for (int i = begin_index; i < pLeaf->GetKeyNum(); ++i) {
                select_result.push_back(pLeaf->GetData(i));
            }
            begin_index = 0;
        }
        for (int i = 0; i <= finalIndex; ++i) {
            select_result.push_back(finalNode->GetData(i));
        }
    }
    std::sort<std::vector<DataType>::iterator>(select_result.begin(), select_result.end());
    return true;
}

bool BPlusTree::Select(KeyType key, SelectType select_type, std::vector<DataType> &select_result) {
    if (root_ == nullptr) {
        return false;
    }

    switch (select_type) {
        case SelectType::EQ:
            if (key > max_key_ || key < data_head_->GetKeyValue(0)) {
                return false;
            } else {
                SearchResult searchResult;
                Search(root_, key, searchResult);
                if (searchResult.TargetNode->GetKeyValue(searchResult.KeyIndex) == key) {
                    select_result.push_back(searchResult.TargetNode->GetData(searchResult.KeyIndex));
                }
            }
            break;
        case SelectType::GT:
        case SelectType::GE: {
            BPlusTreeLeafNode *beginNode = data_head_;
            int beginIndex = 0;
            if (key >= data_head_->GetKeyValue(0)) {
                SearchResult searchResult;
                Search(root_, key, searchResult);
                beginNode = searchResult.TargetNode;
                beginIndex = searchResult.KeyIndex;
                if (beginNode->GetKeyValue(beginIndex) < key ||
                    (beginNode->GetKeyValue(beginIndex) == key && select_type == SelectType::GT)) {
                    beginIndex += 1;
                }
            }

            for (BPlusTreeLeafNode *pLeaf = beginNode; pLeaf != nullptr; pLeaf = pLeaf->GetRightSibling()) {
                for (int i = beginIndex; i < pLeaf->GetKeyNum(); ++i) {
                    select_result.push_back(pLeaf->GetData(i));
                }
                beginIndex = 0;
            }
            break;
        }
        case SelectType::LT:
        case SelectType::LE: {
            BPlusTreeLeafNode *finalNode = nullptr;
            int finalIndex = -1;
            if (key <= max_key_) {
                SearchResult searchResult;
                Search(root_, key, searchResult);
                finalNode = searchResult.TargetNode;
                finalIndex = searchResult.KeyIndex;
                if (finalNode->GetKeyValue(finalIndex) > key ||
                    (finalNode->GetKeyValue(finalIndex) == key && select_type == SelectType::LT)) {
                    finalIndex -= 1;
                }
            }
            for (BPlusTreeLeafNode *pLeaf = data_head_; pLeaf != finalNode; pLeaf = pLeaf->GetRightSibling()) {
                for (int i = 0; i < pLeaf->GetKeyNum(); ++i) {
                    select_result.push_back(pLeaf->GetData(i));
                }
            }
            if (finalNode != nullptr) {
                for (int i = 0; i <= finalIndex; i++) {
                    select_result.push_back(finalNode->GetData(i));
                }
            }
            break;
        }
        default:
            break;
    }
    std::sort<std::vector<DataType>::iterator>(select_result.begin(), select_result.end());

    return true;
}

void BPlusTree::Search(BPlusTreeNode *node, KeyType key, SearchResult &result) {
    if (node == nullptr) {
        return;
    }
    int key_index = node->GetKeyIndex(key);
    int child_index = node->GetChildIndex(key, key_index);
    if (node->GetIsLeaf()) {
        result.KeyIndex = key_index;
        result.TargetNode = (BPlusTreeLeafNode *) node;
        return;
    } else {
        return Search(((BPlusTreeInnerNode *) node)->GetChild(child_index), key, result);
    }
}
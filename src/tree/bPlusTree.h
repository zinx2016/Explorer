#ifndef _EXPLORER_BPLUSTREE_H_
#define _EXPLORER_BPLUSTREE_H_

#include <stdio.h>

#include "bPlusTreeBase.h"

namespace Explorer {

#define OFFSET_META 0
#define OFFSET_BLOCK (OFFSET_META + sizeof(meta))
#define SIZE_NO_CHILDREN (sizeof(leafNode) - ORDER * sizeof(record))

// 元数据
typedef struct {
        size_t order;                           // B+树的阶数
        size_t valueSize;                       // 值大小
        size_t keySize;                         // 键大小
        size_t internalNodeNum;                 // 内部节点数目
        size_t leafNodeNum;                     // 叶节点数目
        size_t height;                          // 树高（不包括叶节点）
        off_t slot;                             // 索引文件中分配新节点的起始偏移位置
        off_t rootOffset;                       // 根节点位置偏移量
        off_t leafOffset;                       // 第一个叶子节点的位置偏移量
} meta;

// 内部节点的索引字段
struct index {
        keyType key;                            // 节点存储的关键字，即键
        off_t child;                            // 子节点在索引文件中的偏移量
};

// 内部节点
struct internalNode {
        typedef index* child;
        off_t parent;                           // 父节点
        off_t next;                             // 下一个内部节点
        off_t prev;                             // 前一个内部节点
        size_t n;                               // 子节点数目
        index children[ORDER];                  // 存储每个子节点位置偏移量及关键字
};

// 叶子节点中的记录
struct record {
        keyType key;
        valueType value;
};

// 叶节点
struct leafNode {
        typedef record* child;
        off_t parent;
        off_t next;
        off_t prev;
        size_t n;                               // 叶节点中记录数目
        record children[ORDER];
};

class BPlusTree
{
public:
        BPlusTree(const char* path, bool empty = false);

        // 成功：0；键已存在：-1
        int Insert(const keyType& key, valueType value);
        // 成功：0；键未找到：-1
        int Remove(const keyType& key);
        // 成功：0；出错：-1
        int Update(const keyType& key, valueType value);
        // 成功：0；未找到：-1
        int Search(const keyType& key, valueType* value) const;
        // 成功：查找到的值数目；参数错误：-1
        int SearchRange(keyType* left, const keyType& right,
                        valueType* values, size_t max, bool *next = NULL) const;
        // 元数据
        meta GetMeta() const;

private:
        char path_[512];                        // 索引文件路径
        meta meta_;
        mutable FILE* fp_;                      // 索引文件指针
        mutable int fpLevel_;                   // 索引文件的引用计数

private:
        // 打开/关闭索引文件
        void openFile(const char* mode = "rb+") const;
        void closeFile() const;

        // 从磁盘读入节点
        template <typename T>
        int map(T* block, off_t offset) const;
        int map(void* block, off_t offset, size_t size) const;

        // 向磁盘写回节点
        template <typename T>
        int unmap(T* block, off_t offset) const;
        int unmap(void* block, off_t offset, size_t size) const;

        // 创建新节点
        template <typename T>
        void nodeCreate(off_t offset, T* node, T* next);

        // 销毁节点
        template <typename T>
        void nodeRemove(T* prev, T* node);

        // 分配/撤销节点
        off_t alloc(size_t size);
        off_t alloc(leafNode* leaf);
        off_t alloc(internalNode* node);
        void unalloc(leafNode* leaf, off_t offset);
        void unalloc(internalNode* leaf, off_t offset);

        // 初始化索引文件存储结构
        void initFromEmpty();

        // 返回键所在叶子节点的父节点偏移量
        off_t searchIndex(const keyType& key) const;

        // 返回键所在叶子节点的偏移量
        off_t searchLeaf(const keyType& key) const;

        // 返回键所在叶子节点的偏移量
        off_t searchLeaf(off_t parent, const keyType& key) const;


        // 不用分裂叶子节点的将key-value插入到叶子节点中适当位置
        // 移动插入点后元素，插入后保持记录有序
        void insertRecordNoSplit(leafNode* leaf,
                                 const keyType& key,
                                 const valueType& value);


        // 分裂的新节点更新父节点索引信息
        void insertKeyToIndex(off_t offset, const keyType& key,
                              off_t old, off_t after);

        // 不用分裂父节点的将分裂的新叶子节点的首个key插入父节点中
        void insertKeyToIndexNoSplit(internalNode& node,
                                     const keyType& key,
                                     off_t value);

        // 设置内部索引节点的父节点
        void resetIndexChildrenParent(index* begin, index* end, off_t parent);

        void removeFromIndex(off_t offset, internalNode& node, const keyType& key);

        void mergeLeafs(leafNode* left, leafNode* right);

        void mergeKeys(index* where, internalNode& node, internalNode& next);

        bool borrowKey(bool fromRight, leafNode& borrower);

        bool borrowKey(bool fromRight, internalNode& borrower, off_t offset);

        void changeParentChild(off_t parent, const keyType& o, const keyType& n);
};

template <typename T>
int BPlusTree::map(T* block, off_t offset) const
{
        return map(block, offset, sizeof(T));
}

template <typename T>
int BPlusTree::unmap(T* block, off_t offset) const
{
        return unmap(block, offset, sizeof(T));
}

template <typename T>
void BPlusTree::nodeCreate(off_t offset, T* node, T* next)
{
        next->parent = node->parent;
        next->next = node->next;
        next->prev = offset;
        node->next = alloc(next);
        if (next->next != 0)
        {
                T oldNext;
                map(&oldNext, next->next, SIZE_NO_CHILDREN);
                oldNext.prev = node->next;
                unmap(&oldNext, next->next, SIZE_NO_CHILDREN);
        }
        unmap(&meta_, OFFSET_META);
}

template <typename T>
void BPlusTree::nodeRemove(T* prev, T* node)
{
        unalloc(node, prev->next);
        prev->next = node->next;
        if (node->next != 0)
        {
                T next;
                map(&next, node->next, SIZE_NO_CHILDREN);
                next.prev = node->prev;
                unmap(&next, node->next, SIZE_NO_CHILDREN);
        }
        unmap(&meta_, OFFSET_META);
}

} // namespace Explorer

#endif // _EXPLORER_BPLUSTREE_H_

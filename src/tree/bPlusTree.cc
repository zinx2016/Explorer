#include <algorithm>

#include "bPlusTree.h"

using std::upper_bound;
using std::lower_bound;
using std::binary_search;

namespace Explorer {

// 在节点对半查找时需要定义比较运算符
// 比较索引/记录与键的大小
OPERATOR_KEYCMP(index)
OPERATOR_KEYCMP(record)

// 节点首关键字位置
template <typename T>
inline typename T::child
begin(T& node)
{
        return node.children;
}

// 节点尾关键字位置
template <typename T>
inline typename T::child
end(T& node)
{
        return node.children + node.n;
}

inline index* find(internalNode& node, const keyType& key)
{
        return upper_bound(begin(node), end(node) - 1, key);
}

inline record* find(leafNode& node, const keyType& key)
{
        return lower_bound(begin(node), end(node), key);
}

BPlusTree::BPlusTree(const char* path, bool empty)
        : fp_(NULL),
          fpLevel_(0)
{
        bzero(path_, sizeof(path_));
        strcpy(path_, path);

        if (!empty)
        {
                // 第一次之后使用时才成功
                if (map(&meta_, OFFSET_META) != 0)
                        empty = true;
        }

        if (empty)
        {
                // 第一次使用时调用，以初始化
                openFile("w+");
                initFromEmpty();
                closeFile();
        }
}

int BPlusTree::Insert(const keyType& key, valueType value)
{
        off_t parent = searchIndex(key);
        off_t offset = searchLeaf(parent, key);
        leafNode leaf;
        map(&leaf, offset);

        // 叶子节点中查找键
        if (binary_search(begin(leaf), end(leaf), key)) // 若键已存在
                return -1;

        if (leaf.n == meta_.order) // 叶子节点已满，分裂
        {
                // 创建新叶子节点
                leafNode newLeaf;
                nodeCreate(offset, &leaf, &newLeaf);

                size_t point =  leaf.n / 2;
                bool placeRight = keyComp(key, leaf.children[point].key) > 0;
                if (placeRight)
                        ++point;

                // 分裂
                std::copy(leaf.children + point, leaf.children + leaf.n, newLeaf.children);
                newLeaf.n = leaf.n - point;
                leaf.n = point;

                // 插入key-value对
                if (placeRight)
                        insertRecordNoSplit(&newLeaf, key, value);
                else
                        insertRecordNoSplit(&leaf, key, value);

                // 写入磁盘
                unmap(&leaf, offset);
                unmap(&newLeaf, leaf.next);

                // 更新父节点
                insertKeyToIndex(parent, newLeaf.children[0].key, offset, leaf.next);
        }
        else
        {
                // 插入key-value对并写入磁盘
                insertRecordNoSplit(&leaf, key, value);
                unmap(&leaf, offset);
        }
        return 0;
}

int BPlusTree::Remove(const keyType& key)
{
        internalNode parent;
        leafNode leaf;

        off_t parentOff = searchIndex(key);
        map(&parent, parentOff);

        index* where = find(parent, key);
        off_t offset = where->child;
        map(&leaf, offset);

        if (!binary_search(begin(leaf), end(leaf), key))
                return -1;

        size_t minN = meta_.leafNodeNum == 1 ? 0 : meta_.order / 2;
        assert(leaf.n >= minN && leaf.n <= meta_.order);

        record* toDelete = find(leaf, key);
        std::copy(toDelete + 1, end(leaf), toDelete);
        leaf.n--;

        // 借键或合并叶子节点
        if (leaf.n < minN)
        {
                bool borrowed = false;
                // 从左节点借
                if (leaf.prev != 0)
                        borrowed = borrowKey(false, leaf);
                // 从右节点借
                if (!borrowed && leaf.next != 0)
                        borrowed = borrowKey(true, leaf);
                // 左右节点都无可借
                if (!borrowed)
                {
                        assert(leaf.next != 0 || leaf.prev != 0);
                        keyType indexKey;
                        if (where == end(parent) - 1)
                        {
                                // 与左节点合并
                                assert(leaf.prev != 0);
                                leafNode prev;
                                map(&prev, leaf.prev);
                                indexKey = begin(prev)->key;

                                mergeLeafs(&prev, &leaf);
                                nodeRemove(&prev, &leaf);
                                unmap(&prev, leaf.prev);
                        }
                        else
                        {
                                // 与右节点合并
                                assert(leaf.next != 0);
                                leafNode next;
                                map(&next, leaf.next);
                                indexKey = begin(leaf)->key;

                                mergeLeafs(&leaf, &next);
                                nodeRemove(&leaf, &next);
                                unmap(&leaf, offset);
                        }
                        removeFromIndex(parentOff, parent, indexKey);
                }
                else
                        unmap(&leaf, offset);
        }
        else
                unmap(&leaf, offset);
        return 0;
}

int BPlusTree::Update(const keyType& key, valueType value)
{
        off_t offset = searchLeaf(key);
        leafNode leaf;
        map(&leaf, offset);

        record *rc = find(leaf, key);
        if (rc != leaf.children + leaf.n)
        {
                if (keyComp(key, rc->key) == 0)
                {
                        rc->value = value;
                        unmap(&leaf, offset);
                        return 0;
                }
        }
        else
                return -1;
}

int BPlusTree::Search(const keyType& key, valueType* value) const
{
        leafNode leaf;
        map(&leaf, searchLeaf(key));
        record *rc = find(leaf, key);
        if (rc != leaf.children + leaf.n)
        {
                *value = rc->value;
                return keyComp(rc->key, key);
        }
        else
                return -1;
}

int BPlusTree::SearchRange(keyType* left, const keyType& right,
                        valueType* values, size_t max, bool *next) const
{
        if (left == NULL || keyComp(*left, right) > 0)
                return -1;

        off_t offLeft = searchLeaf(*left);
        off_t offRight = searchLeaf(right);
        off_t off = offLeft;
        size_t i = 0;
        record *b, *e;

        leafNode leaf;
        while (off != offRight && off != 0 && i < max)
        {
                map(&leaf, off);
                if (offLeft == off)
                        b = find(leaf, *left);
                else
                        b = begin(leaf);
                e = leaf.children + leaf.n;
                for (; b != e && i < max; ++b, ++i)
                        values[i] = b->value;
                off = leaf.next;
        }

        if (i < max)
        {
                map(&leaf, offRight);
                b = find(leaf, *left);
                e = upper_bound(begin(leaf), end(leaf), right);
                for (; b != e && i < max; ++b, ++i)
                        values[i] = b->value;
        }

        if (next != NULL)
        {
                if (i == max && b != e)
                {
                        *next = true;
                        *left = b->key;
                }
                else
                        *next = false;
        }

        return i;
}

meta BPlusTree::GetMeta() const
{
        return meta_;
}

void BPlusTree::initFromEmpty()
{
        // 初始化元数据
        bzero(&meta_, sizeof(meta_));
        meta_.order = ORDER;
        meta_.valueSize = sizeof(valueType);
        meta_.keySize = sizeof(keyType);
        meta_.height = 1;
        meta_.slot = OFFSET_BLOCK;

        // 初始化根节点
        internalNode root;
        root.next = root.prev = root.parent = 0;
        meta_.rootOffset = alloc(&root);

        // 初始化叶子节点
        leafNode leaf;
        leaf.next = leaf.prev = 0;
        leaf.parent = meta_.rootOffset;
        meta_.leafOffset = root.children[0].child = alloc(&leaf);

        // 节点信息写入磁盘
        unmap(&meta_, OFFSET_META);
        unmap(&root, meta_.rootOffset);
        unmap(&leaf, meta_.leafOffset);
}

off_t BPlusTree::searchIndex(const keyType& key) const
{
        off_t pos = meta_.rootOffset;
        int height = meta_.height;
        while (height > 1)
        {
                internalNode node;
                map(&node, pos);
                index* i = upper_bound(begin(node), end(node) - 1, key); // zx
                pos = i->child;
                --height;
        }
        return pos;
}

off_t BPlusTree::searchLeaf(const keyType& key) const
{
        return searchLeaf(searchIndex(key), key);
}

off_t BPlusTree::searchLeaf(off_t parent, const keyType& key) const
{
        internalNode node;
        map(&node, parent);
        index* i = upper_bound(begin(node), end(node) - 1, key);
        return i->child;
}

void BPlusTree::insertRecordNoSplit(leafNode* leaf,
                                    const keyType& key,
                                    const valueType& value)
{
        record* where = upper_bound(begin(*leaf), end(*leaf), key); // 二分查找确定位置
        std::copy_backward(where, end(*leaf), end(*leaf) + 1); // 移动元素
        where->key = key;
        where->value = value;
        leaf->n++;
}

void BPlusTree::insertKeyToIndex(off_t offset, const keyType& key,
                                off_t old, off_t after)
{
        if (offset == 0)
        {
                // 创建新的根节点
                internalNode root;
                root.next = root.prev = root.parent = 0;
                meta_.rootOffset = alloc(&root);
                meta_.height++;

                root.n = 2;
                root.children[0].child = old;
                root.children[0].key = key;
                root.children[1].child = after;

                unmap(&meta_, OFFSET_META);
                unmap(&root, meta_.rootOffset);

                resetIndexChildrenParent(begin(root), end(root), meta_.rootOffset);
                return ;
        }

        internalNode node;
        map(&node, offset);
        assert(node.n <= meta_.order);

        if (node.n == meta_.order) // 父节点同样已满
        {
                internalNode newNode;
                nodeCreate(offset, &node, &newNode);

                size_t point = (node.n - 1) / 2;
                bool placeRight = keyComp(key, node.children[point].key) > 0;
                if (placeRight)
                        ++point;
                if (placeRight && keyComp(key, node.children[point].key) < 0)
                        point--;

                keyType middleKey = node.children[point].key;
                std::copy(begin(node) + point + 1, end(node), begin(newNode));
                newNode.n = node.n - point - 1;
                node.n = point + 1;
                if (placeRight)
                        insertKeyToIndexNoSplit(newNode, key, after);
                else
                        insertKeyToIndexNoSplit(node, key, after);

                unmap(&node, offset);
                unmap(&newNode, node.next);
                resetIndexChildrenParent(begin(newNode), end(newNode), node.next);
                insertKeyToIndex(node.parent, middleKey, offset, node.next); // 递归
        }
        else
        {
                insertKeyToIndexNoSplit(node, key, after);
                unmap(&node, offset);
        }
}

void BPlusTree::insertKeyToIndexNoSplit(internalNode& node,
                                        const keyType& key,
                                        off_t value)
{
        index* where = upper_bound(begin(node), end(node) - 1, key);
        std::copy_backward(where, end(node), end(node) + 1);

        where->key = key;
        where->child = (where + 1)->child;
        (where + 1)->child = value;
        node.n++;
}

void BPlusTree::resetIndexChildrenParent(index* begin, index* end, off_t parent)
{
        internalNode node;
        while (begin != end)
        {
                map(&node, begin->child);
                node.parent = parent;
                unmap(&node, begin->child, SIZE_NO_CHILDREN);
                ++begin;
        }
}

void BPlusTree::removeFromIndex(off_t offset, internalNode& node, const keyType& key)
{
        size_t minN = meta_.rootOffset == offset ? 1 : meta_.order / 2;
        assert(node.n >= minN && node.n <= meta_.order);

        keyType indexKey = begin(node)->key;
        index *toDelete = find(node, key);
        if (toDelete != end(node))
        {
                (toDelete + 1)->child = toDelete->child;
                std::copy(toDelete + 1, end(node), toDelete);
        }
        node.n--;

        if (node.n == 1 && meta_.rootOffset == offset && meta_.internalNodeNum != 1)
        {
                unalloc(&node, meta_.rootOffset);
                meta_.height--;
                meta_.rootOffset = node.children[0].child;
                unmap(&meta_, OFFSET_META);
                return ;
        }

        if (node.n < minN)
        {
                internalNode parent;
                map(&parent, node.parent);

                // 从左右节点借
                bool borrowed = false;
                if (offset != begin(parent)->child)
                        borrowed = borrowKey(false, node, offset);
                if (!borrowed && offset != (end(parent) - 1)->child)
                        borrowed = borrowKey(true, node, offset);

                if (!borrowed)  // 合并
                {
                        assert(node.next != 0 || node.prev != 0);

                        if (offset == (end(parent) - 1)->child)
                        {
                                assert(node.prev != 0);
                                internalNode prev;
                                map(&prev, node.prev);

                                index *where = find(parent, begin(prev)->key);
                                resetIndexChildrenParent(begin(node), end(node), node.prev);
                                mergeKeys(where, prev, node);
                                unmap(&prev, node.prev);
                        }
                        else
                        {
                                assert(node.next != 0);
                                internalNode next;
                                map(&next, node.next);

                                index *where = find(parent, indexKey);
                                resetIndexChildrenParent(begin(next), end(next), offset);
                                mergeKeys(where, node, next);
                                unmap(&node, offset);
                        }

                        removeFromIndex(node.parent, parent, indexKey);
                }
                else
                        unmap(&node, offset);
        }
        else
                unmap(&node, offset);
}

bool BPlusTree::borrowKey(bool fromRight, leafNode& borrower)
{
        off_t lenderOff = fromRight ? borrower.next : borrower.prev;
        leafNode lender;
        map(&lender, lenderOff);

        assert(lender.n >= meta_.order / 2);
        if (lender.n != meta_.order / 2)
        {
                typename leafNode::child whereToLend, whereToPut;
                if (fromRight)
                {
                        whereToLend = begin(lender);
                        whereToPut = end(borrower);
                        changeParentChild(borrower.parent, begin(borrower)->key,
                                          lender.children[1].key);
                }
                else
                {
                        whereToLend = end(lender) - 1;
                        whereToPut = begin(borrower);
                        changeParentChild(lender.parent, begin(lender)->key,
                                          whereToLend->key);
                }

                std::copy_backward(whereToPut, end(borrower), end(borrower) + 1);
                *whereToPut = *whereToLend;
                borrower.n++;

                std::copy_backward(whereToLend + 1, end(lender), whereToLend);
                lender.n--;
                unmap(&lender, lenderOff);
                return true;
        }
        return false;
}

bool BPlusTree::borrowKey(bool fromRight, internalNode& borrower, off_t offset)
{
        typedef typename internalNode::child child;
        off_t lenderOff = fromRight ? borrower.next : borrower.prev;
        internalNode lender;
        map(&lender, lenderOff);

        assert(lender.n >= meta_.order / 2);
        if (lender.n != meta_.order / 2)
        {
                child whereToLend, whereToPut;
                internalNode parent;

                if (fromRight)
                {
                        whereToLend = begin(lender);
                        whereToPut = end(borrower);
                        map(&parent, borrower.parent);
                        child where = lower_bound(begin(parent), end(parent) - 1,
                                                  (end(borrower) - 1)->key);
                        where->key = whereToLend->key;
                        unmap(&parent, borrower.parent);
                }
                else
                {
                        whereToLend = end(lender) - 1;
                        whereToPut = begin(borrower);

                        map(&parent, lender.parent);
                        child where = find(parent, begin(lender)->key);
                        whereToPut->key = where->key;
                        where->key = (whereToLend - 1)->key;
                        unmap(&parent, lender.parent);
                }

                std::copy_backward(whereToPut, end(borrower), end(borrower) + 1);
                *whereToPut = *whereToLend;
                borrower.n++;

                resetIndexChildrenParent(whereToLend, whereToLend + 1, offset);
                std::copy(whereToLend + 1, end(lender), whereToLend);
                lender.n--;
                unmap(&lender, lenderOff);
                return true;
        }
        return false;
}

void BPlusTree::changeParentChild(off_t parent, const keyType& o, const keyType& n)
{
        internalNode node;
        map(&node, parent);

        index* w = find(node, o);
        assert(w != (node.children + node.n));

        w->key = n;
        unmap(&node, parent);
        if (w == node.children + node.n - 1)
        {
                changeParentChild(node.parent, o, n);
        }
}

void BPlusTree::mergeLeafs(leafNode* left, leafNode* right)
{
        std::copy(begin(*right), end(*right), end(*left));
        left->n += right->n;
}

void BPlusTree::mergeKeys(index* where, internalNode& node, internalNode& next)
{
        std::copy(begin(next), end(next), end(node));
        node.n += next.n;
        nodeRemove(&node, &next);
}

void BPlusTree::openFile(const char* mode) const
{
        if (fpLevel_ == 0)
                fp_ = fopen(path_, mode);
        ++fpLevel_;
}

void BPlusTree::closeFile() const
{
        if (fpLevel_ == 1)
                fclose(fp_);
        --fpLevel_;
}

int BPlusTree::map(void* block, off_t offset, size_t size) const
{
        openFile();
        fseek(fp_, offset, SEEK_SET);
        size_t rd = fread(block, size, 1, fp_);         // 从磁盘读入数据
        closeFile();
        return rd - 1;
}

int BPlusTree::unmap(void* block, off_t offset, size_t size) const
{
        openFile();
        fseek(fp_, offset, SEEK_SET);
        size_t wt = fwrite(block, size, 1, fp_);        // 向磁盘写入数据
        closeFile();
        return wt - 1;
}

off_t BPlusTree::alloc(size_t size)
{
        off_t slot = meta_.slot;
        meta_.slot += size;
        return slot;
}

off_t BPlusTree::alloc(leafNode* leaf)
{
        leaf->n = 0;
        meta_.leafNodeNum++;
        return alloc(sizeof(leafNode));
}

off_t BPlusTree::alloc(internalNode* node)
{
        node->n = 1;
        meta_.internalNodeNum++;
        return alloc(sizeof(internalNode));
}

void BPlusTree::unalloc(leafNode* leaf, off_t offset)
{
        --meta_.leafNodeNum;
}

void BPlusTree::unalloc(internalNode* leaf, off_t offset)
{
        --meta_.internalNodeNum;
}

} // namespace Explorer

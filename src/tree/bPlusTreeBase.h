#ifndef _EXPLORER_BPLUSTREE_BASE_H_
#define _EXPLORER_BPLUSTREE_BASE_H_

#include <string.h>
#include <cassert>

namespace Explorer {

// 每个内部节点的子节点数目
// 每个叶子节点存储的记录数目
#define ORDER 20

typedef int valueType;

struct keyType {
        char k[16];

        keyType(const char* str = "")
        {
                bzero(k, sizeof(k));
                strcpy(k, str);
        }
};

inline int keyComp(const keyType& a, const keyType& b)
{
        int diff = strlen(a.k) - strlen(b.k);
        return diff == 0 ? strcmp(a.k, b.k) : diff;
}

#define OPERATOR_KEYCMP(type) \
        bool operator<(const keyType& l, const type& r) { \
                return keyComp(l, r.key) < 0; \
        } \
        bool operator<(const type& l, const keyType& r) { \
                return keyComp(l.key, r) < 0; \
        } \
        bool operator==(const keyType&l, const type& r) { \
                return keyComp(l, r.key) == 0; \
        } \
        bool operator==(const type& l, const keyType& r) { \
                return keyComp(l.key, r) == 0; \
        }

} // namespace Explorer

#endif // _EXPLORER_BPLUSTREE_BASE_H_

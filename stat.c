/*
 * stat.c — 成员B：递归遍历与统计功能
 */

#include <string.h>
#include "stat.h"

int countFiles(TreeNode* node) {
    if (node == NULL) return 0;
    int count = node->isFile ? 1 : 0;
    TreeNode* child = node->children;
    while (child != NULL) {
        count += countFiles(child);
        child = child->next;
    }
    return count;
}

int getMaxDepth(TreeNode* node) {
    if (node == NULL) return 0;
    int maxChildDepth = 0;
    TreeNode* child = node->children;
    while (child != NULL) {
        int d = getMaxDepth(child);
        if (d > maxChildDepth) maxChildDepth = d;
        child = child->next;
    }
    return maxChildDepth + 1;
}

void traverse(TreeNode* node, void (*callback)(TreeNode*)) {
    if (node == NULL || callback == NULL) return;
    callback(node);
    TreeNode* child = node->children;
    while (child != NULL) {
        traverse(child, callback);
        child = child->next;
    }
}

int search(TreeNode* node, const char* keyword,
           TreeNode** results, int maxResults) {
    if (node == NULL || keyword == NULL || keyword[0] == '\0'
        || results == NULL || maxResults <= 0)
        return 0;

    int count = 0;
    if (strstr(node->name, keyword) != NULL) {
        results[count] = node;
        count++;
    }
    TreeNode* child = node->children;
    while (child != NULL && count < maxResults) {
        int sub = search(child, keyword,
                         results + count, maxResults - count);
        count += sub;
        child = child->next;
    }
    return count;
}

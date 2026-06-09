/*
 * stat.c — 成员B：递归遍历与统计功能
 *
 * 任务：
 *   1. 实现 countFiles —— 深度优先遍历，累计 isFile==true 的节点数
 *   2. 实现 getMaxDepth —— 计算树的最大深度（根为第1层）
 *   3. 实现 traverse —— 回调遍历（前序）
 *
 * 依赖：tree.h（TreeNode 结构体定义）
 */

#include "stat.h"

/* ================================================================
 * countFiles(node)
 *   深度优先遍历，返回文件（isFile==true）总数。
 * ================================================================ */
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

/* ================================================================
 * getMaxDepth(node)
 *   返回最大深度。根为第1层，空节点返回0。
 * ================================================================ */
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

/* ================================================================
 * traverse(node, callback)
 *   前序遍历，对每个节点调用 callback。
 * ================================================================ */
void traverse(TreeNode* node, void (*callback)(TreeNode*)) {
    if (node == NULL || callback == NULL) return;
    callback(node);
    TreeNode* child = node->children;
    while (child != NULL) {
        traverse(child, callback);
        child = child->next;
    }
}

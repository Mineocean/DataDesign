/*
 * stat.c — 成员B：递归遍历与统计功能
 *
 * 任务：
 *   1. 实现 countFiles —— 深度优先遍历，累计 isFile==true 的节点数
 *   2. 实现 getMaxDepth —— 计算树的最大深度（根为第1层）
 *
 * 依赖：tree.h（TreeNode 结构体定义）
 * 编译：cl /utf-8 /c stat.c
 *
 * ── 给成员B ──
 * 当前是空壳（返回 0），目的是让主程序能先编译通过。
 * 你只需要把下面两个函数体里的 return 0 替换成真正的实现即可。
 * 函数签名不要改——main.c 和 stat.h 都依赖它们。
 */

#include "stat.h"

/* ================================================================
 * countFiles(node)
 *   从 node 开始深度优先遍历整棵子树，返回文件（isFile==true）总数。
 *   提示：递归遍历 children 链表，遇文件 +1，遇文件夹继续深入。
 *   时间复杂度目标：O(n)
 * ================================================================ */
int countFiles(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }

    int count = 0;

    // 如果当前节点是文件，计数加1
    if (node->isFile) {
        count = 1;
    }

    // 递归遍历所有子节点
    TreeNode* child = node->children;
    while (child != NULL) {
        count += countFiles(child);
        child = child->next;
    }

    return count;
}
/* ================================================================
 * getMaxDepth(node)
 *   返回从 node 开始的最大深度。
 *   约定：根为第 1 层。空节点（NULL）返回 0。
 *   提示：递归取所有子节点深度的最大值 + 1。
 * ================================================================ */
int getMaxDepth(TreeNode* node) {
    if (node == NULL) {
        return 0;
    }

    int maxDepth = 0;

    // 遍历所有子节点，找到最大的子树深度
    TreeNode* child = node->children;
    while (child != NULL) {
        int childDepth = getMaxDepth(child);
        if (childDepth > maxDepth) {
            maxDepth = childDepth;
        }
        child = child->next;
    }

    return maxDepth + 1;

}

/**
 * 函数功能：使用回调函数遍历树的所有节点（前序遍历）
 * 参数 node：要遍历的树的根节点
 * 参数 callback：回调函数，对每个节点执行的操作
 */
void traverse(TreeNode* node, void (*callback)(TreeNode*)) {
    if (node == NULL || callback == NULL) {
        return;
    }

    // 先处理当前节点
    callback(node);

    // 再递归遍历所有子节点
    TreeNode* child = node->children;
    while (child != NULL) {
        traverse(child, callback);
        child = child->next;
    }

}

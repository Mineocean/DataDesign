#include "stat.h"

/* ===== 成员B：递归遍历与统计功能 ===== */

/*
 * 统计文件总数：深度优先遍历，累计 isFile 为 true 的节点。
 * 时间复杂度 O(n)，空间复杂度 O(depth)（递归栈）。
 */
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

/*
 * 文件夹层数：递归计算最大深度。
 * 根为第1层，空树返回0。
 */
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

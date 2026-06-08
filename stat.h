#ifndef STAT_H
#define STAT_H

#include <stddef.h>
#include "tree.h"

/* ===== 成员B：递归遍历与统计功能 ===== */

/*
 * 统计文件总数：从 node 开始深度优先遍历，
 * 累计所有 isFile == true 的节点数。
 */
int countFiles(TreeNode* node);

/*
 * 文件夹层数：树的最大深度。
 * 根节点为第1层，每向下一层 +1。
 */
int getMaxDepth(TreeNode* node);

void traverse(TreeNode* node, void (*callback)(TreeNode*))
#endif

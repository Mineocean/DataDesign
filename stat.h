#ifndef STAT_H
#define STAT_H

#include <stddef.h>
#include "tree.h"

/* ===== 成员B：递归遍历与统计功能 ===== */

int countFiles(TreeNode* node);
int getMaxDepth(TreeNode* node);
void traverse(TreeNode* node, void (*callback)(TreeNode*));
int search(TreeNode* node, const char* keyword,
           TreeNode** results, int maxResults);

#endif

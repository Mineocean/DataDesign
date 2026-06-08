#ifndef TREE_H
#define TREE_H

// 树节点结构
typedef struct TreeNode {
    char* name;            // 文件/文件夹名
    int isFile;            // 1=文件, 0=文件夹
    struct TreeNode* parent;
    struct TreeNode** children; // 子节点指针数组
    int childCount;
    int childCapacity;
} TreeNode;

// 全局根节点（根目录，默认是文件夹）
extern TreeNode* root;

// 初始化根目录
void initRoot();

// ---------- 成员A提供的API ----------
int addNode(const char* path, const char* name, int isFile);   // 增加节点
int deleteNode(const char* path);                              // 删除节点（及所有子节点）
int renameNode(const char* path, const char* newName);         // 重命名

// ---------- 成员B提供的API ----------
int getFileCount(TreeNode* node);      // 统计文件总数（从node开始）
int getMaxDepth(TreeNode* node);       // 文件夹层数（根为1）

// 辅助：根据路径字符串（如 "/home/a"）查找节点
TreeNode* findNodeByPath(const char* path);

// 释放整个树（程序结束时调用）
void freeTree(TreeNode* node);

#endif
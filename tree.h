#ifndef TREE_H
#define TREE_H

#include <stdbool.h>

#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 1024

/* 树节点：采用左孩子-右兄弟（链表）表示法 */
typedef struct TreeNode {
    char name[MAX_NAME_LEN];
    bool isFile;                /* true=文件, false=文件夹 */
    struct TreeNode* parent;
    struct TreeNode* children;  /* 第一个孩子 */
    struct TreeNode* next;      /* 下一个兄弟 */
} TreeNode;

/* 文件系统：持有一个根节点 */
typedef struct {
    TreeNode* root;
} FileSystem;

/* ===== 成员A：树结构模型与基础操作 ===== */

void initFileSystem(FileSystem* fs);

/* 按路径查找节点（内部路径，如 /root/folder1） */
TreeNode* findNode(FileSystem* fs, const char* path);

/* 新增：在指定路径下创建文件夹或文件 */
bool addNode(FileSystem* fs, const char* parentPath,
             const char* name, bool isFile);
/* 删除：删除节点及所有子节点（递归释放） */
bool deleteNode(FileSystem* fs, const char* path);
/* 重命名：修改节点名称（检查同级重名） */
bool renameNode(FileSystem* fs, const char* path, const char* newName);

/* 递归释放整棵树（成员A） */
void freeTree(TreeNode* node);

#endif

#ifndef TREE_H
#define TREE_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 1024

// 树节点结构体
typedef struct TreeNode {
    char name[MAX_NAME_LEN];
    bool isFile;
    struct TreeNode* parent;
    struct TreeNode* children;
    struct TreeNode* next;
} TreeNode;

// 文件系统结构体
typedef struct {
    TreeNode* root;
} FileSystem;

// 基础操作函数
void initFileSystem(FileSystem* fs);
TreeNode* findNode(FileSystem* fs, const char* path);
bool addNode(FileSystem* fs, const char* parentPath, const char* name, bool isFile);
bool deleteNode(FileSystem* fs, const char* path);
bool renameNode(FileSystem* fs, const char* path, const char* newName);
void freeTree(TreeNode* node);

// 序列化函数
bool saveTree(FILE* fp, TreeNode* node);
TreeNode* loadTree(FILE* fp);

#endif
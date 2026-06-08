#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

/* ===== 成员A：树结构模型与基础操作 ===== */

/* 初始化文件系统 */
void initFileSystem(FileSystem* fs) {
    fs->root = (TreeNode*)malloc(sizeof(TreeNode));
    if (fs->root == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(fs->root->name, "root");
    fs->root->isFile = false;
    fs->root->parent = NULL;
    fs->root->children = NULL;
    fs->root->next = NULL;
}

/* 按路径查找 */
TreeNode* findNode(FileSystem* fs, const char* path) {
    if (fs == NULL || fs->root == NULL) return NULL;
    if (strcmp(path, "/root") == 0) return fs->root;

    TreeNode* current = fs->root;
    char tempPath[MAX_PATH_LEN];
    strncpy(tempPath, path, MAX_PATH_LEN - 1);
    tempPath[MAX_PATH_LEN - 1] = '\0';

    char* token = strtok(tempPath, "/");
    while (token != NULL) {
        if (strcmp(token, "root") == 0) {
            token = strtok(NULL, "/");
            continue;
        }
        bool found = false;
        TreeNode* child = current->children;
        while (child != NULL) {
            if (strcmp(child->name, token) == 0) {
                current = child;
                found = true;
                break;
            }
            child = child->next;
        }
        if (!found) return NULL;
        token = strtok(NULL, "/");
    }
    return current;
}

/* 新增节点 */
bool addNode(FileSystem* fs, const char* parentPath,
             const char* name, bool isFile) {
    TreeNode* parent = findNode(fs, parentPath);
    if (parent == NULL) {
        printf("Error: parent path %s does not exist.\n", parentPath);
        return false;
    }
    if (parent->isFile) {
        printf("Error: parent %s is a file, cannot add child.\n", parentPath);
        return false;
    }
    /* 检查是否重名 */
    TreeNode* sibling = parent->children;
    while (sibling != NULL) {
        if (strcmp(sibling->name, name) == 0) {
            printf("Error: name %s already exists in %s.\n", name, parentPath);
            return false;
        }
        sibling = sibling->next;
    }

    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    if (node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }
    strncpy(node->name, name, MAX_NAME_LEN - 1);
    node->name[MAX_NAME_LEN - 1] = '\0';
    node->isFile = isFile;
    node->parent = parent;
    node->children = NULL;
    node->next = parent->children;   /* 头插法 */
    parent->children = node;
    return true;
}

/* 递归释放（内部） */
static void freeSubtree(TreeNode* node) {
    if (node == NULL) return;
    TreeNode* child = node->children;
    while (child != NULL) {
        TreeNode* nextSibling = child->next;
        freeSubtree(child);
        child = nextSibling;
    }
    free(node);
}

void freeTree(TreeNode* node) {
    freeSubtree(node);
}

/* 删除节点 */
bool deleteNode(FileSystem* fs, const char* path) {
    TreeNode* node = findNode(fs, path);
    if (node == NULL) {
        printf("Error: path %s does not exist.\n", path);
        return false;
    }
    if (node->parent == NULL) {
        printf("Error: cannot delete root node.\n");
        return false;
    }
    /* 从父节点的孩子链表中摘除 */
    TreeNode* parent = node->parent;
    if (parent->children == node) {
        parent->children = node->next;
    } else {
        TreeNode* prev = parent->children;
        while (prev != NULL && prev->next != node)
            prev = prev->next;
        if (prev != NULL)
            prev->next = node->next;
    }
    node->next = NULL;
    freeSubtree(node);
    return true;
}

/* 重命名 */
bool renameNode(FileSystem* fs, const char* path, const char* newName) {
    TreeNode* node = findNode(fs, path);
    if (node == NULL) {
        printf("Error: path %s does not exist.\n", path);
        return false;
    }
    /* 检查同级的重名冲突 */
    TreeNode* parent = node->parent;
    if (parent != NULL) {
        TreeNode* sibling = parent->children;
        while (sibling != NULL) {
            if (sibling != node && strcmp(sibling->name, newName) == 0) {
                printf("Error: name %s already exists in %s.\n",
                       newName, parent->name);
                return false;
            }
            sibling = sibling->next;
        }
    }
    strncpy(node->name, newName, MAX_NAME_LEN - 1);
    node->name[MAX_NAME_LEN - 1] = '\0';
    return true;
}



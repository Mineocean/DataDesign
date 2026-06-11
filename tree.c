#include "tree.h"
#include <stdlib.h>
#include <string.h>

// 初始化文件系统
void initFileSystem(FileSystem* fs) {
    fs->root = (TreeNode*)malloc(sizeof(TreeNode));
    strcpy(fs->root->name, "root");
    fs->root->isFile = false;
    fs->root->parent = NULL;
    fs->root->children = NULL;
    fs->root->next = NULL;
}

// 查找节点
TreeNode* findNode(FileSystem* fs, const char* path) {
    if (strcmp(path, "/root") == 0 || strcmp(path, "root") == 0) {
        return fs->root;
    }

    TreeNode* current = fs->root;
    char tempPath[MAX_PATH_LEN];
    strcpy(tempPath, path);

    char* token = strtok(tempPath, "/");
    while (token != NULL) {
        if (strcmp(token, "root") == 0) {
            token = strtok(NULL, "/");
            continue;
        }

        TreeNode* child = current->children;
        bool found = false;
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

// 添加节点
bool addNode(FileSystem* fs, const char* parentPath, const char* name, bool isFile) {
    TreeNode* parent = findNode(fs, parentPath);
    if (parent == NULL) {
        printf("Error: Parent path %s does not exist.\n", parentPath);
        return false;
    }

    if (parent->isFile) {
        printf("Error: %s is a file, cannot add child.\n", parentPath);
        return false;
    }

    // 检查重名
    TreeNode* child = parent->children;
    while (child != NULL) {
        if (strcmp(child->name, name) == 0) {
            printf("Error: Name %s already exists in %s.\n", name, parentPath);
            return false;
        }
        child = child->next;
    }

    // 创建新节点
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    strcpy(newNode->name, name);
    newNode->isFile = isFile;
    newNode->parent = parent;
    newNode->children = NULL;
    newNode->next = parent->children;
    parent->children = newNode;

    return true;
}

// 递归删除子树
static void deleteSubtree(TreeNode* node) {
    if (node == NULL) return;

    TreeNode* child = node->children;
    while (child != NULL) {
        TreeNode* next = child->next;
        deleteSubtree(child);
        child = next;
    }
    free(node);
}

// 删除节点
bool deleteNode(FileSystem* fs, const char* path) {
    if (strcmp(path, "/root") == 0 || strcmp(path, "root") == 0) {
        printf("Error: Cannot delete root.\n");
        return false;
    }

    TreeNode* node = findNode(fs, path);
    if (node == NULL) {
        printf("Error: Path %s does not exist.\n", path);
        return false;
    }

    TreeNode* parent = node->parent;
    if (parent->children == node) {
        parent->children = node->next;
    }
    else {
        TreeNode* prev = parent->children;
        while (prev->next != node) {
            prev = prev->next;
        }
        prev->next = node->next;
    }

    deleteSubtree(node);
    return true;
}

// 重命名节点
bool renameNode(FileSystem* fs, const char* path, const char* newName) {
    TreeNode* node = findNode(fs, path);
    if (node == NULL) {
        printf("Error: Path %s does not exist.\n", path);
        return false;
    }

    TreeNode* parent = node->parent;
    if (parent != NULL) {
        TreeNode* sibling = parent->children;
        while (sibling != NULL) {
            if (sibling != node && strcmp(sibling->name, newName) == 0) {
                printf("Error: Name %s already exists.\n", newName);
                return false;
            }
            sibling = sibling->next;
        }
    }

    strcpy(node->name, newName);
    return true;
}

// 释放整棵树
void freeTree(TreeNode* node) {
    deleteSubtree(node);
}

// 保存树到文件
bool saveTree(FILE* fp, TreeNode* node) {
    if (node == NULL) return true;

    // 写入节点名
    fprintf(fp, "%s\n", node->name);
    // 写入类型（0文件夹 1文件）
    fprintf(fp, "%d\n", node->isFile);

    // 递归保存子节点
    TreeNode* child = node->children;
    while (child != NULL) {
        if (!saveTree(fp, child)) return false;
        child = child->next;
    }

    // 子节点结束标记
    fprintf(fp, "#END\n");
    return true;
}

// 从文件加载树
TreeNode* loadTree(FILE* fp) {
    char line[MAX_NAME_LEN];

    // 读节点名
    if (fgets(line, sizeof(line), fp) == NULL) return NULL;
    line[strcspn(line, "\n")] = '\0';

    // 遇到结束标记
    if (strcmp(line, "#END") == 0) return NULL;

    // 创建节点
    TreeNode* node = (TreeNode*)malloc(sizeof(TreeNode));
    strcpy(node->name, line);
    node->parent = NULL;
    node->children = NULL;
    node->next = NULL;

    // 读类型
    if (fgets(line, sizeof(line), fp) == NULL) {
        free(node);
        return NULL;
    }
    node->isFile = (atoi(line) == 1);

    // 递归加载子节点
    TreeNode* prev = NULL;
    while (1) {
        TreeNode* child = loadTree(fp);
        if (child == NULL) break;

        child->parent = node;
        if (prev == NULL) {
            node->children = child;
        }
        else {
            prev->next = child;
        }
        prev = child;
    }

    return node;
}
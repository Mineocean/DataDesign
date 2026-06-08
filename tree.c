#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_NAME_LEN 256
#define MAX_PATH_LEN 1024

// 定义树节点结构体
typedef struct TreeNode {
    char name[MAX_NAME_LEN];
    bool is_file;
    struct TreeNode* parent;
    struct TreeNode* children;
    struct TreeNode* next;
} TreeNode;

// 定义文件系统结构体
typedef struct {
    TreeNode* root;
} FileSystem;

// 初始化文件系统
void init_file_system(FileSystem* fs) {
    fs->root = (TreeNode*)malloc(sizeof(TreeNode));
    if (fs->root == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    strcpy(fs->root->name, "root");
    fs->root->is_file = false;
    fs->root->parent = NULL;
    fs->root->children = NULL;
    fs->root->next = NULL;
}

// 根据路径查找节点
TreeNode* find_node(FileSystem* fs, const char* path) {
    if (strcmp(path, "/root") == 0) {
        return fs->root;
    }
    TreeNode* current = fs->root;
    char temp_path[MAX_PATH_LEN];
    strcpy(temp_path, path);
    char* token = strtok(temp_path, "/");
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
        if (!found) {
            return NULL;
        }
        token = strtok(NULL, "/");
    }
    return current;
}

// 添加节点
bool add_node(FileSystem* fs, const char* parent_path, const char* name, bool is_file) {
    TreeNode* parent = find_node(fs, parent_path);
    if (parent == NULL) {
        printf("Error: Parent path %s does not exist.\n", parent_path);
        return false;
    }
    TreeNode* child = parent->children;
    while (child != NULL) {
        if (strcmp(child->name, name) == 0) {
            printf("Error: Name %s already exists in %s.\n", name, parent_path);
            return false;
        }
        child = child->next;
    }
    TreeNode* new_node = (TreeNode*)malloc(sizeof(TreeNode));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return false;
    }
    strcpy(new_node->name, name);
    new_node->is_file = is_file;
    new_node->parent = parent;
    new_node->children = NULL;
    new_node->next = parent->children;
    parent->children = new_node;
    return true;
}

// 递归删除节点及其子节点
void delete_recursive(TreeNode* node) {
    if (node == NULL) {
        return;
    }
    TreeNode* child = node->children;
    while (child != NULL) {
        TreeNode* next = child->next;
        delete_recursive(child);
        child = next;
    }
    free(node);
}

// 删除节点
bool delete_node(FileSystem* fs, const char* path) {
    TreeNode* node = find_node(fs, path);
    if (node == NULL) {
        printf("Error: Path %s does not exist.\n", path);
        return false;
    }
    TreeNode* parent = node->parent;
    if (parent != NULL) {
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
    }
    delete_recursive(node);
    return true;
}

// 重命名节点
bool rename_node(FileSystem* fs, const char* path, const char* new_name) {
    TreeNode* node = find_node(fs, path);
    if (node == NULL) {
        printf("Error: Path %s does not exist.\n", path);
        return false;
    }
    TreeNode* parent = node->parent;
    if (parent != NULL) {
        TreeNode* sibling = parent->children;
        while (sibling != NULL) {
            if (sibling != node && strcmp(sibling->name, new_name) == 0) {
                printf("Error: Name %s already exists in %s.\n", new_name, parent->name);
                return false;
            }
            sibling = sibling->next;
        }
    }
    strcpy(node->name, new_name);
    return true;
}

// 单元测试
int main() {
    FileSystem fs;
    init_file_system(&fs);

    // 测试添加节点
    printf("Testing add_node...\n");
    printf("Adding folder 'folder1' in root: %s\n", add_node(&fs, "/root", "folder1", false) ? "Success" : "Failure");
    printf("Adding file 'file1.txt' in root: %s\n", add_node(&fs, "/root", "file1.txt", true) ? "Success" : "Failure");

    // 测试重命名节点
    printf("\nTesting rename_node...\n");
    // 确保使用正确的路径
    printf("Renaming 'folder1' to 'new_folder1': %s\n", rename_node(&fs, "/root/folder1", "new_folder1") ? "Success" : "Failure");

    // 测试删除节点
    printf("\nTesting delete_node...\n");
    // 确保使用重命名后的路径
    printf("Deleting 'new_folder1': %s\n", delete_node(&fs, "/root/new_folder1") ? "Success" : "Failure");

    return 0;
}
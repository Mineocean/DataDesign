#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "tree.h"
#include "stat.h"

/* ===== 成员C：可视化展示与用户交互界面 ===== */

static FileSystem fs;

/* ---------- 辅助函数 ---------- */

/* 去除字符串两端空白 */
static void trim(char* str) {
    char* start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != str)
        memmove(str, start, strlen(start) + 1);
    if (*str == '\0') return;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

/* 安全读取一行输入，去除末尾换行 */
static void readInput(char* buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

/* 用户友好路径 → 内部路径（前面拼 /root） */
static void userPathToInternal(const char* userPath,
                               char* internal, int size) {
    if (strcmp(userPath, "/") == 0 || strcmp(userPath, "") == 0) {
        strncpy(internal, "/root", size);
    } else if (userPath[0] == '/') {
        snprintf(internal, size, "/root%s", userPath);
    } else {
        snprintf(internal, size, "/root/%s", userPath);
    }
}

/* ---------- 树形可视化 ---------- */
static void printTreeNode(TreeNode* node, const char* prefix, int isLast) {
    if (node == NULL) return;
    printf("%s", prefix);
    printf(isLast ? "└── " : "├── ");
    printf("%s", node->name);
    if (node->isFile)
        printf(" (file)");
    printf("\n");

    char newPrefix[512];
    sprintf(newPrefix, "%s%s", prefix, isLast ? "    " : "│   ");
    TreeNode* child = node->children;
    while (child != NULL) {
        printTreeNode(child, newPrefix, child->next == NULL);
        child = child->next;
    }
}

static void printTree(void) {
    if (fs.root == NULL || fs.root->children == NULL) {
        printf("树为空\n");
        return;
    }
    printf("\n当前树结构\n");
    TreeNode* child = fs.root->children;
    while (child != NULL) {
        printTreeNode(child, "", child->next == NULL);
        child = child->next;
    }
    printf("\n");
}

/* ---------- 菜单 ---------- */
static void showMenu(void) {
    printf("\n========== 简易文件目录树管理 ==========\n");
    printf("1. 查看目录树\n");
    printf("2. 新增文件夹或文件\n");
    printf("3. 删除节点\n");
    printf("4. 重命名节点\n");
    printf("5. 统计文件总数和文件夹层数\n");
    printf("0. 退出\n");
    printf("请选择操作：");
}

/* ---------- 操作处理 ---------- */
static void handleAdd(void) {
    char userPath[256], name[256], typeInput[10];
    printf("请输入父目录路径（如 / 或 /folder1）：");
    readInput(userPath, sizeof(userPath));
    trim(userPath);
    if (strlen(userPath) == 0) { printf("无效的路径\n"); return; }

    printf("请输入新节点名称：");
    readInput(name, sizeof(name));
    trim(name);
    if (strlen(name) == 0) { printf("无效的名称\n"); return; }

    printf("请输入节点类型（0=文件夹, 1=文件）：");
    readInput(typeInput, sizeof(typeInput));
    int type = atoi(typeInput);
    if (type != 0 && type != 1) { printf("无效的节点类型\n"); return; }

    char internal[512];
    userPathToInternal(userPath, internal, sizeof(internal));
    bool ok = addNode(&fs, internal, name, type == 1);
    printf(ok ? "节点添加成功\n" : "节点添加失败\n");
}

static void handleDelete(void) {
    char userPath[256];
    printf("请输入要删除的节点路径（如 /folder1/subfolder）：");
    readInput(userPath, sizeof(userPath));
    trim(userPath);
    if (strlen(userPath) == 0) { printf("无效的路径\n"); return; }

    char internal[512];
    userPathToInternal(userPath, internal, sizeof(internal));
    bool ok = deleteNode(&fs, internal);
    printf(ok ? "节点删除成功\n" : "节点删除失败\n");
}

static void handleRename(void) {
    char userPath[256], newName[256];
    printf("请输入要重命名的节点路径（如 /folder1/subfolder）：");
    readInput(userPath, sizeof(userPath));
    trim(userPath);
    if (strlen(userPath) == 0) { printf("无效的路径\n"); return; }

    printf("请输入新名称：");
    readInput(newName, sizeof(newName));
    trim(newName);
    if (strlen(newName) == 0) { printf("无效的名称\n"); return; }

    char internal[512];
    userPathToInternal(userPath, internal, sizeof(internal));
    bool ok = renameNode(&fs, internal, newName);
    printf(ok ? "节点重命名成功\n" : "节点重命名失败\n");
}

static void showStats(void) {
    int fileCnt = countFiles(fs.root);
    int depth = getMaxDepth(fs.root);
    printf("\n统计信息：\n");
    printf("文件总数: %d\n", fileCnt);
    printf("文件夹层数: %d\n", depth);
}

/* ---------- 入口 ---------- */
int main(void) {
    system("chcp 65001 >nul");  /* 控制台切 UTF-8，防止中文乱码 */
    initFileSystem(&fs);
    int choice;
    do {
        showMenu();
        char input[10];
        readInput(input, sizeof(input));
        choice = atoi(input);
        switch (choice) {
        case 1: printTree();   break;
        case 2: handleAdd();   break;
        case 3: handleDelete(); break;
        case 4: handleRename(); break;
        case 5: showStats();   break;
        case 0: printf("退出程序\n"); break;
        default: printf("无效的选择，请重新输入。\n");
        }
    } while (choice != 0);
    freeTree(fs.root);
    return 0;
}

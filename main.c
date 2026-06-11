#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "tree.h"
#include "stat.h"

/* ===== 成员C：可视化展示与用户交互界面 ===== */

static FileSystem fs;
static const char* SAVE_FILE = "tree.dat";

#define SEP "----------------------------------------"

static void trim(char* str) {
    char* start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    if (*str == '\0') return;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

static void readInput(char* buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL)
        buffer[strcspn(buffer, "\n")] = '\0';
}

static void userPathToInternal(const char* userPath, char* internal, int size) {
    if (strcmp(userPath, "/") == 0 || strcmp(userPath, "") == 0)
        strncpy(internal, "/root", size);
    else if (userPath[0] == '/')
        snprintf(internal, size, "/root%s", userPath);
    else
        snprintf(internal, size, "/root/%s", userPath);
}

/* ---------- 分割线 ---------- */
static void printSep(void) {
    printf("\n%s\n", SEP);
}

static void waitEnter(void) {
    printf("按回车继续操作...");
    char buf[10];
    fgets(buf, sizeof(buf), stdin);
}

/* ---------- 树形可视化 ---------- */
static void printTreeNode(TreeNode* node, const char* prefix, int isLast) {
    if (node == NULL) return;
    printf("%s%s%s%s\n", prefix,
           isLast ? "└── " : "├── ",
           node->name, node->isFile ? " (file)" : "");
    char newPrefix[MAX_PATH_LEN];
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
    printf("当前树结构\n");
    TreeNode* child = fs.root->children;
    while (child != NULL) {
        printTreeNode(child, "", child->next == NULL);
        child = child->next;
    }
}

/* ---------- 菜单 ---------- */
static void showMenu(void) {
    printSep();
    printf("简易文件目录树管理\n"
           "1. 查看目录树\n"
           "2. 新增文件夹或文件\n"
           "3. 删除节点\n"
           "4. 重命名节点\n"
           "5. 统计文件总数和文件夹层数\n"
           "6. 搜索节点\n"
           "7. 保存到文件\n"
           "8. 从文件加载\n"
           "0. 退出\n"
           "请选择操作：");
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
    printf("统计信息：\n");
    printf("文件总数: %d\n", countFiles(fs.root));
    printf("文件夹层数: %d\n", getMaxDepth(fs.root));
}

/* ---------- 搜索 ---------- */
#define MAX_RESULTS 64

static void handleSearch(void) {
    char keyword[256];
    printf("请输入搜索关键字：");
    readInput(keyword, sizeof(keyword));
    trim(keyword);
    if (strlen(keyword) == 0) { printf("无效的关键字\n"); return; }
    TreeNode* results[MAX_RESULTS];
    int cnt = search(fs.root, keyword, results, MAX_RESULTS);
    if (cnt == 0) {
        printf("未找到包含 %s 的节点\n", keyword);
        return;
    }
    printf("找到 %d 个匹配节点：\n", cnt);
    for (int i = 0; i < cnt; i++) {
        printf("  %d. [%s] %s\n", i + 1,
               results[i]->isFile ? "文件" : "文件夹",
               results[i]->name);
    }
}

/* ---------- 保存 / 加载 ---------- */
static void handleSave(void) {
    FILE* fp = fopen(SAVE_FILE, "w");
    if (fp == NULL) {
        printf("保存失败，无法写入 %s\n", SAVE_FILE);
        return;
    }
    bool ok = saveTree(fp, fs.root);
    fclose(fp);
    printf(ok ? "已保存到 %s\n" : "保存失败\n", SAVE_FILE);
}

static void handleLoad(void) {
    FILE* fp = fopen(SAVE_FILE, "r");
    if (fp == NULL) {
        printf("加载失败，文件不存在\n");
        return;
    }
    TreeNode* newRoot = loadTree(fp);
    fclose(fp);
    if (newRoot == NULL) {
        printf("加载失败，格式错误\n");
        return;
    }
    freeTree(fs.root);
    fs.root = newRoot;
    printf("已从 %s 加载\n", SAVE_FILE);
}

/* ---------- 预制演示数据 ---------- */
static void createSampleData(void) {
    addNode(&fs, "/root", "项目文档", false);
    addNode(&fs, "/root/项目文档", "设计报告.doc", true);
    addNode(&fs, "/root/项目文档", "需求分析.doc", true);
    addNode(&fs, "/root", "源代码", false);
    addNode(&fs, "/root/源代码", "main.c", true);
    addNode(&fs, "/root/源代码", "tree.h", true);
    addNode(&fs, "/root/源代码", "tree.c", true);
    addNode(&fs, "/root/源代码", "stat.c", true);
    addNode(&fs, "/root", "说明.txt", true);
    printf("已创建演示目录树\n");
}

/* ---------- 入口 ---------- */
int main(void) {
    system("chcp 65001 >nul");

    FILE* fp = fopen(SAVE_FILE, "r");
    if (fp) {
        TreeNode* saved = loadTree(fp);
        fclose(fp);
        if (saved) {
            fs.root = saved;
            printf("已自动加载存档 %s\n", SAVE_FILE);
        } else {
            initFileSystem(&fs);
            createSampleData();
        }
    } else {
        initFileSystem(&fs);
        createSampleData();
    }

    showMenu();
    int choice;
    do {
        char input[10];
        readInput(input, sizeof(input));
        choice = atoi(input);
        switch (choice) {
        case 1: printTree();     break;
        case 2: handleAdd();     break;
        case 3: handleDelete();  break;
        case 4: handleRename();  break;
        case 5: showStats();     break;
        case 6: handleSearch();  break;
        case 7: handleSave();    break;
        case 8: handleLoad();    break;
        case 0:
            printf("确认退出？(y/n)：");
            readInput(input, sizeof(input));
            trim(input);
            if (input[0] == 'y' || input[0] == 'Y') {
                printf("退出程序\n");
            } else {
                choice = -1;
            }
            break;
        default: printf("无效的选择\n");
        }
        if (choice != 0) {
            printSep();
            waitEnter();
            printf("\n请选择操作：");
        }
    } while (choice != 0);

    fp = fopen(SAVE_FILE, "w");
    if (fp) { saveTree(fp, fs.root); fclose(fp); }
    freeTree(fs.root);
    return 0;
}

/**
 * main.c — 成员C：可视化展示与用户交互界面
 *
 * 职责：
 *   1. 树形可视化打印（├── └── │）
 *   2. 命令行菜单交互（do-while 循环 + switch 分发）
 *   3. 调用成员A的 API（addNode/deleteNode/renameNode）
 *   4. 调用成员B的 API（countFiles/getMaxDepth/search）
 *   5. 持久化：调用 saveTree/loadTree（自动保存/加载）
 *
 * 依赖：
 *   tree.h — TreeNode 结构体、FileSystem、树操作函数
 *   stat.h — 统计与遍历函数
 *
 * 存储类别说明：
 *   所有函数和全局变量均使用 static，
 *   限定在本文件内可见，实现模块信息隐藏。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "tree.h"
#include "stat.h"

/* ===== 成员C：可视化展示与用户交互界面 ===== */

/**
 * fs — 全局文件系统实例。
 * static：仅限 main.c 内部访问，其他文件无法引用。
 * 生命周期贯穿整个程序。
 */
static FileSystem fs;

/**
 * SAVE_FILE — 持久化文件名。
 * tree.dat 存放在程序运行目录下，
 * 启动时自动检测并加载，退出时自动保存。
 */
static const char* SAVE_FILE = "tree.dat";

/** SEP — 屏幕分隔线，用于美化输出。 */
#define SEP "----------------------------------------"

/**
 * trim — 去除字符串两端的空白字符。
 *
 * 算法：
 *   1. start 指针从开头跳过 isspace 字符（左去空）
 *   2. 若 start != str，用 memmove 将剩余字符搬回数组开头
 *      （memmove 而非 strcpy，因为源和目的内存重叠）
 *   3. end 指针从末尾往回跳过 isspace 字符
 *   4. 在 end+1 处写入 '\0' 截断（右去空）
 *   5. memmove,将一块内存中的数据复制到另一块内存
 *
 * 参数：
 *   str — 要修改的字符串（原地操作）
 *
 * 注意：
 *   不能直接用 str++ 跳过前导空格，因为会丢失原始指针，
 *   导致调用方无法访问修改后的字符串。必须 memmove。
 */
static void trim(char* str) {
    char* start = str;
    while (*start && isspace((unsigned char)*start)) start++;
    if (start != str) memmove(str, start, strlen(start) + 1);
    if (*str == '\0') return;
    char* end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    *(end + 1) = '\0';
}

/**
 * readInput — 安全读取一行用户输入。
 *
 * 使用 fgets 而非 scanf/gets：
 *   - gets 不检查缓冲区大小，输入超长导致栈溢出（安全漏洞）
 *   - fgets(buffer, size, stdin) 最多读 size-1 个字符，
 *     自动追加 '\0'，不会溢出
 *
 * 参数：
 *   buffer — 存放输入字符串的缓冲区
 *   size   — 缓冲区大小（字节）
 */
static void readInput(char* buffer, int size) {
    if (fgets(buffer, size, stdin) != NULL)
        buffer[strcspn(buffer, "\n")] = '\0';
}

/**
 * userPathToInternal — 将用户友好路径转换为内部路径。
 *
 * 用户输入路径如 "/folder1/sub" 或 "/"，
 * 内部路径统一以 "/root" 开头，例如：
 *   "/folder1/sub" → "/root/folder1/sub"
 *   "/"            → "/root"
 *   "folder1"      → "/root/folder1"
 *
 * 为什么需要转换：
 *   树结构的根节点名为 "root"，所有路径操作都基于 
 *   findNode 从 /root 开始逐级查找。
 *
 * 参数：
 *   userPath — 用户输入的路径字符串
 *   internal — 输出缓冲区（转换后的内部路径）
 *   size     — 输出缓冲区大小
 */
static void userPathToInternal(const char* userPath, char* internal, int size) {
    if (strcmp(userPath, "/") == 0 || strcmp(userPath, "") == 0)
        strncpy(internal, "/root", size);
    else if (userPath[0] == '/')
        snprintf(internal, size, "/root%s", userPath);
    else
        snprintf(internal, size, "/root/%s", userPath);
}

/* ---------- 分割线与等待 ---------- */

/**
 * printSep — 打印屏幕分割线，美化输出。
 */
static void printSep(void) {
    printf("\n%s\n", SEP);
}

/**
 * waitEnter — 等待用户按回车后继续。
 *
 * 用 fgets 而非 getchar：
 *   getchar 在管道输入（pipe）模式下会吃掉 stdin 中
 *   的后续字符，导致输入错位。fgets 按行读取更安全。
 */
static void waitEnter(void) {
    printf("\n按回车继续操作...");
    char buf[10];
    fgets(buf, sizeof(buf), stdin);
}

/* ---------- 树形可视化 ---------- */

/**
 * printTreeNode — 递归打印一个节点及其子树（树形缩进）。
 *
 * 输出格式：
 *   ├── 文件名 (file)
 *   └── 文件夹名
 *       ├── 子文件 (file)
 *       └── 子文件夹
 *
 * 算法：
 *   1. 打印前缀（上级传递的缩进字符串）
 *   2. 打印 ├──（非最后一个子节点）或 └──（最后一个）
 *   3. 打印节点名，文件类型后附加 (file)
 *   4. 计算子节点的前缀 = 当前前缀 + "│   "（非最后）或 "    "（最后）
 *   5. 递归打印所有子节点
 *
 * 参数：
 *   node   — 当前节点
 *   prefix — 缩进前缀字符串（递归传递）
 *   isLast — 是否为父节点的最后一个子节点
 *            （影响连接线符号和缩进）
 *
 * 时间复杂度：O(n)，n 为子树节点总数
 * 空间复杂度：O(depth)，depth 为树的深度（递归栈）
 */
static void printTreeNode(TreeNode* node, const char* prefix, int isLast) {
    if (node == NULL) return;
    /* 合并为一次 printf，减少 IO 系统调用次数 */
    printf("%s%s%s%s\n", prefix,
           isLast ? "└── " : "├── ",
           node->name, node->isFile ? " (file)" : "");
    char newPrefix[MAX_PATH_LEN];
    sprintf(newPrefix, "%s%s", prefix,
            isLast ? "    " : "│   ");
    TreeNode* child = node->children;
    while (child != NULL) {
        printTreeNode(child, newPrefix, child->next == NULL);
        child = child->next;
    }
}

/**
 * printTree — 打印整棵目录树。
 *
 * 跳过根节点 "root"（用户不关心它的存在），
 * 直接从根的第一个子节点开始打印。
 */
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

/**
 * showMenu — 显示操作菜单。
 * 仅在程序启动时调用一次，后续循环只显示"请选择操作："。
 */
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

/**
 * handleAdd — 处理"新增节点"操作。
 *
 * 流程：
 *   1. 提示用户输入父目录路径
 *   2. 去除两端空格，检查是否为空
 *   3. 提示输入新节点名称，去空格校验
 *   4. 提示输入节点类型（0 文件夹 / 1 文件）
 *   5. 将用户路径转换为内部路径
 *   6. 调用 A 的 addNode 执行新增
 *   7. 输出结果
 *
 * 注意：
 *   atoi("abc") 返回 0，无法区分"合法输入0"和"非法输入"。
 *   改进方案：使用 strtol 替代 atoi。
 */
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

/**
 * handleDelete — 处理"删除节点"操作。
 *
 * 调用 A 的 deleteNode：
 *   1. 先断开父节点的子链表连接（摘除）
 *   2. 再递归释放子树内存（后序遍历）
 *   注意：根节点不能被删除。
 */
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

/**
 * handleRename — 处理"重命名节点"操作。
 *
 * 调用 A 的 renameNode：
 *   会检查同级节点是否有重名冲突。
 */
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

/**
 * showStats — 处理"统计信息"操作。
 *
 * 调用 B 的 countFiles 和 getMaxDepth：
 *   countFiles  — DFS 遍历累计 isFile==true 的节点数
 *   getMaxDepth — 递归计算最大深度（根为第1层）
 * 两次独立的深度优先遍历，时间复杂度 O(n)+O(n)=O(n)。
 */
static void showStats(void) {
    printf("统计信息：\n");
    printf("文件总数: %d\n", countFiles(fs.root));
    printf("文件夹层数: %d\n", getMaxDepth(fs.root));
}

/* ---------- 搜索 ---------- */

/** MAX_RESULTS — 搜索结果数组的最大容量。 */
#define MAX_RESULTS 64

/**
 * handleSearch — 处理"搜索节点"操作。
 *
 * 调用 B 的 search（DFS + strstr 模糊匹配）。
 * 搜索范围包括节点名中任意位置包含关键字的节点。
 *
 * 输出格式：
 *   找到 N 个匹配节点：
 *     1. [类型] 节点名
 *     2. [类型] 节点名
 *
 * 注意：
 *   如果匹配数超过 MAX_RESULTS（64），超出部分被丢弃。
 *   当前搜索是区分大小写的。
 */
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

/**
 * handleSave — 处理"保存到文件"操作。
 *
 * 调用 A 的 saveTree(FILE*, TreeNode*)。
 * saveTree 接收文件指针而非文件名，调用方负责 fopen/fclose。
 *
 * 文件格式（纯文本，由 A 实现）：
 *   节点名
 *   类型（0/1）
 *   子节点...
 *   #END
 *
 * 优点：纯文本可读，便于调试和手工编辑。
 */
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

/**
 * handleLoad — 处理"从文件加载"操作。
 *
 * 调用 A 的 loadTree(FILE*) 读取 tree.dat，
 * 成功后替换当前树（先释放旧树，再指向新树）。
 *
 * 安全性：
 *   如果 loadTree 返回 NULL（文件不存在或格式错误），
 *   不修改 fs.root，旧树保持不变。
 */
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

/**
 * createSampleData — 创建预制演示目录树。
 *
 * 在无存档文件时自动调用，方便答辩时演示功能。
 * 结构：
 *   根目录
 *   ├── 说明.txt
 *   ├── 源代码（文件夹）
 *   │   ├── main.c
 *   │   ├── tree.h
 *   │   ├── tree.c
 *   │   └── stat.c
 *   └── 项目文档（文件夹）
 *       ├── 设计报告.doc
 *       └── 需求分析.doc
 */
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

/**
 * main — 程序入口。
 *
 * 流程：
 *   1. 切换控制台代码页为 UTF-8（防中文乱码）
 *      为什么要做：Windows 默认代码页 936(GBK)，
 *      UTF-8 编码的中文在 GBK 下显示为乱码。
 *      chcp 65001 切换到 UTF-8 代码页。
 *
 *   2. 检测 tree.dat 是否存在
 *      存在 → loadTree 加载存档
 *          → 成功：直接用加载的树
 *          → 失败：创建空树 → 生成演示数据
 *      不存在 → 创建空树 → 生成演示数据
 *
 *   3. 菜单循环（do-while）：
 *      第一次显示完整菜单，后续只显示"请选择操作："。
 *      每次操作后等待用户按回车键继续。
 *
 *   4. 退出时自动保存：
 *      fopen("tree.dat","w") → saveTree → fclose
 *      最后 freeTree 释放所有动态内存。
 *
 * 存储类别补充说明：
 *   fs 是 static 全局变量，存储在静态区，
 *   构造函数 initFileSystem 和 addNode 中的
 *   malloc 分配的内存都在堆上，必须 free。
 */
int main(void) {
    /* 切换控制台代码页到 UTF-8，防止中文字符乱码 */
    system("chcp 65001 >nul");

    /* 尝试自动加载已有存档 */
    FILE* fp = fopen(SAVE_FILE, "r");
    if (fp) {
        TreeNode* saved = loadTree(fp);
        fclose(fp);
        if (saved) {
            /* 加载成功，直接用存档的树 */
            fs.root = saved;
            printf("已自动加载存档 %s\n", SAVE_FILE);
        } else {
            /* 文件存在但格式错误，重新初始化 */
            initFileSystem(&fs);
            createSampleData();
        }
    } else {
        /* 没有存档文件，初始化新树并生成演示数据 */
        initFileSystem(&fs);
        createSampleData();
    }

    /* 菜单循环 */
    showMenu();            /* 第一次显示完整菜单 */
    int choice;
    do {
        char input[10];
        readInput(input, sizeof(input));
        choice = atoi(input);
        switch (choice) {
        case 1: printTree();     break;   /* 查看目录树 */
        case 2: handleAdd();     break;   /* 新增节点   */
        case 3: handleDelete();  break;   /* 删除节点   */
        case 4: handleRename();  break;   /* 重命名节点 */
        case 5: showStats();     break;   /* 统计信息   */
        case 6: handleSearch();  break;   /* 搜索节点   */
        case 7: handleSave();    break;   /* 保存到文件 */
        case 8: handleLoad();    break;   /* 从文件加载 */
        case 0:
            /* 退出确认：防止误触 */
            printf("确认退出？(y/n)：");
            readInput(input, sizeof(input));
            trim(input);
            if (input[0] == 'y' || input[0] == 'Y') {
                printf("退出程序\n");
            } else {
                choice = -1;   /* 设为非0值，继续循环 */
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

    /* 退出时自动保存当前状态到文件 */
    fp = fopen(SAVE_FILE, "w");
    if (fp) { saveTree(fp, fs.root); fclose(fp); }
    /* 释放整棵树的动态内存，防止内存泄漏 */
    freeTree(fs.root);
    return 0;
}

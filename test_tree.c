/*
 * test_tree.c — 成员A的单元测试
 *
 * 编译：cl /utf-8 test_tree.c tree.c
 * 运行：test_tree.exe
 *
 * 每个测试用例打印 [PASS] 或 [FAIL]。
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"

static int passed = 0;
static int failed = 0;

#define TEST(name)  printf("  %-40s ", name)
#define PASS()      do { printf("[PASS]\n"); passed++; } while (0)
#define FAIL(msg)   do { printf("[FAIL] %s\n", msg); failed++; } while (0)
#define CHECK(cond, msg) do { if (cond) PASS(); else FAIL(msg); } while (0)

/* ── 测试1：初始化 ── */
static void test_init(void) {
    printf("── 初始化 ──\n");

    FileSystem fs;
    initFileSystem(&fs);
    TEST("根节点不为空");
    CHECK(fs.root != NULL, "root is NULL");
    TEST("根节点名称为 root");
    CHECK(strcmp(fs.root->name, "root") == 0, "name mismatch");
    TEST("根节点为文件夹");
    CHECK(!fs.root->isFile, "root should be folder");
    TEST("根节点无父节点");
    CHECK(fs.root->parent == NULL, "parent should be NULL");
    TEST("根节点无子节点");
    CHECK(fs.root->children == NULL, "children should be NULL");

    freeTree(fs.root);
}

/* ── 测试2：添加节点 ── */
static void test_add(void) {
    printf("── 添加节点 ──\n");

    FileSystem fs;
    initFileSystem(&fs);

    TEST("在根目录添加文件夹");
    CHECK(addNode(&fs, "/root", "dir1", false), "add failed");

    TEST("在根目录添加文件");
    CHECK(addNode(&fs, "/root", "file.txt", true), "add failed");

    TEST("在子目录添加节点");
    CHECK(addNode(&fs, "/root/dir1", "sub.txt", true), "add failed");

    /* 错误情况 */
    TEST("父路径不存在");
    CHECK(!addNode(&fs, "/root/ghost", "x", false), "should fail");

    TEST("父节点是文件（不能有子节点）");
    CHECK(!addNode(&fs, "/root/file.txt", "x", false), "should fail");

    TEST("重名拒绝");
    CHECK(!addNode(&fs, "/root", "dir1", false), "should reject dup");

    /* TODO-1：补充空名称测试
     * 调用 addNode(&fs, "/root", "", false)，预期返回 false。
     * 当前 addNode 未检查空字符串，需要成员A在 tree.c 中加上校验。
     */
    printf("  (跳过) TODO-1: 空名称拒绝 — 需 tree.c 加校验\n");

    freeTree(fs.root);
}

/* ── 测试3：查找节点 ── */
static void test_find(void) {
    printf("── 查找节点 ──\n");

    FileSystem fs;
    initFileSystem(&fs);
    addNode(&fs, "/root", "a", false);
    addNode(&fs, "/root/a", "b", true);

    TEST("查找根节点 /root");
    CHECK(findNode(&fs, "/root") != NULL, "not found");

    TEST("查找一级子节点 /root/a");
    TreeNode* n = findNode(&fs, "/root/a");
    CHECK(n != NULL && strcmp(n->name, "a") == 0, "not found");

    TEST("查找多级路径 /root/a/b");
    n = findNode(&fs, "/root/a/b");
    CHECK(n != NULL && strcmp(n->name, "b") == 0, "not found");

    TEST("路径不存在返回 NULL");
    CHECK(findNode(&fs, "/root/ghost") == NULL, "should be NULL");

    freeTree(fs.root);
}

/* ── 测试4：删除节点 ── */
static void test_delete(void) {
    printf("── 删除节点 ──\n");

    FileSystem fs;
    initFileSystem(&fs);
    addNode(&fs, "/root", "tmp", false);
    addNode(&fs, "/root/tmp", "child.txt", true);

    TEST("删除叶子节点 /root/tmp/child.txt");
    CHECK(deleteNode(&fs, "/root/tmp/child.txt"), "delete failed");
    TEST("删除后节点不存在");
    CHECK(findNode(&fs, "/root/tmp/child.txt") == NULL, "still exists");

    TEST("删除含子节点的文件夹 /root/tmp");
    CHECK(deleteNode(&fs, "/root/tmp"), "delete failed");
    TEST("删除后文件夹不存在");
    CHECK(findNode(&fs, "/root/tmp") == NULL, "still exists");

    TEST("不能删除根节点");
    CHECK(!deleteNode(&fs, "/root"), "should be rejected");

    TEST("删除不存在的路径");
    CHECK(!deleteNode(&fs, "/root/ghost"), "should fail");

    freeTree(fs.root);
}

/* ── 测试5：重命名 ── */
static void test_rename(void) {
    printf("── 重命名 ──\n");

    FileSystem fs;
    initFileSystem(&fs);
    addNode(&fs, "/root", "old", false);
    addNode(&fs, "/root", "other", false);

    TEST("正常重命名 /root/old → /root/new");
    CHECK(renameNode(&fs, "/root/old", "new"), "rename failed");
    TEST("原名不存在");
    CHECK(findNode(&fs, "/root/old") == NULL, "old still exists");
    TEST("新名存在");
    CHECK(findNode(&fs, "/root/new") != NULL, "new not found");

    TEST("重名冲突拒绝 /root/new → other");
    CHECK(!renameNode(&fs, "/root/new", "other"), "should reject dup");

    TEST("路径不存在");
    CHECK(!renameNode(&fs, "/root/ghost", "x"), "should fail");

    /* TODO-2：补充空名称测试
     * 调用 renameNode(&fs, "/root/other", "")，预期返回 false。
     * 需要成员A在 tree.c 的 renameNode 中加上空名称校验。
     */
    printf("  (跳过) TODO-2: 空名称拒绝 — 需 tree.c 加校验\n");

    freeTree(fs.root);
}

/* ── 入口 ── */
int main(void) {
    printf("\n========== Tree API Unit Tests ==========\n\n");

    test_init();
    test_add();
    test_find();
    test_delete();
    test_rename();

    printf("\n========== 结果：%d 通过, %d 失败 ==========\n", passed, failed);
    return failed > 0 ? 1 : 0;
}

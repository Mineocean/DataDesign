#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"tree.h"

static FileSystem fs;

//1.树形可视化打印
void PrintTreeNode(TreeNode* node, const char* prefix, int isLast) {
	if (node == NULL)return;
	printf("%s", prefix);
	printf(isLast ? "└── " : "├── ");
	printf("%s", node->name);
	if (node->isFile) {
		printf(" (file)");
	}
	else {
		printf("\n");
}
	char newPrefix[512];
	sprintf(newPrefix, "%s%s", prefix, isLast ? "    " : "│   ");
	TreeNode* child = node->children;
	while(child!=NULL){
		int lastChild = (child->next == NULL);
		printTreeNode(child, newPrefix, lastChild);
		child = child->next;
	}
}
void printTree(FileSystem* fs) {
	if (fs->root == NULL||fs->root->name[0] == '\0') {
		printf("树为空\n");
		return;
	}
	printf("\n当前树结构\n");
	TreeNode* root = fs->root->children;
	while (child != NULL) {
		printTreeNode(child, "", child->next == NULL);
		child = child->next;
	}
	printf("\n");
}
//辅助函数
//去除输入字符串两端的空白字符
void trim(char* str) {
	char* end;
	while (isspace((unsigned char)*str)) str++;
	if (*str == 0) return;
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) end--;
	end[1] = '\0';
}
void readInput(char* butter，int size) {
	if (fgets(butter, size, stdin) != NULL{
	butter[strcspn(buffer,"\n")] = "\0"
		}
}
//路径转换
void userPathToInternal(const char* userPath, char* internalPath, int size) {
	if (strcmp(userPath, "/") == 0 || strcmp(userPath, "") == 0) {
		strncpy(internalPath, "/root", size);
	}
	else {
		if (userPath[0] == '/') {
			snprintf(internalPath, size, "/root%s", userPath);
		}
		else {
			snprintf(internalPath, size, "/root/%s", userPath);
		}
	}
}
//统计函数
//统计文件总数
int countFiles(TreeNode* node) {
	if (node == NULL) return 0;
	int count = 0;
	if (node->isFile) {
		count = 1;
	}
	TreeNode* child = node->children;
	while (child != NULL) {
		count += countFiles(child);
		child = child->next;
	}
	return count;
}
//统计最大深度
int getMaxDepth(TreeNode* node) {
	if (node == NULL) return 0;
	int maxDepth = 0;
	TreeNode* child = node->children;
	while (child != NULL) {
		int depth = getMaxDepth(child);
		if (depth > maxDepth) {
			maxDepth = depth;
		}
		child = child->next;
	}
	return maxDepth + 1; // 当前节点算一层
}
//显示统计信息
void showStats(FileSystem* fs) {
	int fileCount = countFiles(fs->root);
	int maxDepth = getMaxDepth(fs->root);
	printf("\n统计信息：\n");
	printf("文件总数: %d\n", fileCount);
	printf("文件夹层数: %d\n", maxDepth);
}
//菜单功能函数
void showMenu() {
	printf("\n========== 简易文件目录树管理 ==========\n");
	printf("1. 查看目录树\n");
	printf("2. 新增文件夹或文件\n");
	printf("3. 删除节点\n");
	printf("4. 重命名节点\n");
	printf("5. 统计文件总数和文件夹层数\n");
	printf("0. 退出\n");
	printf("请选择操作：");
}
void handleAdd() {
	char userPath[256];
	char name[256];
	int isFile;
	printf("请输入父目录路径（如 / 或 /folder1）：");
	readInput(userPath, sizeof(userPath));
	trim(userPath);
	if (strlen(userPath) == 0) {
		printf("无效的路径\n");
		return;
	}

	printf("请输入新节点名称：");
	readInput(name, sizeof(name));
	trim(name);
	if (strlen(name) == 0) {
		printf("无效的名称\n");
		return;
	}

	printf("请输入节点类型（0=文件夹, 1=文件）：");
	char typeInput[10];
	readInput(typeInput, sizeof(typeInput));
	isFile = atoi(typeInput);
	if (isFile != 0 && isFile != 1) {
		printf("无效的节点类型\n");
		return;
	}

	char internalPath[512];
	userPathToInternal(userPath, internalPath, sizeof(internalPath));
	bool success = addNode(&fs, internalPath, name, isFile);
	if (success) {
		printf("节点添加成功\n");
	}
	else {
		printf("节点添加失败,请检查路径是否存在或名称是否重复。\n");
	}
}
void handleDelete() {
	char userPath[256];
	printf("请输入要删除的节点路径（如 /folder1/subfolder）：");
	readInput(userPath, sizeof(userPath));
	trim(userPath);
	if (strlen(userPath) == 0) {
		printf("无效的路径\n");
		return;
	}
	char internalPath[512];
	userPathToInternal(userPath, internalPath, sizeof(internalPath));
	bool success = deleteNode(&fs, internalPath);
	if (success) {
		printf("节点删除成功\n");
	}
	else {
		printf("节点删除失败,请检查路径是否存在。\n");
	}
}
void handleRename() {
	char userPath[256];
	char newName[256];
	printf("请输入要重命名的节点路径（如 /folder1/subfolder）：");
	readInput(userPath, sizeof(userPath));
	trim(userPath);
	if (strlen(userPath) == 0) {
		printf("无效的路径\n");
		return;
	}
	printf("请输入新名称：");
	readInput(newName, sizeof(newName));
	trim(newName);
	if (strlen(newName) == 0) {
		printf("无效的名称\n");
		return;
	}
	char internalPath[512];
	userPathToInternal(userPath, internalPath, sizeof(internalPath));
	bool success = renameNode(&fs, internalPath, newName);
	if (success) {
		printf("节点重命名成功\n");
	}
	else {
		printf("节点重命名失败,请检查路径是否存在或新名称是否与同级节点冲突。\n");
	}
}


int main() {
	initFileSystem(&fs);
	int choice;
	do {
		showMenu();
		char input[10];
		readInput(input, sizeof(input));
		choice = atoi(input);
		switch (choice) {
		case 1:
			printTree(&fs);
			break;
		case 2:
			handleAdd();
			break;
		case 3:
			handleDelete();
			break;
		case 4:
			handleRename();
			break;
		case 5:
			showStats(&fs);
			break;
		case 0:
			printf("退出程序\n");
			break;
		default:
			printf("无效的选择，请重新输入。\n");
		}
	} while (choice != 0);
	return 0;
}
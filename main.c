#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include"tree.h"
//1.树形可视化打印
void printtree(TreeNode* node, const char* prefix, int isLast) {
	printf("%s", prefix);
	printf(isLast ? "└── " : "├── ");
	printf("%s", node->name);
	if (node->isFile) {
		printf(" (file)");
	}
	else {
		printf("\n");
}
	char newPrefix[256];
	sprintf(newPrefix, "%s%s", prefix, isLast ? "    " : "│   ");
	for(int i=0;i<node->childCount;i++){
		int lastChild = (i == node->childCount - 1);
		printtree(node->children[i], newPrefix, lastChild);
	}
}
//2.开始从根节点打印树形结构
void showtree() {
	printf("\n当前树结构\n")
    printtree(root, "", 1);
	printf("\n");
}
//3.交互菜单
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
//去除输入字符串两端的空白字符
void trim(char* str) {
	char* end;
	while (isspace((unsigned char)*str)) str++;
	if (*str == 0) return;
	end = str + strlen(str) - 1;
	while (end > str && isspace((unsigned char)*end)) end--;
	end[1] = '\0';
}
void readInput(char *butter)
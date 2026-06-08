// 递归遍历函数（带回调）
void traverse(TreeNode* node, void (*callback)(TreeNode*, int), int depth) {
    if (!node) return;

    // 调用回调函数处理当前节点
    if (callback) {
        callback(node, depth);
    }

    // 如果是文件夹，递归遍历所有子节点
    if (node->is_folder) {
        for (int i = 0; i < node->child_count; i++) {
            traverse(node->children[i], callback, depth + 1);
        }
    }
}

// 获取文件总数
int getFileCount(TreeNode* root) {
    if (!root) return 0;

    int count = 0;

    // 如果当前节点是文件，计数+1
    if (!root->is_folder) {
        count = 1;
    }

    // 如果是文件夹，遍历所有子节点累加
    if (root->is_folder) {
        for (int i = 0; i < root->child_count; i++) {
            count += getFileCount(root->children[i]);
        }
    }

    return count;
}

// 获取最大深度（根为第1层）
int getMaxDepth(TreeNode* root) {
    if (!root) return 0;

    if (!root->is_folder || root->child_count == 0) {
        return 1; // 叶子节点（文件或空文件夹）深度为1
    }

    int max_child_depth = 0;
    for (int i = 0; i < root->child_count; i++) {
        int child_depth = getMaxDepth(root->children[i]);
        if (child_depth > max_child_depth) {
            max_child_depth = child_depth;
        }
    }

    return max_child_depth + 1;
}

// 获取遍历结果列表（用于给成员C展示）
TraverseResult* getTraverseList(TreeNode* root, int* result_count) {
    if (!root) {
        *result_count = 0;
        return NULL;
    }

    // 先计算节点总数
    *result_count = 0;
    void count_nodes(TreeNode * node, int* count) {
        if (!node) return;
        (*count)++;
        if (node->is_folder) {
            for (int i = 0; i < node->child_count; i++) {
                count_nodes(node->children[i], count);
            }
        }
    }
    count_nodes(root, result_count);




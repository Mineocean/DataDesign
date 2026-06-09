#include <stdio.h>
#include <stdbool.h>
#include "file_system.h"  

void test_add_node(FileSystem* fs) {
    bool result = add_node(fs, "/root", "test_dir", 0);
    if (result) {
        printf("add_node test: Passed - Added test_dir successfully.\n");
    }
    else {
        printf("add_node test: Failed - Could not add test_dir.\n");
    }
}

void test_delete_node(FileSystem* fs) {
    bool result = delete_node(fs, "/root/test_dir");
    if (result) {
        printf("delete_node test: Passed - Deleted test_dir successfully.\n");
    }
    else {
        printf("delete_node test: Failed - Could not delete test_dir.\n");
    }
}

void test_rename_node(FileSystem* fs) {
    add_node(fs, "/root", "old_name", 0);
    bool result = rename_node(fs, "/root/old_name", "new_name");
    if (result) {
        printf("rename_node test: Passed - Renamed old_name to new_name successfully.\n");
    }
    else {
        printf("rename_node test: Failed - Could not rename old_name to new_name.\n");
    }
    delete_node(fs, "/root/new_name");
}

int main() {
    FileSystem fs;
    init_file_system(&fs);

    test_add_node(&fs);
    test_delete_node(&fs);
    test_rename_node(&fs);

    return 0;
}
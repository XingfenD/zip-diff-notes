#!/bin/bash

# 切换到run.sh所在目录
dirname "$0" | grep -q '^/' && cd "$(dirname "$0")" || cd "$(pwd)/$(dirname "$0")"

# 检查是否存在zip_demo.zip文件
if [ ! -f "zip_demo.zip" ]; then
    echo "zip_demo.zip不存在，开始创建..."

    # 创建zip_demo目录
    mkdir -p zip_demo

    # 在zip_demo目录中创建文件
echo "this is a test. from a1" > zip_demo/a1.txt
echo "this is b test. from a2.txt" > zip_demo/a2.txt
echo "ABCDEFGHIJKLMNOPQRSTUVWXYZ" > zip_demo/alphabet.txt

    # 将zip_demo目录压缩为zip文件
    zip -r zip_demo.zip zip_demo

    # 删除zip_demo目录
    rm -rf zip_demo

    echo "zip_demo.zip创建完成"
else
    echo "zip_demo.zip已存在，跳过创建步骤"
fi

# # 编译zip_parser.c为可执行文件
# gcc -o zip_parser.out zip_parser.c

# # 检查编译是否成功
# if [ $? -eq 0 ]; then
#     echo "编译成功"

#     # 调用可执行文件解析zip文件
#     ./zip_parser.out zip_demo.zip
# else
#     echo "编译失败"
#     exit 1
# fi
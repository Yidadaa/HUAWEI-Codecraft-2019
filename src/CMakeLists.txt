# 添加子目录
add_subdirectory(core)

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_LIB_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 指定生成目标
add_executable(CodeCraft-2019 ${DIR_SRCS})

# 链接库
target_link_libraries(CodeCraft-2019 Core)
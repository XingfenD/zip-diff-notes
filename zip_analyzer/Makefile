# ZIP Analyzer Makefile

# 编译器设置
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -I./utils -I./zip_seg -MMD -MP
LDFLAGS =

# 目标文件
TARGET = zip_analyzer.out

SRCS = main.cpp $(wildcard utils/*.cpp) $(wildcard zip_seg/*.cpp)

OBJS = $(SRCS:.cpp=.o)
# 依赖文件
DEPS = $(OBJS:.o=.d)

# 默认目标
all: $(TARGET)

# 链接目标
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

# 编译规则
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# 清理规则
clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

# 重新构建
rebuild: clean all

# 调试模式
debug:
	$(MAKE) CXXFLAGS="$(CXXFLAGS) -g -O0" all

# 包含自动生成的依赖文件
-include $(DEPS)

.PHONY: all clean rebuild debug
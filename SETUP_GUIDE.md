# QEMU ARM Development Environment - 使用指南

## 快速开始

### 1. 获取项目代码

```bash
git clone <your-repo-url>
cd qemu_env
```

### 2. 一键设置（推荐）

```bash
# 运行自动化设置脚本
./setup.sh

# 脚本将按顺序执行：
# [INFO] 安装系统依赖...
# [INFO] 下载QEMU源码...  
# [INFO] 集成自定义设备...
# [INFO] 编译QEMU...
# [INFO] 测试自定义设备...
# [SUCCESS] 完整设置流程完成！
```

### 3. 验证安装

```bash
# 检查QEMU是否编译成功
qemu/build/arm-softmmu/qemu-system-arm --version

# 测试自定义UART
cd examples/m0
make test-custom-uart
```

## 分步设置

如果你想要分步骤执行或者遇到问题需要单独处理某个步骤：

### 步骤1: 安装依赖

```bash
./setup.sh --deps
```

这将安装：
- QEMU构建依赖: `ninja-build`, `meson`, `libglib2.0-dev` 等
- ARM交叉编译工具链: `gcc-arm-none-eabi`, `gdb-multiarch`

### 步骤2: 获取QEMU源码

```bash
./setup.sh --source
```

脚本会尝试：
1. 首先通过 `git submodule` 初始化
2. 如果失败，直接从GitHub克隆QEMU仓库
3. 切换到稳定的v4.2.1版本

### 步骤3: 集成自定义设备

```bash
./setup.sh --integrate
```

这一步会：
- 复制 `custom-devices/` 中的文件到QEMU源码对应位置
- 更新 `hw/char/Makefile.objs` 添加custom-uart构建配置
- 更新 `hw/arm/Makefile.objs` 添加microbit-custom机器类型

### 步骤4: 编译QEMU

```bash
./setup.sh --build
```

编译过程：
- 创建 `qemu/build/` 目录
- 配置构建 (ARM target, debug enabled)
- 使用多核编译 (大约15-30分钟)

### 步骤5: 测试设备

```bash
./setup.sh --test
```

运行测试：
- 编译自定义UART测试程序
- 运行测试并验证输出

## 故障排除

### 常见问题

#### 1. 依赖安装失败

```bash
# 手动安装依赖
sudo apt-get update
sudo apt-get install -y git ninja-build pkg-config python3 meson build-essential \
    libglib2.0-dev libpixman-1-dev zlib1g-dev libfdt-dev \
    gcc-arm-none-eabi binutils-arm-none-eabi libnewlib-arm-none-eabi gdb-multiarch
```

#### 2. QEMU源码下载失败

```bash
# 手动下载QEMU
rm -rf qemu
git clone https://github.com/qemu/qemu.git
cd qemu
git checkout v4.2.1
cd ..
```

#### 3. 编译错误

```bash
# 清理并重新编译
rm -rf qemu/build
./setup.sh --integrate  # 重新集成设备
./setup.sh --build      # 重新编译
```

#### 4. 自定义设备文件缺失

```bash
# 检查自定义设备文件
ls -la custom-devices/
# 应该包含：
# custom-uart.c
# custom-uart.h  
# microbit-custom.c
```

### 调试模式

启用脚本调试输出：

```bash
# 显示详细执行过程
bash -x ./setup.sh --all

# 或者修改脚本，添加调试选项
set -x  # 在脚本开头添加这行
```

### 网络问题

如果在网络受限环境中：

```bash
# 使用系统QEMU (跳过源码编译)
sudo apt-get install qemu-system-arm

# 仅测试示例程序
cd examples/m0  
make QEMU_PATH=qemu-system-arm run
```

## 开发工作流

### 日常开发

1. **修改自定义设备**:
   ```bash
   # 编辑 custom-devices/ 中的文件
   vim custom-devices/custom-uart.c
   
   # 重新集成和编译
   ./setup.sh --integrate
   ./setup.sh --build
   ```

2. **测试更改**:
   ```bash
   ./setup.sh --test
   ```

3. **开发新设备**:
   ```bash
   # 1. 在 custom-devices/ 中创建新设备文件
   # 2. 修改 setup.sh 的 integrate_custom_devices() 函数
   # 3. 运行集成和编译
   ./setup.sh --integrate --build
   ```

### 项目维护

定期更新QEMU源码：

```bash
cd qemu
git fetch origin
git checkout v4.2.1  # 或更新版本
cd ..
./setup.sh --integrate --build
```

## 高级配置

### 自定义编译选项

修改 `setup.sh` 中的配置命令：

```bash
# 在 build_qemu() 函数中修改
../configure --target-list=arm-softmmu --enable-debug --enable-trace-backends=log
```

### 添加更多目标平台

```bash
# 支持更多ARM平台
../configure --target-list=arm-softmmu,aarch64-softmmu --enable-debug
```

### 集成到CI/CD

在 `.github/workflows/` 中使用脚本：

```yaml
- name: Setup QEMU Environment
  run: ./setup.sh --all
```

## 技术支持

如果遇到问题：

1. 检查 `setup.sh` 的输出日志
2. 确保系统满足依赖要求
3. 查看本文档的故障排除部分
4. 提交Issue时附带详细错误信息

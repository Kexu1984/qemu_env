#!/bin/bash

# QEMU ARM Development Environment Setup Script
# 这个脚本将自动处理QEMU源码下载、自定义设备集成和编译

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的信息
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# 检查命令是否存在
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_error "命令 '$1' 未找到，请先安装"
        return 1
    fi
}

# 安装依赖
install_dependencies() {
    print_info "检查并安装依赖包..."
    
    # 检查是否为Ubuntu/Debian系统
    if ! command -v apt-get &> /dev/null; then
        print_warning "此脚本主要支持Ubuntu/Debian系统"
        return 0
    fi
    
    print_info "更新包列表..."
    sudo apt-get update
    
    print_info "安装QEMU构建依赖..."
    sudo apt-get install -y git ninja-build pkg-config python3 meson build-essential \
        libglib2.0-dev libpixman-1-dev zlib1g-dev libfdt-dev
    
    print_info "安装ARM交叉编译工具链..."
    sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
        libnewlib-arm-none-eabi gdb-multiarch
    
    print_success "依赖安装完成"
}

# 下载QEMU源码
download_qemu() {
    print_info "处理QEMU源码..."
    
    if [ -d "qemu" ]; then
        print_warning "QEMU目录已存在"
        read -p "是否重新下载？(y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            print_info "删除现有QEMU目录..."
            rm -rf qemu
        else
            print_info "跳过QEMU下载"
            return 0
        fi
    fi
    
    print_info "初始化QEMU子模块..."
    git submodule update --init --recursive
    
    if [ ! -d "qemu" ]; then
        print_error "QEMU子模块初始化失败"
        print_info "尝试直接克隆QEMU仓库..."
        git clone https://github.com/qemu/qemu.git
        cd qemu
        git checkout v4.2.1  # 使用稳定版本
        cd ..
    fi
    
    print_success "QEMU源码准备完成"
}

# 集成自定义设备到QEMU源码
integrate_custom_devices() {
    print_info "集成自定义设备到QEMU源码..."
    
    if [ ! -d "qemu" ]; then
        print_error "QEMU源码目录不存在，请先运行下载步骤"
        exit 1
    fi
    
    # 检查自定义设备文件是否存在
    local custom_files=(
        "custom-devices/custom-uart.c"
        "custom-devices/custom-uart.h"
        "custom-devices/microbit-custom.c"
    )
    
    for file in "${custom_files[@]}"; do
        if [ ! -f "$file" ]; then
            print_error "自定义设备文件 $file 不存在"
            exit 1
        fi
    done
    
    # 复制自定义UART设备文件
    print_info "复制自定义UART设备文件..."
    cp custom-devices/custom-uart.c qemu/hw/char/
    cp custom-devices/custom-uart.h qemu/hw/char/
    
    # 复制自定义机器类型文件
    print_info "复制自定义机器类型文件..."
    cp custom-devices/microbit-custom.c qemu/hw/arm/
    
    # 更新Makefile.objs文件
    print_info "更新构建配置文件..."
    
    # 添加custom-uart到hw/char/Makefile.objs
    if ! grep -q "custom-uart.o" qemu/hw/char/Makefile.objs; then
        echo "common-obj-\$(CONFIG_ARM) += custom-uart.o" >> qemu/hw/char/Makefile.objs
        print_info "已添加custom-uart到char构建配置"
    fi
    
    # 添加microbit-custom到hw/arm/Makefile.objs
    if ! grep -q "microbit-custom.o" qemu/hw/arm/Makefile.objs; then
        echo "obj-\$(CONFIG_ARM) += microbit-custom.o" >> qemu/hw/arm/Makefile.objs
        print_info "已添加microbit-custom到arm构建配置"
    fi
    
    print_success "自定义设备集成完成"
}

# 编译QEMU
build_qemu() {
    print_info "开始编译QEMU..."
    
    cd qemu
    
    # 创建构建目录
    mkdir -p build
    cd build
    
    print_info "配置QEMU构建 (ARM目标，启用调试)..."
    ../configure --target-list=arm-softmmu --enable-debug
    
    print_info "开始编译 (这可能需要15-30分钟)..."
    local cpu_cores=$(nproc)
    print_info "使用 $cpu_cores 个CPU核心进行并行编译"
    
    make -j$cpu_cores
    
    # 验证编译结果
    if [ -f "arm-softmmu/qemu-system-arm" ]; then
        print_success "QEMU编译成功！"
        print_info "QEMU二进制文件位于: $(pwd)/arm-softmmu/qemu-system-arm"
        
        # 显示版本信息
        print_info "QEMU版本信息:"
        ./arm-softmmu/qemu-system-arm --version
    else
        print_error "QEMU编译失败"
        exit 1
    fi
    
    cd ../..  # 回到项目根目录
}

# 测试自定义设备
test_custom_devices() {
    print_info "测试自定义设备..."
    
    cd examples/m0
    
    print_info "编译测试程序..."
    make clean
    make build-custom-uart
    
    print_info "运行自定义UART测试..."
    if make test-custom-uart; then
        print_success "自定义UART测试通过！"
    else
        print_error "自定义UART测试失败"
    fi
    
    cd ../..  # 回到项目根目录
}

# 显示帮助信息
show_help() {
    echo "QEMU ARM Development Environment Setup Script"
    echo ""
    echo "使用方法:"
    echo "  $0 [选项]"
    echo ""
    echo "选项:"
    echo "  -h, --help          显示此帮助信息"
    echo "  -d, --deps          仅安装依赖"
    echo "  -s, --source        仅下载QEMU源码"
    echo "  -i, --integrate     仅集成自定义设备"
    echo "  -b, --build         仅编译QEMU"
    echo "  -t, --test          仅测试自定义设备"
    echo "  -a, --all           执行完整流程 (默认)"
    echo ""
    echo "示例:"
    echo "  $0                  # 执行完整设置流程"
    echo "  $0 --deps          # 仅安装依赖"
    echo "  $0 --build         # 仅编译QEMU"
}

# 主函数
main() {
    print_info "QEMU ARM Development Environment Setup Script"
    print_info "=================================="
    
    # 检查是否在项目根目录
    if [ ! -f "README.md" ] || [ ! -d "examples" ]; then
        print_error "请在项目根目录运行此脚本"
        exit 1
    fi
    
    # 解析命令行参数
    case "${1:-all}" in
        -h|--help)
            show_help
            exit 0
            ;;
        -d|--deps)
            install_dependencies
            ;;
        -s|--source)
            download_qemu
            ;;
        -i|--integrate)
            integrate_custom_devices
            ;;
        -b|--build)
            build_qemu
            ;;
        -t|--test)
            test_custom_devices
            ;;
        -a|--all|all)
            print_info "执行完整设置流程..."
            install_dependencies
            download_qemu
            integrate_custom_devices
            build_qemu
            test_custom_devices
            print_success "完整设置流程完成！"
            ;;
        *)
            print_error "未知选项: $1"
            show_help
            exit 1
            ;;
    esac
    
    print_success "脚本执行完成！"
}

# 运行主函数
main "$@"

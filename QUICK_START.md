# 🚀 Quick Reference Card

## 一键设置

```bash
git clone <your-repo-url>
cd qemu_env
./setup.sh
```

## 常用命令

| 命令 | 功能 |
|------|------|
| `./setup.sh` | 完整设置流程 |
| `./setup.sh --deps` | 安装依赖 |
| `./setup.sh --build` | 编译QEMU |
| `./setup.sh --test` | 测试设备 |

## 测试自定义UART

```bash
cd examples/m0
make test-custom-uart
```

## 项目结构

```
qemu_env/
├── setup.sh           # 🔧 自动化脚本
├── custom-devices/     # 🔌 自定义设备源码
├── examples/m0/        # 💻 ARM测试程序
└── qemu/              # ⚡ QEMU源码 (自动下载)
```

## 故障排除

```bash
# 重新开始
rm -rf qemu/build
./setup.sh --integrate --build

# 使用系统QEMU
sudo apt-get install qemu-system-arm
cd examples/m0
make QEMU_PATH=qemu-system-arm run
```

## 自定义UART地址

```c
#define CUSTOM_UART_BASE 0x60000000
#define UART_STATUS  (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x00))
#define UART_CONTROL (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x04))
#define UART_DATA    (*(volatile uint32_t*)(CUSTOM_UART_BASE + 0x08))
```

## 帮助

- 查看: `./setup.sh --help`
- 详细文档: `SETUP_GUIDE.md`
- 完整说明: `README.md`

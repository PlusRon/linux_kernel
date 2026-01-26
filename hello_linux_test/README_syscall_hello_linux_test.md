## 一、Kernel 編譯前置準備
* ### Download kernel
   ```
   # 進入 root 模式
   $ sudo su

   # 下載 kernel
   # -P : 指定下載檔案要存放的 目錄 (prefix)
   # ~/ : home directory
   $ wget -P ~/ https://cdn.kernel.org/pub/linux/kernel/v5.x/linux-5.15.137.tar.xz

   # 將檔案解壓縮到 /usr/src 目錄底下
   # -xvf : 解壓縮 (x)、顯示過程 (v)、指定檔案 (f)
   # -C : Change directory
   $ tar -xvf linux-5.15.137.tar.xz -C /usr/src
   ```
* ### update and upgrade apt
   ```
   $ sudo apt update && sudo apt upgrade -y
   ```
* ### Download and install the essential packages to compile kernels
    * 編譯 Linux Kernel 需要一組特定的開發工具和函式庫
        - build-essential : gcc、g++、make
        - libncurses-dev : User Interface
        - libssl-dev :  OpenSSL、eCryptfs/fscrypt、AES/RSA/SHA
        - libelf-dev : ELF (Executable and Linkable Format)
        - flex : lexer 字串分析器產生器
        - bison : parser 語法分析器產生器
   ```
   $ sudo apt install build-essential libncurses-dev libssl-dev libelf-dev bison flex -y
   ```
* ### Download vim
    - 多模式操作（**普通模式、插入模式、命令模式**）
   ```
   $ sudo apt install vim -y
   ```
* ### Clean up your installed packages
    * sudo apt clean : 清除快取檔案（ `/var/cache/apt/archives/`）
    * sudo apt autoremove : 移除系統中不再需要的依賴套件
   ```
   $ sudo apt clean && sudo apt autoremove -y
   ```
* ### 新增 測試用的 system call (kernel space code)
    - <font size=3>建立 `hello_linux_test` 資料夾</font>
        ```
        # 到解壓縮的 linux kernel code 檔案
        $ cd /usr/src/linux-5.15.137

        # 創建 hello_linux_test 資料夾
        $ mkdir hello_linux_test
        $ cd hello_linux_test
        ```
    - <font size=3>`hello_linux_test` 目錄下建立 `hello_linux_test.c`撰寫內容</font>
        ```
        $ vim hello_linux_test.c
        ```
    - <font size=3> `hello_linux_test` 目錄下再建立一個 Makefile</font>
        ```
        $ vim Makefile
        ```
        - <font size=3>Makefile 的內容</font>
          ```c=
          obj-y := hello_linux_test.o
          ```
    - <font size=3>回 kernel code 頂目錄改 Makefile</font>
        ```
        # 回頂目錄
        $ cd ..

        # 打開頂目錄下的 主 Makefile
        $ vim Makefile
        ```
        - <font size=2> 利用 `?` 搜尋 Makefile 當中的 `core-y`，並在最後面補上 `hello_linux_test/`</font>
            * <font size=2>告訴 `Kbuild` 編譯流程要進入 `hello_linux_test/` 子目錄，並讀取該目錄下的 Makefile</font>
            * <font size=2>將 `hello_linux_test/` 目錄納入整個 kernel 的編譯範圍</font>
            ```
            core-y += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ hello_linux_test/
            ```
    - <font size=3>修改 `syscall_64.tbl` 註冊檔</font>
        ```
        vim arch/x86/entry/syscalls/syscall_64.tbl
        ```
        - <font size=2>在最後一行添加上 system call，然後請把編號記住, 等一下會用</font>
            ![image.png](https://hackmd.io/_uploads/BycWdTeQp.png)

    - <font size=3>編輯 `syscalls.h` 檔，將 syscall 的原型添加進檔案 (`#endif` 之前)</font>
        ```
        # 回頂目錄
        $ cd /usr/src/linux-5.15.137
        
        $ vim include/linux/syscalls.h

        
        # 直接將這行加在檔案的最下面 #endif 前
        # 註: 在 vim 編輯檔案的時候 可以按 shift + G 就會跳到最後一行
        asmlinkage long sys_hello_linux_test(void);
        ```
        ![image.png](https://hackmd.io/_uploads/rya8HiQQa.png)
* ### 用 `make oldconfig`、`make localmodconfig`、`make menuconfig` 來設定組態 : 需要 ncurses 開發套件支援
   - `.config` 為專案的 **編譯設定檔**，由 `.config` 來決定 `make` 要編譯的模組或功能，決定了編譯出來的 kernel 或工具具備哪些功能
        ```
        # 複製舊版本 .config
        $ cp /boot/config-$(uname -r) .config
      
        # 根據新的核心原始碼，逐一檢查舊 .config 裡是否缺少選項
        $ make oldconfig (擇一 : 完整相容)
    
        # 建議用 make localmodconfig 只抓 kernel 需要的 config (接上需要的硬體)
        # 可大幅降低 kernel 編譯時間，會跳出一大堆問你要不要裝的套件，全按 enter 跳過就好
        $ make localmodconfig (擇一 : 精簡核心)
        
        # 跳出一個畫面，先按 save 存下 .config 編譯設定檔，再按 exit
        $ make menuconfig 
        ```
        ![image](https://hackmd.io/_uploads/Hyq-nWQrT.png)
   -  SUCCESS 驗證
        ```
        # 指令確認
        $ grep -E "CONFIG_VMD=|CONFIG_PCI=|CONFIG_NVME_PCI=|CONFIG_PCI_MSI=|CONFIG_PCI_MSI_IRQ_DOMAIN=|CONFIG_BLK_DEV_SD=|CONFIG_BLK_DEV_NVME=|CONFIG_SCSI=|CONFIG_MSDOS_PARTITION=|CONFIG_EFI_PARTITION=|CONFIG_X86_X2APIC=|CONFIG_SATA_AHCI=|CONFIG_EXT4_FS=|CONFIG_DEVTMPFS=" .config
         ```
         ```
         : output
         CONFIG_X86_X2APIC=y
         CONFIG_MSDOS_PARTITION=y
         CONFIG_EFI_PARTITION=y
         CONFIG_PCI=y
         CONFIG_PCI_MSI=y
         CONFIG_PCI_MSI_IRQ_DOMAIN=y
         CONFIG_VMD=y
         CONFIG_DEVTMPFS=y
         CONFIG_BLK_DEV_NVME=y
         CONFIG_SCSI=y
         CONFIG_BLK_DEV_SD=y
         CONFIG_SATA_AHCI=m
         CONFIG_EXT4_FS=y
         ```
* ### 查看 Virtual Machine 的核心
   ```
   # 顯示目前系統可用的 CPU 核心數（ processing units ）
   $ nproc
   :16
   ```
* ### Make (盡量用核心數的 $1/2$ ~ $3/4$ 去編譯)
   - `make` : 預設是 **單工模式**，一次只編譯一個檔案
   - `make -jN` : make 會同時開啟 **N 個工作 ( jobs )**，並行編譯
      ```
      # 若失敗要重編才需要先清除
      $ make clean
      $ make prepare
        
      # 用 1/3 核心去編譯 kernel
      $ make -j12 2>&1 | tee ./logs/hello_linux_test.log | grep --color -iE "error|fail|warning|$"

      # 自動依照 CPU 核心數決定編譯工作數量
      $ make -j$(nproc)
      ```
      ```
      : output
      MKPIGGY arch/x86/boot/compressed/piggy.S
      AS      arch/x86/boot/compressed/piggy.o
      LD      arch/x86/boot/compressed/vmlinux
      ZOFFSET arch/x86/boot/zoffset.h
      OBJCOPY arch/x86/boot/vmlinux.bin
      AS      arch/x86/boot/header.o
      LD      arch/x86/boot/setup.elf
      OBJCOPY arch/x86/boot/setup.bin
      BUILD   arch/x86/boot/bzImage
      Kernel: arch/x86/boot/bzImage is ready  (#2)
      ```
   - SUCCESS 驗證
      ```
      # 指令確認
      $ echo $?
      : 0
        
      # 查看檔案存在 時間戳為剛剛 
      # 核心主體以及 syscall 都在 bzImage
      # arch/x86/boot/bzImage: 編譯成功後，Linux 核心二進位檔存放的固定路徑
      $ ls -lh arch/x86/boot/bzImage
      : -rw-r--r-- 1 root root 17M Jan 22 19:18 /boot/initrd.img-5.15.137
      ```
* ### Install kernel - 1 : 安裝編譯好的 kernel module 到 系統目錄 `/module/`
   - 將先前編譯好的核心模組（`.ko` 檔案）安裝到 `/lib/modules/<kernel-version>/` 目錄下，並建立相應的模組依賴檔 `modules.dep` 
      ```
      $ sudo make modules_install -j12 2>&1 | tee ./logs/hello_linux_test.log | grep --color -iE "error|fail|warning|$"
      ```
   - SUCCESS 驗證
      ```
      # 指令確認
      $ echo $?
      : 0

      # 檢查模組目錄
      $ ls -l /lib/modules/5.15.137
      total 6120
      lrwxrwxrwx  1 root root      23 Jan 21 16:47 build -> /usr/src/linux-5.15.137
      drwxr-xr-x 14 root root    4096 Jan 22 19:15 kernel
      -rw-r--r--  1 root root 1476298 Jan 22 19:15 modules.alias
      -rw-r--r--  1 root root 1442032 Jan 22 19:15 modules.alias.bin
      -rw-r--r--  1 root root   10937 Jan 22 19:15 modules.builtin
      -rw-r--r--  1 root root   10612 Jan 22 19:15 modules.builtin.alias.bin
      -rw-r--r--  1 root root   13175 Jan 22 19:15 modules.builtin.bin
      -rw-r--r--  1 root root   88134 Jan 22 19:15 modules.builtin.modinfo
      -rw-r--r--  1 root root  713378 Jan 22 19:15 modules.dep
      -rw-r--r--  1 root root  962841 Jan 22 19:15 modules.dep.bin
      -rw-r--r--  1 root root     337 Jan 22 19:15 modules.devname
      -rw-r--r--  1 root root  229603 Jan 22 19:15 modules.order
      -rw-r--r--  1 root root    1586 Jan 22 19:15 modules.softdep
      -rw-r--r--  1 root root  573721 Jan 22 19:15 modules.symbols
      -rw-r--r--  1 root root  701791 Jan 22 19:15 modules.symbols.bin
      lrwxrwxrwx  1 root root      23 Jan 22 19:15 source -> /usr/src/linux-5.15.137

      # 檢查磁碟佔用（確認有寫入檔案）
      $ du -sh /lib/modules/5.15.137
      : 112M	/lib/modules/5.15.137
      ```











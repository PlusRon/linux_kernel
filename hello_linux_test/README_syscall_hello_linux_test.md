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




# Linux OS Kernel - Side Projects
Custom System Call implemented for manual PGD-to-PTE traversal to retrieve physical addresses from virtual memory mappings 
of target processes.

## Outline
- [專案目標](#一專案目標)
- [環境與系統 (Environment)](#二環境與系統-environment)
- [編譯核心程式 (Implement)](#三編譯核心程式-implement)
- [成果 (Result)](#四成果-result)
- [問題與探討 (Issues and Discussions)](#五問題與探討-issues-and-discussions)
- [專案檔案結構 (Structure of Project)](#六專案檔案結構-structure-of-project)
- [參考 (Reference)](#七參考-reference)

### 一、專案目標　
#### [NCU 許富皓 教授 Guide line](https://staff.csie.ncu.edu.tw/hsufh/COURSES/FALL2023/linux_project_1.html)
撰寫 system call 利用 `user_get_physical_addresses()` 進行呼叫，來獲得每個 process 的 virtual address 對應到的 physical address。

接著寫一個 multi-thread 的程式，觀察被這些 threads 所共享的 memory areas，了解各個 segment 的共用狀況，並繪圖說明。
* text segments (code segment)
* data segments (**global / static** variables with **initial values**)
* BSS segments (**global / static** variables with **uninitial values**)
* heap segments (dynamic variables)
* stack segments (local variables)
* libraries
* thread local storages
  
### Virtual Memory Area (VMA) 區塊圖
共同點 ： Stack、TLS、Shared Memory/library (printf) 都是由 kernel 透過 `mmap()` 分配的區域。在「高位址到低位址」的排列裡，它們會依序出現，看起來像混在一起，但其實是 **相鄰而非重疊**
  - Shared Memory (printf 所在的 libc)
    - `printf()` 函式來自共享函式庫 (glibc)
透過 `mmap()` 映射到程式的位址空間
    - 屬於 memory-mapped region，通常在 heap 上方、stack 下方
  - Main thread stack (local_variable)
    - 每個 thread 都有獨立的 stack，`main()` 的區域變數就放在這裡
    - 位於高位址區域，往下成長
  - Thread_1 / Thread_2 stack (local_variable)
    - 每個 thread 建立時，kernel 會分配獨立的 stack，區域變數就放在這裡
    - 位於在高位址區域，往下成長
  - Main thread local storage (TLS)
    - TLS 是 thread 專屬的資料區，通常由 runtime 透過 `mmap()` 分配
    - 位置緊鄰自己的 stack，不是 heap，也不是 stack，而是有獨立的固定大小區塊
  - Thread_1 / Thread_2 local storage (TLS)
    - 每個 thread 都有自己的 TLS，分配方式和 main thread 類似
    - 位置通常緊鄰在各自 stack 附近





<p align="left">
  <img src="images/VMA_Theory.png" alt="Theory of Virtual Memory Area(VMA)" style="width:44.5%">
  <img src="images/VMA_Theory_Expected_Result.png" alt="Expected Result of Virtual Memory Area(VMA)" style="width:53%">
</p>

只要 variable 的 virtual address 指向相同的 physical address 時，就代表他們有 share memory，所以主要的目標分為以下：
1. 寫一個 system call 負責將 virtual address 轉成 physical address。
2. 將此 system call 編入 kernel 當中。
3. 利用 multi-thread 呼叫此 system call，觀察其 physical address 是否相同，可以了解其中的共享狀況。

### 二、環境與系統 (Environment)
|系統環境|CPU|OS|Kernel|
|:---:|:---:|:---:|:---:|
|**版本**|13th Gen Intel(R) Core(TM) i7-13620H|ubuntu 24.04.3 desktop|5.15.137|
|**下載**||https://ubuntu.com/download/desktop|https://www.kernel.org/|

### 三、編譯核心程式 (Implement)
 * #### Test Compile Kernel by [`sys_hello_linux_test`](hello_linux_test)
   - ##### [編譯前置作業、新增測試 `sys_hello_linux_test`、編譯過程、除錯DEBUG](hello_linux_test/README_syscall_hello_linux_test.md)
 * #### Add syscall [`sys_get_physical_addresses`](get_physical_addresses) to get physical address from virtual address traversal level by level.
   - ##### [設計 5-Layer Page Table Traversal(PGD、P4D、PUD、PMD、PTE) 、新增 `sys_get_physical_addresses`、編譯過程](get_physical_addresses/README_syscall_get_physical_addresses.md)

### 四、成果 (Result)
  * #### Each Segment of Virtual Memory Area(VMA) for Main_thread, Thread_1, Thread_2
    - 多執行緒的 stack 分配方式
      - 個 thread 建立時，kernel 會透過 `mmap()` 分配一塊獨立的 stack
      - stack 不一定都在「最高位址」，而是分散在 mmap 區域裡
      -  thread_1 / thread_2 的 stack 可能被分配在 shared libraries 區域的下方或旁邊
    - 共享函式庫的位置
      - `printf()` 來自 glibc，它的程式碼段是透過 `mmap()` 映射的
      - 通常載入在 固定的高位址區域（例如 `0x7ffff7...`）
      - 如果 thread stack 被分配在更低的位址，那麼你就會觀察到 shared library 在 thread stack 的高位址方向
    - 位址空間隨機化 (ASLR)
      - Linux 啟動程式時會啟用 ASLR (Address Space Layout Randomization)
      - 會讓 stack、mmap、shared library 的基底位址在每次執行時都有隨機偏移
      - 所以不同執行可能會看到 shared memory / libraries 和 thread stack 的相對位置不一樣

    <img src="images/VMA_Implement_Actual_Result.png" alt="Implement actual result of Virtual Memory Area(VMA) for Main, Thread_1 and Thread_2" style="width:70%">
  * #### User Space (`bash build.sh`)
    <img src="get_physical_addresses/pic/Main_Thread1_execute_success.png" alt="Result of Main_thread and Thread_1 in user space" style="width:100%">
  * #### Kernel Space (`sudo dmesg`)
    <p align="left">
      <img src="get_physical_addresses/pic/dmesg/dmesg_1.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_2.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_3.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_4.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_5.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_6.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_7.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_8.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_9.png" alt="Result of dmesg in user space" style="width:19%">
      <img src="get_physical_addresses/pic/dmesg/dmesg_10.png" alt="Result of dmesg in user space" style="width:19%">
    </p>
    
### 五、問題與探討 (Issues and Discussions)
  * #### shim signature of Security Boot failed.
    ```
    # ERROR
    make[1]: *** No rule to make target ‘certs/rhel.pem’, 
    needed by ‘certs/x509_certificate_list’. 
    Stop. make: *** [Makefile:1729: certs] Error 2

    # Solution : 將此兩變量賦值為空值 不去找 shim signature 路徑
    CONFIG_SYSTEM_TRUSTED_KEYS=""
    CONFIG_SYSTEM_REVOCATION_KEYS=""
    ```
    - kernel resource 預設保留 Ubuntu 或 Debian 使用的金鑰路徑 `/usr/local/share/certs/xxx.pem`，自行環境編譯時，核心找不到該檔案，所以報錯
    - 將原金鑰檔路徑設為空字串 `""`，編譯時不要內建任何信任金鑰，也不檢查外部模組的簽名，解決編譯期找不到金鑰簽證檔的問題
  * #### pahole of `BTF_DEBUG` is not available.
    ```
    # ERROR
    BTF: .tmp_vmlinux.btf: pahole (pahole) is not available
    Failed to generate BTF for vmlinux
    Try to disable CONFIG_DEBUG_INFO_BTF
    make: *** [Makefile:1227: vmlinux] Error 1
   
    # Solution : 
    $ cd /usr/src/linux-5.15.137中
    $ vim .config 
    # 依序將這兩個進行 disable
    CONFIG_DEBUG_INFO = n
    CONFIG_DEBUG_INFO_BTF = n
    ```
    - 安裝 pahole，就不需要手動去修改 `.config` 關閉兩個選項，編譯程序會自動偵測到pahole的存在並順利完成 BTF 的生成
      ```
      sudo apt-get update
      sudo apt-get install dwarves
      ```
    - pahole : Poke-a-hole
      - 記憶體對齊(Alignment) 工具
      - Linux 核心的除錯資訊通常很大(DWARF 格式)，為了讓 eBPF 的現代監控工具能輕量化運作，Linux 引入了 BTF (BPF Type Format)。pahole 會讀取編譯器產生的肥大 DWARF 資訊，將其濃縮成精簡的 BTF 格式
  * #### `make -j$(nproc)` 失敗
    ```
    # ERROR
    sed: can't read modules.order: No such file or directory
    make: *** [Makefile:1544: __modinst_pre] Error 2

    # Solution : 
    重新編譯 make -j$(nproc)
    ```
  * #### DKMS (Dynamic Kernel Module Support) 編譯錯誤
    ```
    # ERROR
    Error! One or more modules failed to install during autoinstall.
    Refer to previous errors for more information. 
    * dkms: autoinstall for kernel 5.15.137 [fail] run-parts: 
    /etc/kernel/postinst.d/dkms exited with return code 11make: 
    *** [arch/x86/Makefile:266: install] Error 11

    # Solution : 
    # 查看具體的編譯日誌
    $ ls /var/lib/dkms/
    # 查看目前的 dkms 狀態
    $ dkms status
    # 移除失敗的模組（以 nvidia 為例，版本號請依 status 結果而定）  
    $ sudo dkms remove -m mt7902 -v 0.0.1 -k 5.15.137
    # 再次嘗試 $ sudo make install 就不會再報錯
    ```
  * #### bad shim signature :  UEFI 韌體 偵測到自行編譯的核心 沒經過數位簽署，拒絕啟動該核心
    ```
    # ERROR
    Loading Linux 5.15.137...
    error : bad shim signature.
    loading initial ramdisk...
    error: you need to load the kernel first.

    # Solution : 
    Method_1. 關閉 Secure Boot (最快且最推薦)
    Method_2. 產生金鑰並將其匯入 UEFI，然後簽署核心
    ```
    - UEFI 的硬體檢測 Secure Boot 拒絕啟動核心，因為自己編譯的 vmlinuz 的驗證檔是空的、沒簽章，為身分不明的執行檔
    - UEFI 韌體只認微軟（Microsoft）或大型廠商的公鑰
    - 使用 openssl 產生一對 MOK (Machine Owner Key)，自編核心沒蓋章不放行
      - 進入 UEFI 的特殊介面，把公鑰匯入到硬體的信任資料庫中
      - 正常編譯核心完後，使用 sbsigntools 的工具，手動將私鑰蓋在 vmlinuz 檔案上
  * #### initramfs (initial RAM filesystem) : 抓不到臨時根目錄
    ```
    # ERROR
    (initramfs) exit
    ALERT! UUID=46a...5921 does not exist . Dropping to a shell!

    (initramfs) ls /sys/bus/pci/devices/*/device
    0xa715
    ....
    ....
    0x7902

    (initramfs) dmesg | grep -i vmd
    ACPI: UEFI 0x00000716A.... (v01 INTEL RstVmdE 00000000 INTL 00000000)
    ACPI: UEFI 0x000007169.... (v01 INTEL RstVmdV 00000000 INTL 00000000)

    (initramfs) dmesg | grep -i nvme
    ls: /dev/nvme*: No such file or directory

    (initramfs) ls /sys/class/nvme

    (initramfs) cat /proc/partitions
    major minor #block name

    (initramfs) dmesg | grep -i error
    [Firmware Bug] : TSC ADJUST differ with in socket(s), fixing all errors
    RAS: Correctable Errors collector initialized 
    ```
    - **UEFI/BIOS**：識別硬體，並根據 VMD 設定將 NVMe 硬碟掛在 VMD 控制器後方
    - **Kernel 啟動**：載入臨時的 initramfs（存在於 RAM 中的小檔案系統）
    - **尋找真正 root 根目錄**：kernel 嘗試根據 `root=UUID=...` 去尋找硬碟
    - 但如果 **initramfs 裡面沒有 VMD 驅動程式 (vmd.ko)**，kernel 就無法用 VMD 控制器查找到下方的硬碟。看不見硬碟，就抓不到根目錄，卡在 initramfs 殼層（shell）

### 六、專案檔案結構 (Structure of Project)
```
│  README.md
│
├─get_physical_addresses
│  │  README_syscall_get_physical_addresses.md
│  │
│  ├─kernel_space_code
│  │      get_physical_addresses.c
│  │      Makefile
│  │
│  ├─user_space_code
│     │  build.sh
│     │
│     ├─build
│     │      main.exe
│     │
│     ├─include
│     │      thread_TLS.h
│     │      thread_work_function.h
│     │      user_get_physical_addresses.h
│     │
│     └─src
│            main.c
│            thread_work_function.c
│            user_get_physical_addresses.c
│  
│
├─hello_linux_test
│  │  README_syscall_hello_linux_test.md
│  │
│  ├─kernel_space_code
│  │      hello_linux_test.c
│  │      Makefile
│  │      module_hello_linux_test.c
│  │      pro_module_hello_linux_test.c
│  │
│  └─user_space_code
│      │  build.sh
│      │
│      ├─build
│      │      main.exe
│      │
│      ├─include
│      │      user_hello_linux_test.h
│      │
│      └─src
│              main.c
│              user_hello_linux_test.c
│
└─images
        Memory_Segment.png
```
### 七、參考 (Reference)
  * ##### [How to get physical address (Memory Management)](https://stackoverflow.com/questions/41090469/linux-kernel-how-to-get-physical-address-memory-management)
  * ##### [Page Table Traversal 機制 程式碼實現](https://zhuanlan.zhihu.com/p/436879901)
  * ##### [`copy_from_user()`用法](https://www.cnblogs.com/Rainingday/p/12618715.html)
  * ##### [`copy_to_user()`用法](https://blog.csdn.net/qq_30624591/article/details/88544739)
  * ##### [`/arch/alpha/include/asm/page.h` #define](https://elixir.bootlin.com/linux/v4.6/source/arch/alpha/include/asm/page.h#L10)
  * ##### [如何開啟 GRUB 選單](https://magiclen.org/grub-menu/)
## 授權協議 (License)
本專案採用 [MIT License](LICENSE) 授權

## 一、Kernel Space Code 
   ```
   # 進入 root 模式
   $ sudo su
   ```
* ### 新增 system call 
    - <font size=3>建立 `get_physical_addresses` 資料夾</font>
        ```
        $ cd /usr/src/linux-5.15.137

        # 創建 get_physical_addresses 資料夾
        $ mkdir get_physical_addresses
        $ cd get_physical_addresses
        ```
    - <font size=3>`get_physical_addresses/` 目錄下建立 `get_physical_addresses.c`撰寫內容</font>
        ```
        $ vim get_physical_addresses.c
        ```
    - <font size=3> `get_physical_addresses/` 目錄下再建立一個 Makefile</font>
        ```
        $ vim Makefile
        ```
        - <font size=3>Makefile 的內容</font>
          ```c=
          obj-y := get_physical_addresses.o
          ```
    - <font size=3>回 kernel code 頂目錄改 Makefile</font>
        ```
        # 回頂目錄
        $ cd ..

        # 打開頂目錄下的 主 Makefile
        $ vim Makefile
        ```
        - <font size=2> 利用 `?` 搜尋 Makefile 當中的 `core-y`，並在最後面補上 `get_physical_addresses/`</font>
            * <font size=2>告訴 `Kbuild` 編譯流程要進入 `get_physical_addresses/` 子目錄，並讀取該目錄下的 Makefile</font>
            * <font size=2>將 `get_physical_addresses/` 目錄納入整個 kernel 的編譯範圍</font>
            ```
            core-y += kernel/ certs/ mm/ fs/ ipc/ security/ crypto/ hello_linux_test/ get_physical_addresses/
            ```
    - <font size=3>修改 `syscall_64.tbl` 註冊檔</font>
        ```
        vim arch/x86/entry/syscalls/syscall_64.tbl
        ```
        - <font size=2>在最後一行添加上 system call，然後請把編號記住, 等一下會用</font>
            ```
            450  common  get_physical_addresses  sys_get_physical_addresses
            ```
            ![image.png](https://hackmd.io/_uploads/BycWdTeQp.png)

    - <font size=3>編輯 `syscalls.h` 檔，將 syscall 的原型添加進檔案 (`#endif` 之前)</font>
        ```
        # 回頂目錄
        $ cd /usr/src/linux-5.15.137
        
        $ vim include/linux/syscalls.h
        
        # 直接將這行加在檔案的最下面 #endif 前
        # 註: 在 vim 編輯檔案的時候 可以按 shift + G 就會跳到最後一行
        asmlinkage long sys_get_physical_addresses(unsigned long __user* initail, unsigned long __user* result);
        ```
        ![image.png](https://hackmd.io/_uploads/rya8HiQQa.png)
* ### 查看 Virtual Machine 的核心
   ```
   # 顯示目前系統可用的 CPU 核心數（ processing units ）
   $ nproc
   :16
   ```
* ### Make (盡量用核心數的 $1/2$ ~ $3/4$ 去編譯)
   - `make -jN` : make 會同時開啟 **N 個工作 ( jobs )**，並行編譯
      ```
      # 若失敗要重編 才需要先清除
      $ make clean
      $ make prepare
        
      # 用 1/3 核心去編譯 kernel
      $ make -j12 2>&1 | tee ./logs/get_physical_addresses.log | grep --color -iE "error|fail|warning|$"

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
      $ sudo make modules_install -a -j12 2>&1 | tee ./logs/get_physical_addresses.log | grep --color -iE "error|fail|warning|$"
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
   - ERROR
      ```
      # make -j$(nproc) 失敗
      sed: can't read modules.order: No such file or directory
      make: *** [Makefile:1544: __modinst_pre] Error 2
      ```
      - Solution
        - 重新編譯 `make -j$(nproc)`
* ### Install kernel - 2 : 安裝編譯好的 kernel image  到 系統目錄 `/boot/` 與 設定開機載入
   - 根據 `Makefile` 的規則，把編譯好的檔案安裝到正確位置
      - 將核心映像檔（例如 `bzImage`）複製到 `/boot/`
      - 安裝 `System.map`（符號表）到 `/boot/`
      - 安裝 `.config`（編譯設定檔）到 `/boot/`
         ```
         $ sudo make install -j12 2>&1 | tee -a ./logs/get_physical_addresses.log | grep --color -iE "error|fail|warning|$"
         ```
   - SUCCESS 驗證
      ```
      # 指令確認
      $ echo $?
      : 0

      # 會將核心檔案正式放入開機磁碟中
      # 有三個檔案 且 initrd.img 的大小通常在 50MB ~ 150MB 之間
      $ ls -lh /boot/vmlinuz-5.15.137 /boot/initrd.img-5.15.137 /boot/System.map-5.15.137
      -rw-r--r-- 1 root root  17M Jan 22 19:18 /boot/initrd.img-5.15.137
      -rw-r--r-- 1 root root 6.1M Jan 22 19:16 /boot/System.map-5.15.137
      -rw-r--r-- 1 root root  11M Jan 22 19:16 /boot/vmlinuz-5.15.137
      ```
   - ERROR
     - 典型 **DKMS (Dynamic Kernel Module Support)** 編譯錯誤
       - 執行 `sudo make install` 時，系統觸發了 **掛鉤腳本（hook）**，自動為新核心安裝第三方驅動（例如 NVIDIA 顯卡、VirtualBox 或網卡驅動）。
       - **錯誤代碼 11** 通常代表這些驅動無法在你新編譯的 `5.15.137` 核心上完成編譯
      ```
      Error! One or more modules failed to install during autoinstall.
      Refer to previous errors for more information. 
      * dkms: autoinstall for kernel 5.15.137 [fail] run-parts: 
      /etc/kernel/postinst.d/dkms exited with return code 11make: 
      *** [arch/x86/Makefile:266: install] Error 11
      ```
   - Solution - 1
     - 若編譯核心只是為了測試，並不立即需要 NVIDIA 顯卡或其他外加驅動，可以暫時忽略這個錯誤
     ```
     # 因為 $ make install 已經把 **核心檔案（vmlinuz）** 複製到 `/boot` 中
     $ ls -l /boot/vmlinuz-5.15.137*
     ```
   - Solution - 2
     - 徹底解決錯誤 ： 需要這些驅動所以要找出那一個驅動失效
     ```
     # 查看具體的編譯日誌   
     $ ls /var/lib/dkms/
                    
     # 該目錄下的 build/make.log 查看具體錯誤訊息
     # 通常是因為新核心的 API 變動，導致舊版的第三方驅動源碼不相容

     # 暫時移除衝突的 DKMS 模組
     # 某個特定模組一直報錯導致 make install 中斷，可以先手動移除該模組對新核心的關聯
                    
     # 查看目前的 dkms 狀態
     $ dkms status

     # 移除失敗的模組（以 nvidia 為例，版本號請依 status 結果而定）  
     $ sudo dkms remove -m mt7902 -v 0.0.1 -k 5.15.137
                    
     # 再次嘗試 $ sudo make install 就不會再報錯
     ```
* ### Reboot重開機
  ```
  $ reboot
  ```
* ### 連按 `F4` 開啟 GRUB 選單，可以選擇 kernel 開機
   - [安裝 kernel 重啟後版本沒有更新 調出 GRUB 引導介面方法](https://magiclen.org/grub-menu/)
   - 選 `Advanced option for Ubuntu`
   - 選擇開機 kernel kernel 版本  `5.15.137`
   - ERROR 1 : bad shim signature
      ```
      # 表示 UEFI 韌體 偵測到你自行編譯的核心 沒有經過數位簽署，因此拒絕啟動該核心
      Loading Linux 5.15.137...
      error : bad shim signature.
      loading initial ramdisk...
      error: you need to load the kernel first.
      ```
      - Solution - 1 : 關閉 Secure Boot (最快且最推薦)
      - Solution - 2 : 手動簽署你的核心 (進階)
         ```
         # 自己產生金鑰並將其匯入 UEFI，然後簽署核心
         $ sudo apt install sbsigntool

         # 在 /var/lib/shim-signed/mok/ 下，查看是否已經有 MOK (Machine Owner Key) 金鑰
         # 會有 MOK.priv 與 MOK.der

         # 簽署核心檔案
         $ sudo sbsign --key /var/lib/shim-signed/mok/MOK.priv --cert /var/lib/shim-signed/mok/MOK.der /boot/vmlinuz-5.15.137 --output /boot/vmlinuz-5.15.137

         ```
   - ERROR 2 : initramfs (initial RAM filesystem)
     ```
         (initramfs) exit
         ALERT! UUID=46a...5921 does not exist . Dropping to a shell!

         # pci bus 有偵測到裝置
         (initramfs) ls /sys/bus/pci/devices/
         0000:00:0a.0 0000:00:14.2 ............
         ......................... 0000:00:1f.0

         (initramfs) ls /sys/bus/pci/devices/*/device
         0xa715
         ....
         ....
         0x7902

         # VMD 控制器有驅動
         (initramfs) dmesg | grep -i vmd
         ACPI: UEFI 0x00000716A.... (v01 INTEL RstVmdE 00000000 INTL 00000000)
         ACPI: UEFI 0x000007169.... (v01 INTEL RstVmdV 00000000 INTL 00000000)

         # 但 VMD 驅動 沒有抓到 nvme 的裝置
         (initramfs) dmesg | grep -i nvme
         ls: /dev/nvme*: No such file or directory

         (initramfs) ls /sys/class/nvme

         (initramfs) cat /proc/partitions
         major minor #block name


         (initramfs) dmesg | grep -i firmware
         [Firmware Bug] : TSC ADJUST : CPU0:-1306295783 force to 0
         [Firmware Bug] : TSC ADJUST differ with in socket(s), fixing all errors


         (initramfs) dmesg | grep -i error
         [Firmware Bug] : TSC ADJUST differ with in socket(s), fixing all errors
         RAS: Correctable Errors collector initialized
     ```
      - Solution - 1 : 編譯的核心有驅動但韌體沒有被包進去
        - 若 `dmesg | grep -i firmware` 出現 `Direct firmware load...failed` 代表編譯的核心有驅動但韌體沒有被包進去
          ```
          $ sudo apt install --reinstall linux-firmware

          # 再跑一次 update-initramfs
          ```
        
      - Solution - 2 : VMD Controller
        - BIOS 中關閉 VMD (Intel RST with Optane)，硬碟會取消 VMD 保護
          - 直接透過 CONFIG_BLK_DEV_NVME = y 的驅動去抓取在 PCIe 上的原生 NVMe SSD
        - 核心設定檔 `.config` 中將 VMD 驅動編譯進核心
          - `Device Drivers` → `PCI support` → `Intel Volume Management Device Driver`
          - $ vim .config 中修改 `CONFIG_VMD = y`
   * ### 重啟成功後，下指令查看 kernel 版本有是否更新
     ```
     $ uname -r
     5.15.137
     ```
## 二、User Space Code
   - `user_space_code/`
     - `include/`
       - `thread_TLS.h`
       - `thread_work_function.h`
       - `user_get_physical_addresses.h`
     - `src/`
       - `main.c`
       - `thread_work_function.c`
       - `user_get_physical_addresses.c`
     - `build/`
       - `main.exe`
     - `build.sh`
   - 編譯 並 執行
      ```
      $ bash build.sh
      ```
   - 使用 $ dmesg 來查看 kernel 內的訊息
      ```
      $ sudo dmesg
      ```




















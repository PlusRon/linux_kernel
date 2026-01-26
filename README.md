# Linux OS Kernel - Side Projects

### 一、作業目標　[NCU 許富皓 教授 Guide line](https://staff.csie.ncu.edu.tw/hsufh/COURSES/FALL2023/linux_project_1.html)
撰寫 system call 利用 `user_get_physical_addresses()` 進行呼叫，來獲得每個 process 的 virtual address 對應到的 physical address。

接著寫一個 multi-thread 的程式，觀察被這些 threads 所共享的 memory areas，了解各個 segment 的共用狀況，並繪圖說明。
* text segments (code segment)
* data segments (**global / static** variables with **initial values**)
* BSS segments (**global / static** variables with **uninitial values**)
* heap segments (dynamic variables)
* stack segments (local variables)
* libraries
* thread local storages

只要 variable 的 virtual address 指向相同的 physical address 時，就代表他們有 share memory，所以主要的目標分為以下：
1. 寫一個 system call 負責將 virtual address 轉成 physical address。
2. 將此 system call 編入 kernel 當中。
4. 利用 multi-thread 呼叫此 system call，觀察其 physical address 是否相同，可以了解其中的共享狀況。

### 二、環境、系統 (Environment)
* CPU
  - 13th Gen Intel(R) Core(TM) i7-13620H
* OS： ubuntu 24.04.3 desktop
  - ubuntu下載點：https://ubuntu.com/download/desktop
* Kernel 版本： 5.15.137
  - kernal下載點：https://www.kernel.org/

### 三、編譯核心程式 (Implement)
 * #### Test Compile Kernel by [`sys_hello_linux_test`](https://github.com/PlusRon/linux_kernel/tree/dfb34fc8868444c24ec5a50ebf876bce85c18a00/hello_linux_test)
   - [編譯前置作業、新增測試 `sys_hello_linux_test`、編譯過程、除錯DEBUG](https://github.com/PlusRon/linux_kernel/blob/89e90abb961bae78b8029354d4174b41d607229e/hello_linux_test/README_syscall_hello_linux_test.md)
 * #### Add syscall [`sys_get_physical_addresses`](https://github.com/PlusRon/linux_kernel/tree/dfb34fc8868444c24ec5a50ebf876bce85c18a00/get_physical_addresses) to get physical address from virtual address traversal level by level.
   - [設計 5-Layer Page Table Traversal(PGD、P4D、PUD、PMD、PTE) 、新增 `sys_get_physical_addresses`、編譯過程](https://github.com/PlusRon/linux_kernel/blob/89e90abb961bae78b8029354d4174b41d607229e/get_physical_addresses/README_syscall_get_physical_addresses.md)

### 四、成果 (Result)
  * #### Segment 的共用狀況
    <img src="" alt="Share Memory for Main, Thread_1 and Thread_2">
  * #### User Space (`bash build.sh`)
    <img src="get_physical_addresses/pic/Main_Thread1_execute_success.png" alt="Result of Main_thread and Thread_1 in user space" style="width:100%">
  * #### Kernel Space (`sudo dmesg`)
    <img src="get_physical_addresses/pic/dmesg/dmesg_1.png" alt="Result of dmesg in user space" style="width:40%">
### 五、問題與探討 (Issues and Discussions)


### 六、參考 (Reference)
  * [How to get physical address (Memory Management)](https://stackoverflow.com/questions/41090469/linux-kernel-how-to-get-physical-address-memory-management)
  * [Page Table Traversal 機制 程式碼實現](https://zhuanlan.zhihu.com/p/436879901)
  * [`copy_from_user()`用法](https://www.cnblogs.com/Rainingday/p/12618715.html)
  * [`copy_to_user()`用法](https://blog.csdn.net/qq_30624591/article/details/88544739)
  * [`/arch/alpha/include/asm/page.h` #define](https://elixir.bootlin.com/linux/v4.6/source/arch/alpha/include/asm/page.h#L10)
  * [如何開啟 GRUB 選單](https://magiclen.org/grub-menu/)


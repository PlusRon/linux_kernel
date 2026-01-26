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

### 二、系統環境
* CPU
  - 13th Gen Intel(R) Core(TM) i7-13620H
* OS： ubuntu 24.04.3 desktop
  - ubuntu下載點：https://ubuntu.com/download/desktop
* Kernel 版本： 5.15.137
  - kernal下載點：https://www.kernel.org/

### 三、編譯核心程式
 * #### Test Compile Kernel by `sys_hello_linux_test`
   - [編譯前置作業、新增測試 `syscall`、編譯過程、除錯DEBUG](https://github.com/PlusRon/linux_kernel/blob/89e90abb961bae78b8029354d4174b41d607229e/hello_linux_test/README_syscall_hello_linux_test.md)
 * #### Add syscall `sys_get_physical_addresses` to get physical address from virtual address traversal level by level.
   - [設計 5-Layer Page Table Traversal(PGD、P4D、PUD、PMD、PTE) 、新增 `sys_get_physical_addresses`、編譯過程](https://github.com/PlusRon/linux_kernel/blob/89e90abb961bae78b8029354d4174b41d607229e/get_physical_addresses/README_syscall_get_physical_addresses.md)



// Module Basics
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

// Data & Memory Copy
#include <linux/string.h> // memset(), memcpy(), strlen(), strcpy()
#include <linux/uaccess.h> // copy_from_user(), copy_to_user()


#include <linux/init_task.h>
#include <linux/syscalls.h>
// 
#include <linux/mm.h>
#include <linux/pgtable.h>
#include <linux/sched/mm.h>   // current->mm
#include <linux/rwsem.h>      // mmap_read_lock/unlock
#include <linux/slab.h>       // kmalloc/kfree 


static unsigned long virtual_address_to_physical_address(unsigned long vaddr);

static unsigned long virtual_address_to_physical_address(unsigned long vaddr)
{
    pgd_t *pgd; // pgd as pointer of pgd_entry
    p4d_t *p4d; // p4d as pointer of p4d_entry
    pud_t *pud; // pud as pointer of pud_entry
    pmd_t *pmd; // pmd as pointer of pmd_entry
    pte_t *pte; // pte as pointer of pte_entry
    
    /* struct page* page; */
    /* struct task_struct task; */
    
    // current : MACRO of struct task_struct (including schedule, file, memory, signal)
    // currrnt->mm : memory descriptor of process, mm_struct including base-address of PGD, VMAs, memory manager
    struct mm_struct *mm = current->mm;

    unsigned long paddr = 0; // PFN + virtual_addr_offset(12bit)
    unsigned long page_addr = 0; // PFN
    unsigned long page_offset = 0; // virtual_addr_offset(12bit)
    printk("------------------------------------\n");
    
    
    if (!mm) {
        printk("INVALID : current->mm is NULL (be a kernel thread)\n");
        return 0;
    }


    // Traditional API
    // down_read(&mm->mmap_lock);
    // up_read(&mm->mmap_lock);
    
    // Modern API
    // mmap_read_lock(mm);
    // mmap_read_unlock(mm);


    
    pgd = pgd_offset(mm, vaddr); // pgd as pointer of pgd_entry
    printk("pgd_val = 0x%lx\n", pgd_val(*pgd));
    printk("pgd_index = %lu\n", pgd_index(vaddr)); // index of pgd_entry
    // #define pgd_none(pgd)   (!(pgd_val(pgd)))
    if (pgd_none(*pgd)) {
        printk("INVALID : This offset of virtual address is not mapped in PGD table\n");
        return -1;
    }
    
    
    // In x86, P4D is folding layer, but also call by API to keep its protable.
    
    p4d = p4d_offset(pgd, vaddr);
    printk("p4d_val = 0x%lx\n", p4d_val(*p4d));
    printk("p4d_index = %lu\n", p4d_index(vaddr));
    #ifndef __x86_64__
    if (p4d_none(*p4d)) {
        printk("INVALID : This offset of virtual address is not mapped in P4D table\n");
        return -1;
    }
    if(p4d_bad(*p4d)){
		printk("INVALID : P4D table is bad.\n");
	return -1;
    }
    #endif

    pud = pud_offset(p4d, vaddr);
    printk("pud_val = 0x%lx\n", pud_val(*pud));
    printk("pud_index = %lu\n", pud_index(vaddr));
    if (pud_none(*pud)) {
        printk("INVALID : This offset of virtual address is not mapped in PUD table\n");
        return -1;
    }
    // PUD is huge page or not
    if(pud_leaf(*pud)){
        printk("This PUD entry maps to a huge page\n");
        page_addr = pud_pfn(*pud) << PAGE_SHIFT;
        page_offset = vaddr & ~PAGE_MASK;
        paddr = page_addr | page_offset;
        printk("page_addr = %lx, page_offset = %lx\n", page_addr, page_offset);
        printk("vaddr = %lx, paddr = %lx\n", vaddr, paddr);
        printk("------\n");
        return paddr;
    }
    
    
    pmd = pmd_offset(pud, vaddr);
    printk("pmd_val = 0x%lx\n", pmd_val(*pmd));
    printk("pmd_index = %lu\n", pmd_index(vaddr));
    if (pmd_none(*pmd)) {
        printk("INVALID : This offset of virtual address is not mapped in PMD table\n");
        return -1;
    }
    // PMD is huge page or not
    // Check by pmd_huge() or pmd_large()
    if(pmd_leaf(*pmd)){
    /* #define pmd_large(pmd)   (pmd_val(pmd) & _PAGE_PSE) */ 
    /* #define pmd_huge(pmd)    pmd_large(pmd) */
        
        printk("This PMD entry maps to a huge page\n");
        // if true, then get the physical address directory, no longer enter the PTE layer
        
        /* #define PAGE_SHIFT 12 */
        /* #define PAGE_SIZE  (1UL << PAGE_SHIFT)   // 4096 */
        /* #define PAGE_MASK  (~(PAGE_SIZE - 1))    // 0xFFFFF000 */

        /* page_addr = pmd_val(*pmd) & PAGE_MASK; // PFN */
        page_addr = pmd_pfn(*pmd) << PAGE_SHIFT; // PFN
        page_offset = vaddr & ~PAGE_MASK; // virtual_address_offset(12 bit)
        paddr = page_addr | page_offset; // PFN + virtual_address_offset(12 bit)
        printk("page_addr = %lx, page_offset = %lx\n", page_addr, page_offset);
        printk("vaddr = %lx, paddr = %lx\n", vaddr, paddr);
        printk("------\n");
        return paddr;
    }

    
    pte = pte_offset_kernel(pmd, vaddr);
    printk("pte_val = 0x%lx\n", pte_val(*pte));
    printk("pte_index = %lu\n", pte_index(vaddr));
    if (pte_none(*pte)) {
        printk("INVALID : Empty PTE entry\n");
        return -1;
    }

    // Check be swapped out to SSD or not, by pte_present()
    // if page is not in RAM (swapped out), pte_present() will be false
    // Can not be concatenate to physical
    if(!pte_present(*pte)){
        printk("INVALID : PTE entry exists but not present (Maybe swapped out)\n");
        return -1;
    }

    page_addr = pte_pfn(*pte) << PAGE_SHIFT;
    page_offset = vaddr & ~PAGE_MASK;
    paddr = page_addr | page_offset; // PFN + virtual_address_offset(12 bit)
    printk("page_addr = %lx, page_offset = %lx\n", page_addr, page_offset);
    printk("vaddr = %lx, paddr = %lx\n", vaddr, paddr);
    printk("------\n");

    return paddr;
}


SYSCALL_DEFINE2(get_physical_addresses, unsigned long __user *, initial, unsigned long __user *, result)
{
    unsigned long virtual = 0;
    unsigned long physical = 0;

    if (!initial || !result){
        printk("INVALID I/O : pointer of 'initial' or 'result' maybe 'NULL'.\n");
        return -EINVAL;
    }

    // 'unsigned long virtual' use to storage a virtual_address('void* virtual_address') pass from __user.
    if (copy_from_user(&virtual, initial, sizeof(unsigned long))){
        printk("ERROR : 'copy_from_user()' failed.");
        return -EFAULT;
    }

    if (!current->mm){
        printk("ERROR : can not find the mm of process (be a kernel thread).");
        return -ESRCH; // can not find the mm of process (be a kernel thread)
        // return -EINVAL;
    }

    // access to user_page have to get mmap_lock(mmap_sem) to avoid race condition
    mmap_read_lock(current->mm);
    physical = virtual_address_to_physical_address(virtual);
    mmap_read_unlock(current->mm);

    if ((long)physical < 0){
		printk("ERROR : 'virtual_address_to_physical_address()' failed.");
        return -EFAULT;
    }

    if (copy_to_user(result, &physical, sizeof(unsigned long))){
        printk("ERROR : 'copy_to_user()' failed.");
        return -EFAULT;
    }
        
    printk("virtual address: 0x%lx\n", virtual);
    printk("physical address: 0x%lx\n", physical);

    return 0;
}

/*
SYSCALL_DEFINE2(get_physical_addresses, unsigned long , initial, unsigned long, result)
{
    unsigned long *m = (unsigned long *)kmalloc(1,GFP_KERNEL);
    unsigned long physical;
    unsigned long virtual;
    copy_from_user(m, initial, sizeof(unsigned long));
    virtual = *m;
    physical = virtual_address_to_physical_address(virtual);
    printk("virtual address: %1x\n", virtual);
    printk("physical address: %1x\n", physical);
    copy_to_user(result, &physical, sizeof(unsigned long));
    return 0;
}
*/


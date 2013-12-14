#ifndef GDT_H
#define GDT_H

typedef enum {
    GDT_KERNEL_CODE = 0x08,
    GDT_KERNEL_DATA = 0x10,
    GDT_USER_CODE   = 0x18,
    GDT_USER_DATA   = 0x20,
}
gdt_selector_t;

typedef enum {
    GDT_KERNEL = 0,
    GDT_USER   = 3,
}
gdt_privilege_t;

typedef enum {
    GDT_DATA = 0,
    GDT_CODE = 1,
}
gdt_type_t;

void
gdt_init();

#endif

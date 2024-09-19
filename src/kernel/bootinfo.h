#pragma once

static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 0
};

static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 0
};

static volatile struct limine_kernel_address_request kernel_address_request = {
    .id = LIMINE_KERNEL_ADDRESS_REQUEST,
    .revision = 2
};

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static volatile struct limine_smp_request smp_request = {
    .id = LIMINE_SMP_REQUEST,
    .revision = 0
};

static struct limine_internal_module initrd = {
    .path = "initrd",
    .flags = LIMINE_INTERNAL_MODULE_REQUIRED
};

struct limine_internal_module *module_list = &initrd;

__attribute__((used, section(".requests")))
static volatile struct limine_module_request initrd_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0,
    .internal_modules = &module_list,
    .internal_module_count = 1
};
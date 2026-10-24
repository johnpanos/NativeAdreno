/* SPDX-License-Identifier: MIT */
/* Linux host test of the Turnip darwin backend: the few Mach types and calls it uses. Never built for a device. */
#ifndef NATIVE_ADRENO_LINUX_HOST_MACH_H
#define NATIVE_ADRENO_LINUX_HOST_MACH_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef int kern_return_t;
typedef uint32_t mach_port_t;
typedef mach_port_t task_port_t;
typedef mach_port_t task_t;
typedef uint64_t mach_vm_address_t;
typedef uint64_t mach_vm_size_t;
#define KERN_SUCCESS 0
mach_port_t mach_task_self(void);
#ifdef __cplusplus
}
#endif
#endif

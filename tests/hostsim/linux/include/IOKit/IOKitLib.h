/* SPDX-License-Identifier: MIT */
/* Linux host test of the Turnip darwin backend: the IOKit user-client calls it makes, with the real IOReturn and map-option values.
 * libhostiokit.so (host_iokit_shim_linux.cpp) answers them from the host copy of the NativeAdreno kernel stack. */
#ifndef NATIVE_ADRENO_LINUX_HOST_IOKITLIB_H
#define NATIVE_ADRENO_LINUX_HOST_IOKITLIB_H
#include <stddef.h>
#include <stdint.h>
#include <mach/mach.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef mach_port_t io_object_t;
typedef io_object_t io_service_t;
typedef io_object_t io_connect_t;
typedef uint32_t IOOptionBits;
typedef kern_return_t IOReturn;
typedef struct __CFDictionary *CFMutableDictionaryRef;
typedef const struct __CFDictionary *CFDictionaryRef;
#define kIOMainPortDefault ((mach_port_t)0)
#define kIOReturnSuccess        KERN_SUCCESS
#define kIOReturnError          ((IOReturn)0xe00002bc)
#define kIOReturnNoMemory       ((IOReturn)0xe00002bd)
#define kIOReturnNoResources    ((IOReturn)0xe00002be)
#define kIOReturnBadArgument    ((IOReturn)0xe00002c2)
#define kIOReturnUnsupported    ((IOReturn)0xe00002c7)
#define kIOReturnIOError        ((IOReturn)0xe00002ca)
#define kIOReturnBusy           ((IOReturn)0xe00002d5)
#define kIOReturnTimeout        ((IOReturn)0xe00002d6)
#define kIOReturnNotReady       ((IOReturn)0xe00002d8)
#define kIOReturnNotPermitted   ((IOReturn)0xe00002e2)
enum {
    kIOMapAnywhere = 0x00000001,
    kIOMapWriteCombineCache = 0x00000400
};
CFMutableDictionaryRef IOServiceMatching(const char *name);
io_service_t IOServiceGetMatchingService(mach_port_t mainPort, CFDictionaryRef matching);
kern_return_t IOServiceOpen(io_service_t service, task_port_t owningTask, uint32_t type, io_connect_t *connect);
kern_return_t IOServiceClose(io_connect_t connect);
kern_return_t IOObjectRelease(io_object_t object);
kern_return_t IOConnectCallStructMethod(mach_port_t connection, uint32_t selector, const void *inputStruct, size_t inputStructCnt,
                                        void *outputStruct, size_t *outputStructCnt);
kern_return_t IOConnectMapMemory64(io_connect_t connect, uint32_t memoryType, task_port_t intoTask, mach_vm_address_t *atAddress,
                                   mach_vm_size_t *ofSize, IOOptionBits options);
kern_return_t IOConnectUnmapMemory64(io_connect_t connect, uint32_t memoryType, task_port_t fromTask, mach_vm_address_t atAddress);
#ifdef __cplusplus
}
#endif
#endif

/* SPDX-License-Identifier: MIT */
/* Linux host form of host_iokit_shim.cpp: the IOKit entry points the Turnip darwin backend calls, defined directly (the Linux Turnip
 * build leaves them undefined and libhostiokit.so is preloaded) and answered by the host copy of the kernel stack. Same service model
 * as the Mac shim: "IOAcceleratorES" needs open type 0x4147 and admits only the tunnel through one selector, "NativeAdrenoGpuPublished"
 * is the direct form. Never built for a device. */
#include <IOKit/IOKitLib.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
static bool trace(){static int t=-1;if(t<0)t=getenv("HOSTSIM_TRACE")?1:0;return t;}

extern "C" {
int host_gpu_open();
int host_gpu_close();
int host_gpu_call(uint32_t,const void *,size_t,void *,size_t);
int host_gpu_map(uint32_t,void **,uint64_t *);
}

/* The host service returns the fixture's small IOReturn values (XnuMemoryFixture.h); these are the IOKit codes they stand for. */
static kern_return_t translate(int fixture_status) {
    switch(fixture_status) {
    case 0:return KERN_SUCCESS;
    case 1:return kIOReturnBadArgument;
    case 2:return kIOReturnBusy;
    case 3:return kIOReturnNoResources;
    case 4:return kIOReturnNoMemory;
    case 5:return kIOReturnUnsupported;
    case 6:return kIOReturnNotReady;
    case 7:return kIOReturnIOError;
    case 8:return kIOReturnTimeout;
    default:return kIOReturnError;
    }
}
static const uint32_t kAdmitted = 5, kTunnelMagic = 0x4e544e4c;
static uint32_t gOpenKind;   /* 1 direct, 2 tunnel only */

extern "C" {
__attribute__((visibility("default"))) mach_port_t mach_task_self(void) {return 0x103;}
__attribute__((visibility("default"))) CFMutableDictionaryRef IOServiceMatching(const char *name) {
    if(trace())fprintf(stderr,"shim: IOServiceMatching(%s)\n",name);
    if(!strcmp(name,"NativeAdrenoGpuPublished"))return reinterpret_cast<CFMutableDictionaryRef>(1);
    if(!strcmp(name,"IOAcceleratorES"))return reinterpret_cast<CFMutableDictionaryRef>(2);
    return nullptr;
}
__attribute__((visibility("default"))) io_service_t IOServiceGetMatchingService(mach_port_t,CFDictionaryRef matching) {
    return static_cast<io_service_t>(reinterpret_cast<uintptr_t>(matching));
}
__attribute__((visibility("default"))) kern_return_t IOServiceOpen(io_service_t service,task_port_t,uint32_t type,io_connect_t *connect) {
    if(trace())fprintf(stderr,"shim: IOServiceOpen(%u, 0x%x)\n",service,type);
    if(service!=1 && service!=2)return kIOReturnBadArgument;
    if(service==2 && type!=0x4147)return kIOReturnNotPermitted;
    const kern_return_t kr=translate(host_gpu_open());
    if(kr==KERN_SUCCESS) {*connect=7;gOpenKind=service;}
    return kr;
}
__attribute__((visibility("default"))) kern_return_t IOServiceClose(io_connect_t) {return translate(host_gpu_close());}
__attribute__((visibility("default"))) kern_return_t IOObjectRelease(io_object_t) {return KERN_SUCCESS;}
__attribute__((visibility("default"))) kern_return_t IOConnectCallStructMethod(mach_port_t,uint32_t selector,const void *in,size_t in_size,
    void *out,size_t *out_size) {
    if(gOpenKind==2) {
        if(selector!=kAdmitted)return kIOReturnNotPermitted;
        const uint32_t *header=static_cast<const uint32_t *>(in);
        if(!in || in_size<8 || header[0]!=kTunnelMagic)return kIOReturnBadArgument;
        if(header[1]==0xffffffffu)return KERN_SUCCESS;
        static bool stalled=false;
        if(header[1]==2 && !stalled && getenv("HOSTSIM_STALL")) {stalled=true;sleep((unsigned)atoi(getenv("HOSTSIM_STALL")));}
        const int status=host_gpu_call(header[1],static_cast<const uint8_t *>(in)+8,in_size-8,out,out_size?*out_size:0);
        if(trace() && status)fprintf(stderr,"shim: method %u (in %zu, out %zu) -> 0x%x\n",header[1],in_size-8,out_size?*out_size:0,status);
        return translate(status);
    }
    return translate(host_gpu_call(selector,in,in_size,out,out_size?*out_size:0));
}
__attribute__((visibility("default"))) kern_return_t IOConnectMapMemory64(io_connect_t,uint32_t type,task_port_t,mach_vm_address_t *address,
    mach_vm_size_t *size,IOOptionBits) {
    void *mapping=nullptr;uint64_t bytes=0;
    const kern_return_t kr=translate(host_gpu_map(type,&mapping,&bytes));
    if(kr==KERN_SUCCESS) {*address=reinterpret_cast<mach_vm_address_t>(mapping);*size=bytes;}
    return kr;
}
__attribute__((visibility("default"))) kern_return_t IOConnectUnmapMemory64(io_connect_t,uint32_t,task_port_t,mach_vm_address_t) {
    return getenv("HOSTSIM_FAIL_USER_UNMAP") ? kIOReturnIOError : KERN_SUCCESS;
}
}

// RUN: %kcc -m32 -o - %s | %kint --check-prefix=exp32
// RUN: %kcc -m64 -o - %s | %kint --check-prefix=exp64
// RUN: %kcc -D__PATCH__ -m32 -o - %s | %kint --check-prefix=exp-patch
// RUN: %kcc -D__PATCH__ -m64 -o - %s | %kint --check-prefix=exp-patch

// http://git.kernel.org/linus/6a54435560efdab1a08f429a954df4d6c740bddf

#define ENOMEM			12
#define KVM_MAX_CPUID_ENTRIES	80

typedef unsigned int u32;

struct kvm_cpuid2 {
	u32 nent;
};

struct kvm_cpuid_entry2 {
	u32 data[10];
};

void *vmalloc(unsigned long size);
void vfree(const void *addr);

int kvm_dev_ioctl_get_supported_cpuid(struct kvm_cpuid2 *cpuid,
                                      struct kvm_cpuid_entry2 *entries)
{
	int r;
	struct kvm_cpuid_entry2 *cpuid_entries;

	if (cpuid->nent < 1)
		goto out;
#ifdef __PATCH__
	if (cpuid->nent > KVM_MAX_CPUID_ENTRIES)
		cpuid->nent = KVM_MAX_CPUID_ENTRIES;
#endif
	r = -ENOMEM;
	cpuid_entries = vmalloc(sizeof(struct kvm_cpuid_entry2) * cpuid->nent); // exp32: {{umul}}
	if (!cpuid_entries)
		goto out;
	r = 0;
	vfree(cpuid_entries);
out:
	return r;
}

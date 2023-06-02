#include "MapRw.h"

PUCHAR MapSpaceAddress(HANDLE Pid, UINT64 Address) //读内存
{
	NTSTATUS status = STATUS_SUCCESS;

	PEPROCESS Process;

	UINT64 Pml4_idx, Pdpt_idx, Pdt_idx, Pt_idx;

	static BOOLEAN First = TRUE; 

	UINT64 offset = (Address & 0xfff);

	Pml4_idx = (Address & 0x0000ff8000000000) >> 39;

	Pdpt_idx = (Address & 0x0000007fc0000000) >> 30;

	Pdt_idx = (Address & 0x000000003fe00000) >> 21;

	Pt_idx = (Address & 0x00000000001ff000) >> 12;
	

	if (First == TRUE) {

		OBJECT_ATTRIBUTES ObJECT = { 0 };
		UNICODE_STRING PhysicalMemory = { 0 };
		RtlInitUnicodeString(&PhysicalMemory, L"\\Device\\PhysicalMemory");

		InitializeObjectAttributes(&ObJECT, &PhysicalMemory, 0, 0, 0);

		if (!NT_SUCCESS(ZwOpenSection(&g_h_section, SECTION_ALL_ACCESS, &ObJECT))) {

			DbgPrintEx(77, 0, "[+]failed to open physec\r\n");

			return 0;

		}
		First = FALSE;
	}
	if (!NT_SUCCESS(PsLookupProcessByProcessId(Pid, &Process))) {

		DbgPrintEx(77, 0, "[+]failed to get process\r\n");
		return 0;
	}
	UINT64 cr3 = *(PUINT64)((UINT64)Process + 0x28);

	LARGE_INTEGER offset_pa = { 0 };
	offset_pa.QuadPart = cr3;
	UINT64* pml4t_va = 0, * pdpt_va = 0, * pdt_va = 0, * pt_va = 0;
	SIZE_T map_size = PAGE_SIZE;

	status = ZwMapViewOfSection(g_h_section, NtCurrentProcess(), (PVOID*)&pml4t_va, 0, PAGE_SIZE, &offset_pa, &map_size, ViewUnmap, MEM_TOP_DOWN, PAGE_READWRITE);
	if (!NT_SUCCESS(status)) {

		DbgPrintEx(77, 0, "[+]failed to map cr3\r\n");
		return 0;
	}
	offset_pa.QuadPart = (pml4t_va[Pml4_idx] & 0x000ffffffffff000);

	status = ZwMapViewOfSection(g_h_section, NtCurrentProcess(), (PVOID*)&pdpt_va, 0, PAGE_SIZE, &offset_pa, &map_size, ViewUnmap, MEM_TOP_DOWN, PAGE_READWRITE);

	if (!NT_SUCCESS(status)) {

		DbgPrintEx(77, 0, "[+]failed to map pml4e\r\n");
		return 0;
	}

	offset_pa.QuadPart = (pdpt_va[Pdpt_idx] & 0x000ffffffffff000);


	status = ZwMapViewOfSection(g_h_section, NtCurrentProcess(), (PVOID*)&pdt_va, 0, PAGE_SIZE, &offset_pa, &map_size, ViewUnmap, MEM_TOP_DOWN, PAGE_READWRITE);

	if (!NT_SUCCESS(status)) {

		DbgPrintEx(77, 0, "[+]failed to map pdpte\r\n");
		return 0;
	}

	offset_pa.QuadPart = (pdt_va[Pdt_idx] & 0x000ffffffffff000);

	status = ZwMapViewOfSection(g_h_section, NtCurrentProcess(), (PVOID*)&pt_va, 0, PAGE_SIZE, &offset_pa, &map_size, ViewUnmap, MEM_TOP_DOWN, PAGE_READWRITE);

	if (!NT_SUCCESS(status)) {

		DbgPrintEx(77, 0, "[+]failed to map pdpte\r\n");
		return 0;
	}

	offset_pa.QuadPart = (pt_va[Pt_idx] & 0x000ffffffffff000);//4kb 物理页

	//MmMapIoSpaceEx  又可以使用ZwMapViewOfSection
	PUCHAR pa_4kb = { 0 };

	status = ZwMapViewOfSection(g_h_section, NtCurrentProcess(), (PVOID*)&pa_4kb, 0, PAGE_SIZE, &offset_pa, &map_size, ViewUnmap, MEM_TOP_DOWN, PAGE_READWRITE);

	if (!NT_SUCCESS(status)) {

		DbgPrintEx(77, 0, "[+]failed to map pdpte\r\n");
		return 0;
	}

	return pa_4kb + offset;
}



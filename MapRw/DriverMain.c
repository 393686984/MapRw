#include "MapRw.h"
VOID DriverUnload(PDRIVER_OBJECT pDriver)
{
	DbgPrint("����ж�سɹ���\n");

}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriver, PUNICODE_STRING Reg_Path)
{
	NTSTATUS status = STATUS_SUCCESS;
	pDriver->DriverUnload = DriverUnload;

	PUCHAR MapAddress = MapSpaceAddress((HANDLE)6480, 0x1595B87CC3C); //����PID ���̵�ַ

	if (MapAddress == NULL)
	{
		DbgPrint("MapAddress Error !\n");
		return;
	}

	DbgPrintEx(77, 0, "[+]bytes1:0x%x,byte2:0x%x,bytes3:0x%x\r\n", MapAddress[0], MapAddress[1],MapAddress[2]);

	return status;
}
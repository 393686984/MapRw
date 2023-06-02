#pragma once
#include<ntifs.h>
#include <wdm.h>
#include <intrin.h>
HANDLE g_h_section;

PUCHAR MapSpaceAddress(HANDLE Pid, UINT64 Address);



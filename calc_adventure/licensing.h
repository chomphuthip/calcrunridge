#pragma once

#pragma section(".text")

__declspec(allocate(".text")) volatile unsigned char licensing_magic[] = { 0x48, 0x94, 0xC3 };

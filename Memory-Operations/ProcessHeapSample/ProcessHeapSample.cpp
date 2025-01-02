#include <iostream>
#include <Windows.h>
#include <tchar.h>

int main()
{
	//HANDLE hProcHeap = ::GetProcessHeap(); // default heap
	HANDLE hProcHeap = ::HeapCreate(0, 0, 0); // create a new heap
	void* pMem1 = ::HeapAlloc(hProcHeap, 0, 4); // create a 4-byte block
    void* pMem2 = ::HeapAlloc(hProcHeap, 0, 1024 * 1024 * 4);

    PROCESS_HEAP_ENTRY Entry = { 0 };
	while (::HeapWalk(hProcHeap, &Entry) != FALSE) // check the heap entries
	{
        if ((Entry.wFlags & PROCESS_HEAP_ENTRY_BUSY) != 0) {
            _tprintf(TEXT("Allocated block"));

            if ((Entry.wFlags & PROCESS_HEAP_ENTRY_MOVEABLE) != 0) {
                _tprintf(TEXT(", movable with HANDLE %#p"), Entry.Block.hMem);
            }

            if ((Entry.wFlags & PROCESS_HEAP_ENTRY_DDESHARE) != 0) {
                _tprintf(TEXT(", DDESHARE"));
            }
        }
        else if ((Entry.wFlags & PROCESS_HEAP_REGION) != 0) {
            _tprintf(TEXT("Region\n  %d bytes committed\n") \
                TEXT("  %d bytes uncommitted\n  First block address: %#p\n") \
                TEXT("  Last block address: %#p\n"),
                Entry.Region.dwCommittedSize,
                Entry.Region.dwUnCommittedSize,
                Entry.Region.lpFirstBlock,
                Entry.Region.lpLastBlock);
        }
        else if ((Entry.wFlags & PROCESS_HEAP_UNCOMMITTED_RANGE) != 0) {
            _tprintf(TEXT("Uncommitted range\n"));
        }
        else {
            _tprintf(TEXT("Block\n"));
        }

        _tprintf(TEXT("  Data portion begins at: %#p\n  Size: %d bytes\n") \
            TEXT("  Overhead: %d bytes\n  Region index: %d\n\n"),
            Entry.lpData,
            Entry.cbData,
            Entry.cbOverhead,
            Entry.iRegionIndex);
	}

	::HeapFree(hProcHeap, 0, pMem1);
    ::HeapFree(hProcHeap, 0, pMem2);
    ::HeapDestroy(hProcHeap);
}

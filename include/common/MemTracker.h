#ifndef NANODB_MEM_TRACKER_H
#define NANODB_MEM_TRACKER_H

#ifdef DEBUG
#include <cstdio>

namespace NanoDB {

class MemTracker {
public:
    static long totalAllocated;
    static long totalFreed;
    
    static void trackAlloc(size_t size) {
        totalAllocated += size;
    }
    
    static void trackFree(size_t size) {
        totalFreed += size;
    }
    
    static void report() {
        if (totalAllocated != totalFreed) {
            printf("[MEMORY LEAK DETECTED] Allocated: %ld bytes, Freed: %ld bytes, Leaked: %ld bytes\n",
                   totalAllocated, totalFreed, totalAllocated - totalFreed);
        } else {
            printf("[MEMORY OK] All allocations freed. Total: %ld bytes\n", totalAllocated);
        }
    }
};

long MemTracker::totalAllocated = 0;
long MemTracker::totalFreed = 0;

#define NANO_NEW(type) \
    (MemTracker::trackAlloc(sizeof(type)), \
     new type)

#define NANO_NEW_ARRAY(type, count) \
    (MemTracker::trackAlloc(sizeof(type) * (count)), \
     new type[(count)])

#define NANO_DELETE(ptr) \
    do { \
        MemTracker::trackFree(sizeof(*ptr)); \
        delete ptr; \
    } while(0)

#define NANO_DELETE_ARRAY(ptr) \
    do { \
        MemTracker::trackFree(sizeof(*ptr)); \
        delete[] ptr; \
    } while(0)

#else // DEBUG not defined

#define NANO_NEW(type) new type
#define NANO_NEW_ARRAY(type, count) new type[(count)]
#define NANO_DELETE(ptr) delete ptr
#define NANO_DELETE_ARRAY(ptr) delete[] ptr

namespace NanoDB {
    class MemTracker {
    public:
        static void report() {}
    };
}

#endif // DEBUG

} // namespace NanoDB

#endif // NANODB_MEM_TRACKER_H

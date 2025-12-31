// limit the size of printf

// Required for RELEASE-builds
#pragma printf = "%s %d %u"

// Required for DEBUG-builds
// #pragma printf = "%s %d %u %lu"

// limit the size of scanf
// #pragma scanf = "%u"

// room for one exit function
#pragma output CLIB_EXIT_STACK_SIZE = 1

// create heap of given site
#pragma output CLIB_MALLOC_HEAP_SIZE = 0

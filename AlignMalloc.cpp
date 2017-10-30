#include <cstdlib>
#include <cassert>

void* align_malloc(size_t size, size_t align)
{
	assert(!(align & (align - (size_t)1u)));
	char* rdata = (char*)malloc(size + sizeof(void*) + align);
	if (!rdata)
		return NULL;
	size_t offset = (size_t)((char**)rdata) + sizeof(void*); // 2
	char** adata = (char**)((offset | (align - 1u)) + 1u); // 2
	adata[-1] = rdata;
	return (void*)adata;
}

void align_free(void* adata, size_t align = 0u)
{
	char* rdata = ((char**)adata)[-1];
	assert((rdata - (char*)adata) <= (ptrdiff_t)(sizeof(void*) + align));
	free(rdata);
}

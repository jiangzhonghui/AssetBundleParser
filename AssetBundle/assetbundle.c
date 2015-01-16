#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include "AssetBundle.h"
#include "tools.h"

#pragma pack(1)
struct level_info
{
    int pack_size;
	int uncompressed_size;
};
#pragma pack()

struct assetbundle_header
{
	char* signature;
	int format;
	char* version_player;
	char* version_engine;
	size_t minimum_streamed_bytes;
	int header_size;
	int number_of_levels_to_download;
	size_t level_byte_end_count;
	struct level_info* level_byte_end;
	size_t complete_file_size;
	size_t data_header_size;
	unsigned char compressed; // form codingnow.com
};

size_t assetbundle_header_load(struct assetbundle_header* header, unsigned char* data, size_t offset)
{
	size_t start = offset;

	offset += read_string(data, offset, &header->signature);
	offset += read_int32(data, offset, &header->format, false);
	offset += read_string(data, offset, &header->version_player);
	offset += read_string(data, offset, &header->version_engine);
	offset += read_uint32(data, offset, &header->minimum_streamed_bytes, false);
	offset += read_int32(data, offset, &header->header_size, false);
	offset += read_int32(data, offset, &header->number_of_levels_to_download, false);
	offset += read_uint32(data, offset, &header->level_byte_end_count, false);
	offset += read_buffer(data, offset, (unsigned char**)&header->level_byte_end, sizeof(*header->level_byte_end) * header->level_byte_end_count);

	header->complete_file_size = 0;
	if (header->format >= 2) {
		offset += read_uint32(data, offset, &header->complete_file_size, false);
	}

	header->data_header_size = 0;
	if (header->format >= 3) {
		offset += read_uint32(data, offset, &header->data_header_size, false);
	}
	
	offset += read_byte(data, offset, &header->compressed);

    return offset - start;
}

size_t assetbundle_header_save(struct assetbundle_header* header, unsigned char* data, size_t offset)
{
	size_t start = offset;

	offset += write_string(data, offset, header->signature);
	offset += write_int32(data, offset, header->format, false);
	offset += write_string(data, offset, header->version_player);
	offset += write_string(data, offset, header->version_engine);
	offset += write_uint32(data, offset, header->minimum_streamed_bytes, false);
	offset += write_int32(data, offset, header->header_size, false);
	offset += write_int32(data, offset, header->number_of_levels_to_download, false);
	offset += write_uint32(data, offset, header->level_byte_end_count, false);
	offset += write_buffer(data, offset, (unsigned char*)header->level_byte_end, sizeof(*header->level_byte_end) * header->level_byte_end_count);

	if (header->format >= 2) {
		offset += write_uint32(data, offset, header->complete_file_size, false);
	}

	if (header->format >= 3) {
		offset += write_uint32(data, offset, header->data_header_size, false);
	}

	offset += write_byte(data, offset, header->compressed);

    return offset - start;
}

struct assetbundle
{
	struct assetbundle_header header;
};

struct assetbundle* _assetbundle_load(unsigned char* data, size_t length)
{
    struct assetbundle* bundle = (struct assetbundle*)malloc(sizeof(struct assetbundle));
    size_t offset = 0;
    assetbundle_header_load(&bundle->header, data, offset);
    return NULL;
}

struct assetbundle* assetbundle_load(char* file)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1)
		return NULL;
    
    struct stat stat;
    if (fstat(fd, &stat) == -1)
    {
        close(fd);
        return NULL;
    }

	struct assetbundle* asset = NULL;
	unsigned char* data = (unsigned char *)mmap(0, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (data)
	{
		asset = _assetbundle_load(data, stat.st_size);
	}

	close(fd);
	return asset;
}

bool assetbundle_save(struct assetbundle* bundle, char* file)
{
    return  false;
}



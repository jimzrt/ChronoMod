
#include "resourcebin.h"
#include <QtZlib/zlib.h>
#include <cstring>
#include <mutex>
#include <stdexcept>

ResourceBin::ResourceBin(const std::string& path)
    : path(path)
    , stream(path, std::ios::binary | std::ios::ate)
    , length(stream.tellg())
{
    stream.seekg(0);
}

ResourceBin::~ResourceBin()
{
    close();
}

std::vector<ResourceEntry>& ResourceBin::getEntries()
{
    return entries;
}

const static char* magic_header = "ARC1";

void ResourceBin::init()
{
    char buffer[16] = { 0 };
    decode_input(0, 16, buffer);
    if (strncmp(buffer, magic_header, 4) != 0) {
        throw std::runtime_error("Header mismatch!");
    }
    uint32_t size_in_header = *(uint32_t*)&buffer[4];
    std::cout << "size: " << length << std::endl;
    std::cout << "size in header: " << size_in_header << std::endl;

    offset_header = *(uint32_t*)&buffer[8];
    compressed_length_header = *(uint32_t*)&buffer[12];

    load_header();
    init_gzip_stream();
}

void ResourceBin::close()
{
    std::cout << "close" << std::endl;
    closed = true;
    stream.close();
}

void ResourceBin::load_header()
{
    std::vector<char> header_buffer(compressed_length_header);
    decode_input(offset_header, compressed_length_header, &header_buffer[0]);

    uncompressed_length_header = *(int32_t*)&header_buffer[0];
    uncompressed_length_header = _byteswap_ulong(uncompressed_length_header);
    std::vector<char> uncompressed_buffer(uncompressed_length_header);
    std::cout << "uncompressed length " << uncompressed_length_header << std::endl;
    int ret = gzip_uncompress(&header_buffer[4], header_buffer.size() - 4, &uncompressed_buffer[0], &uncompressed_length_header);
    std::cout << "ret: " << ret << std::endl;
    std::cout << "offset header " << offset_header << ", length header " << compressed_length_header << std::endl;
    std::cout << "real uncompressed length " << uncompressed_length_header << std::endl;

    uint32_t entry_count = *(uint32_t*)&uncompressed_buffer[0];
    int offset = 4;
    std::cout << "entry count " << entry_count << std::endl;

    for (uint32_t i = 0; i < entry_count; i++) {
        uint32_t path_name_offset = *(uint32_t*)&uncompressed_buffer[offset];
        offset += 4;
        uint32_t entry_offset = *(uint32_t*)&uncompressed_buffer[offset];
        offset += 4;
        uint32_t entry_length = *(uint32_t*)&uncompressed_buffer[offset];
        offset += 4;
        std::string path(&uncompressed_buffer[path_name_offset], strlen(&uncompressed_buffer[path_name_offset]));

        entries.emplace_back(ResourceEntry(path_name_offset, entry_offset, entry_length, path));
    }
}
std::mutex m;

void inline ResourceBin::decode(int offset, int length, char* in_buffer)
{

    int tmp = 0x19000000 + offset;
    for (int index = 0; index < length; ++index) {
        tmp = tmp * 0x41c64e6d + 0x3039;
        in_buffer[index] = (char)(in_buffer[index] ^ ((tmp) >> 24));
    }
}

void ResourceBin::decode_input(int offset, int length, char* buffer)
{

    {
        std::lock_guard<std::mutex> lockGuard(m);
        stream.seekg(offset);
        stream.read(buffer, length);
    }

    decode(offset, length, buffer);
}

int ResourceBin::gzip_uncompress(char* compressed, int compressed_length, char* uncompressed, int* uncompressed_length)
{
    int err;
    z_stream d_stream; /* decompression stream */

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in = (unsigned char*)compressed;
    d_stream.avail_in = compressed_length;

    d_stream.next_out = (unsigned char*)uncompressed;
    d_stream.avail_out = *uncompressed_length;

    err = inflateInit2(&d_stream, 16 + MAX_WBITS);
    if (err != Z_OK)
        return err;

    err = inflate(&d_stream, Z_FINISH);

    if (err != Z_STREAM_END)
        return err;

    err = inflateEnd(&d_stream);

    *uncompressed_length = d_stream.total_out;

    return err;
}

z_stream strm;

void ResourceBin::init_gzip_stream()
{
    strm.zalloc = (alloc_func)0;
    strm.zfree = (free_func)0;
    strm.opaque = (voidpf)0;
    deflateInit2(&strm, Z_BEST_SPEED, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
}

// https://stackoverflow.com/questions/12398377/is-it-possible-to-have-zlib-read-from-and-write-to-the-same-memory-buffer
/* Compress buf[0..len-1] in place into buf[0..*max-1].  *max must be greater
   than or equal to len.  Return Z_OK on success, Z_BUF_ERROR if *max is not
   enough output space, Z_MEM_ERROR if there is not enough memory, or
   Z_STREAM_ERROR if *strm is corrupted (e.g. if it wasn't initialized or if it
   was inadvertently written over).  If Z_OK is returned, *max is set to the
   actual size of the output.  If Z_BUF_ERROR is returned, then *max is
   unchanged and buf[] is filled with *max bytes of uncompressed data (which is
   not all of it, but as much as would fit).

   Incompressible data will require more output space than len, so max should
   be sufficiently greater than len to handle that case in order to avoid a
   Z_BUF_ERROR. To assure that there is enough output space, max should be
   greater than or equal to the result of deflateBound(strm, len).

   strm is a deflate stream structure that has already been successfully
   initialized by deflateInit() or deflateInit2().  That structure can be
   reused across multiple calls to deflate_inplace().  This avoids unnecessary
   memory allocations and deallocations from the repeated use of deflateInit()
   and deflateEnd(). */
int ResourceBin::gzip_compress_inplace(unsigned char* buf, unsigned len,
    unsigned* max)
{
    int ret; /* return code from deflate functions */
    unsigned have; /* number of bytes in temp[] */
    unsigned char* hold; /* allocated buffer to hold input data */
    unsigned char temp[11]; /* must be large enough to hold zlib or gzip
                                   header (if any) and one more byte -- 11
                                   works for the worst case here, but if gzip
                                   encoding is used and a deflateSetHeader()
                                   call is inserted in this code after the
                                   deflateReset(), then the 11 needs to be
                                   increased to accomodate the resulting gzip
                                   header size plus one */

    /* initialize deflate stream and point to the input data */
    ret = deflateReset(&strm);
    if (ret != Z_OK)
        return ret;
    strm.next_in = buf;
    strm.avail_in = len;

    /* kick start the process with a temporary output buffer -- this allows
       deflate to consume a large chunk of input data in order to make room for
       output data there */
    if (*max < len)
        *max = len;
    strm.next_out = temp;
    strm.avail_out = sizeof(temp) > *max ? *max : sizeof(temp);
    ret = deflate(&strm, Z_FINISH);
    if (ret == Z_STREAM_ERROR)
        return ret;

    /* if we can, copy the temporary output data to the consumed portion of the
       input buffer, and then continue to write up to the start of the consumed
       input for as long as possible */
    have = strm.next_out - temp;
    if (have <= (strm.avail_in ? len - strm.avail_in : *max)) {
        memcpy(buf, temp, have);
        strm.next_out = buf + have;
        have = 0;
        while (ret == Z_OK) {
            strm.avail_out = strm.avail_in ? strm.next_in - strm.next_out : (buf + *max) - strm.next_out;
            ret = deflate(&strm, Z_FINISH);
        }
        if (ret != Z_BUF_ERROR || strm.avail_in == 0) {
            *max = strm.next_out - buf;
            return ret == Z_STREAM_END ? Z_OK : ret;
        }
    }

    /* the output caught up with the input due to insufficiently compressible
       data -- copy the remaining input data into an allocated buffer and
       complete the compression from there to the now empty input buffer (this
       will only occur for long incompressible streams, more than ~20 MB for
       the default deflate memLevel of 8, or when *max is too small and less
       than the length of the header plus one byte) */
    hold = (unsigned char*)strm.zalloc(strm.opaque, strm.avail_in, 1);
    if (hold == Z_NULL)
        return Z_MEM_ERROR;
    memcpy(hold, strm.next_in, strm.avail_in);
    strm.next_in = hold;
    if (have) {
        memcpy(buf, temp, have);
        strm.next_out = buf + have;
    }
    strm.avail_out = (buf + *max) - strm.next_out;
    ret = deflate(&strm, Z_FINISH);
    strm.zfree(strm.opaque, hold);
    *max = strm.next_out - buf;
    return ret == Z_OK ? Z_BUF_ERROR : (ret == Z_STREAM_END ? Z_OK : ret);
}

int ResourceBin::gzip_compress(char* uncompressed, int uncompressed_length, char* compressed, int* compressed_length)
{
    int err;
    z_stream d_stream; /* decompression stream */

    d_stream.zalloc = (alloc_func)0;
    d_stream.zfree = (free_func)0;
    d_stream.opaque = (voidpf)0;

    d_stream.next_in = (unsigned char*)uncompressed;
    d_stream.avail_in = uncompressed_length;

    d_stream.next_out = (unsigned char*)compressed;
    d_stream.avail_out = *compressed_length;

    err = deflateInit2(&d_stream, Z_BEST_SPEED, Z_DEFLATED, 16 + MAX_WBITS, 8, Z_DEFAULT_STRATEGY);

    if (err != Z_OK)
        return err;

    err = deflate(&d_stream, Z_FINISH);

    if (err != Z_STREAM_END)
        return err;

    err = deflateEnd(&d_stream);

    *compressed_length = d_stream.total_out;

    return err;
}

std::vector<char> ResourceBin::extract(const ResourceEntry& entry)
{
    if (closed) {
        return {};
    }
    std::vector<char> entry_compressed(entry.entry_length);
    decode_input(entry.entry_offset, entry.entry_length, entry_compressed.data());
    int32_t entry_uncompressed_length = *(int32_t*)(entry_compressed.data());
    if (entry_uncompressed_length == 0) {
        return {};
    }
    entry_uncompressed_length = _byteswap_ulong(entry_uncompressed_length);
    std::cout << "lenght " << entry.entry_length << ", uncompressed length: " << entry_uncompressed_length << std::endl;
    std::vector<char> entry_uncompressed(entry_uncompressed_length);

    int ret = gzip_uncompress(entry_compressed.data() + 4, entry_compressed.size() - 4, entry_uncompressed.data(), &entry_uncompressed_length);
    if (ret != 0) {
        throw std::runtime_error("error extracting");
    }
    return entry_uncompressed;
}

void ResourceBin::create_with_modifications(std::unordered_map<std::string, Patch>& patchMap, const std::string& path, const std::function<void(int)>& progress_callback)
{
    if (closed) {
        return;
    }
    std::ofstream out_stream(path, std::ios::binary);

    // skip first bytes - we don't know the offsets yet
    out_stream.seekp(16);

    // we keep the updated entries to adjust offsets if needed
    std::vector<ResourceEntry> new_entries;

    // preallocate buffer
    uint32_t max_buffer_size = 0;
    for (auto& entry : entries) {
        if (entry.entry_length > max_buffer_size) {
            max_buffer_size = entry.entry_length;
        }
    }
    std::vector<char> buffer(compressBound(max_buffer_size * 1.5));
    //::vector<char> tmp_buffer(compressBound(max_buffer_size * 1.5));

    int count = 0;

    // write all the entrys
    for (auto& entry : entries) {
        // if there is no replacement, we copy the old entries and write directly
        if (!entry.hasReplacement) {
            {
                std::lock_guard<std::mutex> lockGuard(m);
                stream.seekg(entry.entry_offset);
                stream.read(buffer.data(), entry.entry_length);
            }

            if (out_stream.tellp() < entry.entry_offset) {
                out_stream.seekp(entry.entry_offset);
            }
            // ...unless the entry is at a smaller offset than before - so we decode and encode it with the new offset
            if (out_stream.tellp() != entry.entry_offset) {
                decode(entry.entry_offset, entry.entry_length, buffer.data());
                decode(out_stream.tellp(), entry.entry_length, buffer.data());
            }

            ResourceEntry new_entry(entry.path_name_offset, out_stream.tellp(), entry.entry_length, entry.path);
            new_entries.push_back(new_entry);
            out_stream.write(buffer.data(), entry.entry_length);
        } else {
            // if there is a replacement, we read the entry from path
            Patch patch = patchMap[entry.path];
            std::ifstream patch_stream(patch.path, std::ios::binary | std::ios::ate);
            int32_t entry_length = 4 + (int32_t)patch_stream.tellg();

            patch_stream.seekg(0);
            patch_stream.read(buffer.data() + 4, entry_length - 4);
            patch_stream.close();

            // compress it
            //std::vector<char> tmp(compressBound(entry_length * 1.5));
            uint32_t compressed_length = buffer.size();
            int ret = gzip_compress_inplace((unsigned char*)buffer.data() + 4, entry_length - 4, &compressed_length);
            //int ret = gzip_compress(buffer.data() + 4, entry_length - 4, tmp_buffer.data() + 4, &compressed_length);
            assert(ret == 0);
            //buffer = std::move(tmp);
            entry_length = _byteswap_ulong(entry_length - 4);
            memcpy(buffer.data(), &entry_length, 4);

            //encode it
            decode(out_stream.tellp(), compressed_length + 4, buffer.data());

            ResourceEntry new_entry(entry.path_name_offset, out_stream.tellp(), compressed_length + 4, entry.path);
            new_entries.push_back(new_entry);

            // write it
            out_stream.write(buffer.data(), compressed_length + 4);
        }
        progress_callback(++count);
    }
    int new_header_offset = out_stream.tellp();

    //size of uncompressed header stays the same
    //std::vector<char> new_header(uncompressed_length_header + 4);

    int offset = 4;
    uint32_t new_entry_count = new_entries.size();
    memcpy(buffer.data() + offset, &new_entry_count, 4);
    offset += 4;
    for (auto& entry : new_entries) {
        memcpy(buffer.data() + offset, &entry.path_name_offset, 4);
        offset += 4;
        memcpy(buffer.data() + offset, &entry.entry_offset, 4);
        offset += 4;
        memcpy(buffer.data() + offset, &entry.entry_length, 4);
        offset += 4;
    }
    for (auto& entry : new_entries) {
        // including \0 terminator
        int path_length = entry.path.length() + 1;
        memcpy(buffer.data() + offset, entry.path.c_str(), path_length);
        offset += path_length;
    }
    //std::vector<char> tmp(compressBound(new_header.size() * 1.5));
    int32_t uncompressed_length_header_swapped = _byteswap_ulong(uncompressed_length_header);
    memcpy(buffer.data(), &uncompressed_length_header_swapped, 4);
    unsigned int compressed_length = buffer.size();
    int ret = gzip_compress_inplace((unsigned char*)buffer.data() + 4, uncompressed_length_header, &compressed_length);
    assert(ret == 0);
    //new_header = std::move(tmp);

    // add first 4 bytes
    compressed_length += 4;
    decode(new_header_offset, compressed_length, buffer.data());
    out_stream.write(buffer.data(), compressed_length);
    int file_end = out_stream.tellp();

    char header_buffer[16];
    // write magic
    memcpy(header_buffer, magic_header, 4);
    // write size of file
    memcpy(header_buffer + 4, &file_end, 4);
    // write offset to header
    memcpy(header_buffer + 8, &new_header_offset, 4);
    // write length of header
    memcpy(header_buffer + 12, &compressed_length, 4);

    decode(0, 16, header_buffer);

    out_stream.seekp(0);
    out_stream.write(header_buffer, 16);

    std::cout << "done!" << std::endl;
}

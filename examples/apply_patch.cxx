#include <cstdlib>

#include <fstream>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <xinclude.h>

// an xdiff allocator using the C memory functions
class MallocAllocator
        : public memallocator_t
{
public:
    MallocAllocator()
    {
        this->priv = this;
        this->malloc = &allocate;
        this->free = &release;
        this->realloc = &reallocate;
    }

private:
    static void * allocate(void * _this, unsigned size)
    {
        (void)_this;
        return std::malloc(size);
    }

    static void release(void * _this, void * ptr)
    {
        (void)_this;
        std::free(ptr);
    }

    static void * reallocate(void * _this, void * ptr, unsigned size)
    {
        (void)_this;
        return std::realloc(ptr, size);
    }
};

// wrapper for libxdiff in-memory files
class MMFile
        : public mmfile_t
{
public:
    MMFile(long bsize, unsigned long flags)
    {
        if (xdl_init_mmfile(this, bsize, flags) == -1)
            throw std::runtime_error("xdl_init_mmfile");
    }

    ~MMFile()
    {
        xdl_free_mmfile(this);
    }

    void load(std::string const & filename)
    {
        std::ifstream f(filename, std::ios::binary);
        if (!f.is_open())
            throw std::runtime_error("std::ifstream::open");

        f.seekg(0, std::ios::end);
        long size = static_cast<long>(f.tellg());
        f.seekg(0);

        char * buffer = static_cast<char *>(xdl_mmfile_writeallocate(this, size));
        f.read(buffer, size);
    }

    void patch(MMFile & fold, MMFile & fpatch)
    {
        xdemitcb_t cb;
        cb.priv = this;
        cb.outf = &MMFile::append_buffers;

        if (xdl_bpatch(&fold, &fpatch, &cb) == -1)
            throw std::runtime_error("xdl_bpatch");
    }

    void store(std::string const & filename)
    {
        std::ofstream f(filename, std::ios::binary);
        if (!f.is_open())
            throw std::runtime_error("std::ofstream::open");

        long size;
        char * buffer = static_cast<char *>(xdl_mmfile_first(this, &size));
        while (buffer)
        {
            f.write(buffer, size);
            buffer = static_cast<char *>(xdl_mmfile_next(this, &size));
        }
    }

    int append_buffers(mmbuffer_t * buffers, int count)
    {
        xdl_writem_mmfile(this, buffers, count);
        return 0;
    }

private:
    static int append_buffers(void * _this, mmbuffer_t * buffers, int count)
    {
        return static_cast<MMFile *>(_this)->append_buffers(buffers, count);
    }
};

int main(int argc, char ** argv)
{
    // check arguments
    if (argc != 4)
    {
        std::cerr << "Usage: \"" << argv[0] << "\" [old] [patch] [new]\n";
        return 1;
    }

    std::string name_old = argv[1];
    std::string name_new = argv[3];
    std::string name_patch = argv[2];

    // create the allocator for libxdiff
    MallocAllocator allocator;
    xdl_set_allocator(&allocator);

    try
    {
        // load the original
        MMFile file_old(1024, XDL_MMF_ATOMIC);
        file_old.load(name_old);

        // load the binary patch
        MMFile file_patch(1024, XDL_MMF_ATOMIC);
        file_patch.load(name_patch);

        // apply the patch and store the result
        MMFile file_new(1024, XDL_MMF_ATOMIC);
        file_new.patch(file_old, file_patch);
        file_new.store(name_new);
    }
    catch (std::exception const & ex)
    {
        std::cerr << "An exception was thrown: " << ex.what() << "\n";
        return 2;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// Copyright 2017-2020 Lawrence Livermore National Security, LLC and other
// UMAP Project Developers. See the top-level LICENSE file for details.
//
// SPDX-License-Identifier: LGPL-2.1-only
//////////////////////////////////////////////////////////////////////////////
#include "StoreFile.h"
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <zlib.h>

#include "umap/store/Store.hpp"
#include "umap/util/Macros.hpp"

namespace Umap {
  StoreFile::StoreFile(void* _region_, size_t _rsize_, size_t _alignsize_, int _fd_)
    : region{_region_}, rsize{_rsize_}, alignsize{_alignsize_}, fd{_fd_}
  {
    UMAP_LOG(Debug,
        "region: " << region << " rsize: " << rsize
        << " alignsize: " << alignsize << " fd: " << fd);
  }

  ssize_t StoreFile::read_from_store(char* buf, size_t nb, off_t off)
  {
    size_t rval = 0;

    UMAP_LOG(Debug, "pread(fd=" << fd << ", buf=" << (void*)buf
                    << ", nb=" << nb << ", off=" << off << ")";);

    char *decompress_buf = (char *)malloc(nb * sizeof(char));

    size_t tmp_size = nb;

    // rval = pread(fd, buf, nb, off); // old
    rval = pread(fd, decompress_buf, nb, off);

    uncompress((unsigned char *)buf, &tmp_size, (unsigned char *)decompress_buf,
               nb);

    free(decompress_buf);

    if (tmp_size == 0)
      rval = pread(fd, buf, nb, off);

    if (tmp_size != 0 && tmp_size != nb)
      UMAP_ERROR("uncompress error(uncompress size =" << tmp_size
                                                      << ") rval = " << rval);

    if (rval == -1) {
      int eno = errno;
      UMAP_ERROR("pread(fd=" << fd << ", buf=" << (void*)buf
                      << ", nb=" << nb << ", off=" << off
                      << "): Failed - " << strerror(eno));
    }
    return rval;
  }

  ssize_t  StoreFile::write_to_store(char* buf, size_t nb, off_t off)
  {
    size_t rval = 0;
    size_t sys_page_size = sysconf(_SC_PAGE_SIZE);

    UMAP_LOG(Debug, "pwrite(fd=" << fd << ", buf=" << (void*)buf
                    << ", nb=" << nb << ", off=" << off << ")";);

    size_t compress_size = uzs_round_up(compressBound(nb), sys_page_size);
    char *compress_buf =
        (char *)mmap(NULL, compress_size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    compress((unsigned char *)compress_buf, &compress_size,
             (unsigned char *)buf, nb);

    rval = pwrite(fd, compress_buf, nb, off);
    // rval = pwrite(fd, buf, nb, off); // old

    munmap(compress_buf, compress_size);

    if (rval == -1) {
      int eno = errno;
      UMAP_ERROR("pwrite(fd=" << fd << ", buf=" << (void*)buf
                      << ", nb=" << nb << ", off=" << off
                      << "): Failed - " << strerror(eno));
    }
    return rval;
  }
}

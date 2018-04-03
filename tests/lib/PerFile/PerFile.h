/*
This file is part of UMAP.  For copyright information see the COPYRIGHT
file in the top level directory, or at
https://github.com/LLNL/umap/blob/master/COPYRIGHT
This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License (as published by the Free
Software Foundation) version 2.1 dated February 1999.  This program is
distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
without even the IMPLIED WARRANTY OF MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE. See the terms and conditions of the GNU Lesser General Public License
for more details.  You should have received a copy of the GNU Lesser General
Public License along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/
#ifndef _UMAP_PERFILE_H
#define _UMAP_PERFILE_H
#include <stdint.h>
#include "testoptions.h"

#ifdef __cplusplus
extern "C" {
#endif
  void* PerFile_openandmap(const umt_optstruct_t*, uint64_t);
  void PerFile_closeandunmap(const umt_optstruct_t*, uint64_t, void*);
#ifdef __cplusplus
}
#endif
#endif // _UMAP_PERFILE_H

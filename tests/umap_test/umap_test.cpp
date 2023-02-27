#include "umap/umap.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>

int main() {
  long page_size = sysconf(_SC_PAGE_SIZE);

  void *region_addr = NULL;
  uint64_t region_size = page_size * 1024 * 128;
  int prot = PROT_READ | PROT_WRITE;
  int flags = MAP_PRIVATE;
  std::string filename = "/mnt/ssd/zwb_umap";
  int fd;
  if ((fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC,
                 S_IRUSR | S_IWUSR)) == -1) {
    std::string estr = "Failed to open/create " + filename + ": ";
    perror(estr.c_str());
    return -1;
  }

  int x;
  if ((x = posix_fallocate(fd, 0, region_size) != 0)) {
    std::string estr = "Failed to pre-allocate " + filename + ": ";
    perror(estr.c_str());
    return -1;
  }

  off_t offset = 0; // unused

  char *ptr = (char *)umap(region_addr, region_size, prot, flags, fd, offset);
  if (ptr == UMAP_FAILED) {
    fprintf(stderr, "umap error\n");
    return -1;
  }

  printf("[-] Apply for memory at %p\n", ptr);

  for (size_t i = 0; i < region_size / page_size; i++) {
    // printf("[i = %ld] \n", i);
    memset(ptr + i * page_size, 'a' + (i % 26), page_size);
  }

  for (size_t i = 0; i < region_size / page_size; i += 100) {
    printf("[i = %ld] = %c \n", i, ptr[i * page_size]);
  }

  // uunmap(ptr, region_size);
  close(fd);
  // remove(filename.c_str());
  return 0;
}

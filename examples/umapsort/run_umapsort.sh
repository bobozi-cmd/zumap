#!/bin/bash

##################################################
# This test script does not require sudo privilege 
# The tests run BFS, UMapsort, and Churn tests
# with different parameters
##################################################
UMAP_ROOT=/home/zwb/Templates/umap_tmp
UMAP_INSTALL_PATH=/home/zwb/Templates/umap_tmp

if [ -z "$UMAP_ROOT" ];
then
    echo "UMAP_ROOT is not set."
    exit
fi

if [ -z "$UMAP_INSTALL_PATH" ];
then
    echo "UMAP_INSTALL_PATH is not set. use $UMAP_ROOT/build. "
    UMAP_INSTALL_PATH=$UMAP_ROOT/build
fi

function free_mem {
  m=`grep MemFree /proc/meminfo | awk -v N=2 '{print $N}'`
  fm=$(((${m}/1024)/1024))
  echo $fm GB Free
}

function drop_page_cache {
  echo "Dropping page cache"
  echo 3 > /proc/sys/vm/drop_caches
}

function set_readahead {
  fs=`mount | grep ${SSD_KW} | cut -d " " -f 1`
  blockdev --setra $readahead $fs
  ra=`blockdev --getra $fs`
  echo "Read ahead set to $ra for $fs"
}

function disable_swap {
  echo "Disabling swap"
  swapoff -av
}

export LD_LIBRARY_PATH=$UMAP_INSTALL_PATH/lib:$LD_LIBRARY_PATH

echo "##############################################"
echo "# UMapSort "
echo "##############################################"
SSD_KW="ssd"
SSD_MNT_PATH="/mnt/ssd" 

set_readahead
disable_swap

# umap
echo "-----------------------------umap----------------------"
for umap_psize in 65536 16384 4096;do
  rm -f ${SSD_MNT_PATH}/sort_perf_data
  drop_page_cache
  free_mem
  cmd="env UMAP_PAGESIZE=$umap_psize ./umapsort -f ${SSD_MNT_PATH}/sort_perf_data -p $(((512*1024*1024)/umap_psize)) -N 1 -t 1"
  date
  echo $cmd
  time sh -c "$cmd" 
done

# for t in 16 8 4; do
#     for umap_psize in 65536 16384 4096;do
# 	rm -f ${SSD_MNT_PATH}/sort_perf_data
# 	drop_page_cache
# 	free_mem
# 	cmd="env UMAP_PAGESIZE=$umap_psize ./umapsort -f ${SSD_MNT_PATH}/sort_perf_data -p $(((512*1024*1024)/umap_psize)) -N 1 -t $t"
# 	date
# 	echo $cmd
# 	time sh -c "$cmd"
#     done
# done

# mmap
echo "-----------------------------mmap----------------------"
# for t in 16 8 4; do
#   rm -f ${SSD_MNT_PATH}/sort_perf_data
#   drop_page_cache
#   free_mem
#   cmd=" ./umapsort --usemmap -f ${SSD_MNT_PATH}/sort_perf_data -p $(((512*1024*1024)/4096)) -N 1 -t $t"
#   date
#   echo $cmd
#   sh -c "$cmd"
# done

exit



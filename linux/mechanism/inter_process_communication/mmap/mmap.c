#include <unistd.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>


typedef struct
{
    int index;
    char text[10];

} RECORD;

#define SIZE (50)
#define EDIT_INDEX (10)

int main( void )
{
    RECORD record, * p_mapped_memory_addr;
    int i, fd;
    FILE * fp;

    //创建文件并写入测试数据

    fp = fopen("records.dat", "w+") ;
    for ( i = 0; i < SIZE; i+ + ) {
        record.index = i;
        sprintf(record.text, "No.%d", i) ;
        fwrite(&record, sizeof(record) , 1, fp) ; //因为字节序对齐，在32位机上，sizeof(record)＝16，并不是14。
    }
    fclose(fp) ;
    printf("Ok, write %d records to the file: records.dat .\n" , SIZE) ;

    //将第一30条记录编号修改为300，并相应地修改其内容。
    //采用传统方式

    fp = fopen("records.dat" ,"r+") ;
    fseek(fp, EDIT_INDEX*sizeof(record), SEEK_SET) ;
    fread(&record, sizeof (record) , 1, fp) ;

    record.index = EDIT_INDEX*10;
    sprintf(record.text, "No.%d" , record.index) ;

    fseek(fp, EDIT_INDEX*sizeof(record) , SEEK_SET) ;
    fwrite(&record, sizeof(record) , 1, fp) ;
    fclose(fp) ;
    printf("Ok, edit the file of records.dat using traditional method./n" ) ;

    /////////////////////////////////////////


    fd = open("records.dat" , O_RDWR);
    p_mapped_memory_addr = (RECORD *) mmap(0, SIZE*sizeof(record) , PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    //修改数据

    p_mapped_memory_addr[EDIT_INDEX].index = EDIT_INDEX*10;
    sprintf(p_mapped_memory_addr[EDIT_INDEX].text, "No.%d" ,
            p_mapped_memory_addr[EDIT_INDEX].index);

    /* Synchronize the region starting at ADDR and extending LEN bytes with the
       file it maps. Filesystem operations on a file being mapped are
       unpredictable before this is done. Flags are from the MS_* set.

       This function is a cancellation point and therefore not marked with
       __THROW. extern int msync (void *__addr, size_t __len, int __flags);
     */
    //将修改写回映射文件中(采用异步写方式)
    msync((void *)p_mapped_memory_addr, SIZE*sizeof(record) ,MS_ASYNC);

    /* Deallocate any mapping for the region starting at ADDR and extending LEN
       bytes. Returns 0 if successful, -1 for errors (and sets errno).
       extern int munmap (void *__addr, size_t __len) __THROW;
     */
    //释放内存段
    munmap( ( void * ) p_mapped_memory_addr, SIZE * sizeof ( record) ) ;
    printf ( "Ok, edit the file of records.dat using mmap method./n" ) ;


    close ( fd) ;

    return 0;

}



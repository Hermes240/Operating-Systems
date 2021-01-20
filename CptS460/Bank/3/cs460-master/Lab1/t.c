#include "ext2.h"


typedef struct ext2_group_desc GD;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;

#define BLK 1024

uint8_t prints(char *s)
{
    while(*s) { putc(*s++);}
    return 1;
}

uint8_t gets(char s[])
{
  while((*s = getc()) != '\r') { putc(*s++); }
  *s = 0;
  prints("\n\r");
  return 1;
}

uint8_t getblk(uint16_t blk, char buf[])
{
    //convert blk to (cyl, head, sector)
    //blk *= 2; 
    readfd(blk/18, (blk%18)/9, (blk*2)%18, buf);
    return 1;
}

uint8_t myStrCmp(char *s1, char *s2)
{
    while(*s1 != 0 && *s2 != 0)
    {
        if(*s1++ != *s2++) {return -1;}
    }
    return 0;
}

/*uint8_t myStrCpy(char *dest, char *src, uint8_t len)
{
    uint8_t i = 0;
    while(i < len)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = 0;
    return 1;
}*/

INODE * search(char *str, INODE *ip, uint16_t iblk)
{
    uint8_t i, j;
    char *cp;
    DIR *dp;
    char buf[BLK], temp[64];
   
    for(i = 0; i < 12; i++)
    {
        if(ip->i_block[i] == 0){break;}
        getblk((uint16_t)ip->i_block[i], buf);
        dp = (DIR *)buf;
        cp = buf;

        while(cp < buf+BLK)
        {
            j = 0;
            while(j < dp->name_len)
            {
                temp[j] = dp->name[j];
                j++;
            }
            temp[j]=0;
            if(myStrCmp(str, temp) == 0)
            {
                i = dp->inode;
                getblk(((i-1)/8) + iblk, buf);
                return (INODE *)buf + ((i-1)%8);
            }
            cp += dp->rec_len;
            dp = (DIR*)cp;
        }
    }
    return 0;
}

/*INODE *getInode(uint16_t ino, char buf[], uint16_t iblk)
{
    getblk(((ino-1)/8) + iblk, buf);
    return (INODE *)buf + ((ino-1)%8);
}*/

uint8_t main() 
{
    uint16_t i, iblk;
    uint32_t *indblk;
    char buf1[BLK], buf2[BLK];
    GD *gp;
    INODE *ip;

    /* read blk#2 to get group descriptor 0 */
    getblk(2, buf1);
    gp = (GD *)buf1;
    iblk = (uint16_t)gp->bg_inode_table; // typecase u32 to u16
    
    // 1. Read first inode block into buf1[]
    getblk(iblk, buf1);

    //2. Let ip point to root INODE (inode #2)
    ip = (INODE *)buf1 + 1;

    //3. Prompt for filename to boot
    prints("File: ");
    gets(buf2);

    //4. Find file in /boot directory
    ip = search("boot", ip, iblk);
    if(ip == 0) error();
    
    ip = search(buf2, ip, iblk); 
    if(ip == 0) error();
    
    //5. From file's inode find the disk blocks of the file (has direct & indirect blocks)
    i = ip->i_block[12];
    getblk(i, buf2);
    indblk = buf2;
    
    setes(0x1000);
    //6. Load the blocks into memory at the segment 0x1000
    for(i = 0; i < 12; i++)
    {
        getblk(ip->i_block[i], 0);
        inces();
    }
    //Load indirect blocks
   while(*indblk != 0)
   {
       getblk(*indblk, 0);
       inces();
       indblk++;
   }
     return 1;
}

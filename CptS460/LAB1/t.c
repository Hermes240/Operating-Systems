/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;



//DIR *dp;


int prints(char *s)
{
   while(*s)
   {
      putc(*s);
      s++;
   }
}

int gets(char *s)
{ 
   while((*s = getc()) != '\r')
   {
      putc(*s++);
      //s++;
   }

   *s = 0;
}

char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
   readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);
}

u16 search(INODE *ip, char *name)
{
   
   int i; char c; DIR *dp;

   for(i = 0; i<12; i++)
   {
      if((u16)ip->i_block[i])
      {
         getblk((u16)ip->i_block[i], buf2);
         dp = (DIR *)buf2;

         while((char *)dp < &buf2[BLK])
         {
            c = dp->name[dp->name_len];
            dp->name[dp->name_len] = 0;

            prints(dp->name);
            putc(' ');

            if(strcmp(dp->name,name) == 0)
            {
               return (u16)dp->inode;
            }

            dp->name[dp->name_len] = c;
            dp = (char *)dp + dp->rec_len;
         }
      }
   }
//   search for name in the data block of INODE; 
//   return its inumber if found
  //else 
   error();
  
}

main()
{ 
   INODE *ip;
   GD * gd;
   char * name[2], filename[64];
   u16 i, ino, iblk;
   u32 *up;

  name[0] = "boot";
  name[1] = filename;


   prints("Boot: ");
   gets(filename);

   if(filename[0] == 0)
   {
      name[1] = "mtx";
   }

   getblk(2,buf1);
   gd = (GD *)buf1;
   iblk = (u16)gd->bg_inode_table;

   getblk(iblk,buf1);

   ip = (INODE *)buf1 + 1;

   for(i = 0; i<2; i++)
   {
      ino = search(ip, name[i]) -1;      
      if(ino < 0)
      {
         error();
      }

      getblk((u16)(iblk + (ino/8)),buf1);
      ip = (INODE *)buf1 + (ino%8);
   }

   prints("\n\r");

   if((u16)ip->i_block[12])
   {
      getblk((u16)ip->i_block[12],buf2);
   }
   

// 1. Write YOUR C code to get the INODE of /boot/mtx
//    INODE *ip --> INODE

//    if INODE has indirect blocks: get i_block[12] int buf2[  ]


// 2. 
   setes(0x1000);  // MTX loading segment = 0x1000

// 3. load 12 DIRECT blocks of INODE into memory
   for (i=0; i<12; i++){
      getblk((u16)ip->i_block[i], 0);
      putc('*');
      inces();
   }

   if((u16)ip->i_block[12])
   {
      getblk((u16)ip->i_block[12],buf2);
   }



// 4. load INDIRECT blocks, if any, into memory
   if (ip->i_block[12]){
     up = (u32 *)buf2;      
     while(*up){
        getblk((u16)*up, 0); putc('.');
        inces();
        *up++;
     }
   }
   prints("go?");
   getc();
}  

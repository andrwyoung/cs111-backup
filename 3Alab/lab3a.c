#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>

#include "ext_fs.h"

int block_size = 1024;
typedef char bool;

//fill out the superblock struct
//assumes fd is valid
int grab_superblock(struct ext2_super_block* superblock, int fd) {
	void* buff = malloc(sizeof(void) * block_size); //to read in things
	if(pread(fd, buff, block_size, block_size) < 0) {
		fprintf(stderr, "pread failed\n");
		exit (1);
	}

	superblock->s_blocks_count = *(__uint32_t*)(buff + 4);
	superblock->s_inodes_count = *(__uint32_t*)buff;
	superblock->s_log_block_size = *(__uint32_t*)(buff + 24);
	superblock->s_inode_size = *(__uint16_t*)(buff + 88); 
	superblock->s_blocks_per_group = *(__uint32_t*)(buff + 32);
	superblock->s_inodes_per_group= *(__uint32_t*)(buff + 40);
	superblock->s_first_ino= *(__uint16_t*)(buff + 84);

	free(buff);
	return 0;
}

void print_superblock(struct ext2_super_block* superblock) {
	printf("SUPERBLOCK,%d,%d,%d,%d,%d,%d,%d\n", superblock->s_blocks_count, superblock->s_inodes_count, 
		1024 << superblock->s_log_block_size, superblock->s_inode_size, superblock->s_blocks_per_group, 
		superblock->s_inodes_per_group, superblock->s_first_ino);

	fprintf(stderr, "block count: %d\n", superblock->s_blocks_count);
	fprintf(stderr, "inode count: %d\n", superblock->s_inodes_count);
	fprintf(stderr, "block size: %d\n", 1024 << superblock->s_log_block_size);
	fprintf(stderr, "inode size: %d\n", superblock->s_inode_size);
	fprintf(stderr, "blocks per group: %d\n", superblock->s_blocks_per_group);
	fprintf(stderr, "inodes per group: %d\n", superblock->s_inodes_per_group);
	fprintf(stderr, "first non-reseved inode: %d\n", superblock->s_first_ino);
}

//fills out group descriptor struct
//assumes fd is valid
int grab_group(struct ext2_group_desc* group, int fd, int start_pos) {
	void* buff = malloc(sizeof(void) * block_size); //to read in things
	if(pread(fd, buff, block_size, start_pos) < 0) {
		fprintf(stderr, "pread failed\n");
		exit (1);
	}

	group->bg_free_blocks_count = *(__uint16_t*)(buff + 12); 
	group->bg_free_inodes_count = *(__uint16_t*)(buff + 14); 
	group->bg_block_bitmap =  *(__uint32_t*)buff;
	group->bg_inode_bitmap = *(__uint32_t*)(buff + 4);
	group->bg_inode_table = *(__uint32_t*)(buff + 8);

	free(buff);
	return 0;
}

void print_group(struct ext2_group_desc* group, struct ext2_super_block* superblock, int group_no) {
	int total_groups = superblock->s_blocks_count / superblock->s_blocks_per_group + 
		(superblock->s_blocks_count % superblock->s_blocks_per_group == 0 ? 0 : 1) ;
	printf("GROUP,%d,%d,%d,%d,%d,%d,%d,%d\n", group_no, superblock->s_blocks_count / total_groups,
		 superblock->s_inodes_count / total_groups, group->bg_free_blocks_count, group->bg_free_inodes_count,
		 group->bg_block_bitmap, group->bg_inode_bitmap, group->bg_inode_table); 
		
	fprintf(stderr, "group number: %d\n", group_no);
	fprintf(stderr, "blocks in group: %d\n", superblock->s_blocks_count / total_groups);
	fprintf(stderr, "inodes in group: %d\n", superblock->s_inodes_count / total_groups);
	fprintf(stderr, "free blocks: %d\n", group->bg_free_blocks_count);
	fprintf(stderr, "free inodes: %d\n", group->bg_free_inodes_count);
	fprintf(stderr, "block bitmap: %d\n", group->bg_block_bitmap);
	fprintf(stderr, "inode bitmap: %d\n", group->bg_inode_bitmap);
	fprintf(stderr, "inode table: %d\n", group->bg_inode_table);
}

//scan block bitmap and print out all free blocks
int scan_block_bitmap(int offset, int num_blocks, int fd) {
	//each bit is a block. +1 for extra
	int bitmap_buff = (num_blocks / 8) + 1;
	
	char* buff;
	buff = malloc(sizeof(char) * bitmap_buff);
	//read the info into buffer
	if(pread(fd, buff, bitmap_buff, offset) < 0) {
		fprintf(stderr, "pread failed\n");
		exit (1);
	}
	
	int bits = 0;
	int i; //loop through each character in the buffer
	for(i = 0; i < bitmap_buff && bits < num_blocks; i++) {
		char thing = buff[i];
		//fprintf(stderr, "character value: %d\n", thing);

		int j; //loop through each bit in bitmap
		for(j = 0; j < 8 && bits < num_blocks; j++) {
			bits++;
			if(!(thing & 0x1)) printf("BFREE,%d\n", bits);
			thing = thing >> 1;
			//fprintf(stderr, "looking it bit %d\n", bits);
		}
	}
	
	free(buff);
	return 0;
}

//scan and print inodes. used in below function
void do_inodes(int offset, int inode_size, int fd) {
	void* buff = malloc(sizeof(void) * block_size); //to read in things
	if(pread(fd, buff, inode_size, offset) < 0) {
		fprintf(stderr, "pread failed\n");
		exit (1);
	}

	__uint16_t link_count = *(__uint16_t*)(buff + 26);
	if(link_count == 0) return; //spec says only print nonzero links
	
	printf("link count: %d\n", link_count);

	free(buff);
}

//scan inode bitmap and print out all free blocks
int scan_inode_bitmap(int offset, int num_inodes, int inode_size, int fd) {
	//each bit is a block. +1 for extra
	int bitmap_buff = (num_inodes / 8) + 1;
	
	char* buff;
	buff = malloc(sizeof(char) * bitmap_buff);
	//read the info into buffer
	if(pread(fd, buff, bitmap_buff, offset) < 0) {
		fprintf(stderr, "pread failed\n");
		exit (1);
	}
	
	int bits = 0;
	int i; //loop through each character in the buffer
	for(i = 0; i < bitmap_buff && bits < num_inodes; i++) {
		char thing = buff[i];
		//fprintf(stderr, "character value: %d\n", thing);

		int j; //loop through each bit in bitmap
		for(j = 0; j < 8 && bits < num_inodes; j++) {
			bits++;
			if(!(thing & 0x1)) {
				printf("IFREE,%d\n", bits);
			} else {
				fprintf(stderr, "printing inode %d\n", bits);
				//where bitmap is now + one more block over + inode location
				int inode_offset = offset + block_size + (inode_size * (bits - 1));
				do_inodes(inode_offset, inode_size, fd);
			}
			thing = thing >> 1;
			//fprintf(stderr, "looking it bit %d\n", bits);
		}
	}
	
	free(buff);
	return 0;
}


int main(int argc, char* argv[])
{
	//invaild arguments?
	if(argc != 2) {
		fprintf(stderr, "usage: lab3a pathname\n");
		exit(1);
	}


	int fd = -1; //represents the file being read
	//open failed?
	fd = open(argv[1], 0);
	if(fd < 0) {
		fprintf(stderr, "open failed\n");
		exit(1);
	}


	//fill out the superblock
	struct ext2_super_block* superblock;
	superblock = malloc(sizeof(struct ext2_super_block));
	if(superblock == NULL)
	{
		fprintf(stderr, "superblock struct allocation failed\n");
		exit(1);
	}
	grab_superblock(superblock, fd);
	print_superblock(superblock);
	//change the buffer size in case the block size is not 1024
	block_size = 1024 << superblock->s_log_block_size;



	//now fill out each group
	int total_groups = superblock->s_blocks_count / superblock->s_blocks_per_group + 
		(superblock->s_blocks_count % superblock->s_blocks_per_group == 0 ? 0 : 1) ;
	fprintf(stderr, "total groups: %d\n", total_groups);
	
	int i; 
	struct ext2_group_desc* group[total_groups];
	for(i = 0; i < total_groups; i++)
	{
		group[i] = malloc(sizeof(struct ext2_group_desc));
		if(group[i] == NULL)
		{
			fprintf(stderr, "group %d struct allocation failed\n", i);
			exit(1);
		}
		//fill out the group descriptor struct
		int offset = 2048 + (i * superblock->s_blocks_per_group);
		grab_group(group[i], fd, offset);
		print_group(group[i], superblock, i);

		//for each group print free blocks
		int blocks_in_group = superblock->s_blocks_count / total_groups;
		scan_block_bitmap(group[i]->bg_block_bitmap * block_size, blocks_in_group, fd);
	
		//for each group print free inodes
		//also calls print_inodes (which then calls print_directory and print_indirect)
		//so this one pretty much prints everything
		int inodes_in_group = superblock->s_inodes_count / total_groups;
		scan_inode_bitmap(group[i]->bg_inode_bitmap * block_size, inodes_in_group, superblock->s_inode_size, fd);
	}


	
	

	//freeing everything
	for(i = 0; i < total_groups; i++)
	{
		free((void*)group[i]);
	}
	free(superblock);
	return 0;
}

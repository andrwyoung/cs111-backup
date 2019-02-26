//NAME: Andrew Yong, Aaron Philip
//EMAIL: yong.andrew11@gmail.com, aaronpphilip@ucla.edu
//ID: 604905807, 604924943
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>

#include "ext_fs.h"

int block_size = 1024;
typedef char bool;


//some helper functions

//recurssive indirect block scanner
void indirect_scanner(__uint32_t block, int depth, int blocks_behind, int owner, int fd) {
	if(depth == 0) {
		//fprintf(stderr, "	finished\n");
		return;
	}

	int block_offset = block_size * block;
	//fprintf(stderr, "block offset: %d\n", block_offset);
	__uint32_t* buff = malloc(sizeof(__uint32_t) * block_size); //to read in things
	if(pread(fd, buff, block_size, block_offset) < 0) {
		fprintf(stderr, "pread failed\n");
		exit (1);
	}
	
	
	
	int i; //now going into each inode in this block
	for(i = 0; i < block_size / 4; i++) {
		__uint32_t curr_inode = *(__uint32_t*)(buff + i);
		//fprintf(stderr, "inode #%d: %d\n", i, curr_inode);
	
		if(curr_inode != 0) {
			blocks_behind += i + (depth == 2 ? (i + 1) * 256 : 0) + (depth == 3 ? (i + 1) * 256 * 256: 0);
			printf("INDIRECT,%d,%d,%d,%d,%d\n", owner, depth, blocks_behind + (depth == 3 ? 256 : 0), block, curr_inode);
			
			indirect_scanner(curr_inode, depth - 1, blocks_behind, owner, fd);
		}
	}

	free(buff);
}

//prints the directories
void print_directory(int dir, int owner, int fd) {
	int block_offset = block_size * dir;
	void* buff = malloc(sizeof(void) * block_size); //to read in things
	if(pread(fd, buff, block_size, block_offset) < 0) {
		fprintf(stderr, "pread failed\n");
		exit (1);
	}

	int dirent_offset = 0; //directory entries are offset
	void* curr_dirent = buff;
	
	//looping through the directory entries
	while(1) {
		__uint32_t inode = *(__uint32_t*)curr_dirent;
		if(inode <= 0) return;
		printf("DIRENT,%d,%d,%d,", owner, dirent_offset, inode); 

		//entry length. used later too
		__uint16_t ent_len = *(__uint16_t*)(curr_dirent + 4);
		printf("%d,", ent_len);

		//doing name now
		unsigned char name_len = *(unsigned char*)(curr_dirent + 6);
		char name[name_len + 1];
		int i; //building the name
		for(i = 0; i < name_len; i++) {
			name[i] = *(unsigned char*)(curr_dirent + 8 + i);
		}
		name[name_len] = '\0';
		printf("%d,'%s'", name_len, name);
		printf("\n");

		//go to next directory entry
		dirent_offset += ent_len;
		curr_dirent += ent_len;
	}

	free(buff);
}






//fill out the superblock struct
//assumes fd is valid
int scan_superblock(struct ext2_super_block* superblock, int fd) {
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

	/*
	fprintf(stderr, "block count: %d\n", superblock->s_blocks_count);
	fprintf(stderr, "inode count: %d\n", superblock->s_inodes_count);
	fprintf(stderr, "block size: %d\n", 1024 << superblock->s_log_block_size);
	fprintf(stderr, "inode size: %d\n", superblock->s_inode_size);
	fprintf(stderr, "blocks per group: %d\n", superblock->s_blocks_per_group);
	fprintf(stderr, "inodes per group: %d\n", superblock->s_inodes_per_group);
	fprintf(stderr, "first non-reseved inode: %d\n", superblock->s_first_ino);
	*/
}

//fills out group descriptor struct
//assumes fd is valid
int scan_group(struct ext2_group_desc* group, int fd, int start_pos) {
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
	
	/*
	fprintf(stderr, "group number: %d\n", group_no);
	fprintf(stderr, "blocks in group: %d\n", superblock->s_blocks_count / total_groups);
	fprintf(stderr, "inodes in group: %d\n", superblock->s_inodes_count / total_groups);
	fprintf(stderr, "free blocks: %d\n", group->bg_free_blocks_count);
	fprintf(stderr, "free inodes: %d\n", group->bg_free_inodes_count);
	fprintf(stderr, "block bitmap: %d\n", group->bg_block_bitmap);
	fprintf(stderr, "inode bitmap: %d\n", group->bg_inode_bitmap);
	fprintf(stderr, "inode table: %d\n", group->bg_inode_table);
	*/
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
void print_inodes(int inode_table_offset, int inode_size, int inode_num, int fd) {
	int inode_offset = inode_table_offset + (inode_size * (inode_num - 1));
	void* buff = malloc(sizeof(void) * block_size); //to read in things
	if(pread(fd, buff, inode_size, inode_offset) < 0) {
		fprintf(stderr, "pread failed\n");
		exit (1);
	}

	//checklink count (spec says only print nonzero link)
	__uint16_t link_count = *(__uint16_t*)(buff + 26);
	if(link_count == 0) return;
	//printf("link count: %d\n", link_count);
	
	
	
	
	//starting to print 
	printf("INODE,%d,", inode_num);
	
	//checking if type is directory or file
	__uint16_t type = *(__uint16_t*)buff;
	bool symbolic_link = 0;
	if(type & 0x8000) 
		printf("f,");
	else if(type & 0x4000) 
		printf("d,");
	else if(type & 0xA000) {
		symbolic_link = 1;
		printf("s,");
	} else
		printf("?,");
	//mode is lower 12 bits of type
	printf("%o,", type & 0xfff);

	//group and user ids 
	printf("%d,", *(__uint16_t*)(buff + 2));
	printf("%d,", *(__uint16_t*)(buff + 24));
	//printing link count
	printf("%d,", link_count);

	//doing the three times
	char time_buff[80];
	struct tm ts;
	//convert the last inode, format, then print
	long ctime = *(__uint32_t*)(buff + 12);
	ts = *gmtime(&ctime);
	strftime(time_buff, sizeof(time_buff), "%m/%d/%y %H:%M:%S", &ts);
	printf("%s,", time_buff);
	//same for modification time
	long mtime = *(__uint32_t*)(buff + 16);
	ts = *gmtime(&mtime);
	strftime(time_buff, sizeof(time_buff), "%m/%d/%y %H:%M:%S", &ts);
	printf("%s,", time_buff);
	//same for last access time
	long atime = *(__uint32_t*)(buff + 8);
	ts = *gmtime(&atime);
	strftime(time_buff, sizeof(time_buff), "%m/%d/%y %H:%M:%S", &ts);
	printf("%s,", time_buff);

	//file size
	__uint32_t file_size = *(__uint32_t*)(buff + 4);
	printf("%d,", file_size);
	//num of 512byte blocks (comma is gone so i can loop next)
	printf("%d", *(__uint32_t*)(buff + 28));
	
	//for symbolic link, if size short, then exit
	if(symbolic_link && file_size <= 60) {
		printf("\n");
		free(buff);
		return;
	}


	//now for each inode, where is the block address?
	int i, direct, num_blocks = 0;
	int directory_blocks[12];
	for(i = 0; i < 15; i++) {
		direct = *(__uint32_t*)(buff + 40 + (i * 4));
		printf(",%d", direct);

		//for printing out later
		//can't ruin format by printing here
		if(direct != 0) {
			directory_blocks[i] = direct;
			num_blocks++;
		}
	}
	printf("\n");

	//now for the indirect blocks
	int indirect1 = *(__uint32_t*)(buff + 88);
	if(indirect1 != 0) indirect_scanner(indirect1, 1, 12, inode_num, fd);
	int indirect2 = *(__uint32_t*)(buff + 92);
	if(indirect2 != 0) indirect_scanner(indirect2, 2, 12, inode_num, fd);
	int indirect3 = *(__uint32_t*)(buff + 96);
	if(indirect3 != 0) indirect_scanner(indirect3, 3, 12, inode_num, fd);


	//if it's a directory, print DIRENT lines
	if(type & 0x4000) {
		for(i = 0; i < num_blocks; i++) {
			print_directory(directory_blocks[i], inode_num, fd);
		}
	}
	
	free(buff);
}


//scan inode bitmap and print out all free blocks
int scan_inode_bitmap(int bitmap_offset, int num_inodes, int inode_size, int fd) {
	//each bit is a block. +1 for extra
	int bitmap_buff = (num_inodes / 8) + 1;
	
	char* buff;
	buff = malloc(sizeof(char) * bitmap_buff);
	//read the info into buffer
	if(pread(fd, buff, bitmap_buff, bitmap_offset) < 0) {
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
				//where bitmap is now + one more block over + inode location
				print_inodes(bitmap_offset + block_size, inode_size, bits, fd);
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
	scan_superblock(superblock, fd);
	print_superblock(superblock);
	//change the buffer size in case the block size is not 1024
	block_size = 1024 << superblock->s_log_block_size;



	//now fill out each group
	int total_groups = superblock->s_blocks_count / superblock->s_blocks_per_group + 
		(superblock->s_blocks_count % superblock->s_blocks_per_group == 0 ? 0 : 1) ;
	//fprintf(stderr, "total groups: %d\n", total_groups);
	
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
		scan_group(group[i], fd, offset);
		print_group(group[i], superblock, i);

		//for each group print free blocks
		int blocks_in_group = superblock->s_blocks_count / total_groups;
		scan_block_bitmap(group[i]->bg_block_bitmap * block_size, blocks_in_group, fd);
	
		//for each group print free inodes
		//if not free, call print_inodes (which then calls print_directory and print_indirects)
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

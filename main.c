#include <stdio.h>
#include <stdlib.h>

typedef struct fat_BS {
	unsigned char bootjmp[3];
	unsigned char oem_name[8];
	unsigned short bytes_per_sector;
	unsigned char sectors_per_cluster;
	unsigned short reserved_sector_count;
	unsigned char table_count;
	unsigned short root_entry_count;
	unsigned short total_sectors_16;
	unsigned char media_type;
	unsigned short table_size_16;
	unsigned short sectors_per_track;
	unsigned short head_side_count;
	unsigned int hidden_sector_count;
	unsigned int total_sectors_32;

	// this will be cast to it's specific type once the driver actually knows
	// what type of FAT this is.
	unsigned char extended_section[54];

} __attribute__((packed))
fat_BS_t; 

typedef struct fat_std
{
	unsigned char filename[8];
	unsigned char extension[3];
	unsigned char file_attributes;
	unsigned char reserved;
	unsigned char creation_time;
	unsigned short file_time;
	unsigned short file_date;
	unsigned short last_acess_date;
	unsigned short high_16;
	unsigned short last_modification_time;
	unsigned short last_modification_date;
	unsigned short low_16;
	unsigned int file_size;

}__attribute__((packed)) fat_std_t;

int main() {
	FILE *fp;
	fat_BS_t boot_record;
  fat_std_t standard_directory;

	fp = fopen("./floppyfat2.img", "rb");
	if (fp == NULL) {
		printf("ERRO IMAGEM");
		exit(0);
	}

	fseek(fp, 0, SEEK_SET);						 
	fread(&boot_record, sizeof(fat_BS_t), 1, fp);

	printf("======== Boot record data ========");
	printf(
		"Bytes per secto (bytes_per_sector): %hd\n",
		boot_record.bytes_per_sector);
	printf(
		"Sectors per cluster (sectors_per_cluster): %d \n",
		(int)boot_record.sectors_per_cluster);
	printf("Number of  FATs (table_count): %d\n", (int)boot_record.table_count);
	printf("Total of setores:  %hu\n", boot_record.total_sectors_16);
	printf(
		"Sectors per FAT /  FAT Size (table_size_16): %hu\n",
		boot_record.table_size_16);
	printf(
		"Sectors per track (sectors_per_track:)%hu\n",
		boot_record.sectors_per_track);

	printf("======== First Data, FAT and Root Directory sectors ========\n");
	// First fat Sector
	printf("First sector in the FAT: %hu\n", boot_record.reserved_sector_count);

	// First root directory sector
	unsigned short fat_size = boot_record.table_size_16;
	unsigned short root_dir_sectors = ((boot_record.root_entry_count * 32) +
									   (boot_record.bytes_per_sector - 1)) /
		boot_record.bytes_per_sector;

	unsigned short first_data_sector = boot_record.reserved_sector_count +
		(boot_record.table_count * fat_size) + root_dir_sectors;
	printf("First data allocation sector: %hu\n", first_data_sector);

	// First root directory sector
	unsigned short first_root_dir_sector = first_data_sector - root_dir_sectors;
	printf("First root directory sector: %hu\n", first_root_dir_sector);

	unsigned int first_root_sector = (boot_record.reserved_sector_count + (int)boot_record.table_count * boot_record.table_size_16) * boot_record.bytes_per_sector;
  printf("First sector : %d \n", first_root_sector);
  
  printf("======== Reading Directories ========\n");
	
	int index= boot_record.root_entry_count;
  
	while(index> 0){
		index-= 1;
		fseek(fp, first_root_sector, SEEK_SET);
		fread(&standard_directory, sizeof(fat_std_t), 1, fp);

		if (standard_directory.file_attributes == '\x10' || standard_directory.file_attributes == '\x20'){ 
			if ((int)standard_directory.filename[0] != 229){  
				printf("\n- Name: ");
				for (int i = 0; i < sizeof(standard_directory.filename); i++){
					printf("%c",standard_directory.filename[i]);
				}
				printf("\n- Ext: ");
				for (int i = 0; i< sizeof(standard_directory.extension); i++){
					printf("%c", standard_directory.extension[i]);
				}

				printf("\n- Atribute: %x", (int)standard_directory.file_attributes);
				printf("\n- Fist file/dir cluster: %d", (int)standard_directory.low_16);
				printf("\n- Size in bytes : %d", standard_directory.file_size);

				int cluster = ((standard_directory.low_16 - 2) * boot_record.sectors_per_cluster) + first_data_sector;
				fseek(fp, cluster*boot_record.bytes_per_sector, SEEK_SET);

				int val = (int)standard_directory.file_size;
				char buffer[val];

				fread(buffer,standard_directory.file_size, 1, fp);
				buffer[val] = '\0';
        printf("\n\n======== Reading Directories ========\n\n");
				printf("File Content: %s\n", buffer);

			}
		}
		first_root_sector += 32;			
	}
	return 0;
}

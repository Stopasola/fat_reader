#include<stdio.h>
#include<stdlib.h>

typedef struct fat_BS
{
    unsigned char       bootjmp[3];
    unsigned char       oem_name[8];
    unsigned short      bytes_per_sector;
    unsigned char       sectors_per_cluster;
    unsigned short      reserved_sector_count;
    unsigned char       table_count;
    unsigned short      root_entry_count;
    unsigned short      total_sectors_16;
    unsigned char       media_type;
    unsigned short      table_size_16;
    unsigned short      sectors_per_track;
    unsigned short      head_side_count;
    unsigned int        hidden_sector_count;
    unsigned int        total_sectors_32;

    //this will be cast to it's specific type once the driver actually knows what type of FAT this is.
    unsigned char       extended_section[54];

}__attribute__((packed)) fat_BS_t; //sem isso, as variaveis ocupam campos de 4 bytes, com isso, o C não otimiza, assim não caio no deslocamento errado


int main(){

    FILE *fp;
    fat_BS_t boot_record;

    fp = fopen("./imgfat16.img", "rb");
    if(fp == NULL){
        printf("ERRO IMAGEM");
        exit(0);
    }
    
    fseek(fp, 0, SEEK_SET); // Início do boot record
    fread(&boot_record, sizeof(fat_BS_t), 1, fp); // Lê do boot record todo

    // Principais informações do boot record
    printf("Bytes por setor (bytes_per_sector): %hd\n", boot_record.bytes_per_sector);
    printf("Setores por cluster (sectors_per_cluster): %d \n", (int)boot_record.sectors_per_cluster);
    printf("Numero de  FATs (table_count)%d\n", (int)boot_record.table_count);
    printf("Total de setores  %hu\n", boot_record.total_sectors_16);
    printf("Setores por FAT / Tamanho FAT (table_size_16)%hu\n", boot_record.table_size_16);
    printf("Setores por track (sectors_per_track)%hu\n", boot_record.sectors_per_track);

    // First fat Sector
    printf("First Sector in the FAT %hu\n", boot_record.sectors_per_track);



    return 0;
}


// a) Apresente as informações mais importantes do boot record
// b) Apresente os setores onde se inicia a FAT, diretório raiz e de dados
// c) Apresentar o nome, extensão, atributo, primeiro cluster e tamanho, das entradas válidas do diretório raiz (apenas no padrão 8.3)
// d) Mostrar o conteúdo de um arquivo existente no diretório raiz (o nome do arquivo pode ser hardcoded no fonte) 
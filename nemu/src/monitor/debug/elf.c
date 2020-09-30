#include "common.h"
#include <stdlib.h>
#include <elf.h>

char *exec_file = NULL;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;
uint32_t getVarval(char *var, bool *success) {
	int i = 0, len = strlen(var);
	for(; i < nr_symtab_entry; ++ i) {
		// STT type numbered from 0 to 15, so &0xf
		if ((symtab[i].st_info&0xf) == STT_OBJECT) {
			char var_name[len+5];
			// strtab = where string table begins
			// symble table is Elf32_Sym, like :
			/*  {st_name;st_value;st_size;st_info;st_other;st_shndx;} */
			// st_name is name zai strtab li bian de bias
			strncpy(var_name, strtab + symtab[i].st_name, len);
			var_name[len] = '\0';
			if (strcmp(var_name, var) == 0) return symtab[i].st_value;
		}
	}
	*success = false;
	return 0;
}

void getBt(swaddr_t eip, char* str){
	int i;
	printf("eip: %x\n",eip);
	for (i = 0; i < nr_symtab_entry; ++ i) {
		// in fun && type is func
		if ((symtab[i].st_value <= eip && symtab[i].st_value+symtab[i].st_size >= eip && symtab[i].st_info&0xf) == STT_FUNC ) {
			strcpy(str, strtab + symtab[i].st_name);
			printf("str:%s\n",str);
			break;
		}
	}
	str[0] = '\0';
}
void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret = fread(buf, sizeof(Elf32_Ehdr), 1, fp);
	assert(ret == 1);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	Elf32_Shdr *sh = malloc(sh_size);
	fseek(fp, elf->e_shoff, SEEK_SET);
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {

			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
		}
		else if(sh[i].sh_type == SHT_STRTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
	}

	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);

	fclose(fp);
}


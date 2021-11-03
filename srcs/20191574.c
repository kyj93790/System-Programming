#include "20191574.h"

HISTORY head;
int mem[MEM_SIZE];
HASH table[20];
int last;	// last address
int assembled;	//assemble 호출 여부
int loaded;

int main(void){
	char op[100];	// operation

	// initialize
	head.f = NULL;
	head.count = 0;
	head.r = NULL;
	last = -1;
	reset();
	buildHash();
	assembled = 0;
	loaded = 0;
	prog = 0;

	while(1){	
		//shell
		printf("sicsim> ");
		fgets(op,100,stdin);
		
		if(menu(op)) break;
	}
	return 0;
}

int menu(char* op) {
	char* ptr = NULL;
	char** inst = (char**)malloc(sizeof(char*)*4);	//tokens
	int tok = 0;	//count number of tokens
	int last_comma, flag_comma;
	int i;
	int temp;
	char** file = (char**)malloc(sizeof(char*)*3);

	if(op==NULL || !strcmp(op,"\n")){
		errorMessage(1);
		return 0;
	}

	// seperate instruction to token
	ptr = strtok(op, " \n\t");
	inst[tok] = (char*)malloc(sizeof(char)*100);
	strcpy(inst[tok], ptr);

	//if loader, 다른 방식으로 seperate( ',' 사용 x)
	if(!strcmp(inst[0],"loader")){
		while(1){
			ptr = strtok(NULL, " \n\t");
			if(ptr == NULL) break;
			if(tok>=4){
				errorMessage(3);
				return 0;
			}
			file[tok] = (char*)malloc(sizeof(char)*7);
			strcpy(file[tok],ptr);
			tok++;
		}
		tok++;
	}
	else{
		tok++;
		flag_comma = 1;	// check comma's existence
		while(1){
			ptr = strtok(NULL, " \n\t");
			if(ptr == NULL) break;		
			if(tok>=4) {
				errorMessage(3);
				return 0;
			}

			if((flag_comma==0 && ptr[0]!=',') || (flag_comma==1 && ptr[0]==',')){	// (no comma || duplication of comma)
				errorMessage(3);
				return 0;
			}
			else if(flag_comma==0 && ptr[0]==','){	// flag setting for checking location of last comma
				last_comma = 0;
			}
			else if(flag_comma==1 && ptr[0]!=','){
				last_comma = -1;
			}
		
			// check token format
			for(i=0; i<strlen(ptr); i++){
				if(i==0 && ptr[0]==',' && strlen(ptr)==1) {	// only comma
					flag_comma = 1;
				}
				else if(i==strlen(ptr)-1 && ptr[i]!=','){	// last factor of string isn't comma
					inst[tok] = (char*)malloc(sizeof(char)*100);
					strncpy(inst[tok],ptr+(last_comma+1),i-last_comma);
					inst[tok][i-last_comma] = '\0';
					flag_comma=0;
					tok++;
				}
				else if(ptr[i]==','){
					if(i-last_comma==1){	// duplication of comma
						errorMessage(3);
						return 0;
					}
					inst[tok] = (char*)malloc(sizeof(char)*100);
					strncpy(inst[tok],ptr+(last_comma+1),i-last_comma-1);
					inst[tok][i-last_comma-1] = '\0';
					if(i==strlen(ptr)-1) {	// flag setting for next loop
						flag_comma = 1;
					}
					else{
						last_comma = i;
					}
					tok++;
				}
			}
		}
	}	
	
	// option
	if((!strcmp(inst[0], "h") || !strcmp(inst[0], "help")) && tok == 1){
		push(inst[0]);
		help();
	}
	else if((!strcmp(inst[0], "d") || !strcmp(inst[0], "dir")) && tok == 1){
		push(inst[0]);
		dir();
	}
	else if((!strcmp(inst[0], "q") || !strcmp(inst[0], "quit")) && tok == 1){
		push(inst[0]);
		quit();
		//free(inst[0]);
		return 1;
	}
	else if((!strcmp(inst[0], "hi") || !strcmp(inst[0], "history")) && tok == 1){
		push(inst[0]);
		history();
	}
	else if(!strcmp(inst[0], "du") || !strcmp(inst[0], "dump")){
		if(tok == 1){	// dump
			push(inst[0]);
			if(last+160 >= 0xfffff){
				dump(last+1, 0xfffff);
				last = -1;	//reset
			}
			else {
				dump(last+1, last+160);
				last = last+160;
			}
		}
		else if(tok == 2){	//dump start
			if(dumpBound(tok, inst[1], NULL)) return 0;
			push(concat(inst,tok));
		}
		else if(tok == 3){	//dump start, end
			if(dumpBound(tok, inst[1], inst[2])) return 0;
			push(concat(inst,tok));
		}
		else errorMessage(1);	//invalid
	}
	else if(!strcmp(inst[0], "e") || !strcmp(inst[0], "edit")){
		if(tok == 3){
			if(edit(inst[1], inst[2])) return 0;
			push(concat(inst,tok));
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "f") || !strcmp(inst[0], "fill")){
		if(tok == 4){
			if(fill(inst[1], inst[2], inst[3])) return 0;
			push(concat(inst,tok));
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "reset")){
		if(tok == 1){
			push(inst[0]);
			reset();
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "opcode")){
		if(tok == 2){
			temp = opcode(inst[1]);
			if(temp > 0) {
				printf("opcode is %X\n",temp);
				push(concat(inst,tok));
			}
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "opcodelist")){
		if(tok == 1){
			push(inst[0]);
			opcodelist();
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "type")){
		if(tok == 2){
			if(typeFile(inst[1])) return 0;
			push(concat(inst,tok));
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "assemble")){
		if(tok == 2){
			if(assemble(inst[1])) return 0;
			assembled = 1;
			push(concat(inst,tok));
		}
	}
	else if(!strcmp(inst[0], "symbol")){
		if(tok == 1){
			if(assembled) symbol();
			push(inst[0]);
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "progaddr")){
		if(tok == 2){
			progaddr(inst[1]);
			push(concat(inst,tok));
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "loader")){
		if(tok==1) errorMessage(1);
		else{
			if(tok==2) { //file 1개 load
				loader(tok, file[0], NULL, NULL);
				strcat(op," ");
				strcat(op,file[0]);
			}
			else if(tok==3) {	//file 2개 load
				loader(tok, file[0], file[1], NULL);
				strcat(op," ");
				strcat(op,file[0]);
				strcat(op," ");
				strcat(op,file[1]);
			}
			else if(tok==4) {	//file 3개 load
				loader(tok, file[0], file[1], file[2]);
				strcat(op," ");
				strcat(op,file[0]);
				strcat(op," ");
				strcat(op,file[1]);
				strcat(op," ");
				strcat(op,file[2]);
			}
			else {
				errorMessage(1);
				return 0;
			}
			loaded = 1;
			push(op);
		}	
		
	}
	else if(!strcmp(inst[0], "bp")){
		if(tok==1){	// bp
			print_bp();
			push(inst[0]);
		}
		else if (tok==2){
			if(!strcmp(inst[1],"clear")){	//bp clear
				bpClear();
			}
			else{	// bp address
				set_bp(inst[1]);
			}
			push(concat(inst,tok));
		}
		else errorMessage(1);
	}
	else if(!strcmp(inst[0], "run")){
		if(tok==1){
			run();
			push(inst[0]);
		}
		else errorMessage(1);
	}
	else errorMessage(1);

	return 0;
}

void push(char* str){
	NODE* pnew = (NODE*)malloc(sizeof(NODE));
	
	strcpy(pnew->inst, str);
	pnew->next = NULL;

	// add new instruction to history list
	if(head.f == NULL) {
		head.f = pnew;
	}
	else{
		head.r->next = pnew;
	}
	head.r = pnew;
	head.count++;

	return;
}

char* concat(char** inst, int tok){
	int i;
	char* dest = (char*)malloc(sizeof(char));
	strcpy(dest,inst[0]);
	strcat(dest," ");
	strcat(dest,inst[1]);
	
	//concatenate inst[2], inst[3] if they exist
	for(i=2; i<tok; i++){
		strcat(dest,", ");
		strcat(dest,inst[i]);
	}
	strcat(dest,"\0");
	return dest;
}

void help(void){	// print available command
	printf("	h[elp]\n");
	printf("	d[ir]\n");
	printf("	q[uit]\n");
	printf("	hi[story]\n");
	printf("	du[mp] [start, end]\n");
	printf("	e[dit] address, value\n");
	printf("	f[ill] start, end, value\n");
	printf("	reset\n");
	printf("	opcode mnemonic\n");
	printf("	opcodelist\n");
	printf("	assemble filename\n");
	printf("	type filename\n");
	printf("	symbol\n");
	printf("	progaddr [address]\n");
	printf("	loader [object filename1] [object filename2] [...]\n");
	printf("	bp [address]\n");
	printf("	run\n");
	return;
}

void dir(void){
	DIR* dp = NULL;
	struct dirent* entry;
	struct stat stbuf;	

	//open current directory
	dp = opendir(".");
	if(dp != NULL){
		while((entry = readdir(dp)) != NULL){
			if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
			stat(entry->d_name, &stbuf);
			if(S_ISDIR(stbuf.st_mode)) {		// if it is directory
				printf("\t%s/\n", entry->d_name);
			}
			else if(stbuf.st_mode & S_IXUSR){	// if it is exe file
				printf("\t%s*\n", entry->d_name);
			}
			else{
				printf("\t%s\n", entry->d_name);
			}
		}
		closedir(dp);
	}
	return;
}

void quit(void){	// free history, hash table
	int i, j;
	NODE *curr1, *pCur1;
	OPCODE *curr2, *pCur2;

	//free history
	curr1 = head.f;
	for(i=0; i<head.count; i++){
		pCur1 = curr1;
		curr1 = pCur1->next;
		free(pCur1);
	}

	//free hash table
	for(i=0; i<20; i++){
		curr2 = table[i].next;
		for(j=0; j<table[i].count; j++){
			pCur2 = curr2;
			curr2 = pCur2->next;
			free(pCur2);
		}
	}
	
	//remove symtab
	if(assembled){
		rmStab(1);
	}

	//remove estab
	if(loaded){
		freeEstab();
	}

	return;
}

void history(void){
	int i;

	// if list is empty, quit
	if(head.count == 0) return;

	NODE* curr = head.f;

	// print all element of list
	for(i=1; i<=head.count; i++){
		printf("\t%d\t%s\n",i,curr->inst);
		curr = curr->next;
	}
	return;
}

int typeFile(char* filename){
	FILE* fp;
	char* res;
	char str[100];

	fp = fopen(filename,"r");
	if(fp == NULL) {
		errorMessage(4);
		return 1;
	}

	while(1){
		res = fgets(str,100,fp);
		if(res == NULL) break;
		printf("%s",str); 
	}	

	fclose(fp);
	return 0;
}

void dump(int s, int e){
	int i;
	int firstline;	// first address of first line
	int lastline;	// first address of last line
	int curr;		// first address of current line
	
	firstline = s/16;
	lastline = e/16;
	for(curr=firstline; curr<=lastline; curr++){
		printf("%05X ",curr*16);
		for(i=0; i<16; i++){	// print memory of each line
			if((curr==firstline && i<s%16) || (curr==lastline && i>e%16)){	
				printf("   ");
			}
			else printf("%02X ",mem[curr*16+i]);
		}
		printf("; ");
		for(i=0; i<16; i++){	// print ascii code
			if(mem[curr*16+i]<0x20 || mem[curr*16+i]>0x7E) printf(".");
			else printf("%c",mem[curr*16+i]);
		}
		printf("\n");
	}
	return;
}

int checkHex(char* s){
	int i;

	for(i=0; i<strlen(s); i++){
		if(('0'<=s[i] && s[i]<='9') || ('a'<=s[i] && s[i]<='f') || ('A'<=s[i] && s[i]<='F')){
			continue;
		}
		return 1;
	}
	return 0;
}

int dumpBound(int tok, char* s1, char* s2){
	int n1, n2;

	if(checkHex(s1)) {
		errorMessage(3);
		return 1;
	}
	n1 = strtol(s1, NULL, 16);

	// case : tok == 2
	if(tok == 2){
		if(n1<0 || n1>0xfffff){
			errorMessage(2);
			return 1;
		}
		else{
			if(n1+159>=0xfffff){
				dump(n1, 0xfffff);
			}
			else{
				dump(n1, n1+159);
			}
			return 0;
		}
	}

	// case : tok == 3
	if(checkHex(s2)) {
		errorMessage(3);
		return 1;
	}
	n2 = strtol(s2, NULL, 16);
	if(n1<0 || n1>0xfffff || n2<0 || n2>0xfffff) {
		errorMessage(2);
		return 1;
	}
	else if(n1>n2) {
	errorMessage(3);
		return 1;
	}
	dump(n1, n2);
	
	return 0;
}

int edit(char* str_a, char* str_v){
	int a, v;	

	if(checkHex(str_a) || checkHex(str_v)){	// check token format
		errorMessage(3);
		return 1;
	}
	//transform string to integer
	a = strtol(str_a, NULL, 16);
	v = strtol(str_v, NULL, 16);

	if(a<0 || a>0xfffff || v<0 || v>0xff){	// check range
		errorMessage(2);
		return 1;
	}
	mem[a] = v;	// edit value
	return 0;
}

int fill(char* str_s, char* str_e, char* str_v){
	int i;
	int s, e, v;

	if(checkHex(str_s) || checkHex(str_e) || checkHex(str_v)){	// check token format
		errorMessage(3);
		return 1;
	}
	//transform string to integer
	s = strtol(str_s, NULL, 16);
	e = strtol(str_e, NULL, 16);
	v = strtol(str_v, NULL, 16);

	if(s<0 || s>0xfffff || e<0 || e>0xfffff || v<0 || v>0xff){	// check range
		errorMessage(2);
		return 1;
	}
	else if(s>e){
		errorMessage(3);
		return 1;
	}

	for(i=s; i<=e; i++){	// fill value
		mem[i] = v;
	}
	return 0;
}

void reset(void){
	int i;

	for(i=0; i<MEM_SIZE; i++){
		mem[i] = 0;
	}
	return;
}

int getvalue(int address){
	return mem[address];
}

void buildHash(void){
	FILE* fp;
	int res, temp;
	int i;
	OPCODE* x;

	//initialize
	for(i=0; i<20; i++){
		table[i].count = 0;
		table[i].next = NULL;
	}
	fp = fopen("opcode.txt","r");
	if(fp==NULL) {
		errorMessage(4);
		return;
	}

	// push all mnemonic in hash table
	while(1){
		x = (OPCODE*)malloc(sizeof(OPCODE));
		res = fscanf(fp, "%x %s %[^ \n]", &(x->num), x->op, x->t);
		if(res == EOF) break;
		fgetc(fp);
		
		temp = 0;
		for(i=0; i<strlen(x->op); i++){
			temp += x->op[i];
		}
		temp %= 20;
	
		x->next = table[temp].next;
		table[temp].next = x;
		(table[temp].count)++;
	}

	fclose(fp);
	return;
}

int opcode(char* op){
	OPCODE* curr;
	int temp = 0;
	int i;

	// token format check and calculate hash value
	for(i=0; i<strlen(op); i++){
		if(op[i]<'A' || op[i]>'Z'){
			errorMessage(3);
			return -1;
		}
		temp += op[i];
	}
	temp %= 20;

	if(table[temp].count == 0){
		errorMessage(5);
		return -1;
	}
	
	// find mnemonic in hash table
	curr = table[temp].next;
	for(i=0; i<table[temp].count; i++){
		if(!strcmp(op,curr->op)){
			return curr->num;
		}
		curr = curr->next;
	}
	errorMessage(5);
	return -1;
}

// 해당 opcode가 있는지 탐색하고 format을 확인한다.
char* searchOtab(char* op){
	OPCODE* curr;
	int temp = 0;
	int i;

	//token error check
	for(i=0; i<strlen(op); i++){
		if(op[i]<'A' || op[i]>'Z') return "ERROR";
		temp += op[i];
	}
	temp %= 20;

	if(table[temp].count == 0) return NULL;	// not found

	curr = table[temp].next;
	for(i=0; i<table[temp].count; i++){
		if(!strcmp(op,curr->op)) return curr->t;	// found
		curr = curr->next;
	}

	return NULL;	// not found
}

// 해당 opcode에 대한 node를 반환한다.
OPCODE* searchCode(int op){
	OPCODE* curr;
	int i, j;
	char temp[3];
	
	//16진수로 변환
	sprintf(temp,"%02X",op);
	op = strtol(temp,NULL,16);

	for(i=0; i<20; i++){
		curr = table[i].next;
		for(j=0; j<table[i].count; j++){
			if(curr->num == op) return curr;
			curr = curr->next;
		}
	}
	
	return NULL;
}

void opcodelist(void){
	int i, j;
	OPCODE* curr;

	for(i=0; i<20; i++){
		printf("%2d : ",i);
		if(table[i].count==0) {
			printf("\n");
			continue;
		}
		curr = table[i].next;
		for(j=0; j<table[i].count-1; j++){
			printf("[%s,%X] -> ",curr->op, curr->num);
			curr = curr->next;
		}
		printf("[%s,%X]\n",curr->op, curr->num);
	}
	
	return;
}

void errorMessage(int n){
	printf("Error : ");
	if(n==1){
		printf("Invalid command\n");
	}
	else if(n==2){
		printf("Out of boundary\n");	// token's format is okay, 
	}
	else if(n==3){
		printf("Token format\n");	// number of token : valid, but token isn't available thing
	}
	else if(n==4){
		printf("File open\n");
	}
	else if(n==5){	// input mnemonic does not exist in hash table
		printf("Non-exist\n");
	}

	return;
}

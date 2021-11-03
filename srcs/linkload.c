#include "20191574.h"

int csaddr;
int fnum;	// parameter로 들어온 file의 개수
//int linkflag = 0;

//estab
ESTAB estab[3];
int refnum[3][13];

void progaddr(char* addr){
	prog = strtol(addr,NULL,16);
	return;
}

void loader(int tok, char* file1, char* file2, char* file3){
	//파일 하나씩 넘겨서 load 구현

	//load1
	csaddr = prog;
	fnum = 1;
	load1(0, file1);
	if(file2 != NULL) {
		load1(1, file2);
		fnum++;
	}
	if(file3 != NULL){
		load1(2, file3);
		fnum++;
	}
	
	//load2
	csaddr = prog;
	exeaddr = prog;
	load2(0, file1);
	if(file2 != NULL){
		load2(1,file2);
	}
	if(file3 != NULL){
		load2(2, file3);
	}
	
	//load map 출력
	printMap(tok-1);

	bp = (int*)calloc(totlen,sizeof(int));
	initReg();
//	linkflag = 1;
	return;
}

void load1(int num, char* file){
	FILE* fp;
	char* p;
	char rec[2];	//record type
	char name[7];	//section name
	char temp[8];
	char temp2[40];
	int cslth;		//length of program
	int addr;
	int i;			//temporary

	fp = fopen(file,"r");
	fgets(rec,2,fp);
	fgets(name,7,fp);
	p = strtok(name," ");
	fgets(temp,7,fp);	// 시작주소가 0 이 아닌 경우 있을지 check
	fgets(temp,8,fp); 
	cslth = strtol(temp,NULL,16);
	
	//save control section
	estab[num].count = 0;
	strcpy(estab[num].section,p);
	estab[num].addr = csaddr;
	estab[num].len = cslth;
	estab[num].next = NULL;
	//find

	while(1){
		fgets(rec,2,fp);
		if(rec[0] == 'E') break;
		else if(rec[0] == '.') continue;
		else if(rec[0] == 'D') {	// D record
			i = 0;
			fgets(temp2,40,fp);
			while(1){
				strncpy(temp,temp2+i,6);
				p = strtok(temp," ");
				strcpy(name,p);
				strncpy(temp,temp2+i+6,6);
				addr = strtol(temp,NULL,16);
				addr += csaddr;
				//find
				pushEstab(num,name,addr);
				i += 12;
				if(i+1 >= strlen(temp2)) break;	// finish D record
			}
		}
		else{
			fgets(temp2,40,fp);	//skip else
		}

	}

	csaddr += cslth;	// for next control section
	fclose(fp);
	return;
}

void load2(int num, char* file){
	FILE* fp;
	char* p;
	char rec[2];
	char name[7];
	char temp[8];
	char temp2[40];
	char temp3[4];
	char result[10];	//최종 memory에 올라가는 값 저장
	int cslth;
	int index;	//for reference array
	int curraddr;
	int count;	//text record 개수 count || memory에 올라가는 값
	int i;		//반복문, temprorary 값 등에 사용
	char op;	//modify의 operator를 저장(+ or -)

	fp = fopen(file,"r");
	fgets(rec,2,fp);
	fgets(name,7,fp);
	p = strtok(name," ");
	fgets(temp,7,fp);
	fgets(temp,8,fp);
	cslth = strtol(temp,NULL,16);
	while(1){
		fgets(rec,2,fp);
		if(rec[0] == 'E') break;
		else if(rec[0] == '.' || rec[0] == '\n') continue;
		// 오류 체크해야 할 수 있음.
		else if(rec[0] == 'R'){
			// set reference array
			refnum[num][1] = estab[num].addr;	// control section name
			while(1){
				fgets(temp,3,fp);
				if(temp[0] == '.') break;	// R record finish
				index = atoi(temp);
				fgets(temp,7,fp);
				p = strtok(temp," \n");
				refnum[num][index] = findRef(num,p);
			}
		}
		else if(rec[0] == 'T'){
			fgets(temp,7,fp);
			curraddr = strtol(temp,NULL,16) + csaddr;
			fgets(temp,3,fp);
			count = strtol(temp,NULL,16);
			// read text record
			for(i=0; i<count; i++){
				fgets(temp,3,fp);
				p = strtok(temp," \n");
				sprintf(temp2,"%0X",curraddr);
				edit(temp2, p);	// edit memory
				curraddr++;
			}
		}
		else if(rec[0] == 'M'){
			fgets(temp,7,fp);
			fgets(temp2,3,fp);
			fgets(&op,2,fp);
			fgets(temp3,3,fp);
			index = strtol(temp3,NULL,16);

			curraddr = strtol(temp,NULL,16) + csaddr;
			//메모리 3개로부터 값을 take후 concatenate
			i = getvalue(curraddr);
			sprintf(result,"%02X",i);
			i = getvalue(curraddr+1);
			sprintf(temp3,"%02X",i);
			strcat(result,temp3);
			i = getvalue(curraddr+2);
			sprintf(temp3,"%02X",i);
			strcat(result,temp3);

			//결과 값을 int로 변환 후 address modify
			count = strtol(result,NULL,16);
			if(count >= 0x800000){	// 실제로는 움수
				count = -1*(0xFFFFFF - count + 1);
			}

			if(op == '+'){
				count += refnum[num][index];
			}
			else{
				count -= refnum[num][index];
			}
			sprintf(result,"%08X",count);		
			strcpy(result, result+2);

			//modify된 값을 다시 memory에 저장.
			sprintf(temp2,"%X",curraddr);
			strncpy(temp,result,2);
			temp[2]='\0';
			edit(temp2,temp);

			curraddr++;
			sprintf(temp2,"%X",curraddr);
			strncpy(temp,result+2,2);
			temp[2]='\0';
			edit(temp2,temp);

			curraddr++;
			sprintf(temp2,"%X",curraddr);
			strcpy(temp,result+4);
			edit(temp2,temp);
		}
		else{
			fgets(temp2,40,fp);
		}
	}	
	csaddr += cslth;

	fclose(fp);
	return;
}

int findRef(int num, char* label){
	int i, j;
	ENODE* curr;

	for(i=0; i<fnum; i++){
		curr = estab[i].next;
		for(j=0; j<estab[i].count; j++){
			if(!strcmp(label, curr->symbol)){
				return curr->addr;	// return address of referenced label
			}
			curr = curr->next;
		}
	}

	return -1;
}

void pushEstab(int num, char* label, int addr){
	ENODE* pnew, *curr;
	int i;

	pnew = (ENODE*)malloc(sizeof(ENODE));
	strcpy(pnew->symbol, label);
	pnew->addr = addr;
	pnew->next = NULL;

	if(estab[num].count == 0){
		estab[num].next = pnew;
	}
	else{
		curr = estab[num].next;
		for(i=0; i<estab[num].count-1; i++){
			curr = curr->next;
		}
		curr->next = pnew;
	}
	estab[num].count++;
	return;
}

void printMap(int num){
	int i, j;
	ENODE* curr;

	totlen = 0;
	
	printf("control	symbol	address	length\n");
	printf("section	name\n");
	printf("--------------------------------\n");
	for(i=0; i<num; i++){
		printf("%s\t \t%04X\t%04X\n",estab[i].section,estab[i].addr,estab[i].len);
		totlen += estab[i].len;
		curr = estab[i].next;
		for(j=0; j<estab[i].count; j++){
			printf("  \t%s\t%04X\n",curr->symbol, curr->addr);
			curr = curr->next;
		}
	}
	printf("--------------------------------\n");
	printf("		total length %04X\n",totlen);

	return;
}

void freeEstab(void){
	int i,j;
	ENODE* curr, *pCur;

	//free all node of estab
	for(i=0; i<fnum; i++){
		curr = estab[i].next;
		for(j=0; j<estab[i].count-1; j++){
			pCur = curr;
			curr = pCur->next;
			free(pCur);
		}
		free(estab[i].next);
	}

	return;
}

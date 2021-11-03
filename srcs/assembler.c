#include "20191574.h"

SYMTAB* stable;	//temporary symtab pointer
SYMTAB* symtab;	//save final symtab
char codename[7];
char lstfile[30];
char objfile[30];
int staddr, prolen;
int count4;	// 4형식이면서 operand에 symbol이 있는 경우를 count

int assemble(char* filename){
	stable = (SYMTAB*)malloc(sizeof(SYMTAB)*26);
	buildStab();

	if(pass1(filename)) {
		remove("midfile");
		rmStab(0);
		return 1;
	}
	if(pass2(filename)) {
		rmStab(0);
		remove("midfile");
		cpyFile(0);
		return 1;
	}
	remove("midfile");
	symtab = stable;
	cpyFile(1);
	return 0;
}

void cpyFile(int i){
	FILE* objt, *lstt;
	FILE* obj, *lst;
	char* res;
	char str[100];
	char t1[30];
	char t2[30];

	if(i==1){	// assemble success
		lstt = fopen(lstfile,"r");
		strcpy(t1,lstfile);
		lstfile[strlen(lstfile)-1] = '\0';
		lst = fopen(lstfile,"w");
		while(1){
			res = fgets(str,100,lstt);
			if(res == NULL) break;
			fprintf(lst,"%s",str);
		}
		fclose(lstt);
		fclose(lst);
	
		objt = fopen(objfile,"r");
		strcpy(t2,objfile);
		objfile[strlen(objfile)-1] = '\0';
		obj = fopen(objfile,"w");
		while(1){
			res = fgets(str,100,objt);
			if(res == NULL) break;
			fprintf(obj,"%s",str);
		}
		fclose(objt);
		fclose(obj);
		printf("[%s], [%s]\n",lstfile,objfile);
		remove(t1);
		remove(t2);
	}
	else{	// assemble failed
		remove(lstfile);
		remove(objfile);
	}

	return;
}

int pass1(char* filename){
	FILE* fp, *imd;
	char* p0, *p1, *p2;
	char temp[3][100];
	char* format;
	int line = 5;
	int locctr;

	fp = fopen(filename, "r");
	imd = fopen("midfile","w");
	count4 = 0;

	if(fp == NULL) {
		errorMessage(4);
		fclose(fp);
		fclose(imd);
		return 1;
	}
	if(strcmp(filename+(strlen(filename)-4),".asm")){
		errorMessage(3);
		fclose(fp);
		fclose(imd);
		return 1;	
	}

	// pass 1
	fgets(codename,8,fp);
	fgets(temp[0],8,fp);
	p0 = strtok(temp[0]," ");
	if(strlen(p0) == 5 && !strcmp(p0, "START")){
		fgets(temp[1],100,fp);
		p1 = strtok(temp[1]," \n");
		locctr = strtol(p1,NULL,16);
		staddr = locctr;
		fprintf(imd,"%d\t%X\t%s\t%s\t%s\t%X\n",line,locctr,codename,p0,p1,locctr);
	}
	else {
		locctr = 0;
	}
	line += 5;

	while(1){
		// get line
		fgets(temp[0],8,fp);
		p0 = strtok(temp[0]," ");
		fprintf(imd,"%d\t%X\t",line,locctr);

		if(p0!=NULL &&  !strcmp(p0,"END")) break;
		
		// if comment line
		if(p0!=NULL && p0[0]=='.'){
			if(p0[1] != '\n'){	// 뒤에 comment가 붙음.
				fgets(temp[1],100,fp);
				p1 = strtok(temp[1],"\n");
				fprintf(imd,"%s\t1\t%s\t",p0,p1);
			}
			else {
				p0 = strtok(p0,"\n");
				fprintf(imd,"%s\t0\t \t",p0);
			}
		}
		else {
			if(p0 != NULL){
				fprintf(imd,"%s\t",p0);
			}
			else{
				fprintf(imd," \t");
			}
			// get opcode and operands
			fgets(temp[1],8,fp);
			p1 = strtok(temp[1]," ");

			// to do not get p2
			if(!strcmp(p1,"RSUB\n")){	// return to caller
				//imd file에 write
				locctr += 3;
				line += 5;
				fprintf(imd,"RSUB\t \t%x\n",locctr);
				continue;			
			}
			fprintf(imd,"%s\t",p1);

			fgets(temp[2],100,fp);
			p2 = strtok(temp[2],"\n");
			fprintf(imd,"%s\t",p2);

			// if there is a symbol
			if(p0 != NULL){
				// if it already exists
				if(searchStab(p0) > 0){
					printf("Error : Duplicate symbol, Incorrect line : %d\n", line);
					fclose(fp);
					fclose(imd);
					return 1;
				}
				pushStab(p0,locctr);
			}

			if(!strcmp(p1, "END")) {
				fprintf(imd,"%X\n",locctr);
				break;
			}
			if(!strcmp(p1,"BASE")){
				fprintf(imd,"%x\n",locctr);
				line += 5;
				continue;
			}

			// search OPTAB for opcode
			if(p1[0] == '+'){
				format = searchOtab(p1+1);
			}
			else {
				format = searchOtab(p1);
			}

			if(format != NULL && !strcmp(format,"ERROR")){	// error : format of opcode
				printf("Error : Invalid Format, incorrect line %d\n",line);
				fclose(fp);
				fclose(imd);
				return 1;
			}

			if(p1[0]=='+'){	// format 4
				locctr += 4;
				if('0'<=p2[1] && p2[1]<='9');
				else count4++;
			}
			else if(format != NULL && format[0] == '3'){	// format 3
				locctr += 3;
			}
			else if(format != NULL && format[0] == '2'){	// format 2
				locctr += 2;
			}
			else if(format != NULL && format[0] == '1'){	// format 1
				locctr += 1;
			}
			else if (!strcmp(p1,"WORD")){
				locctr += 3;
			}
			else if (!strcmp(p1,"RESW")){
				locctr += 3*atoi(p2);
			}
			else if (!strcmp(p1,"RESB")){
				locctr += atoi(p2);
			}
			else if (!strcmp(p1,"BYTE")){
				if(p2[0] == 'C'){	//character
					locctr += strlen(p2+2)-1;
				}
				else if(p2[0] == 'X'){	//hexadecimal
					locctr += (strlen(p2+2)-1)/2;
				}
				else{	//incorrect line error
					printf("Error : Invalid format of BYTE, incorrect line %d\n",line);
					fclose(fp);
					fclose(imd);
					return 1;
				}
			}
			else{
				printf("Error : Invalid operand, incorrect line %d\n",line);
				fclose(fp);
				fclose(imd);
				return 1;
			}
		}
		fprintf(imd,"%X\n",locctr);	//write pc counter;
		line += 5;
	}
	// save (LOCCTR-starting address) as program length
	prolen = locctr-staddr;

	fclose(fp);
	fclose(imd);

	return 0;
}

int pass2(char* filename){
	FILE *imd, *obj, *lst;
	char read[150];
	char temp[3][100];
	char* format;
	char* ptr;
	int line, locctr;
	int pc, base;	// pc counter and base counter;
	char text[61];	// text record;
	char* modi;

	int r1, r2;
	char *p0, *p1, *p2;
	char t0[11], t1[11], t2[11];
	char obcode[9];	// object code
	int op;		// opcode value return 받음
	int loc;	// symbol의 loc return 받음
	int disp;
	int index;	// indexed addressing mode 여부
	int i;		// 반복문과 길이 계산에서 사용
	int t_st;	// text start address

	// pass 2
	imd = fopen("midfile","r");
	filename[strlen(filename)-4] = '\0';
	strcpy(objfile,filename);
	strcat(objfile,".objt");
	obj = fopen(objfile,"w");
	strcpy(lstfile,filename);
	strcat(lstfile,".lstt");
	lst = fopen(lstfile,"w");

	// initialize text and modi
	strcpy(text,"\0");
	modi = (char*)malloc(sizeof(char)*(count4*10+1));
	strcpy(modi,"\0");

	while(1) {
		fgets(read,150,imd);
		
		ptr = strtok(read,"\t\n");
		line = atoi(ptr);
		ptr = strtok(NULL,"\t\n");
		locctr = strtol(ptr,NULL,16);
		ptr = strtok(NULL,"\t\n");
		strcpy(temp[0],ptr);
		ptr = strtok(NULL,"\t\n");
		strcpy(temp[1],ptr);
		ptr = strtok(NULL,"\t\n");
		strcpy(temp[2],ptr);
		ptr = strtok(NULL,"\t\n");
		pc = strtol(ptr,NULL,16);	//pc = locctr of next line

		if (!strcmp(temp[0],".")){
			fprintf(lst,"%d",line);
			if (!strcmp(temp[1],"0")){
				fprintf(lst,"\n");
			}
			else{
				fprintf(lst,"\t    \t \t%s\n",temp[2]);
			}
			continue;
		}
		else if (!strcmp(temp[1],"BASE")){
			base = searchStab(temp[2]);
			if(base<0){
				printf("Error : Not exist in SYMTAB, incorrenct line : %d\n",line);
				fclose(imd);
				fclose(obj);
				fclose(lst);
				free(modi);
				return 1;
			}
			fprintf(lst, "%d\t    \t \t%s\t%s\n",line,temp[0],temp[1]);
			continue;
		}
		else if (!strcmp(temp[1],"END")) {
			fprintf(lst, "%d\t    \t \t%s\t%s\t%s\n",line,temp[0],temp[1],temp[2]);
			i = strlen(text)/2;
			if(i!=0){
				fprintf(obj,"T%07X%02X%s\n",t_st,i,text);
			}
			fprintf(obj,"%s",modi);
			fprintf(obj,"E%06X\n",staddr);
			break;
		}
		else if(!strcmp(temp[1],"START")) {
			fprintf(lst, "%d\t%04X\t%s\t%s\t%s\n",line,locctr,temp[0],temp[1],temp[2]);	
			fprintf(obj, "H%s%06X%06x\n",codename,staddr,prolen);
			continue;
		}
		else{
			fprintf(lst, "%d\t%04X\t%s\t%s\t%-20s",line,locctr,temp[0],temp[1],temp[2]);
		}	

		//object code 생성 및 출력을 위한 처리

		//check format
		if(temp[1][0] == '+'){
			format = searchOtab(temp[1]+1);
		}
		else{
			format = searchOtab(temp[1]);
		}
		
		if(format != NULL){	// found
			// format 1
			if(format[0] == '1'){
				op = opcode(temp[1]);
				sprintf(t0, "%02X",op);
				strcpy(obcode,t0);
			}
			// format 2
			else if(format[0] == '2'){
				p1 = strtok(temp[2]," ,");
				p2 = strtok(NULL," ,");
				op = opcode(temp[1]);
				sprintf(t0,"%02X",op);
				strcpy(obcode, t0);

				//register part
				r1 = isReg(p1);
				sprintf(t1, "%X", r1);
				strcat(obcode, t1);
				if(p2 == NULL){	// op r1
					strcat(obcode,"0");
				}
				else {
					r2 = isReg(p2);
					sprintf(t2, "%X", r2);
					strcat(obcode,t2);
				}
			}
			//format 4
			else if(temp[1][0] == '+'){
				op = opcode(temp[1]+1);
				if(temp[2][0] == '#'){
					op += 1;
					sprintf(t0,"%02X",op);
					strcpy(obcode,t0);
					strcat(obcode,"1");
					// # 뒤에 immdiate value
					if('0'<=temp[2][1] && temp[2][1]<='9'){
						loc = atoi(temp[2]+1);
					}
					// # 뒤에 label
					else{
						loc = searchStab(temp[2]+1);
						if(loc<0){
							printf("Error : Not exist in SYMTAB, incorrect line : %d\n",line);
							fclose(imd);
							fclose(obj);
							fclose(lst);
							free(modi);
							return 1;
						}
					}
					sprintf(t2,"%05X",loc);
					strcat(obcode,t2);
				}
				else if(temp[2][0] == '@'){	// indirect
					op += 2;
					sprintf(t0,"%02X",op);
					strcpy(obcode,t0);
					strcat(obcode,"1");
					loc = searchStab(temp[2]+1);
					if(loc<0){
						printf("Error : Not exist in SYMTAB, incorrect line : %d\n",line);
						fclose(imd);
						fclose(obj);
						fclose(lst);
						free(modi);
						return 1;
					}
					sprintf(t2,"%05X",loc);
					strcat(obcode,t2);
				}
				else{	// simple
					op += 3;
					sprintf(t0,"%02X",op);
					strcpy(obcode,t0);
					strcat(obcode,"1");
					loc = searchStab(temp[2]);
					if(loc<0){
						printf("Error : Not exist in SYMTAB, incorrect line : %d\n",line);
						fclose(imd);
						fclose(obj);
						fclose(lst);
						free(modi);
						return 1;
					}
					sprintf(t2,"%05X",loc);
					strcat(obcode,t2);
				}
			}
			// format 3
			else {
				// oprand가 별도로 없는 opcode
				if(!strcmp(temp[2]," ")){
					op = opcode(temp[1])+3;
					sprintf(t0, "%02X", op);	
					strcpy(obcode,t0);
					strcat(obcode,"0000");
				}
				else if(temp[2][0] == '#'){ // immediate
					op = opcode(temp[1]);
					op += 1;
					sprintf(t0,"%02X",op);
					strcpy(obcode,t0);
					// # 뒤에 immediate value
					if('0'<=temp[2][1] && temp[2][1]<='9'){
						loc = atoi(temp[2]+1);

						// direct addressing mode
						strcat(obcode,"0");
						sprintf(t2,"%03X",loc);
						strcat(obcode,t2);
					}
					// # 뒤에 label
					else {
						loc = searchStab(temp[2]+1);
						if(loc<0){
							printf("Error : Not exist in SYMTAB, incorrect line : %d\n",line);
							fclose(imd);
							fclose(obj);
							fclose(lst);
							free(modi);
							return 1;
						}
						//check pc relative
						disp = loc-pc;
						if(-2048<=disp && disp<2048){
							strcat(obcode,"2");
							sprintf(t2,"%03X",disp);
							if(strlen(t2)>3){
								strcat(obcode,t2+5);
							}
							else strcat(obcode,t2);
						}
						else{
							//check base relative
							disp = loc-base;
							if(disp<4096){
								strcat(obcode,"4");
								sprintf(t2,"%03X",disp);
							}
							else{	//direct addressing mode
								strcat(obcode,"0");
								sprintf(t2,"%03X",loc);
							}
							strcat(obcode,t2);
						}
					}
				}
				else if(temp[2][0] == '@'){	// indirect
					op = opcode(temp[1]);
					op += 2;
					sprintf(t0,"%02X",op);
					strcpy(obcode,t0);
					
					loc = searchStab(temp[2]+1);
					if(loc<0){
						printf("Error : Not exist in SYMTAB, incorrect line : %d\n",line);
						fclose(imd);
						fclose(obj);
						fclose(lst);
						free(modi);
						return 1;
					}
					//check pc relative
					disp = loc-pc;
					if(-2048<=disp && disp<2048){
						strcat(obcode,"2");
						sprintf(t2,"%03X",disp);
						if(strlen(t2)>3){
							strcat(obcode,t2+5);
						}
						else strcat(obcode,t2);
					}
					else{
						//check base relative
						disp = loc-base;
						if(disp<4096){
							strcat(obcode,"4");
							sprintf(t2,"%03X",disp);
						}
						else{
							strcat(obcode,"0");
							sprintf(t2,"%03X",loc);
						}
						strcat(obcode,t2);
					}
				}
				else {
					// ','기준으로 분리시키기 for indexed addressing mode
					p0 = strtok(temp[2],", ");
					p1 = strtok(NULL,", ");
					op = opcode(temp[1])+3;	//simple
					sprintf(t0,"%02X",op);
					strcpy(obcode,t0);
					
					//set index flag
					if(p1==NULL) index = 0;
					else if(p1[0] == 'X') index = 1;
					else{
						printf("Error : Invalid operand, incorrect line : %d\n",line);
						fclose(imd);
						fclose(obj);
						fclose(lst);
						free(modi);
						return 1;
					}

					loc = searchStab(p0);
					if(loc<0){
						printf("Error : Not exist in SYMTAB, incorrect line : %d\n",line);
						fclose(imd);
						fclose(obj);
						fclose(lst);
						free(modi);
						return 1;
					}
					disp = loc-pc;
					if(-2048<=disp && disp<2048){
						if(index) strcat(obcode,"A");
						else strcat(obcode,"2");
						sprintf(t2,"%03X",disp);
						if(strlen(t2)>3){	// negative
							strcat(obcode,t2+5);
						}
						else{
							strcat(obcode,t2);
						}
					}
					else{
						disp = loc-base;
						if(disp<4096){
							if(index) strcat(obcode,"C");
							else strcat(obcode,"4");
							sprintf(t2,"%03X",disp);
						}
						else{
							if(index) strcat(obcode,"8");
							else strcat(obcode,"0");
							sprintf(t2,"%03X",loc);
						}
						strcat(obcode,t2);
					}
				}
			}
		}
		else if(!strcmp(temp[1],"BYTE")){
			p0 = strtok(temp[2],"' ");
			p1 = strtok(NULL,"' ");
			if(temp[2][0] == 'C'){
				sprintf(t0,"%02X",p1[0]);
				strcpy(obcode,t0);
				for(i=1; i<strlen(p1); i++){
					sprintf(t0,"%02X",p1[i]);
					strcat(obcode,t0);
				}
			}
			else{	// X (hexadecimal)
				strcpy(obcode,p1);
			}
		}
		else if(!strcmp(temp[1],"WORD")){
			loc = atoi(temp[2]);
			sprintf(t0,"%06X",loc);
			strcpy(obcode,t0);
		}
		else {	// RESW, RESB ...
			fprintf(lst,"\n");
		}

		if(!strcmp(temp[1],"RESW") || !strcmp(temp[1],"RESB")){
			i = strlen(text)/2;
			if(i!=0){
				fprintf(obj,"T%07X%02X%s\n",t_st,i,text);
				strcpy(text,"\0");
			}
			continue;
		}

		fprintf(lst,"%s\n",obcode);
		if(strlen(text)==0){	// current text record empt
			t_st = locctr;
			strcpy(text,obcode);
		}
		else if((strlen(text)+strlen(obcode))>60){	// over the range
			//print on obf file
			i = strlen(text)/2;
			fprintf(obj,"T%07X%02X%s\n",t_st,i,text);
			t_st = locctr;
			strcpy(text,obcode);
		}
		else{
			strcat(text,obcode);
		}

		// format 4이면서 operand에 symbol
		if(strlen(obcode) == 8 && !('0'<=temp[2][1] && temp[2][1]<='9')){
			i = strlen(text)/2-4+1;
			sprintf(t0,"M%06X05\n",t_st+i);
			strcat(modi,t0);
		}
	}

	fclose(imd);
	fclose(lst);
	fclose(obj);
	free(modi);
	return 0;
}

int isReg(char* r){
	if(!strcmp(r,"A")) return 0;
	else if(!strcmp(r,"X")) return 1;
	else if(!strcmp(r,"L")) return 2;
	else if(!strcmp(r,"B")) return 3;
	else if(!strcmp(r,"S")) return 4;
	else if(!strcmp(r,"T")) return 5;
	else if(!strcmp(r,"F")) return 6;
	else if(!strcmp(r,"PC")) return 8;
	else if(!strcmp(r,"SW")) return 9;
	else return -1;
}

void buildStab(void){
	int i;

	for(i=0; i<26; i++){
		stable[i].count = 0;
		stable[i].next = NULL;	
	}

	return;
}

int searchStab(char* label){
	int i, j;
	SNODE* curr;

	i = label[0] - 'A';

	if(stable[i].count == 0){
		return -1;
	}

	curr = stable[i].next;
	for(j=0; j<stable[i].count; j++){
		if(!strcmp(curr->label,label)) return curr->loc;	// 해당 symbol의 loc 반환
		curr = curr->next;
	}
	return -1;
}

void pushStab(char* label, int loc){
	SNODE* pnew;
	SNODE* curr;
	int i, j;

	pnew = (SNODE*)malloc(sizeof(SNODE));
	strcpy(pnew->label,label);
	pnew->loc = loc;
	pnew->next = NULL;
	i = label[0]-'A';

	if(stable[i].count == 0){	// empty key
		stable[i].next = pnew;
	}
	else {
		curr = stable[i].next;
		if(strcmp(curr->label,label) > 0){	// need to store in first
			pnew->next = stable[i].next;
			stable[i].next = pnew;
		}
		else{	// need to search appropriate location (alphabetical order)
			for(j=0; j<stable[i].count; j++){
				if(strcmp(curr->label,label) > 0){
					pnew->next = curr->next;
					curr->next = pnew;
					break;
				}
				if(curr->next == NULL){
					curr->next = pnew;
					break;
				}
				curr = curr->next;
			}
		}
	}
	stable[i].count++;
	return;
}

void symbol(void){
	int i, j;
	SNODE* curr;

	for(i=0; i<26; i++){
		if(symtab[i].count==0){
			continue;
		}
		curr = symtab[i].next;
		for(j=0; j<symtab[i].count; j++){
			printf("\t%-8s\t%04X\n",curr->label,curr->loc);
			curr = curr->next;
		}
	}	
	return;
}

void rmStab(int op){
	int i, j;
	SNODE* curr, *pCur;

	if(op==0){	//remove stable(temporary table)
		for(i=0; i<26; i++){
			if(stable[i].count == 0){
				continue;
			}
			curr = stable[i].next;
			for(j=0;j<stable[i].count-1;j++){
				pCur = curr;
				curr = pCur->next;
				free(pCur);
			}
			
		}

	}
	else{	//remove symtab(permanent table)
		for(i=0; i<26; i++){
			if(symtab[i].count == 0){
				continue;
			}
			curr = symtab[i].next;
			for(j=0;j<symtab[i].count;j++){
				pCur = curr;
				curr = pCur->next;
				free(pCur);
			}
		}
	}
	return;
}

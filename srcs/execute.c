#include "20191574.h"

//register
int reg[10];
char CC;

void initReg(void){
	int i;
	for(i=0; i<10; i++){
		reg[i] = 0;
	}
	reg[2] = totlen;
	reg[8] = exeaddr;
}

void run(void){
	int curraddr = exeaddr;
	int code, ni, xbpe, addr;
	int r1, r2;	// register
	char temp[7];
	char temp2[4];
	OPCODE* node;
	int i;
	int value;

	while(1){
		if(reg[8] == totlen){	// END program
			printf("A : %06X  X : %06X\n",reg[0],reg[1]);
			printf("L : %06X PC : %06X\n",reg[2],reg[8]);
			printf("B : %06X  S : %06X\n",reg[3],reg[4]);
			printf("T : %06X\n",reg[5]);
			printf("		End Program\n");
			exeaddr = prog;	//exeaddr 초기화
			initReg();
			return;
		}

		//get code and ni
		code = getvalue(curraddr);
		ni = code&0b00000011;
		code -= ni;
		node = searchCode(code);
		curraddr++;
		
		if(node->t[0]=='1') continue;
		else if(node->t[0]=='2'){
			i = getvalue(curraddr++);
			sprintf(temp2,"%02X",i);
			r2 = atoi(temp2+1);
			temp2[1] = '\0';
			r1 = atoi(temp2);
			
			if(code == 0x90){	//ADDR
				reg[r2] = reg[r2] + reg[r1];
			}
			else if(code == 0xB4){	//CLEAR
				reg[r1] = 0;
			}
			else if(code == 0xA0){	//COMPR
				if(reg[r1]>reg[r2]) CC = '>';
				else if(reg[r1]<reg[r2]) CC = '<';
				else CC = '=';
			}
			else if(code == 0x9C){	//DIVR
				reg[r2] = reg[r2]/reg[r1];
			}
			else if(code == 0x98){	//MULR
				reg[r2] = reg[r2]*reg[r1];
			}
			else if(code == 0xAC){	//RMO
				reg[r2] = reg[r1];
			}
			else if(code == 0xA4){	//SHIFTL
				for(i=0;i<r2;i++){
					reg[r1] *= 2;
				}
			}
			else if(code == 0xA8){	//SHIFTR
				for(i=0;i<r2;i++){
					reg[r1] /= 2;
				}
			}
			else if(code == 0x94){	//SUBR
				reg[r2] = reg[r2] - reg[r1];
			}
			else if(code == 0xB8){	//TIXR
				reg[1]++;
				if(reg[1]>reg[r1]) CC = '>';
				else if(reg[1]<reg[r1]) CC = '<';
				else CC = '=';
			}
		}
		else if(node->t[0]=='3'){
			//get bpxe
			xbpe = getvalue(curraddr++);
			addr = xbpe&0b00001111;
			xbpe -= addr;
			sprintf(temp,"%01X",addr);		

			//get address
			addr = getvalue(curraddr++);
			sprintf(temp2,"%02X",addr);
			strcat(temp,temp2);				
			if((xbpe&0b00010000)>0){	// format 4
				addr = getvalue(curraddr++);
				sprintf(temp2,"%02X",addr);
				strcat(temp,temp2);
				addr = strtol(temp,NULL,16);
				if(addr >= 0x80000){
					addr = -1*(0xFFFFF - addr + 1);
				}
			}
			else{
				addr = strtol(temp,NULL,16);
				if(addr >= 0x800){
					addr = -1*(0xFFF - addr + 1);
				}
			}

			if((xbpe&0b00100000)>0){	//pc relative
				addr += curraddr;
			}
			else if((xbpe&0b01000000)>0){	//base relative
				addr += reg[3];
			}
			
			if((xbpe&0b10000000)>0){	//index
				addr += reg[1];
			}		
	
			if(ni==2){	//indirect
				addr = getmem(addr);	// 내부 값이 다시 주소가 됨.
				value = getmem(addr);
			}
			else if(ni==1){	//immediate
				value = addr;
			}
			else if(ni==3){	//simple
				value = getmem(addr);
			}

			if(code == 0x18){	//ADD
				reg[0] = value;
			}
			else if(code == 0x40){	//AND
				reg[0] = reg[0]&value;
			}
			else if(code == 0x28){	//COMP
				if(reg[0]<value) CC = '<';
				else if(reg[0]>value) CC = '>';
				else CC = '=';
			}
			else if(code == 0x24){	//DIV
				reg[0] = reg[0]/value;
			}
			else if(code == 0x3C){	//J	
				printf("J\n");
				curraddr = addr;
			}
			else if(code == 0x30){	//JEQ
				if(CC == '=') {
					curraddr = addr;
				}
			}
			else if(code == 0x34){	//JGT
				if(CC == '>') {
					curraddr = addr;
				}
			}
			else if(code == 0x38){	//JLT
				if(CC == '<') {
					curraddr = addr;
				}
			}
			else if(code == 0x48){	//JSUB
				reg[2] = curraddr;
				curraddr = addr;
			}
			else if(code == 0x00){	//LDA
				reg[0] = value;
			}
			else if(code == 0x68){	//LDB
				reg[3] = value;
			}
			else if(code == 0x50){	//LDCH
				reg[0] = reg[0]&0b111111111111111100000000;
				value = getvalue(addr);
				reg[0] += value;
			}
			else if(code == 0x08){	//LDL
				reg[2] = value;
			}
			else if(code == 0x6C){	//LDS
				reg[4] = value;
			}
			else if(code == 0x74){	//LDT
				reg[5] = value;
			}
			else if(code == 0x04){	//LDX
				reg[1] = value;
			}
			else if(code == 0x20){	//MUL
				reg[0] = reg[0]*value;
			}
			else if(code == 0x44){	//OR
				reg[0] = reg[0]|value;
			}
			else if(code == 0xD8){	//RD - I/O
				continue;
			}
			else if(code == 0x4C){	//RSUB
				curraddr = reg[2];
			}
			else if(code == 0x0C){	//STA
				setmem(addr,reg[0]);
			}
			else if(code == 0x78){	//STB
				setmem(addr,reg[3]);
			}
			else if(code == 0x54){	//STCH
				i = reg[0]&0b000000000000000011111111;
				sprintf(temp2,"%02X",i);
				sprintf(temp,"%05X",addr);
				edit(temp, temp2);
			}
			else if(code == 0x14){	//STL
				setmem(addr,reg[2]);
			}
			else if(code == 0x7C){	//STS
				setmem(addr,reg[4]);
			}
			else if(code == 0xE8){	//STSW
				setmem(addr,reg[9]);
			}
			else if(code == 0x84){	//STT
				setmem(addr,reg[5]);
			}
			else if(code == 0x10){	//STX
				setmem(addr,reg[1]);
			}
			else if(code == 0x1C){	//SUB
				reg[0] = reg[0]-value;
			}
			else if(code == 0xE0){	//TD - I/O
				CC = '<';
			}
			else if(code == 0x2C){	//TIX
				reg[1]++;
				if(reg[1]<value) CC = '<';
				else if(reg[1]>value) CC = '>';
				else CC = '=';
			}
			else if(code == 0xDC){	//WD - I/O
				continue;
			}
		}	

		reg[8] = curraddr;
		if(bp[curraddr]==1){	// break point
			printf("A : %06X  X : %06X\n",reg[0],reg[1]);
			printf("L : %06X PC : %06X\n",reg[2],reg[8]);
			printf("B : %06X  S : %06X\n",reg[3],reg[4]);
			printf("T : %06X\n",reg[5]);
			printf("		Stop at checkpoint[%X]\n",reg[8]);
			exeaddr = reg[8];
			return;
		}
	}

	return;
}

// get value (m .. m+2)
int getmem(int staddr){
	char temp[7];
	char temp2[4];
	int value;

	value = getvalue(staddr++);
	sprintf(temp,"%02X",value);
	value = getvalue(staddr++);
	sprintf(temp2,"%02X",value);
	strcat(temp,temp2);
	value = getvalue(staddr);
	sprintf(temp2,"%02X",value);
	strcat(temp,temp2);

	value = strtol(temp,NULL,16);
	return value;
}

// save value on (m .. m+2)
void setmem(int addr, int value){
	char temp[7];
	char temp2[4];
	char address[7];

	sprintf(address,"%05X",addr);
	sprintf(temp,"%06X",value);

	strncpy(temp2,temp,2);
	temp2[2] = '\0';
	edit(address,temp2);

	addr++;
	sprintf(address,"%05X",addr);
	strncpy(temp2,temp+2,2);
	temp2[2] = '\0';
	edit(address,temp2);

	addr++;
	sprintf(address,"%05X",addr);
	strcpy(temp2,temp+4);
	edit(address,temp2);	

	return;		
}

void set_bp(char* address){
	int addr;

	//해당 addr을 array의 index로 두고 값을 1로 set.
	addr = strtol(address,NULL,16);
	bp[addr] = 1;	
	printf("		[ok] create breakpoint %X\n",addr);


	return;
}

void bpClear(void){
	int i;
	for(i=0; i<totlen; i++){
		bp[i] = 0;
	}
	printf("		[ok] clear all breakpoints\n");
	return;
}

void print_bp(void){
	int i;
	printf("		breakpoint\n");
	printf("		----------\n");
	for(i=0; i<totlen; i++){	// 1로 set된 array의 index를 print
		if(bp[i]==1) printf("		%X\n",i);
	}
	return;
}

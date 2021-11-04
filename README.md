컴파일 방법	: shell에 make 입력

실행 방법	: shell에 ./20191574.out 입력

프로그램을 실행시키면 화면에 sicsim> 이 출력된다.
이는 SIC/XE의 shell과 같은 역할이고 이 화면에 원하는 명령어를 입력한다.

명령어 실행
- 명령어는 소문자만 인식이 가능하다.
- 공백포함 100자리 이상의 명령어는 입력이 불가하다.


/* Shell 명령어 */

1. h 또는 help
	사용 가능한 명령어를 모두 출력한다.
2. d 또는 dir
	현재 디렉터리 내의 파일 정보를 출력한다.
	디렉터리인 경우 파일명 옆에 '/', 실행파일의 경우 '*'가 출력된다.
3. q 또는 quit
	프로그램을 종료한다.
4. hi 또는 history
	shell에 입력했던 명령어들을 입력 순으로 출력한다.
5. type filename
	filename 부분에는 출력을 원하는 파일의 이름을 넣고 실행하면 된다.
	filename에 해당하는 파일을 현재 디렉터리에서 읽어 화면에 그대로 출력한다.	


/* Memory 명령어 */

1. du 또는 dump
	i)	du[mp]
		이전 까지 호출했던 dump가 출력한 이후 번지부터 160byte의 메모리를 출력한다.
		처음 호출하는 경우에는 0번째 번지부터 160byte를 출력한다.
	ii) du[mp] start
		해당 인자는 16진수 형태여야 한다.
		start 번지부터 160byte의 메모리를 출력한다.
	iii) du[m] start, end
		해당 인자들은 16진수 형태여야 한다.
		start 번지부터 end 번지의 메모리들을 출력한다.
2. e 또는 edit
	e[dit] address, value의 꼴로 입력해야 한다.
	address, value 모두 16진수 형태여야 한다.
	메모리의 address 번지의 값을 value에 지정된 값으로 변경한다.
3. f 또는 fill
	f[ill] start, end, value의 꼴로 입력해야 한다.
	start, end, value 모두 16진수의 형태여야 한다.
	메모리의 start 번지부터 end번지까지의 값을 value에 지정된 값으로 변경한다.
4. reset
	메모리 전체를 0으로 초기화한다.


/* OPCODE 명령어 */

1. opcode mnemonic
	mnemonic의 자리에는 opcode를 찾고 싶은 명령어를 입력하면 된다.
	ex ) opcode ADD, opcode LDB
	단, mnemonic은 모두 대문자로 입력해야 한다.
	적절한 명령어를 입력하면 해당 명령어의 opcode를 출력한다.
2. opcodelist
	opcode Hash Table의 내용을 출력한다.

/* SIC/XE 어셈블러 명령어 */

1. assemble filename
	filename 부분에는 assemble하기를 원하는 소스 파일의 이름을 입력하면 된다.
	filename에 해당하는 소스 파일을 읽어서 object 파일과 리스팅 파일을 만든다.
	[filename.obj, filename.lst] 에 해당하는 파일 두 개가 만들어 진다.
	에러가 발생하는 경우 에러가 발생한 라인을 출력한다.

2. symbol
	assemble 과정 중에서 생성된 symbol table을 화면에 출력한다.
	가장 최근에 성공적으로 assemble된 파일의 symbol table을 알파벳 오름차순으로 출력한다.


/* LOAD 명령어 */

1. progaddr address
	program이 load 되기를 원하는 주소를 token으로 넘겨 progaddr을 초기화한다.
	프로그램 수행 시 자동으로 0x00으로 지정된다.

2. loader filename1 [filename2] [filename3]
	filename에 해당하는 object file을 읽어 linking 작업을 수행하고, 가상 메모리에 load 한다.
	파일은 3개까지 입력 가능하다.
	load가 정상적으로 끝나면 load map이 화면에 출력된다.


/* RUN 명령어 */

1. bp address
	address에 해당하는 주소를 break point로 지정한다.
	run을 수행했을 때 breakpoint가 되는 순간까지 프로그램이 실행되고 정지한다.
	다음 run을 수행하면 정지된 breakpoint부터 수행된다.
	break point가 없는 경우 프로그램 끝까지 수행된다. 

2. bp clear
	지정된 모든 break point를 초기화한다.

3. bp
	지정된 모든 break point를 출력한다.

4. run
	메모리에 저장된 프로그램을 실행한다.
	progaddr 명령어로 지정된 주소부터 실행된다.
	run이 정지된 시점의 register 값을 화면에 출력한다.

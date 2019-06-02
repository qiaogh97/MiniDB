/*
	3.4版本说明
	已完成功能： 
		实现了创建数据库，创建表，插入，查询，退出，删除，更新，保存。
	日志：1.如果插入的数据主键重复，则提示错误 
		  2.如果在创建表的时候创建两个主键，则提示错误。
		  3.插入很多数据的时候，会提示“不支持的SQL类型” 
		　4.加上LOAD函数
		  5.加上一些提示语句
		  6.实现读取 
		  7.Load函数，读取之后，插入就插入不进去了(已解决，现在可以插入)
		  8.创建表的时候，居然可以创建多个主键 (已解决，不能创建多个主键了)
		  9. 没有找到要删除的记录，应该提示一下
		  10.不能允许创建重复的表 
*/
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAX_STRING_LENGTH 20//表头存储的字符串类型的数据最长是多少 
#define MAX_DATA_LENTH 10     //表头最多有几个 
#define MAX_NAME_LENTH 21     //表头的名称最长是多少 

union Elemtype//定义元素类型
{
	char CHAR;   //字符型
	char STRING[MAX_STRING_LENGTH];  //字符串型
	int INT;//整数型
};

struct Node//定义元素节点类型
{
	union Elemtype arr[MAX_DATA_LENTH];//字段数目，也就是表头最多有几个 
	struct Node *next;
};
 
struct Data//字段基本信息
{
	char strName[MAX_NAME_LENTH];  //字段名
	int nLen;  //字段长度
	char cType; //字段类型
    int key;//是否是主码
};

struct DataNode//记录字段基本信息的节点
{
	struct Data data;//字段基本信息结构体
	struct DataNode *pNext;//指针
};


struct Table//记录所有字段信息的表结构体
{						//这个结构体的作用就是保存一个表名称，以及该表字段的所有信息 
	char NameOfTable[20];//保存用户输入的表的名称 
	int nLen;//表的长度
	struct Data Head[MAX_NAME_LENTH];//记录字段全部信息的数组
	int nCount;//记录字段数目 
};

struct File//定义文件结构
{		   //这个结构体的作用是 定义一个表的表头指针，和一个存储数据Data的指针 ，只能存储一个表，要存储多表必须建立数组 
	struct Table *pTable;//表头指针
	struct Node *pHead;//数据区指针
	int    nCount;//数据记录的条数，即有几行 
};

//以上均为数据结构 

char SQL[200]; //保存用户输入的SQL语句以便于解析 
char NameOfDatabase[100];//保存用户输入的数据库的名称 

struct Table MYTABLE[10];
struct File MYFILE[10]; 


int NumberOfTable=0;
char DataBase_Path[100];//保存的是数据库所存储的路径				  
int IsCreateDatabase=0;				  
				  

int SqlExplain(char *SQL);
/*
	函数解析：输入一个SQL字符串，如：	CREATE-DATABASE-MYDATABASE   则返回1，并把数据库的名字赋值给NameOfDatabase   解析意为：创建一个数据库 
	CREATE-TABLE-MYTABLE(Number-int-1;Name-string-0;Gender-char-0;) 返回2，把表的名字赋值给NameOfTable，并准确赋值给Deposit_SQL[10][25] 解析意为：创建一个表
	INSERT-MYTABLE("1001","Jack","b")返回3，解析意为：插入数据
	DELETE-MYTABLE("1001","Jack","b")返回4，解析意为：删除数据
	UPDATE-MYTABLE("1001","Jack","g")返回5，解析意为：更新数据
	SELETE-MYTABLE("Number","Name","Gender")-WHERE-NUMBER="1001"返回6，解析意为：查询数据  
*/
void CreateDatabase(char *SQL);
/*
	函数解析:创建一个数据库，让用户输入一个路径，在用户指定的路径下创建一个文件夹，文件夹名为数据库名 
*/
void CreateTable(char *SQL);
/*
	创建一个表，读取用户输入的字段信息，并用链表的形式保存到内存中 
*/
void InsertData(char *SQL);
/*
	插入一条数据，读取用户输入的语句之后，用链表的形式保存到内存中 
*/
void DeleteData(char *SQL);
/*
	删除一条数据，读取用户输入的语句之后，查找到要删除的语句，删除链表节点并释放内存 
*/
void UpdateData(char *SQL);
/*
	更新数据，判断主键，主键不能被更新，然后修改数据到内存 
*/ 
void SelectData(char *SQL); 
/*
	查询语句，从内存中查询，还要判断WHERE 
*/
void Save(char *SQL);
/*
	保存语句，用户输入一个指定的路径及文件名，保存成为一个TXT文件 
*/
void Load(char *SQL);
/*
	加载语句，将TXT文件加载到内存中，一次只能加载一个表 
*/ 
int  Exit(char *SQL); 

 
int IsCreate(char *SQL);//判断第一个单词是不是CREATE，如果是，返回1

int IsDataBase(char *SQL);//判断是否含有DATABASE，如果有，返回1

int IsTable(char *SQL);//判断是否含有TABLE，如果有，返回1

int IsInsert(char *SQL);//判断是否含有INSERT ，如果有，返回1

int IsDelete(char *SQL);//判断是否含有DELETE，如果有，返回1

int IsUpdate(char *SQL);//判断是否含有UPDATE，如果有，返回1

int IsSelect(char *SQL);//判断是否含有SELECT，如果有，返回1 

int IsExit(char *SQL); //判断是否含有EXIT，如果有，返回1 

int IsLoad(char *SQL);//判断是否含有LOAD,如果有，返回，1 
 
int main(){    		    
	
	int exit=0;
	
	int temp2;
	for(temp2=0;temp2<10;temp2++){
		MYFILE[temp2].pHead=NULL;
		//MYFILE[temp2].pHead->next=NULL;
		MYFILE[temp2].pTable=NULL;
	} 
	
	while(1){
		int i=0;
		char temp1[30];
		temp1[0]='\0'; 
		for(i=0;i<strlen(SQL);i++){
			SQL[i]='\0'; 
		} 
		printf("请输入SQL语句:(用'-'连接,全部大写)\n");
		scanf("%s",SQL); 
		 
		int flag1;
		flag1=SqlExplain(SQL);
		switch(flag1)
			{
				case 1:
					{
						//printf("创建一个数据库\n");
						CreateDatabase(SQL);
						IsCreateDatabase=1; 
						break;
					} 
					 
				case 2:
					{
						//printf("创建一个表\n");
						if(IsCreateDatabase==0)
						printf("请先创建一个数据库\n"); 
						else{
							CreateTable(SQL);
						} 
						break;
					} 
				case 3:
					{
						//printf("插入数据insert\n");
						InsertData(SQL);
						break; 
					} 
				case 4:
					{
						//printf("删除数据delete\n");
						DeleteData(SQL);
						break; 
					} 
				case 5:
					{
						//printf("更新数据update\n");
						UpdateData(SQL);
						break; 
					} 
				case 6:
					{
						//printf("查询语句\n");
						SelectData(SQL);
						break; 
					} 
				case 7:
					{
						//printf("保存语句\n"); 
						Save(SQL);
						break;
					}
				case 8:
					{
						//printf("退出语句\n");
						exit = Exit(SQL);
						break; 
					} 
				case 9:
					{
						//printf("读取语句\n");
						Load(SQL);
						break; 
					} 
				default:	printf("不支持的SQL语句类型\n");	
			} 
			if(exit==1){
				break;
			} 
	}
}
int SqlExplain(char *SQL){
	//printf("跳转到SQL语言解析函数\n");
	if((IsCreate(SQL)==1)&&(IsDataBase(SQL))){
		return 1;
		}
	else if((IsCreate(SQL)==1)&&(IsTable(SQL))){
		return 2;
		}
	else if(IsInsert(SQL)==1){
		return 3;
	} 
	else if(IsDelete(SQL)==1){
		return 4;
	} 
	else if(IsUpdate(SQL)==1){
		return 5;
	} 
	else if(IsSelect(SQL)==1){
		return 6;
	}
	else if(IsSave(SQL)==1){
		return 7;
	} 
	else if(IsExit(SQL)==1){
		return 8;
	}
	else if(IsLoad(SQL)==1){
		return 9;
	} 
} 

void CreateDatabase(char *SQL){      		//创建一个数据库文件夹    
	//printf("这里是创建数据库的函数\n");   
	char b[100]="md ";
	int m=16;
	int n=0;
	for(;m<strlen(SQL);m++,n++){
		NameOfDatabase[n]=SQL[m]; 
	} 
	NameOfDatabase[n]='\0'; 
	printf("请输入数据库要保存的路径:\n"); 
	char Table_Path[100]={'\0'}; 
	scanf("%s",DataBase_Path);
	strcat(DataBase_Path,"\\"); 
	strcat(DataBase_Path,NameOfDatabase); 
	system(strcat(b,DataBase_Path));
	printf("数据库已经成功创建\n"); 
} 

void CreateTable(char *SQL){
	//printf("这里是创建表的函数\n"); 
	char Deposit_SQL[10][25];/*保存用户输入的字段信息，如：SQL语句：CREATE-TABLE-MYTABLE(Number-int-1;Name-string-0;Gender-char-0;)
则字符串数组应为: Deposit_SQL[0]:Number-int-1
				  Depoist_SQL[1]:Name-string-0
				  Deposit_SQL[2]:Gender-char-0; */
	char DataName2[20];//保存的是表的名字 
	//char DataName3[20];//保存的是创建的TXT文本文件名称 
	char Table_Path[50];//保存的是表的txt文本要保存的路径,直接用数据库的路径就可以了 
	int m=13;			//CREATE-TABLE-MYTABLE(Number-int-1;Name-string-0;Gender-char-0;)
	int n=0;
	for(;m<strlen(SQL)&&SQL[m]!='(';m++,n++){
		DataName2[n]=SQL[m]; 
	} 
	DataName2[n]='\0';
	strcpy(MYTABLE[NumberOfTable].NameOfTable,DataName2);
	//printf("获取表名称%s成功！\n",DataName2); 		  //调试用语句
	//printf("DataName2的长度为%d\n",str//len(DataName2));//调试用语句 
	
	int i;
	for(i=0;i<NumberOfTable;i++) // 搜索已创建的所有表 
	{
		if(strcmp(MYTABLE[i].NameOfTable,DataName2)==0){
			printf("创建失败，已创建表%s\n",DataName2); 
			return;
		} 
	}
	
	//char Deposit_SQL[10][25];
	int Record_Data=0; 
	if(SQL[m]=='('){			//这段代码的作用是把SQL语句中创建表的字段信息先存储到一个二维数组里面Deposit_SQL[][] 
		int j=0;				
		int k=m+1;				//k=m+1是为了跳过那个'(' 
		int l=0;
		for(j=0;j<10;j++){ 		
			for(l=0;SQL[k]!=';';k++,l++){  //以分号为间隔，进入下一层循环 
				Deposit_SQL[j][l]=SQL[k]; 
			}
			Deposit_SQL[j][l]='\0'; 		//为了防止出意外，在每个一维数组后面加一个'\0' 
			Record_Data++;
			if(SQL[++k]==')'){
				break;						//++k正好跳过了分号，如果下一个是')'就结束循环，不能用k++ 
			} 
		}
	}

	//printf("一共输入了%d个字段\n",Record_Data); //调试用语句 
	
	//CREATE-TABLE-MYTABLE(Number-int-1;Name-string-0;Gender-char-0;)
	int k=0,l=0;
	int PrimaryKey=0;
	char *p1;
	p1=SQL;
	for(;*(p1+1)!=';';p1++);
	for(k=0;k<Record_Data;k++){
		if(p1[0]=='1'){
			PrimaryKey++;
			p1++;
			for(;*(p1+1)!=';'||*p1==')';p1++);
		} 
		else{
			for(;*(p1+1)!=';'||*p1==')';p1++); 
		}  
	} 
	if(PrimaryKey>1){
		printf("创建的表中有%d个主键，无法创建此表\n",PrimaryKey);
		return; 
	} 
	else if(PrimaryKey<1){
		printf("该表中没有主键，无法创建此表\n");
		return; 
	} 
	
	
	MYTABLE[NumberOfTable].nCount=Record_Data;
	
	
	int m2=0;
	int n2=0;
	int n3=0;
	int count_line=0;
	for(;m2<Record_Data;m2++){
		n3=0;
		n2=0;
		count_line=0;
		for(;n2<strlen(Deposit_SQL);n3++,n2++){
			if(Deposit_SQL[m2][n2]=='-'){
				if(count_line==0){
				MYTABLE[NumberOfTable].Head[m2].strName[n3]='\0'; 
				} 
				count_line++;
				n2++;
			} 
			if(count_line==0){
				MYTABLE[NumberOfTable].Head[m2].strName[n3]=Deposit_SQL[m2][n2];
			}
			else if(count_line==1){
				
				if(Deposit_SQL[m2][n2]=='s'||Deposit_SQL[m2][n2]=='S'){
					MYTABLE[NumberOfTable].Head[m2].cType='s';
					MYTABLE[NumberOfTable].Head[m2].nLen=10;
					n2+=5;
				}
				else if(Deposit_SQL[m2][n2]=='i'||Deposit_SQL[m2][n2]=='I'){
					MYTABLE[NumberOfTable].Head[m2].cType='i';
					MYTABLE[NumberOfTable].Head[m2].nLen=10;
					n2+=2;
				} 
				else if(Deposit_SQL[m2][n2]=='c'||Deposit_SQL[m2][n2]=='C'){
					MYTABLE[NumberOfTable].Head[m2].cType='c';
					MYTABLE[NumberOfTable].Head[m2].nLen=1; 
					n2+=3;
				}
			} 
			else if(count_line==2){
				MYTABLE[NumberOfTable].Head[m2].key = Deposit_SQL[m2][n2]-'0';
				break; 
			} 
		} 
	} 
	
	MYFILE[NumberOfTable].pTable=&MYTABLE[NumberOfTable];
	struct Node *Head = (struct Node*)malloc(sizeof(struct Node));
	Head->next=NULL; 
	MYFILE[NumberOfTable].pHead=Head;
	
	/*for(m=0;m<Record_Data;m++)
	{
		printf("字段名%s，字段长度%d，字段类型%c，是否为主键%d\n",MYTABLE[NumberOfTable].Head[m].strName,MYTABLE[NumberOfTable].Head[m].nLen,MYTABLE[NumberOfTable].Head[m].cType,MYTABLE[NumberOfTable].Head[m].key ); 
	} //调试用语句 
	*/
	printf("成功创建表%s\n",MYTABLE[NumberOfTable].NameOfTable); 
	NumberOfTable++;
}

void InsertData(char *SQL){
	int m=0;		//m用来保存第几个是主键 
	char cType='\0';//保存主键的类型 
	char *p=SQL;
	for(;*(p-1)!='-';p++);//p指到表名的第一个字母处 
	char *p1=p;
	for(;*(p1+1)!='(';p1++);//p1指到表名的最后一个字母处 
	char tableName[20];
	strncpy(tableName,p,p1-p+1);//从p开始，把以p为地址后的p1-p+1个字母都赋值到tableName中 
	tableName[p1-p+1]='\0';//在最后加上一个'\0' 
	
	int i,j;
	for(i=0;i<NumberOfTable;i++) // 搜索已创建的所有表 
	{
		//MYFILE[i].pHead.next=NULL; 
		if(strcmp(MYTABLE[i].NameOfTable,tableName)==0)  // 表名如果相同,则可以插入 
		{
			for(j=0;MYTABLE[i].nCount;j++){
				if(MYTABLE[i].Head[j].key==1){
					cType=MYTABLE[i].Head[j].cType; 
					break;
				}
				else{
					m++;
				} 
			} 
			MYFILE[i].nCount++; // 该表的记录数 +1 
			
			struct Node *aNode = (struct Node*)malloc(sizeof(struct Node)); // 新建结点,用来存 
			aNode->next=NULL;
			
			p1+=3;  // 让 p和p1 指向 第一个字段内容的 第一个字符 
			p=p1;   //现在p1和p都指向INSERT-MYTABLE("1001","Jack","b")中的第一个1 
			for(;*(p1+1)!='"';p1++); // p1指向 第一个字段内容的 最后一个字符 
			
			int k; 
			for(k=0;k<MYTABLE[i].nCount;k++) // 按照各个字段来读取信息, nCount是字段数 
			{
				for(;*(p1+1)!='"';p1++);
				switch(MYTABLE[i].Head[k].cType)
				{
					case 'i':
					{
							aNode->arr[k].INT=0;//要存储的类型是int型的，所以为了读取方便，把INT的值先赋值为0. 
							for(;p<=p1;p++)
							{
								aNode->arr[k].INT=aNode->arr[k].INT*10+*p-'0';
							}
							break;
					}
						
					case 'c':
						{
							aNode->arr[k].CHAR=*p1;
							break;
						}
						
					case 's':
						{
							strncpy(aNode->arr[k].STRING,p,p1-p+1);
							aNode->arr[k].STRING[p1-p+1]='\0';
							break;
						}
				}
				
				p1+=4;
				p=p1;
			}
			/*
			aNode->next=MYFILE[i].pHead;
			MYFILE[i].pHead=aNode;
			*/
			
			struct Node *p2;
			/*if(MYFILE[i].pHead==NULL){		//已经插入了头结点，这条语句可能永远不会执行！ 
				struct Node *Head = (struct Node*)malloc(sizeof(struct Node)); 
				MYFILE[i].pHead=Head;
				Head->next=aNode;
				aNode->next=NULL;
			} */
			//else{ 
			
			p2=MYFILE[i].pHead; 
			for(;p2->next!=NULL;p2=p2->next){
				 switch(cType){
				 	case 'i':{
				 		 if(p2->arr[m].INT==aNode->arr[m].INT){
						 	printf("已存在主键%d,插入失败\n",aNode->arr[m].INT);
							free(aNode); 
							return; 	
						 } 	 
						 break;
					}
					case 'c':{
					 	 if(p2->arr[m].CHAR == aNode->arr[m].CHAR ){
						 	printf("已存在主键%c,插入失败\n",aNode->arr[m].CHAR);
							free(aNode); 
							return; 	
						 } 	 
						 break;
					}
					case 's':{
					 	 if(strcmp(p2->arr[m].STRING,aNode->arr[m].STRING)==0){
						 	printf("已存在主键%s,插入失败\n",aNode->arr[m].STRING);
							free(aNode); 
							return; 	
						 } 	
						 break;
					} 
				 } 
			} 
			switch(cType){
				 	case 'i':{
				 		 if(p2->arr[m].INT==aNode->arr[m].INT){
						 	printf("已存在主键%d,插入失败\n",aNode->arr[m].INT);
							free(aNode); 
							return; 	
						 }
						 else{
						 	p2->next=aNode;
							aNode->next=NULL; 
							printf("记录插入完毕!\n");
						 } 	 
						 break;
					}
					case 'c':{
					 	 if(p2->arr[m].CHAR == aNode->arr[m].CHAR ){
						 	printf("已存在主键%c,插入失败\n",aNode->arr[m].CHAR);
							free(aNode); 
							return; 	
						 } 	
						 else{
						 	p2->next=aNode;
							aNode->next=NULL; 
							printf("记录插入完毕!\n");
						 } 
						 break;
					}
					case 's':{
					 	 if(strcmp(p2->arr[m].STRING,aNode->arr[m].STRING)==0){
						 	printf("已存在主键%s,插入失败\n",aNode->arr[m].STRING);
							free(aNode); 
							return; 	
						 } 	
						 else{
						 	p2->next=aNode;
						 	aNode->next=NULL; 
							printf("记录插入完毕!\n");
						 } 
						 break;
					} 
				 }
				break;
		}//if语句的右括号 
	}//for语句的右括号 

	if(i==NumberOfTable)
	{
		printf("未创建该表,请确认表名!\n"); 
	}

}

void DeleteData(char *SQL){					//删除语句:DELETE-FROM-MYTABLE-WHERE-"Name=Jack" 
	//printf("这里是删除函数\n");
	char *p1=SQL;
	for(;*(p1-1)!='-';p1++);
	p1++; 
	for(;*(p1-1)!='-';p1++);//p1指到表名的第一个字母处 
	char *p2=p1;
	for(;*(p2+1)!='-';p2++);//p2指到表名的最后一个字母处 
	char TableName[20];
	strncpy(TableName,p1,p2-p1+1);//从p1开始，把以p1为地址后的p2-p1+1个字母都赋值到TableName中 
	TableName[p2-p1+1]='\0';//在TableName最后加上一个'\0'
	
	int i,j,temp1;
	int flag1=0;//flag1用来保存限制的字段在字段中是第几个
	//char cType='\0';
	struct Node *p3,*p4;
	p3=p4=NULL;
	char TempFieldName[20]={'\0'};//这个变量存储的是例子中的Name 
	char TempDataName[20]={'\0'};//这个变量存储的是例子中的Jack
	int TempInt=0;//如果是int类型，则把转换后的存储到这里
	int Delete_Count=1;//记录下删除了几条数据 
	for(i=0;i<NumberOfTable;i++) // 搜索已创建的所有表 
	{ 
		if(strcmp(MYTABLE[i].NameOfTable,TableName)==0)  // 表名如果相同,则可以删除 
		{
			//MYFILE[i].pHead=(struct Node*)malloc(sizeof(struct Node)); 
			p4=MYFILE[i].pHead;
			p3=p4->next; 
			for(;*(p1-1)!='"';p1++);
			for(;*(p2+1)!='=';p2++);
			strncpy(TempFieldName,p1,p2-p1+1);     //从p1开始，把以p1为地址后的p2-p1+1个字母都赋值到TempFieldName中
			for(j=0;j<MYTABLE[i].nCount;j++){     //用来查找WHERE语句限制的是哪个字段  
				 
				if(strcmp(TempFieldName,MYTABLE[i].Head[j].strName)==0){
					for(;*(p1-1)!='=';p1++);
					for(;*(p2+1)!='"';p2++);
					strncpy(TempDataName,p1,p2-p1+1);     //从p1开始，把以p1为地址后的p2-p1+1个字母都赋值到TempDataName中
					if(p3==NULL){
						printf("该表中没有插入任何数据，无法删除\n");
						return; 	
					} 
					else{
						if(p3->next==NULL){
							switch(MYTABLE[i].Head[j].cType)//起始判断！！ 
								{
									case 'i':
									{
										TempInt=0;
										for(temp1=0;temp1<strlen(TempDataName);temp1++){
											TempInt=TempInt*10+(TempDataName[temp1]-'0'); 
										} 				//把TempDataName转换成int类型并保存在TempInt中。
										
										if(p3->arr[flag1].INT!=TempInt){
											printf("未找到要删除的节点，删除失败\n");
											return;
										}
										else if(p3->arr[flag1].INT==TempInt){
											p4->next=NULL;
											free(p3);
											printf("成功删除第%d条数据!\n",Delete_Count); 
											Delete_Count++;
											MYFILE[i].nCount--; // 该表的记录数 -1 
											return;
										} 
										
										break;
									} 
										
									
										
									case 'c':
										{
											if(p3->arr[flag1].CHAR!=TempDataName[0]){
													printf("未找到要删除的节点，删除失败\n"); 
													return;
											}
											else if(p3->arr[flag1].CHAR==TempDataName[0]){
												p4->next=NULL;
												free(p3);
												printf("成功删除第%d条数据!\n",Delete_Count); 
												Delete_Count++;
												MYFILE[i].nCount--; // 该表的记录数 -1 
												return;
											} 
											
											
												break;
											
										}
										
									case 's':
										{
											if(strcmp(p3->arr[flag1].STRING,TempDataName)!=0){
												printf("未找到要删除的节点，删除失败\n"); 
												return;
											}
											else if(strcmp(p3->arr[flag1].STRING,TempDataName)==0){
												p4->next=NULL;
												free(p3);
												printf("成功删除第%d条数据!\n",Delete_Count); 
												Delete_Count++;
												MYFILE[i].nCount--; // 该表的记录数 -1 
												return;
											}
											break;
										
					
								 	    }//case: s 
					}//switch
						} 
						while(p3->next){			
							switch(MYTABLE[i].Head[j].cType)		//中间判断！！ 
								{
									case 'i':
									{
										TempInt=0;
										for(temp1=0;temp1<strlen(TempDataName);temp1++){
											TempInt=TempInt*10+(TempDataName[temp1]-'0'); 
										} 				//把TempDataName转换成int类型并保存在TempInt中。
										
										if(p3->arr[flag1].INT!=TempInt){
											if(p3->next==NULL){
												printf("未找到要删除的节点，删除失败\n"); 
											} 
											else{
												p4=p4->next;
												p3=p4->next;
											} 
										}
										else if(p3->arr[flag1].INT==TempInt){
											p4->next=p3->next;
											free(p3);
											printf("成功删除第%d条数据!\n",Delete_Count); 
											Delete_Count++;
											MYFILE[i].nCount--; // 该表的记录数 -1 
											p3=p4->next;
										} 
										
										break;
									} 
										
									
										
									case 'c':
										{
											if(p3->arr[flag1].CHAR!=TempDataName[0]){
												if(p3->next==NULL){
													printf("未找到要删除的节点，删除失败\n"); 
												} 
												else{
													p4=p4->next;
													p3=p4->next;
												} 
											}
											else if(p3->arr[flag1].CHAR==TempDataName[0]){
												p4->next=p3->next;
												free(p3);
												printf("成功删除第%d条数据!\n",Delete_Count); 
												Delete_Count++;
												MYFILE[i].nCount--; // 该表的记录数 -1 
												p3=p4->next;
											} 
												break;
											
										}
										
									case 's':
										{
											if(strcmp(p3->arr[flag1].STRING,TempDataName)!=0){
												if(p3->next==NULL){
													printf("未找到要删除的节点，删除失败\n"); 
												} 
												else{
													p4=p4->next;
													p3=p4->next;
												} 
											}
											else if(strcmp(p3->arr[flag1].STRING,TempDataName)==0){
												p4->next=p3->next;
												free(p3);
												printf("成功删除第%d条数据!\n",Delete_Count); 
												Delete_Count++;
												MYFILE[i].nCount--; // 该表的记录数 -1 
												p3=p4->next;
											}
											break;
										
					
									}//case: s 
					}//switch
					//if(MYTABLE[i].Head[j].key==1){
						//break;
					//} 
					}//while循环 
					
					switch(MYTABLE[i].Head[j].cType){			//最后判断！! 
						case 'i':
							{
								TempInt=0;
								for(temp1=0;temp1<strlen(TempDataName);temp1++){
									TempInt=TempInt*10+(TempDataName[temp1]-'0'); 
								} 				//把TempDataName转换成int类型并保存在TempInt中。
								
								if(p3->arr[flag1].INT!=TempInt){
									if(Delete_Count==1){
											printf("未找到要删除的数据，无法进行删除操作！\n"); 
									}
									return;
								}
								else if(p3->arr[flag1].INT==TempInt){
									p4->next=NULL;
									free(p3);
									printf("成功删除第%d条数据!\n",Delete_Count); 
									Delete_Count++;
									MYFILE[i].nCount--; // 该表的记录数 -1 
								} 
								break;
							} 
							
						case 'c':
							{
								if(p3->arr[flag1].CHAR!=TempDataName[0]){
									if(Delete_Count==1){
											printf("未找到要删除的数据，无法进行删除操作！\n"); 
									}
									return;
								}
								else if(p3->arr[flag1].CHAR==TempDataName[0]){
									p4->next=NULL;
									free(p3);
									printf("成功删除第%d条数据!\n",Delete_Count); 
									Delete_Count++;
									MYFILE[i].nCount--; // 该表的记录数 -1
								} 
								break;
							}
							
						case 's':
							{
								if(strcmp(p3->arr[flag1].STRING,TempDataName)!=0){
									if(Delete_Count==1){
											printf("未找到要删除的数据，无法进行删除操作！\n"); 
									}
									return;
								}
								else if(strcmp(p3->arr[flag1].STRING,TempDataName)==0){
									p4->next=NULL;
									free(p3);
									printf("成功删除第%d条数据!\n",Delete_Count); 
									Delete_Count++;
									MYFILE[i].nCount--; // 该表的记录数 -1
								}
								break;
							}//'s'
				}//switch
				}//else
				break; 
				}
				flag1++;
		    }
		} 
	}
	/*if(Delete_Count==1){
		printf("未找到要删除的数据，无法进行删除操作！\n"); 
	} */
	if(i==NumberOfTable) {
		printf("未找到要删除的表!\n"); 
	} 
}

void UpdateData(char *SQL){//测试用例：UPDATE-MYTABLE-SET-"Name=Jack"-WHERE-"Number=1002"
	//printf("这里是更新函数\n");//把学号为1002的同学的名字全部设置为Jack 
	//利用p1和p2把所有要读取的字符串都读取出来！ 
	char *p1=SQL; 
	for(;*(p1-1)!='-';p1++);//p1指到表名的第一个字母处 
	char *p2=p1;
	for(;*(p2+1)!='-';p2++);//p2指到表名的最后一个字母处 
	char TableName[20];
	strncpy(TableName,p1,p2-p1+1);//从p1开始，把以p1为地址后的p2-p1+1个字母都赋值到TableName中 
	TableName[p2-p1+1]='\0';//在TableName最后加上一个'\0'
	
	for(;*(p1-1)!='"';p1++);
	for(;*(p2+1)!='=';p2++);
	char NameOfField1[20];//用来存储要更改的字段(如:Name) 
	char NameOfField2[20]; //用来存储WHERE限制的字段(如:Number)
	char NameOfData1[20];//用来存储更改的数据(如:Jack) 
	char NameOfData2[20]; //用来存储限制的条件(如:1002) 
	strncpy(NameOfField1,p1,p2-p1+1);//从p1开始，把以p1为地址后的p2-p1+1个字母都赋值到NameOfField1中
	NameOfField1[p2-p1+1]='\0'; 
	
	for(;*(p1-1)!='=';p1++);
	for(;*(p2+1)!='"';p2++); 
	strncpy(NameOfData1,p1,p2-p1+1); 
	NameOfData1[p2-p1+1]='\0';
	
	p2+=4; 
	p1=p2;
	for(;*(p1-1)!='"';p1++);
	for(;*(p2+1)!='=';p2++); 
	strncpy(NameOfField2,p1,p2-p1+1);//从p1开始，把以p1为地址后的p2-p1+1个字母都赋值到NameOfField2中 
	NameOfField2[p2-p1+1]='\0';
	
	//测试用例：UPDATE-MYTABLE-SET-"Name=Jack"-WHERE-"Number=1002"
	for(;*(p1-1)!='=';p1++);
	for(;*(p2+1)!='"';p2++);
	strncpy(NameOfData2,p1,p2-p1+1); 
	NameOfData2[p2-p1+1]='\0';
	//读取完毕，全部保存到了字符串中 
	
	int flag1=0,flag2=0,temp1=0;//flag1用来存储Jack是第几个字段，flag2用来存储1002是第几个字段 
	int i,j,k;
	int TempInt=0,TempInt2=0,Update_Count=1;
	char cType='\0';//Jack的类型是什么？ 
	struct Node *p3,*p4;
	p3=p4=NULL;
	for(i=0;i<NumberOfTable;i++){ // 搜索已创建的所有表
		if(strcmp(MYTABLE[i].NameOfTable,TableName)==0){
			for(j=0;j<MYTABLE[i].nCount;j++){     //用来查找WHERE语句限制的是哪个字段
				if(strcmp(NameOfField1,MYTABLE[i].Head[j].strName)==0){
					cType=MYTABLE[i].Head[j].cType; 
					if(MYTABLE[i].Head[j].key==1){
						printf("主键不能被修改，更新错误！\n");
						return; 
					} 
					break;
				}//if语句的右括号 
				flag1++;
			}//for循环的右括号 
			//测试用例:UPDATE-MYTABLE-SET-"Name=Jason"-WHERE-"Gender=b"
			for(j=0;j<MYTABLE[i].nCount;j++){     //用来查找WHERE语句限制的是哪个字段
				if(strcmp(NameOfField2,MYTABLE[i].Head[j].strName)==0){
					 p4=MYFILE[i].pHead;
					 p3=p4->next;
					 if(p4->next==NULL){
					 	printf("该表中没有任何数据，无法更新\n");
						return;  
					 } 
					 while(p4->next){
					 	switch(MYTABLE[i].Head[j].cType){	//测试用例：UPDATE-MYTABLE-SET-"Name=Jack"-WHERE-"Number=1002"
							case 'i':
								{
									TempInt=0; 
									for(temp1=0;temp1<strlen(NameOfData2);temp1++){
										TempInt = TempInt*10+(NameOfData2[temp1]-'0'); 
									} 				//把NameOfData2转换成int类型并保存在TempInt中。
									if(p3->arr[flag2].INT==TempInt){
										 switch(cType){
										 	case 'i':
											 {
											 	TempInt2=0; 
												for(temp1=0;temp1<strlen(NameOfData1);temp1++){
													TempInt2=TempInt2*10+(NameOfData1[temp1]-'0'); 
												}
												p3->arr[flag1].INT = TempInt2;
												printf("成功修改第%d条数据\n",Update_Count++);
												p4=p4->next;
												p3=p4->next; 
												break;
											 }					
											 case 'c':
											 {
											 	p3->arr[flag1].CHAR=NameOfData1[0]; 
											 	printf("成功修改第%d条数据\n",Update_Count++); 
											 	p4=p4->next;
												p3=p4->next;
												break;
											 } 
											 case 's':
											 {
											 	for(k=0;k<strlen(p3->arr[flag1].STRING);k++){
												 	p3->arr[flag1].STRING[k]='\0'; 
												} 
											 	strcpy(p3->arr[flag1].STRING,NameOfData1); 
											 	printf("成功修改第%d条数据\n",Update_Count++); 
											 	p4=p4->next;
												p3=p4->next;
												break;
											 }  
										 } //循环里的switch的右括号 
										
										
										if(MYTABLE[i].Head[j].key==1){
											return;
										} 
									}//case 'i'里面的if语句 
									else{
										p4=p4->next;
										p3=p4->next;
									} 
									break;
								}//case 'i'
							case 'c'://测试用例：UPDATE-MYTABLE-SET-"Name=Jack"-WHERE-"Gender='b'"
								{
									if(p3->arr[flag2].CHAR ==NameOfData2[0]){
										 switch(cType){	
										 	case 'i':
											 {
											 	TempInt2=0; 
												for(temp1=0;temp1<strlen(NameOfData1);temp1++){
													TempInt2=TempInt2*10+(NameOfData1[temp1]-'0'); 
												}
												p3->arr[flag1].INT = TempInt2;
												printf("成功修改第%d条数据\n",Update_Count++);
												p4=p4->next;
												p3=p4->next; 
												break;
											 }					
											 case 'c':
											 {
											 	p3->arr[flag1].CHAR=NameOfData1[0]; 
											 	printf("成功修改第%d条数据\n",Update_Count++); 
											 	p4=p4->next;
												p3=p4->next;
												break;
											 } 
											 case 's':
											 {
											 	for(k=0;k<strlen(p3->arr[flag1].STRING);k++){
												 	p3->arr[flag1].STRING[k]='\0'; 
												} 
											 	strcpy(p3->arr[flag1].STRING,NameOfData1); 
											 	printf("成功修改第%d条数据\n",Update_Count++); 
											 	p4=p4->next;
												p3=p4->next;
												break;
											 }  
										 } //循环里的switch的右括号 
										
										
										if(MYTABLE[i].Head[j].key==1){
											return;
										} 
									}//case 'c'里面的if语句 
									else{
										p4=p4->next;
										p3=p4->next;
									} 
									break;
								} //case 'c'
							case 's':
								{
									if(strcmp(p3->arr[flag2].STRING, NameOfData2)==0){
										 switch(cType){	
										 	case 'i':
											 {
											 	TempInt2=0; 
												for(temp1=0;temp1<strlen(NameOfData1);temp1++){
													TempInt2=TempInt2*10+(NameOfData1[temp1]-'0'); 
												}
												p3->arr[flag1].INT = TempInt2;
												printf("成功修改第%d条数据\n",Update_Count++);
												p4=p4->next;
												p3=p4->next; 
												break;
											 }					
											 case 'c':
											 {
											 	p3->arr[flag1].CHAR=NameOfData1[0]; 
											 	printf("成功修改第%d条数据\n",Update_Count++); 
											 	p4=p4->next;
												p3=p4->next;
												break;
											 } 
											 case 's':
											 {
											 	for(k=0;k<strlen(p3->arr[flag1].STRING);k++){
												 	p3->arr[flag1].STRING[k]='\0'; 
												} 
											 	strcpy(p3->arr[flag1].STRING,NameOfData1); 
											 	printf("成功修改第%d条数据\n",Update_Count++); 
											 	p4=p4->next;
												p3=p4->next;
												break;
											 }  
										 } //循环里的switch的右括号 
										
										
										if(MYTABLE[i].Head[j].key==1){
											return;
										} 
									}//case 's'里面的if语句
									else {
										p4=p4->next;
										p3=p4->next;
									}
									break;
								} //case 's'
							
						}//switch语句 
					 }//while语句的右括号 
					
					
					
					
				}//这里是if语句的右括号
				flag2++;//flag2用来保存当前是第几个字段 
			}//这里是for循环的右括号 
			break;//永远放在if的右括号前面 
		} //这里是if语句，功能是匹配表头名字 
	}//这里是for循环，功能是搜索创建的所有表 
}

void SelectData(char *SQL){
	char *p=SQL;
	for(;*(p-1)!='-';p++);
	char *p1=p;
	for(;*(p1+1)!='-';p1++);
	char lineMessage[100];
	char tableName[20];
	
	if(*p=='*')  
	{
		p+=7;
		p1=p;
		
		int i;
		if(SQL[strlen(SQL)-1]!='"') // 如果最后一个字符不为 " ,则证明为全表输出 
		{
			for(;*(p1+1)!='\0';p1++);
			strncpy(tableName,p,p1-p+1);
			tableName[p1-p+1]='\0';
			for(i=0;i<NumberOfTable;i++) // 遍历所有表 
			{
				if(strcmp(MYTABLE[i].NameOfTable,tableName)==0) // 找到表了 
				{
					int j;
					for(j=0;j<MYTABLE[i].nCount;j++)   // 输出表头 
					{
							printf("%-20s",MYTABLE[i].Head[j].strName);
					}
					printf("\n");
				
					struct Node *p=MYFILE[i].pHead->next; // p指向第一条记录 
					while(p!=NULL)
					{
						for(j=0;j<MYTABLE[i].nCount;j++) // 输出所有的字段 
						{
							switch(MYTABLE[i].Head[j].cType)
							{
								case 'i':
									{
										printf("%-20d",p->arr[j].INT);
										break;
									}
								case 'c':
									{
										printf("%-20c",p->arr[j].CHAR);
										break;
									}
								case 's':
									{
										printf("%-20s",p->arr[j].STRING);
										break;
									}                     
							}
						}
						p=p->next;
						printf("\n");
					}
					break; 
				}
			}
		}
		
		else if(SQL[strlen(SQL)-1]=='"')
		{
			for(;*(p1+1)!='-';p1++);
			strncpy(tableName,p,p1-p+1);
			tableName[p1-p+1]='\0';

			char message[MAX_STRING_LENGTH]; // 获取 属性名 
			char *p1=&SQL[strlen(SQL)-2]; // 指向 字段信息 的最后一个字符 
			char *p=p1;
			for(;*(p-1)!='=';p--); // 指向字段信息的 第一个字符
			strncpy(message,p,p1-p+1);
			message[p1-p+1]='\0';
			
			char attribution[MAX_NAME_LENTH]; // 获取字段名 
			p1=p-2;
			for(;*(p-1)!='"';p--);
			strncpy(attribution,p,p1-p+1);
			attribution[p1-p+1]='\0';

			for(i=0;i<NumberOfTable;i++) // 遍历所有表 
			{
				if(strcmp(MYTABLE[i].NameOfTable,tableName)==0) // 找到表明相同的表 
				{
					int j; // j 输出完表头后就没有用了 不会再用到,后面可以再次使用 
					for(j=0;j<MYTABLE[i].nCount;j++)   // 输出表头 
					{
							printf("%-20s",MYTABLE[i].Head[j].strName);
					}
					printf("\n");
					
					int k;
					for(k=0;strcmp(MYTABLE[i].Head[k].strName,attribution)!=0;k++); // 找到 相同的字段是在 表头字段数组中是第几号元素 
					char type=MYTABLE[i].Head[k].cType; // 记下这个字段的类型 

					struct Node *p=MYFILE[i].pHead->next; // 遍历所有记录
					switch(type)
					{
						case 'i':
							{
								int num=0,length=strlen(message);		// 将message转换为整型数 
								int l;
								for(l=0;l<length;l++)
								{
									num=num*10+message[l]-'0';
								}
								while(p!=NULL)
								{
									if(p->arr[k].INT==num)
									{
										for(j=0;j<MYTABLE[i].nCount;j++) // 输出所有的字段 
										{
											switch(MYTABLE[i].Head[j].cType)
											{
												case 'i':
													{
														printf("%-20d",p->arr[j].INT);
														break;
													}
												case 'c':
													{
														printf("%-20c",p->arr[j].CHAR);
														break;
													}
												case 's':
													{
														printf("%-20s",p->arr[j].STRING);
														break;
													}                     
											}
										}
										printf("\n");
									}
									p=p->next;
								}
								break;
							}
						case 'c':
							{
								while(p!=NULL)
								{
									if(p->arr[k].CHAR==message[0])
									{
										for(j=0;j<MYTABLE[i].nCount;j++) // 输出所有的字段 
										{
											switch(MYTABLE[i].Head[j].cType)
											{
												case 'i':
													{
														printf("%-20d",p->arr[j].INT);
														break;
													}
												case 'c':
													{
														printf("%-20c",p->arr[j].CHAR);
														break;
													}
												case 's':
													{
														printf("%-20s",p->arr[j].STRING);
														break;
													}                     
											}
										}
										printf("\n");
									}
									p=p->next;
								}
								break;
							}
						case 's':
							{
								while(p!=NULL)
								{
									if(strcmp(p->arr[k].STRING,message)==0)
									{
										for(j=0;j<MYTABLE[i].nCount;j++) // 输出所有的字段 
										{
											switch(MYTABLE[i].Head[j].cType)
											{
												case 'i':
													{
														printf("%-20d",p->arr[j].INT);
														break;
													}
												case 'c':
													{
														printf("%-20c",p->arr[j].CHAR);
														break;
													}
												case 's':
													{
														printf("%-20s",p->arr[j].STRING);
														break;
													}                     
											}
										}
										printf("\n");
									}
									p=p->next;
								}
								break;
							}
					}
					break;
				}
			}
		}

		if(i==NumberOfTable)
		{
			printf("未创建该表,请确认表名!\n");
		}
	}

	else // 按输入的字段要求进行输出显示 
	{
		strncpy(lineMessage,p,p1-p+1); // 把所有要输出的字段 提取出来 
		lineMessage[p1-p+1]='\0';
		
		p1+=7;
		p=p1;
		for(;*(p1+1)!='-';p1++);
		strncpy(tableName,p,p1-p+1);  // 把表名 提取出来 
		tableName[p1-p+1]='\0';
		
		p1+=9;
		p=p1;
		for(;*(p1+1)!='=';p1++);
		char attribution[MAX_NAME_LENTH];
		strncpy(attribution,p,p1-p+1); // 把字段名 提取出来 
		attribution[p1-p+1]='\0';
		
		p1+=2;
		p=p1;
		for(;*(p1+1)!='"';p1++);
		char message[MAX_STRING_LENGTH];
		strncpy(message,p,p1-p+1); // 把 属性名 提取出来, 即详细的字段信息 
		message[p1-p+1]='\0'; 
		
		p=p1=&lineMessage[0];
		char atts[MAX_DATA_LENTH][MAX_NAME_LENTH];
		int attNum=0;
		do
		{
			if(*p1==','||*p1=='\0')
			{
				strncpy(atts[attNum],p,p1-p);  // 将需要比较的字段 存入一个二维数组中 
				atts[attNum][p1-p]='\0';
				p=p1+1;
				attNum++; 
			}
			p1++;
		}while(*(p1-1)!='\0');   // 到这里 SQL 输入 处理已经完成 
		
		int i;
		for(i=0;i<NumberOfTable;i++) // 遍历所有表 
		{
			if(strcmp(MYTABLE[i].NameOfTable,tableName)==0) // 找到表明相同的表 
			{
				int j; // j 输出完表头后就没有用了 不会再用到,后面可以再次使用 
				for(j=0;j<attNum;j++)   // 输出表头 
				{
						printf("%-20s",atts[j]);
				}
				printf("\n");
				
				int k;
				for(k=0;strcmp(MYTABLE[i].Head[k].strName,attribution)!=0;k++); // 找到 需要匹配的字段 是在 表头字段数组中是第几号元素 
				char type=MYTABLE[i].Head[k].cType; // 记下这个字段的类型 

				struct Node *p=MYFILE[i].pHead->next; // 遍历所有记录
				switch(type)
				{
					case 'i':
						{
							int num=0,length=strlen(message);		// 将message转换为整型数 
							int l;
							for(l=0;l<length;l++)
							{
								num=num*10+message[l]-'0';
							}
							while(p!=NULL)
							{
								if(p->arr[k].INT==num)
								{
									int count;
									for(count=0;count<attNum;)
									{
										for(j=0;j<MYTABLE[i].nCount;j++) // 输出条件相同的字段 
										{
											if(strcmp(MYTABLE[i].Head[j].strName,atts[count])==0)
											{
												switch(MYTABLE[i].Head[j].cType)
												{
													case 'i':
														{
															printf("%-20d",p->arr[j].INT);
															break;
														}
													case 'c':
														{
															printf("%-20c",p->arr[j].CHAR);
															break;
														}
													case 's':
														{
															printf("%-20s",p->arr[j].STRING);
															break;
														}                     
												}
												count++;  // 输出成功 ,则 count++ 
												break; 
											}
										}
									}
									printf("\n");
								}
								p=p->next;
							}
							break;
						}
					case 'c':
						{
							while(p!=NULL)
							{
								if(p->arr[k].CHAR==message[0])
								{
									int count;
									for(count=0;count<attNum;)
									{
										for(j=0;j<MYTABLE[i].nCount;j++) // 输出条件相同的字段 
										{
											if(strcmp(MYTABLE[i].Head[j].strName,atts[count])==0)
											{
												switch(MYTABLE[i].Head[j].cType)
												{
													case 'i':
														{
															printf("%-20d",p->arr[j].INT);
															break;
														}
													case 'c':
														{
															printf("%-20c",p->arr[j].CHAR);
															break;
														}
													case 's':
														{
															printf("%-20s",p->arr[j].STRING);
															break;
														}                     
												}
												count++;  // 输出成功 ,则 count++ 
												break; 
											}
										}
									}
									printf("\n");
								}
								p=p->next;
							}
							break;
						}
					case 's':
						{
							while(p!=NULL)
							{
								if(strcmp(p->arr[k].STRING,message)==0)
								{
									int count;
									for(count=0;count<attNum;)
									{
										for(j=0;j<MYTABLE[i].nCount;j++) // 输出条件相同的字段 
										{
											if(strcmp(MYTABLE[i].Head[j].strName,atts[count])==0)
											{
												switch(MYTABLE[i].Head[j].cType)
												{
													case 'i':
														{
															printf("%-20d",p->arr[j].INT);
															break;
														}
													case 'c':
														{
															printf("%-20c",p->arr[j].CHAR);
															break;
														}
													case 's':
														{
															printf("%-20s",p->arr[j].STRING);
															break;
														}                     
												}
												count++;  // 输出成功 ,则 count++ 
												break; 
											}
										}
									}
									printf("\n");
								}
								p=p->next;
							}
							break;
						}
				}
				break;
			}
		}
		if(i==NumberOfTable)
		{
			printf("未创建该表,请确认表名!\n"); 
		} 
	}
} 

void Save(char *SQL){
	char *p=&SQL[0];
	p+=5;
	char *p1=&SQL[strlen(SQL)-1];
	char tableName[20];
	strncpy(tableName,p,p1-p+1);
	tableName[p1-p+1]='\0'; // 获取表名
	
	int i;
	for(i=0;i<NumberOfTable;i++)
	{
		if(strcmp(MYTABLE[i].NameOfTable,tableName)==0)
		{
			char tempPath[100];
			strcpy(tempPath,DataBase_Path);
			strcat(tempPath,"\\");
			strcat(tempPath,tableName); // 拼接文件的路径名 
			strcat(tempPath,"_TableHead.txt");
			FILE *fp=fopen(tempPath,"w+");
			
			int k;
			for(k=0;k<MYTABLE[i].nCount;k++) // 写表头 
			{
				fwrite(&MYTABLE[i].Head[k].strName,sizeof(MYTABLE[i].Head[k].strName),1,fp);
				fwrite("-",sizeof("-"),1,fp);
				fwrite(&MYTABLE[i].Head[k].cType,sizeof(MYTABLE[i].Head[k].cType),1,fp);
				fwrite("-",sizeof("-"),1,fp);
				char type;
				type=(char)(MYTABLE[i].Head[k].key+'0');
				fwrite(&type,sizeof(type),1,fp);
				if(k!=MYTABLE[i].nCount-1)
				fwrite("\n",sizeof("\n"),1,fp);
			}
			fclose(fp);
			
			char tempPath2[100];
			strcpy(tempPath,DataBase_Path);
			strcat(tempPath,"\\");
			strcat(tempPath,tableName); // 拼接文件的路径名 
			strcat(tempPath,"_Records.txt");
			FILE *fp1=fopen(tempPath,"w+");
			
			struct Node *p=MYFILE[i].pHead->next;
			while(p!=NULL)
			{
				int l;
				for(l=0;l<MYTABLE[i].nCount;l++)
				{
					switch(MYTABLE[i].Head[l].cType)
					{
						case 'i':
							{
								int temp=p->arr[l].INT;
								char temp2[10],pos=0;
								char output[10]={0};
								for(;temp!=0;temp/=10,pos++) // 将整型数 转换为字符 
								{
									temp2[pos]=(char)(temp%10+'0');
								}
								int x,t;
								for(t=0,x=pos-1;x>=0;x--,t++)
								{
									output[t]=temp2[x];
								}
								fwrite(&output,sizeof(output),1,fp1);
								break;
							}
						case 'c':
							{
								fwrite(&p->arr[l].CHAR,sizeof(p->arr[l].CHAR),1,fp1);
								break;
							}
						case 's':
							{
								char temp[MAX_STRING_LENGTH]={0};
								strcpy(temp,p->arr[l].STRING);
								fwrite(&temp,sizeof(temp),1,fp1);
								break;
							}
					}
					fwrite("-",sizeof("-"),1,fp1);
				}
				if(p->next!=NULL)
				fwrite("\n",sizeof("\n"),1,fp1);
				p=p->next;
			}
			fclose(fp1);
			break;
		}
	}
	if(i==NumberOfTable)
	{
		printf("未创建该表,请确认表名!\n"); 
	}
} 
void Load(char *SQL){
	char DatabaseAndTablePath[100];   // 用户指定的路径 
	char TableHeadPath[100];    
	char TableName[100]; //  
	printf("请输入数据库及表的路径:\n");
	scanf("%s",DatabaseAndTablePath);
	char *p=&DatabaseAndTablePath[strlen(DatabaseAndTablePath)-1];
	for(;*(p-1)!='\\';p--);
	strcpy(TableName,p); // 获取 表名 
	TableName[strlen(TableName)]='\0'; 
	//printf("获取的表名是:%s,长度为%d\n",TableName,strlen(TableName)); 
	//DataBase_Path[100]={'\0'}; 
	strncpy(DataBase_Path,DatabaseAndTablePath,strlen(DatabaseAndTablePath)-strlen(TableName)-1);
	DataBase_Path[strlen(DataBase_Path)]='\0'; 
	//printf("数据库的路径是:%s,长度是:%d\n",DataBase_Path,strlen(DataBase_Path)); 
	strcpy(TableHeadPath,DatabaseAndTablePath); // 获取路径名
	strcat(TableHeadPath,"_TableHead.txt"); //  形成表头文件路径 
	FILE *fp;
	char TableHead[50];
	char TableHead2[50];
	
	if(fopen(TableHeadPath,"r+")!=NULL)
	{
		printf("打开表成功\n");
		fp=fopen(TableHeadPath,"r+");
		MYTABLE[NumberOfTable].nCount=0;
		strcpy(MYTABLE[NumberOfTable].NameOfTable,TableName); 
		MYFILE[NumberOfTable].pTable=&MYTABLE[NumberOfTable];
		struct Node *aHead=(struct Node*)malloc(sizeof(struct Node));
		aHead->next=NULL;
		MYFILE[NumberOfTable].pHead=aHead; // 头结点 
		struct Node *tail=aHead; // 尾指针 
		MYFILE[NumberOfTable].nCount=0;
		
		char *p1;
		int count=0; // 记录这个表头里的字段数 
		while (fgets(TableHead,35,fp)!=NULL )  // 读取表头 
		{
			p=&TableHead[0];
			for(;*p=='\0';p++);
			p1=p;
			for(;*(p1+1)!='\0';p1++);
			strncpy(MYFILE[NumberOfTable].pTable->Head[count].strName,p,p1-p+1);
			MYFILE[NumberOfTable].pTable->Head[count].strName[p1-p+1]='\0';
			for(;(*p1)!='-';p1++);
			p=p1+2;
			MYFILE[NumberOfTable].pTable->Head[count].cType=*p;
			switch(*p)
			{
				case 'i':
					{
						MYFILE[NumberOfTable].pTable->Head[count].nLen=10;
						break;
					}
				case 'c':
					{
						MYFILE[NumberOfTable].pTable->Head[count].nLen=1;
						break;
					}
				case 's':
					{
						MYFILE[NumberOfTable].pTable->Head[count].nLen=10;
						break;
					}
			}
			p+=3;
			MYFILE[NumberOfTable].pTable->Head[count].key=*p-'0';
			MYFILE[NumberOfTable].pTable->nCount++;
			count++;
    	}
    	
    	char RecordsPath[100];
    	strcpy(RecordsPath,DatabaseAndTablePath);
    	strcat(RecordsPath,"_Records.txt");
    	FILE *fp2=fopen(RecordsPath,"r+");
    	char records[200];
    	while(fgets(records,200,fp2)!=NULL)  // 读取记录 
    	{
    		p=&records[0];
    		struct Node *aHead=(struct Node*)malloc(sizeof(struct Node));
	    	aHead->next=NULL;
    		int i;
    		for(i=0;i<MYTABLE[NumberOfTable].nCount;i++) // 扫描这个表的表头来进行读取 
    		{
	    		for(;*p=='\0'&&*p!='\n';p++);
	    		if(*p=='\n') break;
	    		p1=p;
	    		for(;*(p1+1)!='\0'&&*(p1+1)!='-';p1++);
	    		char temp[20];
	    		strncpy(temp,p,p1-p+1);   // 记录下数据
	    		temp[p1-p+1]='\0';
				switch(MYTABLE[NumberOfTable].Head[i].cType)
				{
					case 'i':
						{
							int num=0;
							int j;
							for(j=0;temp[j]!='\0';j++) // 字符转换成数字 
							{
								num=num*10+temp[j]-'0';
							}
							aHead->arr[i].INT=num;
							break;
						}
					case 'c':
						{
							aHead->arr[i].CHAR=*p1;
							break;
						}
					case 's':
						{
							strcpy(aHead->arr[i].STRING,temp);
							break;
						}
				} 
				for(;*p1!='-';p1++);
				p=p1+1;
	    	}
	    	tail->next=aHead;
			tail=tail->next;
			MYFILE[NumberOfTable].nCount++;
		}
    	NumberOfTable++; 
    	IsCreateDatabase=1;
	}
	else
	{
		printf("该表不存在，请确认表的路径或表名！\n"); 
	}
} 
int  Exit(char *SQL){
	//printf("这里是退出函数\n");
	if(IsExit(SQL)==1){
		return 1;
	} 
	else{
		return 0;
	}  
} 
int IsCreate(char *SQL){		//判断第一个单词是不是Create 
	if(SQL[0]=='C'&&SQL[1]=='R'&&SQL[2]=='E'&&SQL[3]=='A'&&SQL[4]=='T'&&SQL[5]=='E') 
	return 1;
	else
	return 0;
}

int IsDataBase(char *SQL){		//判断是否含有database
	int i; 
	for(i=0;i<strlen(SQL);i++){
		if(SQL[i]=='-'){
			if(SQL[i+1]=='D'&&SQL[i+2]=='A'&&SQL[i+3]=='T'&&SQL[i+4]=='A'//这里可能会有数组越界的问题 ，但我并不打算处理它 
			&&SQL[i+5]=='B'&&SQL[i+6]=='A'&&SQL[i+7]=='S'&&SQL[i+8]=='E')
			{
				return 1;
			}
			} 
	} 
	return 0;
} 

int IsTable(char *SQL){		    //判断是否含有table 
	int i; 
	for(i=0;i<strlen(SQL);i++){
		if(SQL[i]=='-'){
			if(SQL[i+1]=='T'&&SQL[i+2]=='A'&&SQL[i+3]=='B'&&SQL[i+4]=='L'//这里可能会有数组越界的问题 ，但我并不打算处理它 
			&&SQL[i+5]=='E')
			{
				return 1;
			}
			} 
	} 
	return 0;
}

int IsInsert(char *SQL)// 判断是否含有insert ，如果有，返回1
{
		if(SQL[0]=='I'&&SQL[1]=='N'&&SQL[2]=='S') 	
			return 1;
		else
			return 0;
} 
int IsDelete(char *SQL)//判断是否含有delete，如果有，返回1
{
	 if(SQL[0]=='D'&&SQL[1]=='E'&&SQL[2]=='L') 	
			return 1;
		else
			return 0;
} 
int IsUpdate(char *SQL)//判断是否含有update，如果有，返回1
{
	if(SQL[0]=='U'&&SQL[1]=='P'&&SQL[2]=='D') 	
			return 1;
		else
			return 0;
} 
int IsSelect(char *SQL)//判断是否含有select，如果有，返回1 
{
	if(SQL[0]=='S'&&SQL[1]=='E'&&SQL[2]=='L') 	
			return 1;
		else
			return 0;
} 
int IsSave(char *SQL){
	if(SQL[0]=='S'&&SQL[1]=='A'&&SQL[2]=='V') 	
			return 1;
		else
			return 0;
} 
int IsExit(char *SQL){
	if(SQL[0]=='E'&&SQL[1]=='X'&&SQL[2]=='I') 	
			return 1;
		else
			return 0;
} 
int IsLoad(char *SQL){
	if(SQL[0]=='L'&&SQL[1]=='O'&&SQL[2]=='A') 	
		return 1;
	else
		return 0;
} 

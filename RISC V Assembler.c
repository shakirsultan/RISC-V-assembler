
#include <string.h>
#include <stdio.h>
#include<ctype.h>
#include<stdlib.h>


/***
  Name : Shakir Sultan

Submitted as  project for AKEANA training
  ***/
/////////////////////////////////////////////////////////  START  ////////////////////////////////////////////////////////////////////////////////////////

int isOpcode(char *lptr);
int opcode_type;
int size=0;
int flag=0;
int sec_pass=0;
#define MAX_LABEL_LEN 20
#define MAX_SYMBOLS 255
#define MAX_LINE_LENGTH 255

  


/*****************global data ******************/
//Structure for symbol table
typedef struct symboltable{
  int address;                
  char label[MAX_LABEL_LEN+1];   
  struct symboltable *next; //MODIFIED: added a pointer to make a link list
} symboltable;

symboltable *first,*last; //Two pointers to keep track of our symbol table

enum
{
   DONE, OK, EMPTY_LINE//DONE=0, OK=1, EMPTY_LINE=2
};

//Search label Function to traverse through link list of symbol table
int search(char lab[])
{
          int i,address=0;
          flag=0;
          symboltable *p;//pointer to node
          p=first;
          for(i=0;i<size;i++)//traversing through whole link list to search label
          {
                   if(strcmp(p->label,lab)==0) 
                   {
                             address=p->address;//put address of found label in integer
                             flag=1;//Label is found
                             return address;
                   }
                   p=p->next;//move to next node in the link list
                   
          }
          
          if(i==size && sec_pass==1)// in case of 2nd pass of program, doing a label check
                   {
                      fprintf(stderr, "Error code 1:undefined label! Exiting...\n");//print error if not found
                      exit(1);
                   }
          else 
          return 0;
          
}
//function to insert label in the link list of symbol table
 void insert (char label[],int curr_pc)
  {
    search(label); //check if label is already present in the list
    if(flag==1)
      {
        //printf("label already added\n");
        return; //return if label is already inserted
      }
    else
      {
        symboltable *p; //pointer to a node
        p=malloc(sizeof(symboltable));//allocating memory to node
        strcpy(p->label,label); //insert label in the node
        p->address=curr_pc; //insert address along with current label
        p->next=NULL; //point to NULL

        if(size==0)//check if the created node is first node of list
        {
            first=p;//point to the node
            last=p;
          }
          else
          {
            last->next=p;//link the previous node to new created node
            last=p;//point to new node
          }
          size++;//increment the size of symbol table
      }
   return;
  }


int bin_instruction[32]={0.0}; /*a binary array of integers to hold the instruction.*/
symboltable mytable[MAX_SYMBOLS]; /*** the array of symbol table ***/
FILE* infile = NULL;
FILE* outfile = NULL;


char* decToBin(int num, int length) {
 //This function will convert a decimal number to binary decimal immediates
  int i= length-1; //Length of array is always 1 less
  int *binary=(int *)malloc(sizeof(int)*length);//allocating memory to the bianry array
  int temp;
  int rem;
  int neg_flag=0;//negative flag
  int carry = 1;//for two's complement
  int  one[length], two[length]; //arrays for one's and two's complement
  char *char_dec;
  char_dec = (char *) malloc (sizeof(char) * (length+1)); //memory allocation for decimal array
  
  //check if the received number is a negative number
  if(num<0) 
  {
        neg_flag=1;
        num=num*-1;
  }
  //decimal to binary conversion
  for(i;i>=0;i--)
  { 
        rem = num%2;
        num = num/2;
        binary[i]=rem;
       
  }

//if numner is negative we will take 2's complement
if(neg_flag==1)
{
   
    //Taking 1's complement of the binary array
      for(i = 0; i < length; i++){
          if(binary[i] == 0){
            one[i] = 1;           //flip 1 bits to zero
          }
          else if(binary[i] == 1){
            one[i] = 0;           //flip 0 bits to one
          }
      }
     /* printf("Ones's complemented number is ");
      for(i=0;i<length;i++)    printf("%d",  one[i]);
      printf(" \n");*/
    //Taking 2's complement of the binary array
      for(i = length-1; i >= 0; i--)
      {
          if(one[i] == 1 && carry == 1){
            two[i] = 0;
          }
          else if(one[i] == 0 && carry == 1){
            two[i] = 1;
            carry = 0;
          }
          else{
            two[i] = one[i];
          }
      }
     /* printf("Twos's complemented number is ");
          for(i=0;i<length;i++)   
          printf("%d", two[i]);
          printf("\n");*/
      //return the two's complemented binary
      for(i=0;i<length;++i)
      {
             char_dec[i] = two[i] + '0';
      }	
    // adding '\0' at the end of character array
      char_dec[length] = '\0';
    return char_dec;
}
//Return the binary string as it is
for(i=0;i<length;++i)
	{
    	char_dec[i] = binary[i] + '0'; //type casting of binary integer array to binary character array
	}	
// adding '\0' at the end of character array
char_dec[length] = '\0';
return char_dec;
}

char* hexToBin(char *num, int length) {
//This function will convert a Hex number to binary if you are using hex immediates
int i=0,lNeg=0,arr_length=0;
int decimal=0;
int base=1;
num=num+2; //To skip the 0 and x of hex string
// check if hex is negative 
    if( *num == '-' ) 
    {
        lNeg = 1;
        num++;
    }
//find length of array
arr_length = strlen(num);
//conversion of hex to decimal
    for(i = --arr_length; i >= 0; i--)
    {
        if(num[i] >= '0' && num[i] <= '9')
        {
            decimal += (num[i] - 48) * base;
            base *= 16;
        }
        else if(num[i] >= 'A' && num[i] <= 'F')
        {
            decimal += (num[i] - 55) * base;
            base *= 16;
        }
        else if(num[i] >= 'a' && num[i] <= 'f')
        {
            decimal += (num[i] - 87) * base;
            base *= 16;
        }
    }
//if the hex number was negative, make the decimal number negative
 if(lNeg==1)  
    {
        decimal=-decimal;
    }
//Receiving binary string
    char *binary; 
    binary=decToBin(decimal,length); 
    return binary;
}


char* regToBin(char *reg)
{
//This function will convert the register number to a binary string
  char *t_ptr;//Temporary pointer
  t_ptr=reg;
  //check if the received value is a valid register
      if (*(t_ptr+0)!='x'&&*(t_ptr+0)!='\0')
       {
          printf("Error 4: invalid operand......exiting");
          exit(4);
       }
   //check if the received value is a valid entry
       else if(*(t_ptr+0)=='\0')
       {
          printf("Error 4: missing operand......exiting");
          exit(4);
       }


  reg=++reg;//to omit 'x' of the register
  int dec;
  dec=atoi(reg); //convert the character to integer type
  if(dec>31||dec<0)
  {
      fprintf(stderr,"error Code 4: Invalid register......exiting");
      exit(4);
  }
  //convert the register value to binary array
  char *binary=decToBin(dec,5);
  return binary;

}

int readAndParse( FILE * pInfile, char * pLine, char ** pLabel, char
    ** pOpcode, char ** pArg1, char ** pArg2, char ** pArg3, char ** pArg4)
{
      
//this function will read lines from input file one by one . 
  char * lRet, * lPtr;
  int i;
  if( !fgets( pLine, MAX_LINE_LENGTH, pInfile ) )
  return( DONE );
       
           /* convert entire line to lowercase */
       for( i = 0; i < strlen( pLine ); i++ )
        pLine[i] = tolower( pLine[i] );
       
       *pLabel = *pOpcode = *pArg1 = *pArg2 = *pArg3 = *pArg4 = pLine + strlen(pLine);

       /* ignore the comments */
       lPtr = pLine;
     
       while( *lPtr != '#' && *lPtr != '\0' &&*lPtr != '\n' ) //Modified: Added '#' to check for comment
       {
            lPtr++;
        }

       *lPtr = '\0';
       if( !(lPtr = strtok( pLine, "\t\n ,():" ) ) ) //Modified: Added '()' and ':' for tokenization of addresses and labels
            return( EMPTY_LINE );
      
       if( isOpcode( lPtr ) == 0 && lPtr[0] != '.' )      
       {
       
            *pLabel = lPtr;
           
            if( !( lPtr = strtok( NULL, "\t\n ,():" ) ) ) return( OK );
       }
          
          isOpcode(lPtr); //Modified: To change the value of opcode_type flag after label is passed
        
           *pOpcode = lPtr;
         
       if( !( lPtr = strtok( NULL, "\t\n ,():" ) ) ) return( OK );
       
      *pArg1 = lPtr;
       
      if( !( lPtr = strtok( NULL, "\t\n ,():" ) ) ) return( OK );

       *pArg2 = lPtr;
       if( !( lPtr = strtok( NULL, "\t\n ,():" ) ) ) return( OK );

       *pArg3 = lPtr;

       if( !( lPtr = strtok( NULL, "\t\n ,():" ) ) ) return( OK );

       *pArg4 = lPtr;

       return( OK );

    }

int isOpcode(char *lptr)
    {
      //Check opcodes and change the flag opcode_type
    
        if (strcmp(lptr,"add")==0||strcmp(lptr,"and")==0||strcmp(lptr,"sub")==0||strcmp(lptr,"xor")==0||strcmp(lptr,"or")==0||
            strcmp(lptr,"sll")==0||strcmp(lptr,"srl")==0||strcmp(lptr,"sra")==0||strcmp(lptr,"slt")==0||strcmp(lptr,"sltu")==0) 
        {
          opcode_type=1;
          return (1);                       
          
        } 
        else if (strcmp(lptr,"lui")==0||strcmp(lptr,"auipc")==0)
        {
          opcode_type=2;
          return (2);
        }         
        else if (strcmp(lptr,"addi")==0||strcmp(lptr,"andi")==0||strcmp(lptr,"xori")==0||strcmp(lptr,"ori")==0||
            strcmp(lptr,"slli")==0||strcmp(lptr,"srli")==0||strcmp(lptr,"srai")==0||strcmp(lptr,"slti")==0||strcmp(lptr,"sltiu")==0) 
        {
          opcode_type=3;
          return (3);                       
          
        } 
        else if (strcmp(lptr,"lb")==0||strcmp(lptr,"lh")==0||strcmp(lptr,"lw")==0||strcmp(lptr,"lbu")==0||strcmp(lptr,"lhu")==0) 
        {
          opcode_type=4;
          return (4);                       
          
        }
        else if (strcmp(lptr,"ecall")==0||strcmp(lptr,"ebreak")==0) 
        {
          opcode_type=5;
          return (5);                       
          
        }
        else if (strcmp(lptr,"jalr")==0) 
        {
          opcode_type=6;
          return (6);                       
        }
        else if (strcmp(lptr,"sb")==0||strcmp(lptr,"sh")==0||strcmp(lptr,"sw")==0) 
        {
          opcode_type=7;
          return (7);   
        }
        
        else if (strcmp(lptr,"jal")==0) 
        {
          opcode_type=8;
          return (8);  
        } 
        else if (strcmp(lptr,"beq")==0||strcmp(lptr,"bne")==0||strcmp(lptr,"blt")==0||strcmp(lptr,"bge")==0||strcmp(lptr,"bltu")==0||strcmp(lptr,"bgeu")==0) 
        {
          opcode_type=9;
          return (9);   
        }
        else 
        {   opcode_type=10;        
            return 0;
        }
}


int toNum( char * pStr )
{
   char lBuf[10];
   char * t_ptr;
   char * orig_pStr;
   int t_length,k;
   int lNum, lNeg = 0;
   orig_pStr = pStr;
  
   //Check if the value is not missing
   if(*(pStr+0)=='\0')
       {
         printf("to num\n");
          printf("Error 4: missing operand......exiting");
          exit(4);
       }
    //if the argument passed is a valid value
   else if( *pStr=='0' && *(pStr+1) == 'x' )/*check if the number is a Hex */
   { 
        pStr=pStr+2; //To skip the 0 and x of hex string
        if( *pStr == '-' ) /*if hex is negative */
        {
            lNeg = 1; //Flag on
            pStr++; // skip the minus(-)
        }
        t_ptr = pStr; //temporary pointer
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isxdigit(*t_ptr))//To check if the value is a valid hex digit
            {
                printf("Error 4: invalid hex operand, %s\n",orig_pStr);
                exit(4);
            }
            
            t_ptr++;
        }
        sprintf( lBuf, "0x%s", pStr );     /* convert hex string into integer */
        sscanf( lBuf, "%i", &lNum );
        
        //if the value was negative, make the resutl negative
        if( lNeg )
            lNum = -lNum;
        return lNum;
       }
  //if the value was not hex check for decimal value
   else   
   {
        
        if( *pStr == '-' ) /* dec is negative */
        {
            lNeg = 1;
            pStr++;
        }
        t_ptr = pStr;
        t_length = strlen(t_ptr);
        for(k=0;k < t_length;k++)
        {
            if (!isdigit(*t_ptr)) //if the value in the array is a valid deciaml digit
            {
                printf("Error 4: invalid operand, %s\n",orig_pStr);
                exit(4);
            }
            t_ptr++;
        }
        lNum = atoi(pStr);  //convert character to integer
        //if the value was a negative number make the result negative
        if (lNeg)
            lNum = -lNum;
 
        return lNum;
   
    }
    
}


char bin2hex(int highbit){
  char  retval;
  int i;  
  unsigned char dec; /* declared char to be compatible with retval*/
  dec=0;

  /* this is an iterative algo working from msb to lsb */
  for( i=0; i < 4; i++){
    dec = 2*dec; /* add to the power of 2 */
    dec = dec+ bin_instruction[highbit+i];
  }

  if( dec > 9){ /* the hex value must be an alphabet */
    dec = dec - 10;
    retval = dec + 'A';
  } else{ /* the hex value is a number 0--9 */
    retval = dec + '0';
  }

  return retval;
}


/********************write_instruction**********************/
/* writes the instruction into the output file as a hex val*/
void write_instruction(){
  char a;
  /* first put the '0x' in the beginning of the line*/
  fputc('0', outfile);
  fputc('x', outfile);
for(int i=0;i<=31;i=i+4) //Add 4 in "i" to do for all nibbles
{
      a = bin2hex(i); //binary nibble is being converted to hex number
      fputc(a, outfile);  //Putting each hex in output file
  }
 /** now put a newline at the end*/
  fputc('\n',outfile);
  return;
}



//This function will convert all S type instruction into binary string
void sType(char *opcode, char *arg1, char *arg2,char *arg3) {

//check if an arguments are complete
if(arg1[0]=='\0'||arg2[0]=='\0'||arg3[0]=='\0')
{
  fprintf(stderr,"error code 4: Missing operand......exiting\n");//print error if an argument is missing
  exit(4);
}

  char opcode1[]="0100011"; //opcode for s Type
  char func3[]="000"; //default case for store byte instruction
  char *bin_arg1=regToBin(arg1);  //convert registers to  binary strings
  char *bin_arg3=regToBin(arg3);
  int binary_array[12]={0.0}; //Binary array of 12 bits
  char arg2_p1[]="0000000"; //To make two parts of the immediate value
  char arg2_p2[]="00000"; //2nd part of the immediate value
  //Flush the binary instruction array of 32 bits
  for(int i=0;i<32;i++)
  bin_instruction[i]=0;

  int i;
  int dec_num;  //to save decimal number of integer type
  //Change function 3 according to opcodes
  if((strcmp(opcode,"sh")==0))
    {
      strcpy(func3,"001");
    }
  else if((strcmp(opcode,"sw")==0))
    {
      strcpy(func3,"010");
    }
//check if the argument is a hex value
  if(arg2[0]=='0' && arg2[1]=='x')//for hex number
    {
        //check range of the input number for immediate
        int hex_max=toNum(arg2);
        if(hex_max>2047|| hex_max<-2048)
        {
          fprintf(stderr,"error 3: Invalid constat...exiting");
          exit(3);    
        } 
          arg2=hexToBin(arg2,12); //convert into 12 bits binary of immediate value
    }
    else if(arg2[0]=='0' && arg2[1]=='b')//check for binary number
    {
          arg2=arg2+2; //to omit 0 and b from the binary number
          int len=strlen(arg2)-1; //minus 1 to not count NULL at end of array in size calculation
         //Casting from character to integer array type
          for(i=11;len>=0;--i)
          {
            binary_array[i]=*(arg2+len)-'0'; 
            len=--len;
          }
        
        //Putting binary value of 7 bits in binary instuction array(25-31)
          for(i=0;i<=6;++i) 
          {
            bin_instruction[i] = binary_array[i];
          }
              
          for(i=11;i>=7;--i) //Putting destination register in binary instruction(20-24)
          {
            bin_instruction[i]=*(bin_arg1+i-7)-'0';
          }
          for(i=16;i>=12;--i) //Putting source register in binary instruction(15-19)
          {
            bin_instruction[i]=*(bin_arg3+i-12)-'0';
          }
        
          for(i=19;i>=17;--i) //Putting function 3 register into binary insturction array(12-14)
          {
            bin_instruction[i]=*(func3+i-17)-'0'; 
          }
          for(i=24;i>=20;--i) //Putting destination register into binary insturction array(7-11)
          {
            bin_instruction[i] = binary_array[i-13]; 
          }
          for(i=31;i>=25;--i) //Putting opcode into binary insturction array(0-6)
          {
            bin_instruction[i]=*(opcode1+i-25)-'0'; 
          }
      //Printing the 32 bit binary instruction array for s Type
      for(i=0;i<=31;++i)
      printf("%d",bin_instruction[i]);
      printf("    S Type\n");
      return;    
    }
  //for decimal number
   else  
    {
      dec_num=toNum(arg2);
      //Check the immediate is in range
      if(dec_num>2047 ||dec_num<-2048)
      {
          //printf("The immediate value in deciaml is  %d\n",dec_num); 
          fprintf(stderr,"error 3: Invalid constat...exiting");
          exit(3);  
      }
      arg2=decToBin(dec_num,12);//12 bits of immediate value
      
    } 
  //making two parts of the immediate value
    for(i=0;i<=6;++i)
      arg2_p1[i]=*(arg2+i);
    for(i=7;i<=11;i++)
      arg2_p2[i-7]=*(arg2+i);
  //putting all character array into a single character array of 32 bits
    char arr[32];
    strcpy(arr,arg2_p1);
    strcat(arr,bin_arg1);
    strcat(arr,bin_arg3);
    strcat(arr,func3);
    strcat(arr,arg2_p2);
    strcat(arr,opcode1);

  //Type casting of character array into integer array of 32 bit along with printing for s Type
    for(i=0;i<32;i++)
    {
        bin_instruction[i]=*(arr+i)-'0';
        printf("%d",bin_instruction[i]);
   }
  printf("    S Type\n");  
  return;
}

void bType(char *opcode, char *arg1, char *arg2,char *label,int curr_pc) {
//This function will convert all B type instruction into binary string

  //check if an arguments are complete
    if(label[0]=='\0'||arg1[0]=='\0'||arg2[0]=='\0')
    {
      fprintf(stderr,"error code 4: Missing operand......exiting\n");
      exit(4);
    }
    char opcode1[]="1100011"; //Opcode for b Type
    char func3[]="000";   //Default opcode for load byte
    char *bin_arg1=regToBin(arg1); //conversion of register values to binary arrays
    char *bin_arg2=regToBin(arg2);
    int offset; //To calculate offsets of addresses
    int i;
    //Check if there is a label but an integer
    if(isalpha(label[0])==0) //executes in case there is an integer instead of label
    {
    // printf("The value in label is %d\n",isalpha(label[0]));
      int num=toNum(label);
      offset=num;  //the offset will be the integer in this case
    }
    else //in case of labels
    {
      int label_address=0;
      label_address=search(label);//search the label in the link list and return the pc-address of that label
      offset=label_address-curr_pc;//calculate offset 
    }
    //Check if the offset can be accessed with 12 bit immediate
    if(offset>2047||offset<-2048)
    {
      fprintf(stderr,"The offset is out of range of b Type instructions");//BONUS JOB: Exits if the range is out of 12 bits
      exit(5);
    }

    char *bin_offset;         //Character array to save binary of offset
    bin_offset=decToBin(offset,13); //Offset converted to a binary number and received in array(13 bit offset is created, the 13th bit is discarded)
    //Flush binary instruction array of 32 bits  
    for(int i=0;i<32;i++)
        bin_instruction[i]=0;
    //Change function 3 according to opcodes
    if((strcmp(opcode,"bne")==0))
      {
        strcpy(func3,"001");
      }
    else if((strcmp(opcode,"blt")==0))
      {
        strcpy(func3,"100");
      }
    else if((strcmp(opcode,"bge")==0))
      {
        strcpy(func3,"101");
      }
    else if((strcmp(opcode,"bltu")==0))
      {
        strcpy(func3,"110");
      }
    else if((strcmp(opcode,"bgeu")==0))
      {
        strcpy(func3,"111");
      }
  //Putting binary value of 7 bits in binary instuction array(25-31)
    for(i=31;i>=25;i--)
        bin_instruction[i]=*(opcode1+i-25)-'0';
  //Putting binary value of 3 bits in binary instuction array(17-19)
    for(i=19;i>=17;i--)
        bin_instruction[i]=*(func3+i-17)-'0';

    //Putting source register 1 in 5 bits in binary instuction array    
    for(i=16;i>=12;i--)
        bin_instruction[i]=*(bin_arg1+i-12)-'0';
    //Putting source register 2 in 5 bits in binary instuction array  
    for(i=11;i>=7;i--)
        bin_instruction[i]=*(bin_arg2+i-7)-'0';
    //Putting 12 bit immediate value according to the format of b type instruction   
      bin_instruction[24]=*(bin_offset+1)-'0';  
    for(i=23;i>=20;i--)
        bin_instruction[i]=*(bin_offset+i-12)-'0';
    for(i=6;i>=1;i--)
        bin_instruction[i]=*(bin_offset+i+1)-'0';  
    bin_instruction[0]=*(bin_offset+0)-'0'; 
    //Printing binary array of 32 bits instruction of b Type
    for(i=0;i<32;i++)
    {
      printf("%d",bin_instruction[i]);
    }
  printf("    b Type\n");
  return;
}

void iType(char *opcode, char *arg1, char *arg2,char *arg3,int curr_pc) 
{
  //This function will convert all I type instruction into binary string
//Check if the opcode has ecall and ebreak
if(strcmp(opcode,"ecall")!=0 && strcmp(opcode,"ebreak")!=0)
{
      //Check if there are arguments missing for other opcode types
        if(arg1[0]=='\0'||arg2[0]=='\0'||arg3[0]=='\0')
        {
          fprintf(stderr,"error code 4: Missing operand......exiting\n");
          exit(4);
        }
    
    int dec_num;
    int binary_array[12]={0.0}; //To hold binary array if any
    char opcode1[]="0010011"; //Opcode of i Type
    char *bin_arg1=regToBin(arg1);  //convert value of destination register to binary bits
    char func7[]="0000000"; //default function 7 for slli instruction
    char func3[]="101";//default function 3 for srai and srli instructinos
    int i;

    //Change function 3 according to instruction
    if((strcmp(opcode,"srai")==0))
    {
      func7[1]='1';
    }
    if((strcmp(opcode,"addi")==0))
    {
      strcpy(func3,"000");
    }
    else if((strcmp(opcode,"xori")==0))
    {
      strcpy(func3,"100");
    }
    else if((strcmp(opcode,"ori")==0))
    {
      strcpy(func3,"110");
    }
    else if((strcmp(opcode,"andi")==0))
    {
      strcpy(func3,"111");
    }
    else if((strcmp(opcode,"slli")==0))
    {
      strcpy(func3,"001");
    }
    else if((strcmp(opcode,"slti")==0))
    {
      strcpy(func3,"010");
    }
    else if((strcmp(opcode,"sltiu")==0))
    {
      strcpy(func3,"011");
    }

    //for addi and other I type instructions for which func3 is manipulated above
    if (strcmp(opcode,"addi")==0||strcmp(opcode,"andi")==0||strcmp(opcode,"xori")==0||strcmp(opcode,"ori")==0||
      strcmp(opcode,"slti")==0||strcmp(opcode,"sltiu")==0||strcmp(opcode,"jalr")==0)
      
      {
        //for jalr instructino the oppcode and func3 changes but all other arguments remains the same
        if(strcmp(opcode,"jalr")==0)
        {
          strcpy(opcode1,"1100111");
          strcpy(func3,"000");
        }
        //Convert 2nd argument(register) to binary
        char *bin_arg2=regToBin(arg2);

      //Check if the immediate is a hex number
      if(arg3[0]=='0' && arg3[1]=='x')//for hex number
        {
      //check range of the input number for immediate
            int hex_max=toNum(arg3);
            if(hex_max>2047|| hex_max<-2048)
            {
              fprintf(stderr,"error 3: Invalid constat...exiting");
              exit(3);    
            } 
        arg3=hexToBin(arg3,12); //12 bits of immediate value

        }
      //if the binary number is  given as immediate
      else if(arg3[0]=='0' && arg3[1]=='b')//for binary number
        {
            arg3=arg3+2; //to omit 0 and b from the binary number
            int len=strlen(arg3)-1;//minus 1 to not count NULL at end of array in size calculation
            //Casting from character to integer array type
            for(i=11;len>=0;--i)
            {
                binary_array[i]=*(arg3+len)-'0'; 
                len=--len;
            }
            //Putting values in 32 bit binary instruction array
            for(i=0;i<=11;++i) 
            {
                bin_instruction[i] = binary_array[i];//putting binary value of 12 bits in binary instuction array(20-31)
            }       
            for(i=16;i>=12;--i)
            {
              bin_instruction[i]=*(bin_arg2+i-12)-'0';//Putting source register in binary instruction(15-19)
            }
          
            for(i=19;i>=17;--i){
              
              bin_instruction[i]=*(func3+i-17)-'0'; //Putting function 3 register into binary insturction array(12-14)
            }
            for(i=24;i>=20;--i){
              
              bin_instruction[i]=*(bin_arg1+i-20)-'0'; //Putting destination register into binary insturction array(7-11)
            }
            for(i=31;i>=25;--i){
              
              bin_instruction[i]=*(opcode1+i-25)-'0'; //Putting opcode into binary insturction array(0-6)
            }
            //Print the whole binary instruction of 32 bits
            for(i=0;i<=31;++i)
                printf("%d",bin_instruction[i]);
            printf("    I Type\n");

            return;
        }
      else  //for decimal number
        {
            //check if the arg3 is a label
              if(isalpha(arg3[0])!=0)
                { 
                  //check if the received argument is a label
                  int address=search(arg3);
                  if(address>2047 ||address<-2048) //no offset to be calculated due to indirect call
                    {
                        fprintf(stderr,"error 3: Invalid constat...exiting\n");
                        exit(3);  
                    }
                  dec_num=address; //put value in dec_num for further process
                }
              //if not label, arg3 is a decimal number
              else 
                { 
                  dec_num=toNum(arg3);//convert the operand to a number from character array

                //BONUS TASK:Check the immediate is in range of 12 bits
                  if(dec_num>2047 ||dec_num<-2048)
                  { 
                      fprintf(stderr,"error 3: Invalid constat...exiting");
                      exit(3);  
                  }
                }
          arg3=decToBin(dec_num,12);//chage decimal value in 12 bits of immediate value
          
        }
        //putting all character array into a single character array of 32 bits   
        char arr[32];
        strcpy(arr,arg3);
        strcat(arr,bin_arg2);
        strcat(arr,func3);
        strcat(arr,bin_arg1);
        strcat(arr,opcode1);
        //Type casting of character array into integer array of 32 bit along with printing for i Type
        for(i=0;i<32;i++)
        {
            bin_instruction[i]=*(arr+i)-'0';//subtract ascii of zero to get binary values
            printf("%d",bin_instruction[i]);
      }
      printf("    I Type\n");  
      return; 
      }

      else if(strcmp(opcode,"srli")==0 || strcmp(opcode,"srai")==0 || strcmp(opcode,"slli")==0)//for srli,srai,slli
      {
      
        char *bin_arg2=regToBin(arg2); //convert value of register to binary array

        if(arg3[0]=='0' && arg3[1]=='x')//for hex number
        {
            //check range of the input number for immediate
            int hex_max_s= toNum(arg3);
            if(hex_max_s>31 || hex_max_s<0)
            {

                  fprintf(stderr,"error 3: Invalid constat...exiting");
                  exit(3);    
            }
            //convert valid hex immediate value to binary array
              arg3=hexToBin(arg3,5);//5 bits of immediate value
        }
        else if(arg3[0]=='0' && arg3[1]=='b')//for binary number
        {
              
          for(i=6;i>=0;i--)  //putting func7 of 7 bits in binary instuction array(25-31)
            {
              binary_array[i]=*(func7+i)-'0';
            }

            arg3=arg3+2; //to omit 0 and b from the binary number
            int len=strlen(arg3)-1;

            for(i=7;len>=0;--i)  
            {
              binary_array[i]=*(arg3+len)-'0';//putting immediate of 5 bits in binary instuction array(25-31)
              len=--len;
            }
                
            for(i=16;i>=12;--i)
            {
              bin_instruction[i]=*(bin_arg2+i-12)-'0';//Putting source register in binary instruction(15-19)
            }
          
            for(i=19;i>=17;--i)
            {
              bin_instruction[i]=*(func3+i-17)-'0'; //Putting function 3 register into binary insturction array(12-14)
            }
            for(i=24;i>=20;--i)
            {
              
              bin_instruction[i]=*(bin_arg1+i-20)-'0'; //Putting destination register into binary insturction array(7-11)
            }
            for(i=31;i>=25;--i)
            {
              bin_instruction[i]=*(opcode1+i-25)-'0'; //Putting opcode into binary insturction array(0-6)
            }
            //printing the whole 32 bit instruction array
            for(i=0;i<=31;++i)
              printf("%d",bin_instruction[i]);
            printf("    I Type\n");
          return;
        }
        
        else  //for decimal number
        {
            dec_num=toNum(arg3); //Conversion of decimal number array to integer 
            
            //Check the immediate is in range
            if(dec_num>31 ||dec_num<-0)
            {
                fprintf(stderr,"error 3: Invalid constat...exiting");
                exit(3);  
            }

            arg3=decToBin(dec_num,5); //convert the decimal number to 5 bits of binary
          
        }    
        //putting all character array into a single character array of 32 bits   
        char arr[32];
        strcpy(arr,func7);
        strcat(arr,arg3);
        strcat(arr,bin_arg2);
        strcat(arr,func3);
        strcat(arr,bin_arg1);
        strcat(arr,opcode1);
        //Type casting of character array into integer array of 32 bit along with printing for i Type
        for(i=0;i<32;i++)
        {
              bin_instruction[i]=*(arr+i)-'0';//subtract ascii of zero to get binary values
              printf("%d",bin_instruction[i]);
        }
          printf("    I Type\n");
      return;
      }
      else if(strcmp(opcode,"lb")==0 || strcmp(opcode,"lh")==0 || strcmp(opcode,"lw")==0|| strcmp(opcode,"lbu")==0 || strcmp(opcode,"lhu")==0)
      {
      //changing opcode for load i Type instructions
        strcpy(opcode1,"0000011");
      
        char *bin_arg3=regToBin(arg3); //convert register number to binary

      //changing function 3 according to oppcodes
        if((strcmp(opcode,"lb")==0))
        {
          strcpy(func3,"000");
        }
        else if((strcmp(opcode,"lh")==0))
        {
          strcpy(func3,"001");
        }
        else if((strcmp(opcode,"lw")==0))
        {
          strcpy(func3,"010");
        }
        else if((strcmp(opcode,"lbu")==0))
        {
          strcpy(func3,"100");
        }
        else if((strcmp(opcode,"lhu")==0))
        {
          strcpy(func3,"101");
        }
      

        //checking if the operand is a hex number
        if(arg2[0]=='0' && arg2[1]=='x')
        {
            //check range of the input number for immediate
            int hex_max_l=toNum(arg2);
            if(hex_max_l>2047|| hex_max_l<-2048)
            {
                  fprintf(stderr,"error 3: Invalid constat...exiting");
                  exit(3);    
            } 
              arg2=hexToBin(arg2,12); //convert hex to 12 bits of immediate value
        }
        else if(arg2[0]=='0' && arg2[1]=='b')//if not hex check if operand is a binary number
        {
          arg2=arg2+2; //to omit 0 and b from the binary number
          
          int len=strlen(arg2)-1;//minus 1 to not count NULL at end of array in size calculation
          //Casting from character to integer array type
          for(i=11;len>=0;--i)
          {
            binary_array[i]=*(arg2+len)-'0';
            len=--len;
          }
      
        for(i=0;i<=11;++i) 
          {
            bin_instruction[i] = binary_array[i];//putting 12 bit immediate binary value in binary instuction array(20-31)
          }
          
          for(i=16;i>=12;--i)
          {
            bin_instruction[i]=*(bin_arg3+i-12)-'0';//Putting source register in binary instruction(15-19)
          }
          for(i=19;i>=17;--i)
          {  
            bin_instruction[i]=*(func3+i-17)-'0'; //Putting function 3 register into binary insturction array(12-14)
          }
          for(i=24;i>=20;--i)
          {
            bin_instruction[i]=*(bin_arg1+i-20)-'0'; //Putting destination register into binary insturction array(7-11)
          }
          for(i=31;i>=25;--i)
          {
            bin_instruction[i]=*(opcode1+i-25)-'0'; //Putting opcode into binary insturction array(0-6)
          }
          //Printing the whole 32 bit binary instruction of i Type
          for(i=0;i<=31;++i)
              printf("%d",bin_instruction[i]);
          printf("    I Type\n");
        return;
        }

        else  //for decimal number
        {
          dec_num=toNum(arg2);//convert decimal number array to decimal integer
          //Check the immediate is in range
          if(dec_num>2047 ||dec_num<-2048)
          {
              fprintf(stderr,"error 3: Invalid constat...exiting");
              exit(3);  
          }
          arg2=decToBin(dec_num,12);//convert decimal integer to 12 bits of immediate value
          } 
        //putting all character array into a single character array of 32 bits        
        char arr[32];
        strcpy(arr,arg2);
        strcat(arr,bin_arg3);
        strcat(arr,func3);
        strcat(arr,bin_arg1);
        strcat(arr,opcode1);
        //Type casting of character array into integer array of 32 bit along with printing for i Type
        for(i=0;i<32;i++)
        {
            bin_instruction[i]=*(arr+i)-'0';//subtract ascii of zero to get binary values
            printf("%d",bin_instruction[i]);
        }
        printf("    I Type\n");  
        return;
      }
}
else if(strcmp(opcode,"ecall")==0 || strcmp(opcode,"ebreak")==0)
{
  char opcode1[]="1110011";//Define and initialize opcode for ebreak and ecall
  int i;
  //Flush binary instruction array of 32 bits 
  for(i=0;i<32;i++)
  bin_instruction[i]=0;
  //change in function 7 for ebreak
  if(strcmp(opcode,"ebreak")==0)
    {
       bin_instruction[11]=1;
    }  
  for(i=31;i>=25;i--)
  {
  bin_instruction[i]=*(opcode1+i-25)-'0';//putting opcode in 32 binary instruction array
  }
  //Print the whole 32 bit binary array for i type
  for(i=0;i<32;i++)
      printf("%d",bin_instruction[i]);
  printf("    I Type\n"); 
  return;
  }
}

void RType(char *opcode, char *arg1, char *arg2,char *arg3) 
{
      //check if an arguments are not missing
        if(arg1[0]=='\0'||arg2[0]=='\0'||arg3[0]=='\0')
      {
        fprintf(stderr,"error code 4: Missing operand......exiting\n");
        exit(4);
      }
      char *opcode1="0110011";//Opcode for R type instructions
      char *bin_arg1=regToBin(arg1);//convert register number to binary
      char *bin_arg2=regToBin(arg2);
      char *bin_arg3=regToBin(arg3);
      char func7[]="0000000";//default function 7 other than subtract and sra instructions
      char func3[]="000";//default function 3 for add and subtract
      int i;
      //change in function 7 for subtract and sra instruction
      if((strcmp(opcode,"sub")==0||strcmp(opcode,"sra")==0))
      {
        func7[1]='1';
      }
      //change in function 3 according to oppcodes
      if(((strcmp(opcode,"srl")==0)||(strcmp(opcode,"sra")==0)))
      {
        strcpy(func3,"101");
      }
      else if((strcmp(opcode,"xor")==0))
      {
        strcpy(func3,"100");
      }
      else if((strcmp(opcode,"or")==0))
      {
        strcpy(func3,"110");
      }
      else if((strcmp(opcode,"and")==0))
      {
        strcpy(func3,"111");
      }
      else if((strcmp(opcode,"sll")==0))
      {
        strcpy(func3,"001");
      }
      else if((strcmp(opcode,"slt")==0))
      {
        strcpy(func3,"010");
      }
      else if((strcmp(opcode,"sltu")==0))
      {
        strcpy(func3,"011");
      }
     
      //putting all character array into a single character array of 32 bits  
      char arr[32];
      strcpy(arr,func7);
      strcat(arr,bin_arg3);
      strcat(arr,bin_arg2);
      strcat(arr,func3);
      strcat(arr,bin_arg1);
      strcat(arr,opcode1);
      //conversion of character array to integer array and printing
      for(i=0;i<32;i++){
          bin_instruction[i]=*(arr+i)-'0';//subtract ascii of zero to get binary values
          printf("%d",bin_instruction[i]);
      }
      printf("    Rtype\n");
    return;

}

void uType(char* opcode,char *arg1,char *arg2) {
//This function will convert all U type instruction into binary string
//check if an arguments are complete
   if(arg1[0]=='\0'||arg2[0]=='\0')
      {
        fprintf(stderr,"error code 4: Missing operand......exiting\n");
        exit(4);
      }  
  char opcode1[8];//Array to hold opcode bits
  int dec_num;
  int binary_array[20]={0.0};// Array to hold 20 bits binary immediates
  int i;
  char *bin_arg1=regToBin(arg1);//convert register number to binary string
//Change opcode value according to instruction
  if(strcmp(opcode,"lui")==0)
  {
    strcpy(opcode1,"0110111");
  }
  else if(strcmp(opcode,"auipc")==0)
  {
    strcpy(opcode1,"0010111");
  }
 //Check if the immediate value is given in hex
  if(arg2[0]=='0' && arg2[1]=='x')
    {
      //check range of the input number for immediate
      int hex_max=toNum(arg2);
      if(hex_max>1048575 || hex_max<0)
      {
          fprintf(stderr,"error 3: Invalid constat...exiting");
          exit(3);    
      } 
      arg2=hexToBin(arg2,20);
    }
  else if(arg2[0]=='0' && arg2[1]=='b')//for binary number
   {
      arg2=arg2+2; //to omit 0 and b from the binary number
      int len=strlen(arg2)-1;//minus 1 to not count NULL at end of array in size calculation
    
      for(i=19;len>=0;--i)
      {
        binary_array[i]=*(arg2+len)-'0';//putting immediate value into binary array of 20 bits
        len=--len;
      }
      //Putting values in binary array of 32 bits 
      for(i=0;i<=19;++i) 
      {
        bin_instruction[i] = binary_array[i];//putting binary value of 20 bits in binary instuction array(12-31)
      } 
      for(i=24;i>=20;--i)
      {
        bin_instruction[i]=*(bin_arg1+i-20)-'0';//Putting destination register in binary instruction(7-11)
      }
      for(i=31;i>=25;--i)
      {        
        bin_instruction[i]=*(opcode1+i-25)-'0'; //Putting opcode into binary insturction array(0-6)
      }
      //Printing whole 32 bit instruction array
      for(i=0;i<=31;++i)
        printf("%d",bin_instruction[i]);
      printf("    U Type\n");
      return;
    }

  else  //for decimal number
  {
      dec_num=toNum(arg2);//Convert decimal array to decimal integer
      //Check the immediate is in range
        if(dec_num>1048575 ||dec_num<0)
        {
            fprintf(stderr,"error 3: Invalid constat...exiting");
            exit(3);  
        }
      arg2=decToBin(dec_num,20);//Convert decimal integer in 20 bits binary immediate value    
  }
  //putting all character array into a single character array of 32 bits  
  char arr[32];
  strcpy(arr,arg2);
  strcat(arr,bin_arg1);
  strcat(arr,opcode1);
  //Type casting of character array into integer array of 32 bit along with printing for i Type
  for(i=0;i<32;i++){
      bin_instruction[i]=*(arr+i)-'0';//subtract ascii of zero to get binary values
      printf("%d",bin_instruction[i]);
  }
  printf("    U Type\n");
  return;
}

void jType(char* opcode,char *arg1,char *label,int curr_pc) 
{
//This function will convert all J type instruction into binary string
//check if an arguments are complete
   if(arg1[0]=='\0'||label[0]=='\0')
    {
        fprintf(stderr,"error code 4: Missing operand......exiting\n");
        exit(4);
    }
    int i,offset=0;
    int label_address=0;
    char opcode1[]="1101111";//Default opcode for j Type
     //Flush the binary instruction array of 32 bits
    for(int i=0;i<32;i++)
        bin_instruction[i]=0;
    char *bin_arg1=regToBin(arg1);//Convert register value into binary array

    //check if the operand is an immediate value instead of a proper label
    if(isalpha(label[0])==0) 
      {
        int num=toNum(label);
        offset=num;

      }
    else  //in case of proper label
    {
      label_address=search(label);//search label in link list
      offset=label_address-curr_pc;//calculate offset of addresses
    }
    //Check if the offset can be accessed with 12 bit immediate
    if(offset>52427||offset<-52428)
    {
        fprintf(stderr,"The offset is out of range of j Type instructions");//BONUS TASK:Checking if a jump is possible in 20 bits
        exit(5);
    }
      char *bin_offset;         //Character array to save binary of offset
      bin_offset=decToBin(offset,21); //Offset converted to a binary number
      
    //Putting values in 32 bit binary instruction array
    for(i=31;i>=25;i--)
      bin_instruction[i]=*(opcode1+i-25)-'0';//Putting opcode into binary insturction array(0-6)
    for(i=24;i>=20;i--)
      bin_instruction[i]=*(bin_arg1+i-20)-'0';//Putting opcode into binary insturction array(7-11)

    //Putting immediate of 20 bits according to the j Type format
    for(i=19;i>=12;i--)
    bin_instruction[i]=*(bin_offset+i-11)-'0';
    bin_instruction[11]=*(bin_offset+9)-'0';
    for(i=10;i>=1;i--)
    bin_instruction[i]=*(bin_offset+i+9)-'0';
    bin_instruction[0]=*(bin_offset+0)-'0';
    for(i=0;i<32;i++){
    printf("%d",bin_instruction[i]);
  }
  printf("    J Type\n");

  return;

}


int main(int argc, char* argv[]) {
  
     /* open the source file */
     infile = fopen(argv[1], "r"); //First file for reading
     outfile = fopen(argv[2], "w");// second file for writing machine language
     char c;
     char lLine[MAX_LINE_LENGTH + 1], *lLabel, *lOpcode, *lArg1,
          *lArg2, *lArg3, *lArg4;

    int lRet1=1;
    int immediate_op;
     if (!infile) {
       printf("Error: Cannot open input file %s\n", argv[1]);
       exit(4);
     }
     if (!outfile) {
       printf("Error: Cannot open output file %s\n", argv[2]);
       exit(4);
     }

     
  /*look for the .orig statement*/  
   //int start_adress = 0;
 
/****************************************************************/
   FILE * lInfile;
   FILE * lInfile2;

   lInfile = infile;  /* open the input file */
   lInfile2 = infile;
  int current_pc = 0;
  int start_address=0;
  char *templabel;
  int index_table = 0;

  for(start_address=0;lRet1!=DONE;start_address+=4)
    {
        lRet1 = readAndParse( lInfile, lLine, &lLabel,&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );

        if(lRet1!=DONE)//Need to be implemented as the code was being run for two times at the end of file because of God knows what reason('..')
        {
        insert(lLabel,start_address);//Insert the labels along with respective address in a link list
        } 
       
  }

  printf("1st pass done \n\n");

  lRet1=20; //getting ready for second pass
  current_pc=0; 
  opcode_type=0;
  fseek((lInfile+0), 0, SEEK_SET);  //Starting to check the file from start
  //Second pass starts here
  printf("2nd pass start \n");

  for(current_pc=0;lRet1!=DONE;current_pc+=4) //pc increments with 4 each time the loop encounters
  {
    sec_pass=1;//flag for second pass
    lRet1 = readAndParse( lInfile, lLine, &lLabel,&lOpcode, &lArg1, &lArg2, &lArg3, &lArg4 );
    

    if(lRet1!=DONE)
    {
    
      if( opcode_type == 1 )  //Calling r Type for arithmatic instructions
      {
        RType(lOpcode,lArg1,lArg2,lArg3);
        write_instruction(); //write instructions in output file
     
      }
      else if( opcode_type == 2 )  //Calling utype for uType instructions
      {
        uType(lOpcode,lArg1,lArg2);
        write_instruction();
     
      }
      else if(opcode_type==3||opcode_type==4||opcode_type==5||opcode_type==6)//Calling I-type for all opcodes types of immediate instructions
      {
        iType(lOpcode,lArg1,lArg2,lArg3,current_pc);
        write_instruction();
      }
      else if( opcode_type == 7 )  //Calling utype for sType instructions
      {
        sType(lOpcode,lArg1,lArg2,lArg3);
        write_instruction();
  
      }
       else if( opcode_type == 8 )  //Calling utype for jType instructions
      {
        jType(lOpcode,lArg1,lArg2,current_pc);
        write_instruction();  
      }
      else if( opcode_type == 9 )  //Calling utype for bType instructions
      {
        bType(lOpcode,lArg1,lArg2,lArg3,current_pc);
        write_instruction();  
      }
      else if(opcode_type==10)  //Invalid opcode type
      {
        if(search(lLabel)!=0)// check if the opcode_type was changed to invalid because of a label
        {
        fprintf(stderr, "Error code 2: invalid opcode\n");
        exit(2);
        }
      }
         
    }
  } 

   fclose ( infile );     /* Now close files */
  fclose ( outfile);  
}

///////////////////////////////////////////////////THE END OF A HECTIC JOB//////////////////////////////////////////////////////////////////////////////

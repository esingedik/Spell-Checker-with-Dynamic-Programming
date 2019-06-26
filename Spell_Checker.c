#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<conio.h>

typedef enum { 
    INSERTION,
    DELETION,
    CHANGE,
    NONE
}operation;
 
typedef struct word{ 
	char word[50];
}WORD;

typedef struct edit{ 
    char word1;
	char word2;	 
	int value;              
    int index;	           
    struct edit *before;  
    operation type;		 
}EDIT;

void Create_Hash_Dictionary()
{	
	
	int i,control,size=249989; 
	unsigned long int address=0;
	char *temp,**dictionary;
	FILE *f,*p;
	
	f=fopen("book.txt","r");
	if(f==NULL)
	{
		printf("book.txt could not open!\n");
	}
	
	p=fopen("Hash_Dictionary.txt","w");
	if(p==NULL)
	{
		printf("Hash_Dictionary.txt could not open!\n");
	}

	temp=(char*)malloc(125010*sizeof(char));
	dictionary=(char**)malloc(size*sizeof(char*));

	
	while(fscanf(f,"%s\n",temp)!=EOF)  
	{
		for(i=address=0;i<strlen(temp);i++) 
		{
			address+=temp[i]*26*i;	
		}
			
		address=address%size;  
		control=0;
	
		while(control<(size-1) && (dictionary[address]!=NULL)) 
		{
			address=address+1;
			address=address%size;
			control++;
		}
		dictionary[address]=(char*)malloc(strlen(temp)*sizeof(char)); 
		strcpy(dictionary[address],temp); 
		printf("%s is in %d. address\n",temp,address);
	}
	
	for(i=0;i<size;i++) 
	{
		if(dictionary[i]!=NULL)
		{
			fprintf(p,"%s\n",dictionary[i]);
		}
	}

	free(temp);
	free(dictionary);
	fclose(f);
	fclose(p);
}

int Find_Minimum_Value(int x, int y,int z) 
{
    int minimum=x;
    
    if (minimum > y) 
		minimum = y;

    if (minimum > z) 
		minimum = z;
		
    return minimum;
}

EDIT **Initialize_Levenshtein_Matrix(char *string1,int length1,char *string2,int length2) 
{
    int i,j;
    EDIT **matrix;
    
    matrix=(EDIT**)calloc((length1+1),sizeof(EDIT*)); 
    for (i=0;i<length1+1;i++) 
	{
        matrix[i]=(EDIT*)malloc((length2+1)*sizeof(EDIT));
    }
    
    for (i=0;i<length1+1;i++) 
	{
		matrix[i][0].word1=0;
        matrix[i][0].word2=0;
        matrix[i][0].value=i; 
        matrix[i][0].before=NULL;
   	}
 
    for (j=0;j<length2+1;j++)
	{
		matrix[0][j].word1=0;
        matrix[0][j].word2=0;
        matrix[0][j].value=j; 	
        matrix[0][j].before=NULL;
    }
    return matrix; 
}

int Calculate_Levenshtein_Matrix(EDIT **matrix,char *string1,int length1,char *string2,int length2)
{
    int i,j,change_rate,minimum;
    int distance_deletion,distance_insertion,distance_change;
			
    for (j=1;j<length2+1;j++) 
	{
        for (i=1;i<length1+1;i++) 
		{
			distance_deletion=0,distance_insertion=0,distance_change=0;
            
            if (string1[i-1]==string2[j-1]) 
			{
                change_rate=0;
            }
            
			else 
			{
                change_rate=1; 
            }

            distance_change=matrix[i-1][j-1].value+change_rate; //Change 
            distance_deletion=matrix[i-1][j].value+1;    //Delete
            distance_insertion=matrix[i][j-1].value+1; //Insert
            
            minimum=Find_Minimum_Value(distance_deletion,distance_insertion,distance_change); 
            
            matrix[i][j].word1=string1[i-1];
            matrix[i][j].word2=string2[j-1];
            matrix[i][j].value=minimum;                  
            matrix[i][j].index=i-1; 
            
            if(minimum==distance_insertion)  
			{
                matrix[i][j].type=INSERTION; 		  
                matrix[i][j].before=&matrix[i][j-1]; 
            }                                       
            
            else if(minimum==distance_deletion) 
			{
                matrix[i][j].type=DELETION;
                matrix[i][j].before=&matrix[i-1][j];
            }
            
            else 
			{ 
                if(change_rate>0)  
				{
                    matrix[i][j].type=CHANGE;
                }
                
                else 
				{
                    matrix[i][j].type=NONE; 
                }
                
                matrix[i][j].before=&matrix[i-1][j-1]; 
            }
        }
    }
    return matrix[length1][length2].value; 
}

int Find_Difference(EDIT **info,char *string1,char *string2)
{
	int i,distance,length1,length2;
   	EDIT **matrix, *first; 
   	
   	length1=strlen(string1);
	length2=strlen(string2);
 	*info=(EDIT*)calloc(distance,sizeof(EDIT));
 	
    if (length1==0) 
        return length2;
    
    if (length2==0)
        return length1;

    matrix=Initialize_Levenshtein_Matrix(string1,length1,string2,length2);  
    if (!matrix) 
	{
        *info=NULL;
        return 0;
    }

    distance=Calculate_Levenshtein_Matrix(matrix,string1,length1,string2,length2);  
    
    if (*info) 
	{ 
        i=distance-1;
        for (first=&matrix[length1][length2];first->before!=NULL;first=first->before) 
		{
            if (first->type!=NONE) 
			{
                memcpy(*info+i,first,sizeof(EDIT));
                i--;
            }
        }
    }
    
    else 
	{
        distance=0;
    }

    for (i=0;i<=length1;i++)
	{
        free(matrix[i]);
    }
    free(matrix);
    return distance;
}

int Is_The_Word_In_Dictionary(char *input)
{
	FILE *fp;
	WORD *reader;
	unsigned long int address=0;
	int i,j,control,size=249989;
	
	reader=(WORD*)malloc(size*sizeof(WORD));

	fp=fopen("Hash_Dictionary.txt","r"); 
	if(fp==NULL)
	{
		printf("Hash_Dictionary.txt could not open");
		exit(1);
	}
	
	i=0;
	while(!feof(fp))  
	{
		fscanf(fp,"%s\n",reader[i].word);
		i++;
	}
	
	for(j=address=0;j<strlen(input);j++) 
	{
		address+=input[j]*26*j;	
	}
			
	address=address%size; 
	
	if(strcmp(reader[address].word,input)!=0) 
	{
		control=0;
		while(control<(size-1)&&(reader[address].word!=NULL)&&(strcmp(reader[address].word,input)!=0)) 
		{
			address=address+1;
			address=address%size;
			control++;
		}	
	}
	
		if((strcmp(reader[address].word,input)==0)) 
		{
			printf("Success");
			return 1;
		}
			
		else
			return 0;
}

int Calculate_Edit_Distance(char *word1,char *word2) 
{
	int distance_change,distance_insert,distance_deletion;
	
	if (word1[0]=='\0' && word2[0]=='\0')
		return 0;
	
	if (word1[0]=='\0') 
		return strlen(word2);
	
	if (word2[0]=='\0') 
		return strlen(word1);
	
	if (word1[0]==word2[0])
		return Calculate_Edit_Distance(word1+1,word2+1);
	
	distance_change=Calculate_Edit_Distance(word1+1,word2+1)+2;  
	distance_insert=Calculate_Edit_Distance(word1,word2+1)+1;  
	distance_deletion=Calculate_Edit_Distance(word1+1,word2)+1; 

	return Find_Minimum_Value(distance_change,distance_insert,distance_deletion);
}

void Find_Edit_Distance(char *input) 
{
	FILE *fp,*t;
	WORD *reader;
	int i=0,j,size=249989,distance,minimum_distance,minimum_index;
	char *temp;
	
	temp=(char*)malloc(50*sizeof(char)); 
	reader=(WORD*)calloc(size*2,sizeof(WORD)); 
	
	fp=fopen("Hash_Dictionary.txt","r");
	if(fp==NULL)
	{
		printf("Hash_Dictionary.txt could not open");
		exit(1);
	}

	t=fopen("testout.txt","a+"); 
	if(t==NULL)
	{
		printf("testout.txt could not open!");
		exit(1);
	}
	
	while (fscanf(fp,"%s\n",temp)!=EOF) 
	{
		if(temp!=NULL)
		{
			strcpy(reader[i].word,temp); 
			i++;
		}
	}

    minimum_distance=40; 

	fprintf(t,"%s\t",input);

    for(i=0;i<size;i++)
    {
    	distance=Calculate_Edit_Distance(input,reader[i].word); 
    	
    	if(distance<minimum_distance)
		{
			minimum_distance=distance;
			minimum_index=i;
		}
		
		if(distance==0) 
		{
			fprintf(t,"OK.\n");
			fclose(t);
			break;
		}
	}
	
	if(minimum_distance>4) 
	{
		fprintf(t,"NONE.\n");
	}

	else if(minimum_distance!=0 && minimum_distance<5)
	{
		EDIT *info; 
		int dist;
		fprintf(t,"%s\t",reader[minimum_index].word);
		
	    dist=Find_Difference(&info,input,reader[minimum_index].word); 
	  
		for (i=0;i<minimum_distance;i++) 
		{
	    	if (info[i].type == INSERTION)
		    {
		    	if(info[i].word2!=NULL)
		    		fprintf(t,"%c(Insert)\t",info[i].word2);
		    		
		    	fprintf(t,"%c(Delete)\t",input[0]);
			}
		       
		    else if (info[i].type == DELETION)
		        fprintf(t,"%c(Delete)\t",info[i].word1);
		    
		    else if(info[i].type==CHANGE) 
		        fprintf(t,"%c(Change)\t",info[i].word1);
		}								
	}
	fprintf(t,"\n");
fclose(t);
}

int main()
{
	char *input,str[200]; 
	int i,j;
	FILE *f;
	input=(char*)malloc(sizeof(char)*50); 

		
 	while(1) 
	{
        scanf("%s",input); 
	    
		if(Is_The_Word_In_Dictionary(input)!=1) 
        	Find_Edit_Distance(input); /

		system("cls");
		
		f=fopen("testout.txt","r"); 
		while(!feof(f)) 
		{
			if(fgets(str,200,f)!=NULL)
				printf("%s",str);
		}
		fclose(f);
		
		char tus=getch(); 
		if(tus==char(27)) 
			exit(0);
    }
	return 0;
}

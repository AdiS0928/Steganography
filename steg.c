#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
struct RGB // As structure that stores the rgb values of a pixle 
{
    int R;
    int G;
    int B;
};

struct PPM  // structure that stores the ppm file
{
    char code[3];
    int width;
    int height;
    int max;
    struct RGB * ** img;
};

struct comment // structure to create a linked list of comments
{
    char com[70];
    struct comment * next;
};

struct RGB ** IMG;

int hight,width,max;

struct comment * Point=NULL;
struct comment * TempPoint=NULL;

char file[20];

struct RGB ** makeMatrix (int m,int n)  // To create the matrix 
{
   struct RGB ** newm = (struct RGB **)malloc(m*sizeof(struct RGB *));
   int i; 
   for(i=0;i<m;i++)
    newm[i] = (struct RGB *)malloc(n*sizeof(struct RGB));
   return newm;
}

void StoreCom(char ch,FILE * f) // function to create a linked list that stores the comments
{
    int i=0;
    struct comment * comn=NULL;
    comn = (struct comment *)malloc(sizeof(struct comment));
    comn->next=NULL;
    
        while(ch!='\n')
        {
            comn->com[i]=ch;
            ch = getc(f);
            i++;
        }

        if(Point==NULL)
        { 
            Point = comn;
            TempPoint=Point;
    }
        else 
        {
            TempPoint->next=comn;
            TempPoint=comn;
        }

}

void ShowCom(FILE * f)  // To print the comments into a file
{
    fprintf(f,"%s\n",Point->com);
    while(Point->next!=NULL)
    {
        Point=Point->next;
        fprintf(f,"%s\n",Point->com);
    }
    
}

void CheckCom(FILE * f)     // To check for comments in a file
{
    char ch = getc(f);
    while(1)
    {
        if(ch=='#')
        {
            StoreCom(ch,f);
            ch = getc(f);
            continue;
        }
        else
        {
            fseek(f,-1,SEEK_CUR);
            break;
        }
        
    }

}

void readMatrix(FILE * f,struct RGB * * M,int m,int n) // To fill the matrix with the rgb pixle values
{
    int i,j,count=0;
    struct RGB pix;
        
   for(i=0;i<m;i++){
    CheckCom(f);
    for(j=0;j<n;j++)
    {
        fscanf(f,"%d\n",&(pix.R));
        fscanf(f,"%d\n",&(pix.G));
        fscanf(f,"%d\n",&(pix.B));

        M[i][j] = pix;
    }
   }
}

struct PPM * getPPM(FILE * f)   // Funtion that reads and creates the ppm image
{
    struct PPM * P = (struct PPM*)malloc(sizeof(struct PPM));
    fscanf(f,"%s\n",P->code);
    CheckCom(f);
    fscanf(f,"%d\n",&(P->width));
    CheckCom(f);
    fscanf(f,"%d\n",&(P->height));
    CheckCom(f);
    fscanf(f,"%d\n",&(P->max));
    CheckCom(f);
    IMG = makeMatrix(P->height,P->width);
    readMatrix(f,IMG,P->height,P->width);
    P->img = &IMG;
    return P;

}

showPPM(struct PPM * im)    // function to display the ppm file
{
    printf("%s\n",im->code);

    printf("%d %d\n",im->width,im->height);
    printf("%d\n",im->max);

    int i,j;
    for(i=0;i<im->height;i++)
    for(j=0;j<im->width;j++)
    {
        printf("%d ",IMG[i][j].R);
        printf("%d ",IMG[i][j].G);
        printf("%d\n",IMG[i][j].B);
     }
}

struct PPM * encode(struct PPM * im, char * message,unsigned int secret)       // Function to hide the message into the image
{
    srand(secret);
    int lenM = strlen(message);
    int i,j,k=0;
    int * bit = (int*)malloc((lenM+1)*8*sizeof(int));
    for(i=0;i<(lenM+1);++i)
    {
        char ch=message[i];
        for(j=0;j<8;++j)
        {
            bit[k++]= (ch>>j)&1;
        }
         
    }
    int l,m,counter=0;
    while(1)
    {
        l = rand()%(im->height);
        m= rand()%(im->width);
        IMG[l][m].R = IMG[l][m].R & 0xFE | bit[counter++] ;
        if(counter==(lenM+1)*8)
            break;

        IMG[l][m].G = IMG[l][m].G & 0xFE | bit[counter++] ;
        if(counter==(lenM+1)*8)
            break;

        IMG[l][m].B = IMG[l][m].B & 0xFE | bit[counter++] ;
        if(counter==(lenM+1)*8)
            break;
    }

    FILE * fout = fopen(file,"w");
    fprintf(fout,"%s\n",im->code);
    ShowCom(fout);
    fprintf(fout,"%d %d\n",im->width,im->height);
    fprintf(fout,"%d\n",im->max);

    int n,o;
    for(n=0;n<im->height;n++)
    for(o=0;o<im->width;o++)
    {
         fprintf(fout,"%d ",IMG[n][o].R);
         fprintf(fout,"%d ",IMG[n][o].G);
         fprintf(fout,"%d\n",IMG[n][o].B);
     }
    free(bit);
    fclose(fout);
    return im;
}

char * decode(struct PPM * im, unsigned int secret)     // Function to decode the message hidden inside a message
{
    srand(secret);
    
    int i,j,k=0,counter= 0;
    char ch;

    int ch2[8];

    while(1)
    {
        i= rand()%(im->height);
        j= rand()%(im->width);
        ch2[k++] = IMG[i][j].R &0b00000001;
        if(k==8)
            {
                
                ch= (ch2[7]* pow(2,7)) + (ch2[6]* pow(2,6)) + (ch2[5] * pow(2,5)) + (ch2[4]* pow(2,4)) + (ch2[3]* pow(2,3)) + (ch2[2]* pow(2,2)) + (ch2[1]* pow(2,1)) + (ch2[0]* pow(2,0));
                counter++;
                if(ch=='\0')
                    break;
                k=0;
            }

        ch2[k++] = IMG[i][j].G &0b00000001;
        if(k==8)
            {
                
                ch= (ch2[7]* pow(2,7)) + (ch2[6]* pow(2,6)) + (ch2[5] * pow(2,5)) + (ch2[4]* pow(2,4)) + (ch2[3]* pow(2,3)) + (ch2[2]* pow(2,2)) + (ch2[1]* pow(2,1)) + (ch2[0]* pow(2,0));
                counter++;
                if(ch =='\0')
                    break;
                k=0;
            }

        ch2[k++] = IMG[i][j].B &0b00000001;
        if(k==8)
            {
                
                ch= (ch2[7]* pow(2,7)) + (ch2[6]* pow(2,6)) + (ch2[5] * pow(2,5)) + (ch2[4]* pow(2,4)) + (ch2[3]* pow(2,3)) + (ch2[2]* pow(2,2)) + (ch2[1]* pow(2,1)) + (ch2[0]* pow(2,0));
                counter++;
                if(ch=='\0')
                    break;
                k=0;
            }
        

    }

    srand(secret);

    int l,m,p=0,q=0;
    char * ch3 = (char*) malloc(counter*sizeof(char));

    while(1)
    {
        l= rand()%(im->height);
        m= rand()%(im->width);
        ch2[q++] = IMG[l][m].R &0b00000001;
        if(q==8)
            {
                
                ch3[p]= (ch2[7]* pow(2,7)) + (ch2[6]* pow(2,6)) + (ch2[5] * pow(2,5)) + (ch2[4]* pow(2,4)) + (ch2[3]* pow(2,3)) + (ch2[2]* pow(2,2)) + (ch2[1]* pow(2,1)) + (ch2[0]* pow(2,0));
                if(ch3[p]=='\0')
                    break;
                p++;
                q=0;
            }

        ch2[q++] = IMG[l][m].G &0b00000001;
        if(q==8)
            {
                
                ch3[p]= (ch2[7]* pow(2,7)) + (ch2[6]* pow(2,6)) + (ch2[5] * pow(2,5)) + (ch2[4]* pow(2,4)) + (ch2[3]* pow(2,3)) + (ch2[2]* pow(2,2)) + (ch2[1]* pow(2,1)) + (ch2[0]* pow(2,0));
                if(ch3[p]=='\0')
                    break;
                p++;
                q=0;
            }

        ch2[q++] = IMG[l][m].B &0b00000001;
        if(q==8)
            {
                
                ch3[p]= (ch2[7]* pow(2,7)) + (ch2[6]* pow(2,6)) + (ch2[5] * pow(2,5)) + (ch2[4]* pow(2,4)) + (ch2[3]* pow(2,3)) + (ch2[2]* pow(2,2)) + (ch2[1]* pow(2,1)) + (ch2[0]* pow(2,0));
                if(ch3[p]=='\0')
                    break;
                p++;
                q=0;
            }
        

    }
    return ch3;
    
}

int main(int argc, char* argv[])
{ 

    if(strcmp(argv[1],"e")==0)
    {
        int secret;
        char ch[20];
        printf("Enter the message to be encoded(limit 20 characters)\n");
        scanf ("%[^\n]s",ch);
        printf("Enter the secret");
        scanf("%d",&secret);
        FILE * f = fopen(argv[2],"r");
        file = argv[3];
        struct PPM * im = getPPM(f);
        im = encode(im,ch,secret);
        int i;
        for(i=0;i<im->height;i++)
            free(IMG[i]);
        fclose(f);
    }
    else
    {
        int secret;
        printf("Enter the secret\n");
        scanf("%d",&secret);
        FILE * f = fopen(argv[2],"r");
        struct PPM * im = getPPM(f);
        char * ch = decode(im,secret);
        printf("%s\n",ch);
        int i;
         for(i=0;i<im->height;i++)
             free(IMG[i]);
        free(ch);
        fclose(f);
    }

}

#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <openssl/cmac.h>
#include <assert.h>

/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
#define ANSI_COLOR_RED     "\033[31m"
#define ANSI_COLOR_GREEN   "\033[32m"
#define ANSI_COLOR_RESET   "\033[0m"
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
void printBytes(unsigned char *buf, size_t len) {
  int i;
  for(i=0; i<len; i++) {
    printf("%02x ", buf[i]);
  }
  printf("\n");
}

int comparestrings (unsigned char *conv, unsigned char *orig, int vb)
{
    int j = strcasecmp((const char*)conv,(const char*)orig);
    if (vb == 1) {
        if (j !=0) printf (ANSI_COLOR_RED"Error"ANSI_COLOR_RESET" converted: (%s) vs original: (%s)\n",conv,orig);
        else       printf (ANSI_COLOR_GREEN"Good"ANSI_COLOR_RESET" converted: (%s) vs original: (%s)\n",conv,orig);
    }
    else if (vb == 2) {
        if (j !=0) printf (ANSI_COLOR_RED"MAC DO NOT MATCH! Error!!!!!!!!\n"ANSI_COLOR_RESET);
        else       printf (ANSI_COLOR_GREEN"MATCH :-)"ANSI_COLOR_RESET".\n");
    }
	//printf("\n"ANSI_COLOR_RESET);
	return j;
}
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
typedef struct pixel {

 int index;
 int row;
 int col;
 unsigned int val;
} pix;
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
typedef enum packmodes{
           mode0_8,mode0_12,mode0_16,mode0_20,mode0_24,
           mode1_8,mode1_12,mode1_16,mode1_20,mode1_24 } packmodes;

/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
packmodes getoperation(unsigned int d, unsigned int m) 
{
    assert(m <= 2);
    assert(d == 8 || d == 12 || d == 16 || d == 20 || d == 24);

    printf("function call: getoperation\n");
    printf("\tdataformat: 'd%d\n\tmode: 'd%d\n",d,m);
    if        (( d == 8) && (m ==0)) {
        return mode0_8 ;
    } else if ((d == 12) && (m==0)) {
        return mode0_12 ;
    } else if ((d== 16) && (m==0)){
        return mode0_16 ;
    } else if ((d== 20) && (m==0)){
        return mode0_20 ;
    } else if ((d== 24) && (m==0)){
        return mode0_24 ;
    } else if ((d== 8) && (m==1)){
        return mode1_8 ;
    } else if ((d== 12) && (m==1)){
        return mode1_12 ;
    } else if ((d== 16) && (m==1)){
        return mode1_16 ;
    } else if ((d== 20) && (m==1)){
        return mode1_20 ;
    } else if ((d== 24) && (m==1)){
        return mode1_24 ;
    }
    else {
        return(0);
    }
}
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
int roi_reduce_pixdb(int         idxdb, 
                     pix         *db , 
                     pix         *dbroi, 
                     unsigned int firstcol, 
                     unsigned int lastcol,
                     unsigned int skipcol,
                     unsigned int firstrow, 
                     unsigned int lastrow,
                     unsigned int skiprow,
                     int          top_embed,
                     int          sft_embed, 
                     int verb )
{
    pix *ptr, *roiptr;
    int i;
    int roiindex = 0;
    ptr = db;
    roiptr = dbroi;
    unsigned int firstcolf = firstcol *4; // ROI definition converns 4 pixels per clock cycle
    unsigned int lastcolf  = lastcol  *4;
    unsigned int skipcolf  = skipcol  *4;
    int p;
	int loop = 0;

    for (i=0; i<idxdb; i++) { 
        if ( (ptr->row >= firstrow) && (ptr->row <= lastrow) && (((ptr->row - firstrow)%skiprow)==0) ) {
            if ( (ptr->col >= firstcolf) && (ptr->col <= lastcolf) &&  (((ptr->col - firstcolf)%skipcolf )==0) ) {
                for(p=0;p<4;p++) { // need to grep the next 4 pixels on the line (e,o,i and u)
                    if(verb && (loop < 11))  {
                        printf  ("[DEBUG](index:%d) in ROI row:%d, col:%d, value:%d <=> 0x%06x\n",i,ptr->row,ptr->col,ptr->val,ptr->val);
						loop++;
                    }
                    roiptr->index=roiindex;
                    roiptr->col = ptr->col;
                    roiptr->row = ptr->row; 
                    roiptr->val = ptr->val;
                    roiindex++;
                    roiptr++; 
                    if (p < 3) ptr=ptr+1;      
                }
                i=i+3;
                //if(verb) printf  ("\t\t\t\t\t\t\t\t\t\t\t\tindex:%d -- pixel e,o,i,u \n",i);
            }
        }
        ptr++;        
    }
    if(verb) printf  ("[DEBUG] roi index: %d\n",roiindex);
    return(roiindex);
}
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
void get_mac_from_sensor (int         idxdb, 
                          pix         *db,     
                          int         macrow,
                          int         sft_embed,
                          unsigned char        *mac)
{
    pix *ptr;
    ptr = db;
    int k;
    printf("------------------------------------------------------------------\n");
    printf("Retrieving MAC from mac rows...\n");    
    printf("\tmac rows starts at row nb: %0d\n",macrow);
    unsigned char * tmpmac; 
    tmpmac = mac;
	int pflag = 1;
	int mac_strt_pix = 0;
    for (k=0; k<idxdb;k++) {
        if (ptr->row == macrow) {
			if (pflag == 1) {
				pflag = 0;
				mac_strt_pix = k;
				printf("\tindex for LSB inline MAC is: %d\n",k);
				printf("\tvalue of LSB in MAC line is: %d\n",ptr->val >> sft_embed); //assuming shift here is the same as per top embedded??
				//break;
				printf("\tmacrow is: %d ", ptr->val);
			}
			else if(k <= mac_strt_pix+15) printf("%d ", ptr->val);
			else break;
        }
        ptr++;
    }
    ptr = db;
    ptr = ptr+mac_strt_pix+15;  // this is the last mac byte...TODO: arrrgh forgot their are MSB aligned..!!
	printf("\n\ttmpmac = ");
    for (k=0; k<16; k++) {
        sprintf((char*)tmpmac,"%02x",ptr->val >> sft_embed); //assuming shift here is the same as per top embedded??
        tmpmac=tmpmac+2;
		printf("%d ",ptr->val >> sft_embed);
		ptr--;
    }
    printf("\tbyte values (MSB aligned) shifted by %d bits\n",sft_embed);
    printf("\t----------------------------------------------------------\n");
    printf("\tSensor MAC: 0x%s\n",mac);
    printf("\t----------------------------------------------------------\n");
    
}
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
void  get_framecount_plus_iv(int         idxdb, 
                              pix         *db,     
                              unsigned int piv32_0,
                              unsigned int piv32_1, 
                              unsigned int piv32_2,
                              unsigned char *iv, char * ivplusfc, unsigned int ignorefc,
                              unsigned int sft_top_embed, int embedded_data_en)

{
    pix *ptr;
    unsigned int fc;
    char * ppi;
    long long fc2;

    printf("------------------------------------------------------------------\n");
    printf("embedded_data_en=%d, Retrieving Frame counter value (32bit register)...\n",embedded_data_en);
    ptr = db;                                                                                          // reset pointer to first address of database

    if (embedded_data_en) {   //TBR Mayur for making FC 0 ; testing (if u want frame count from embedded data then enable this part of code)
		ptr = ptr + 6;  // first frame counter info is pixel 7 in embedded data
		printf  ("\t[31:16] of 0x2000 FRAME_COUNT2_(MSB): (at row:%d,col:%d):0x%04x\n",ptr->row,ptr->col,ptr->val&0x0FF);
		fc2 = (long long)(ptr->val&0x000FF)<< 48;

		ptr = ptr + 2;
		printf  ("\t[15:0]  of 0x2000 FRAME_COUNT2_:      (at row:%d,col:%d):0x%04x\n",ptr->row,ptr->col,ptr->val&0x0FF);
		fc = fc2 + ((long long)(ptr->val&0x000FF)<< 32);

		//printf("fc2: %lld\n",fc2);


		printf  ("\t*-------- IGNORING FRAME_COUNTER2 ------------*/\n\n");

		ptr = ptr + 2;
		fc = fc + (((ptr->val >> sft_top_embed)&0x000FF)<<8);
		printf  ("\t[15:8] of 0x2002 FRAME_COUNT_:       (at row:%d,col:%d,val:0x%04x):0x%04x\n",ptr->row,ptr->col, ptr->val,(ptr->val >> sft_top_embed)&0x000FF);
		printf  (" \tfc: %024x\n",fc);
		ptr = ptr + 2;

		fc = fc + ((ptr->val >> sft_top_embed)&0x000FF);
		printf  ("\t[7:0]  of 0x2002 FRAME_COUNT_(LSB):  (at row:%d,col:%d,val:0x%4x):0x%04x\n",ptr->row,ptr->col,ptr->val,(ptr->val >> sft_top_embed)&0x000FF);
		printf  (" \tfc: %024x\n",fc);
	}
	else {
		fc = 0;
	}
    printf  ("\tframecounter value: %024x\n",fc);

    printf  ("\t                    +\n");
    printf  ("\tInitial given iv:   %s\n",iv);
    ppi = ivplusfc;
    sprintf(ppi, "%08x",piv32_2&0xffffffff);
    ppi = ppi+8;
    sprintf(ppi, "%08x",piv32_1&0xffffffff);
    ppi = ppi+8;
    if(!ignorefc) {
        sprintf(ppi, "%08x",(piv32_0+fc)&0xffffffff);
    }
    else {
        sprintf(ppi, "%08x",piv32_0&0xffffffff);  //cheecky switch to ignore fc addition
    }

    //printf("\tchecking for wrap...\n");
    ppi = ivplusfc;
    if (piv32_0+fc < fc) {
        if ( piv32_1+1 < piv32_1) {
            sprintf(ppi, "%08x",(piv32_2+1)&0xffffffff);
            ppi = ppi + 8;
            sprintf(ppi, "%08x",(piv32_1+1)&0xffffffff);
            ppi = ppi + 8;
            sprintf(ppi, "%08x",(piv32_0+fc)&0xffffffff);
        } else {
            ppi = ppi + 8;
            sprintf(ppi, "%08x",(piv32_1+1)&0xffffffff);
            ppi = ppi + 8;
            sprintf(ppi, "%08x",(piv32_0+fc)&0xffffffff);
        }
    }
    printf  ("\t                    ________________________\n");    
    printf  ("\t           iv+fc = '%s'\n",ivplusfc); 
}

/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
char *substring(char *string, int position, int length)
{
   char *pointer;
   int c;
 
   pointer = malloc(length+1);
   if (pointer == NULL)
   {
      printf("Unable to allocate memory.\n");
      exit(1);
   }
   for (c = 0 ; c < length ; c++)
   {
      *(pointer+c) = *(string+position-1);      
      string++;  
   }
   *(pointer+c) = '\0';
   return pointer;
}
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
void printbytearray2file(unsigned char *h, char * name, int l, FILE *fp)
{
	int i;
    int s = 0;
	if (l > 0) {
		for (i=0; i<l; i++)
		{
			fprintf(fp,"%02x", h[i]);
			//fprintf(fp,"0x%02x:", h[i]);
            s++;
            if(s == 16) {
                s = 0;
                fprintf(fp,"\n");
            }

		}
	}
    //fprintf(fp,"\n");
}
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
void printpixdb ( pix *db, int size, char *filename)
{
    pix *pdb;
    int i;
    pdb = db; 
    FILE *f; 
    f = fopen(filename,"w");
    fprintf  (f,"\n###############################################################################\n");
    fprintf  (f,"- Data parsed with row,col position and corresponding hex value for each pixel\n");
    fprintf  (f,"- Note embedded data value has been read from MSB-aligned input!\n");
    fprintf  (f,"- Nb of elements: %0d\n",size);
    fprintf  (f,"###############################################################################\n");

    for (i=0; i<size; i++) {
        fprintf  (f,"row:%d, col:%d, value:%d <=> 0x%06x\n",pdb->row,pdb->col,pdb->val,pdb->val);
        pdb++;
    }
    fclose(f);
}
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
void get_shift_active_compand (int dataf, int compandf, int *sft, int *mask)
{
//int t;

    if  ((dataf == 20) && (compandf == 12)) {
        printf("Companding! --> zero'ing out [7:0] of data-stream\n");
        *sft = 8;     
        *mask = 0x00055; 
        return;   
    }
    else if ((dataf == 24) && (compandf == 12)) { 
        printf("Companding! -->  zero'ing out [11:0] of data-stream\n");
        *sft = 12;
        *mask = 0x000555; 
       return;   
    }
    else if ((dataf == 16) && (compandf == 14)) { 
        printf("Companding! -->  zero'ing out [1:0] of data-stream\n");
        *sft = 2;
        *mask = 0x000001; //Ashu
       return;   
    }
	else if ((dataf == 20) && (compandf == 14)) { 
        printf("Companding! -->  zero'ing out [1:0] of data-stream\n");
        *sft = 6;
        *mask = 0x000015;
       return;   
    }
	else if ((dataf == 16) && (compandf == 16)) { 
        printf("Companding! -->  zero'ing out [1:0] of data-stream\n");
        *sft = 0;
        *mask = 0x000000;
       return;   
    }
	else if ((dataf == 20) && (compandf == 16)) { 
        printf("Companding! -->  zero'ing out [3:0] of data-stream\n");
        *sft = 4;
        *mask = 0x000005;
       return;   
    }
    else {
        printf(ANSI_COLOR_RED"Error!! "ANSI_COLOR_RESET"No support for data-format: 'd%d and companded format: 'd%d\n",dataf,compandf);
        exit(1);
    }
}

/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
void bytearraytoascii(unsigned char *h, unsigned char *s, int l, char * name)
{
	unsigned char sb[l*2];
	//printf ("lenght of array %s: %d\n",name,l);
	sb[0] = '\0';
	char buf[3];
	//printf ("------------------\n");
	int i;
	//printf ("sb:");

	for (i=0; i<l; i++)
	{
		sprintf(buf,"%02x",h[i]);
		strcat((char*)sb,buf);
	}
	//printf ("s=> \n", s);
	//printf ("sb=>%s\n", sb);
	#if 0 //For Debug purpose
	printf ("sb = \n");
	for (i=0; i<l; i++) {
		printf("%02x",sb[i]);
	}
	#endif
	memcpy(s,sb,(l*2)+1);
	#if 0 //For Debug purpose
	printf ("\ns = \n");
	for (i=0; i<l; i++) {
		printf("%02x",s[i]);
	}
	printf ("\n");
	#endif

}
/* ----------------------------------------------------------------------------- */
/* ----------------------------------------------------------------------------- */
int asciitobytearray_faster(unsigned char *s, unsigned char *h)
{
	unsigned int str_len = strlen((const char*)s);
	//printf("Ashu1 %d", str_len);
#if 0  //Commented by Ashutosh due to Segmentation fault error for full size image data
	char buffer[str_len/2];
	//unsigned char buffer_v2[str_len/2];
	strcpy(buffer, "");
#endif	
	int i;
    char *tmpcat;
	int f= 0;
    
    //printf("Ashu2"); 
    for (i=0; i< str_len/2; i++)
	{
        //printf("substring: %s\n",tmpcat);
        tmpcat = substring( (char*)s,(2*i)+1,2);
		//sscanf(tmpcat,"%02x", (h+i)/*&buffer[i]*/);
		sscanf(tmpcat,"%p", (void **)(h+i)/*&buffer[i]*/);
		//sscanf(tmpcat,"%hu", (h+i)/*&buffer[i]*/);
		if (f<6) {
			printf("%p, tmpcat=%s, position=%d, %c, %c\n", s+2*i, tmpcat, (2*i)+1, s[2*i], s[2*i+1]);
		}
        free(tmpcat);
		f++;

		//printf("%p:\t%02X--->%c%c ::::: v2: %s\n",s+2*i,buffer[i],s[2*i],s[2*i+1],tmpcat);

	}
	//printf("\n\tmemcpy of buffer to output...\n");
	//memcpy (h, buffer,(str_len/2));  //Commented by Ashutosh due to Segmentation fault error for full size image data
	for (i=0; i< 17; i++) {
		printf("%02x",h[i]);
	}
	printf("\tsize = %d\n", str_len/2);

	return (str_len/2);
}
/* ----------------------------------------------------------------------------- */
/*                                   MAIN                                        */
/* ----------------------------------------------------------------------------- */
int main(int argc, char **argv)

{



    const struct option l_option[]={

             { "help",      0,                 NULL, 'h'},
             { "file",      required_argument, NULL, 'f'},
             { "dataformat",required_argument, NULL, 'd'},
             { "compand",   required_argument, NULL, 'p'},
             { "packmode",  required_argument, NULL, 'm'},
             { "paddingreg",required_argument, NULL, 'r'},
             { "iv",        required_argument, NULL, 'i'},
             { "key",       required_argument, NULL, 'k'},
             { "embedlines",required_argument, NULL, 'e'},
             { "cmac",      required_argument, NULL, 'c'},   //assume gmac unless option set
             { "verbose",   required_argument, NULL, 'v'},   
             { "colroi",    required_argument, NULL, 'x'},   
             { "rowroi",    required_argument, NULL, 'y'},   
             { "ignorefc",  required_argument, NULL, 'g'},   
             { "statrow",   required_argument, NULL, 's'},
             { "live_data_en",  required_argument, NULL, 'L'},   
             { "embedded_data_en",   required_argument, NULL, 'E'}, 			 
    };
    const char *s_option="hf:d:p:m:r:i:k:e:s:L:E:cx:y:vg";
    int ignorefc = 0;
    int pmode;
    packmodes packmode; 
    int dataformat;
    int compandformat = 0;
    int sft_act_compand;
    int padding_embed_compand;
    int paddingreg;
    int top_embed_lines = 2;
    unsigned char *iv;
    unsigned char *key;
    char n_opt;
    FILE *fp, *ofp, *ofp2;
    const char *f;
    int cmac = 0; 
    int verbose = 0;
    char outputFilename[]  = "data2string.txt";
    char outputFilename2[] = "data2bytearray.txt";
    printf  ("------------------------------------------------------------------\n");
    printf  ("Entries:\n");
    unsigned int piv32_0 ;
    unsigned int piv32_1 ;
    unsigned int piv32_2 ;
    unsigned int sft_top_embed;
    unsigned int firstrowroi = 0;
    unsigned int lastrowroi  = 0;
    unsigned int skiprowroi  = 0;
    unsigned int firstcolroi = 0;
    unsigned int lastcolroi  = 0;
    unsigned int skipcolroi  = 0;
    unsigned int stat_lines = 10000;
	int embedded_data_en = 1;
	int live_data_en = 1;
    unsigned char sensormac[128] ;
                              
 
    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    do {
              n_opt=getopt_long(argc,argv,s_option,l_option,NULL);
              switch(n_opt){
                      case 'h':
                              printf("help!!\n");
                              printf("----------------------------------------------------------------------------------------------------\n");
                              printf("\nUsage:\n");
                              printf("----------------------------------------------------------------------------------------------------\n");
                              printf("        Inputs:\n\n");
                              printf("\t-f (--file):\t\timage txt file (decimal pixels, tab separated from DevWare/Migmate) to process (default: NULL)\n\t\t\t\t"ANSI_COLOR_RED"..might need to go through dos2unix\n"ANSI_COLOR_RESET); 
                              printf("\t-d (--dataformat):\tsupported data format [bits]: 8, 12, 16, 20, 24 (default: NULL)\n"); 
                              printf("\t-m (--packmode):\tpacking/padding MAC mode: 0|1 (default:0)\n"); 
                              printf("\t-r (--paddingreg):\t0x value of padding register (LSB byte is relevant) eg: 0x55 (default: NULL)\n"); 
                              printf("\t-i (--iv):\t\tinitial vector value for GMAC only: 96bits (default: NULL) \n"); 
                              printf("\t-e (--embedlines):\tnumber of top embedded lines: eg:2 (default:2)\n"); 
                              printf("\t-k (--key):\t\tstring of of hex 128 bits eg:'1c7135af627c04c32957f33f9ac08590'\n"); 
                              printf("\t-c (--cmac):\t\tno args, to run AES-CBC for CMAC \n"); 
                              printf("\t-y (--colroi):\t\tdefines columms ROI in this format: firstCol_lastCol_skipCol, eg: 2_10_2   (all decimal)\n"); 
                              printf("\t-x (--rowroi):\t\tdefines rows ROI in this format:    firstRow_lastRow_skipRow, eg: 2_2150_2 (all decimal)\n"); 
                              printf("\t-g (--ignorefc):\tignore Frame count addition to iv!..to be used only with -e 0\n");
                              printf("\t-s (--statrow):\t\tstart of stat row (end does not matter as stat is just before the last 2 mac_rows (default: 'd10000)\n"); 
                              printf("\t-p (--compand):\t\tcompanded output data-format (if different from raw data-format)\n"); 
							  printf("\t-L (--live_data_en):\tImage with Live data & embedded data, default 1 else set it to 0(for test pattern, embedded data ('-E') need to set either 0 or 1)\n");
							  printf("\t-E (--embedded_data_en):\tImage with embedded data aplicable only for test pattern(for '-L 0'), default 1 else set it to 0 (w/o embedded data)\n");
                              printf("\t-v (--verbose):\t\tno args, to run in verbose (careful, long stdout!)\n"); 
                              printf ("----------------------------------------------------------------------------------------------------\n");
                              printf("        Outputs:\n\n");
                              printf("               $cwd/data_from_image.txt: dump of pixels position and value\n");
                              printf("               $cwd/data2strings.txt:    complete string of hex pixels+padding to be tagged\n");
                              printf("               $cwd/data2strings.txt:    complete byte-array (in 128 bit chunks) of hex pixels+padding to be tagged\n");
                              printf("----------------------------------------------------------------------------------------------------\n");
                              printf("Support: roland.philibert@onsemi.com\nUpdated by: mayur.verma@onsemi.com\n");
                              printf("----------------------------------------------------------------------------------------------------\n");
                              exit(0);

                      case 'f':
                              f = optarg;
                              fp = fopen(f, "r");
                              printf("\timage txt file:\t\t%s\n",f);
                             if(fp == NULL) {
                                  printf("Error opening file!\n");
                              }
                              ofp = fopen(outputFilename,"w");
                              ofp2 = fopen(outputFilename2,"w");
                              break;
                      case 'd':
                              dataformat = atoi(optarg);
                              printf("\tData-format [bits]:\t%d\n",dataformat);
                              switch (dataformat)
                              {
                                case 8:  {sft_top_embed = 0;  break;}
                                case 12: {sft_top_embed = 4;  break;}
                                case 16: {sft_top_embed = 8;  break;}
                                case 20: {sft_top_embed = 12; break;}
                                case 24: {sft_top_embed = 16; break;}
                              }
                              printf("\tShifting MSB-aligned embedded data by:\t%d bits\n",sft_top_embed);
                              break;
                      case 'p' :
                              compandformat = atoi(optarg);
                              printf("\tCompander Data-format:\t%d\n",compandformat);
                              break;                     
                      case 'm':
                              pmode = atoi(optarg);
                              printf("\tPacking mode:\t\t%d\n",pmode);
                              break;
                      case 'r':
                              paddingreg = strtoul(optarg, NULL, 16); 
                              printf("\tPacking date reg:\t0x%08x\n",paddingreg);
                              break;
                      case 'e':
                              top_embed_lines = atoi(optarg);
                              printf("\tnb top embedded lines:\t%d\n",top_embed_lines);
                              break;
                      case 'i':
                              iv = (unsigned char*)optarg;
                              printf("\tGiven iv string is:\t%s\n",iv);
                              char *piv;
                              piv = substring((char*)optarg,17,8);                               
                              piv32_0 = strtoul(piv, NULL, 16); 
                              if(verbose) printf  ("[DEBUG]\tpiv32_0:\t\t%0x\n",piv32_0);
                              free(piv);
                              piv = substring((char*)optarg,9,8);                               
                              piv32_1 = strtoul(piv, NULL, 16); 
                              if(verbose) printf  ("[DEBUG]\tpiv32_1:\t\t%0x\n",piv32_1);
                              free(piv);
                              piv = substring((char*)optarg,1,8);                               
                              piv32_2 = strtoul(piv, NULL, 16); 
                              if(verbose) printf  ("[DEBUG]\tpiv32_2:\t\t%0x\n",piv32_2);
                              free(piv);
                             
                              break;
                      case 'k':
                              key = (unsigned char*)optarg;
                              printf("\tGiven key string is:\t%s\n",key);
                              break;
                      case 's':
                              stat_lines = atoi(optarg);
                              printf("\tstat lines starts row:\t%d\n",stat_lines);
                              break;
                      case 'c':
                              cmac = 1;
                              printf(ANSI_COLOR_RED"\tCMAC "ANSI_COLOR_RESET"AES set:\t\t%d\n",cmac);
                              break;
                      case 'v':
                              verbose = 1;
                              printf("\tVerbosity set:\t%d\n",verbose);
                              break;
                      case 'g':
                              ignorefc = 1;
                              printf("\tIgnoring Frame count addition to iv!:\t%d\n",ignorefc);
                              break;
                      case 'y':
                              printf("\tcol ROI [first_last_skip]: %s\n",optarg);
                              char *colroi = strtok(optarg,"_");
                              firstcolroi = strtoul(colroi, NULL, 10);                         // base 10 if input is decimal, 16 if hex ...
                              printf ("\t\t->firstcolroi:\t%d\n",firstcolroi);
                              colroi = strtok(NULL,"_");                                       // get the subsecquent item in the arg
                              lastcolroi = strtoul(colroi, NULL, 10);  
                              printf ("\t\t->lastcolroi:\t%d\n",lastcolroi);
                              colroi = strtok(NULL,"_");                                       // get the subsecquent item in the arg
                              skipcolroi = strtoul(colroi, NULL, 10);  
                              printf ("\t\t->skipcolroi:\t%d\n",skipcolroi);
                              //free(colroi);
                              break;
 
                      case 'x':
                              printf("\trow ROI [first_last_skip]: %s\n",optarg);
                              
                              char *rowroi = strtok(optarg,"_");
                              firstrowroi = strtoul(rowroi, NULL, 10);                         // base 10 if input is decimal, 16 if hex ...
                              printf ("\t\t->firstrowroi:\t%d\n",firstrowroi);
                              rowroi = strtok(NULL,"_");                                       // get the subsecquent item in the arg
                              lastrowroi = strtoul(rowroi, NULL, 10);  
                              printf ("\t\t->lastrowroi:\t%d\n",lastrowroi);
                              rowroi = strtok(NULL,"_");                                       // get the subsecquent item in the arg
                              skiprowroi = strtoul(rowroi, NULL, 10);  
                              printf ("\t\t->skiprowroi:\t%d\n",skiprowroi);
                              break;
					  case 'L':
							  live_data_en = atoi(optarg);
							  if (live_data_en)
                                 printf("\tImage is with live data\t\n");
							  else
							     printf("\tImage is with test pattern\t\n");
                              break;
					  case 'E':
							  embedded_data_en = atoi(optarg);
							  if (embedded_data_en)
                                 printf("\tImage is with embedded data\t\n");
							  else
							     printf("\tImage is w/o embedded data\t\n");
                              break;
                      case -1:
                              break;
                      default:
                              printf(".....what the heck?!?!\n");
                              exit(0);
              } /* switch */
    } while (n_opt!=-1);


    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */


    printf("------------------------------------------------------------------\n");
    packmode = getoperation(dataformat,pmode);
    if(verbose) printf("[DEBUG]\t(internal packmode: %d)\n",packmode);
    printf("------------------------------------------------------------------\n");


    printf("------------------------------------------------------------------\n");
    if ( compandformat > 0) {
        get_shift_active_compand (dataformat,compandformat,&sft_act_compand,&padding_embed_compand);
        printf("\t- active data to vcore to shift by: %d\n",sft_act_compand);
        printf("\t- embedded data to vcore to shift by: %d & LSB masked: 0x%x\n",sft_act_compand,padding_embed_compand);
        printf("------------------------------------------------------------------\n");
    }

    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */

    printf("parsing pixels into dynamic array...\n");
    printf("\tdelimiter is tab (\\t)\n");
    //char delim[] = " \t\n";
    char delim[] = "\t";

    printf("\tnb of top embedded lines: %d\n\tbyte value (MSB aligned) shifted by %d bits\n",top_embed_lines,sft_top_embed);
    size_t read;
    size_t len = 0;
    char * line = NULL;
    int colnb = 0;
    int rownb = 0;
    int macrow = 0;
    int index = 0;
	int loop = 0;
	int loop1 = 0;
	int loop2= 0;
	int loop3 = 0;
	int loop4 = 0;
	//int read_count = 0;

    pix *pixptr, *dbpix;                                     //declare pointer
    dbpix = (pix*) malloc(sizeof(pix));                      //allocate initial storage
    if(dbpix==NULL){
       printf("No Memory \n ");
    }
    if(f) {
		printf("\t***********sft_act_compand: %0x, padding_embed_compand:%0x ************\n",  sft_act_compand, padding_embed_compand);
        //--------------------------------------
       // while ((read = getline(&line, &len, fp)) != -1) {
        while ((read = getline(&line, &len, fp)) > 15) {      // this should then finish when seeing CSV_FOOTER...
            //if(verbose) printf("\t%zu characters were read by getline().\n",read);
            colnb = 0;
			/*read_count ++; 
			if ((read_count > 4) && (read_count <= 6)) {
			   printf("rownb = %d", rownb);
			   continue;	
			}
			else */ if (1) {
			
            char *ptr = strtok(line, delim);                                    // get the first item in the line 
            //printf("line:%s, first item l:%d), %c%c%c....\n",line,strlen(ptr),ptr[0],ptr[1],ptr[2]);
			while(ptr != NULL)
            {                
                unsigned long n = strtoul(ptr, NULL, 10);                       // base 10 if input is decimal, 16 if hex ...
               // printf("prt before strtok of NULL: %s, n:%d\n",ptr,n);
                ptr = strtok(NULL,delim);                                       // get the subsecquent item in the line, until null
               // printf("prt after  strtok of NULL: %s, n:%d\n-------\n",ptr,n);
				dbpix=realloc(dbpix,(index+1)*sizeof(pix));                     // dynamic allocation: increase database by one for next round if any
				pixptr = (dbpix+index);                                         // position pointer to database by current pixel count
                //printf("index:%d", index);
                pixptr->index=index;                                            // assigning value to pointer, hence array of pix type
                pixptr->col=colnb;
                pixptr->row=rownb;
                pixptr->val=(unsigned int)n;

	            //printf("compandformat ----> %d" , compandformat);
                if (live_data_en) { // Ashu: for Live video               
                    if ((compandformat > 0) && (rownb >= top_embed_lines) && (rownb < stat_lines) ) { 
					        pixptr->val = (pixptr->val << sft_act_compand); 
					        // print this ACTIVE data
					        if (loop < 8 && (rownb > top_embed_lines)) { printf("LiveData index:%d [DEBUG} rownb:%0d, pixptr->val: %06x, pixptr->val << sft_act_compand: %06x\n",   
                                  index, rownb,pixptr->val,(pixptr->val << sft_act_compand));
						          loop++;
					        }
				    }
                    else if ((compandformat > 0) && ((rownb <  top_embed_lines) || (rownb >= stat_lines)) ) {                                                                     // this TOP & BOTTOM(STAT) EMBEDDED data                  
                             if(loop1 < 15 /*&& (rownb > (top_embed_lines-2))*/) printf("LiveDataEmbd index:%d [DEBUG} rownb:%0d, pixptr->val: %06x, pixptr->val << sft_act_compand: %06x, + padding_embed_compand(%0x):%06x\n",   
                                  index, rownb,pixptr->val,(pixptr->val << sft_act_compand),padding_embed_compand,(pixptr->val << sft_act_compand) + padding_embed_compand);
                             pixptr->val = (pixptr->val << sft_act_compand) + padding_embed_compand; 
							 loop1++;
                    }
			    }
                else { // For test pattern
	                if (embedded_data_en) { //with embedded data
                        if ((compandformat > 0) && (rownb >= top_embed_lines) && (rownb < top_embed_lines+2) ) {
							pixptr->val = (pixptr->val << sft_act_compand); //TBR Ashu                         // this ACTIVE data
							if(loop2 < 6) { printf("TestPattern statsrow index:%d [DEBUG} rownb:%0d, pixptr->val: %06x, pixptr->val << sft_act_compand: %06x\n",   
                                  index, rownb,pixptr->val,(pixptr->val << sft_act_compand));
						          loop2++;
					        }
					    }
                        else if ((compandformat > 0) && (rownb >= top_embed_lines+2) && (rownb < stat_lines) ) {
							pixptr->val = (pixptr->val << sft_act_compand); //TBR Ashu                         // this ACTIVE data
							if(loop3 < 8) { printf("TestPattern data index:%d [DEBUG} rownb:%0d, pixptr->val: %06x, pixptr->val << sft_act_compand: %06x\n",   
                                  index, rownb,pixptr->val,(pixptr->val << sft_act_compand));
						          loop3++;
					        }
				        }
                        else if ((compandformat > 0) && ((rownb <  top_embed_lines) || (rownb >= stat_lines)) ) {                                                                     // this TOP & BOTTOM(STAT) EMBEDDED data                  
                               if(loop4 < 15) printf("TestPatternEmbd index:%d [DEBUG} rownb:%0d, pixptr->val: %06x, pixptr->val << sft_act_compand: %06x, + padding_embed_compand(%0x):%06x\n",   
                                        index, rownb,pixptr->val,(pixptr->val << sft_act_compand),padding_embed_compand,(pixptr->val << sft_act_compand) + padding_embed_compand);
                               pixptr->val = (pixptr->val << sft_act_compand) + padding_embed_compand; 
							   loop4++;
                        }
				    }
	                else { // w/o embd data
				         if ((compandformat > 0) && (rownb >= top_embed_lines/*+2*/) && (rownb < stat_lines) ) pixptr->val = (pixptr->val << sft_act_compand); //TBR for framecounter 0 with no embedded data
				         else if ((compandformat > 0) && ((rownb >= stat_lines)) ) { // this TOP & BOTTOM(STAT) EMBEDDED data 
                               if(loop3 < 15) printf("TestPatternW/oEmbd [DEBUG} rownb:%0d, pixptr->val: %06x, pixptr->val << sft_act_compand: %06x, + padding_embed_compand(%0x):%06x\n",   
                                    rownb,pixptr->val,(pixptr->val << sft_act_compand),padding_embed_compand,(pixptr->val << sft_act_compand) + padding_embed_compand);
                               pixptr->val = (pixptr->val << sft_act_compand) + padding_embed_compand; 
							   loop3++;
                         }
		            }
	            }
	            colnb++;
                index++;
				
            }
			
            rownb++;
            macrow = rownb-2;
			
            }
			if (rownb % 500 == 0)	printf("Parsing imagefile... rownb=%d\n", rownb);
        }
        fclose(fp);
		printf("macrow = %d\n", macrow);
    }
    int k=0;

    printf("------------------------------------------------------------------\n");
    printf("Writing out pixel database:\n");
    pixptr      = dbpix;                                                                                                 // reset pointer to first address of database
    
    //pix *roipixptr, *dbroipix;
	pix *dbroipix;
    dbroipix = (pix*) malloc(index*sizeof(pix));                                                                            //pessimistic but who cares!  
    //roipixptr   = dbroipix;                                                                                              // reset pointer to first address of database
    unsigned int roiindex= 0;
    printf("\t-------------------------------------------------------\n");
    if(cmac) {
                printf("\tParsing and reducing pix db for CMAC ROI\n");
                roiindex = roi_reduce_pixdb(index,dbpix,dbroipix,
                                            firstcolroi,lastcolroi,skipcolroi,
                                            firstrowroi,lastrowroi,skiprowroi,
                                            top_embed_lines,sft_top_embed,verbose);
                printf("\tnb of index in reduced pixdb: %d\n",roiindex);
             }
    
    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    //printf("\t\tfile:\t"ANSI_COLOR_GREEN"./data_from_image.txt\n"ANSI_COLOR_RESET);
    printf("\t\tfile:\tdata_from_image.txt\n");
    if(cmac) printpixdb (dbroipix,roiindex,"data_from_image.txt");
    else     printpixdb (dbpix,index, "data_from_image.txt");



    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    printf("------------------------------------------------------------------\n");
    char ivplusfc[24];
    if(!cmac) get_framecount_plus_iv(index,dbpix,piv32_0,piv32_1,piv32_2,iv,ivplusfc,ignorefc,sft_top_embed,embedded_data_en);
    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    get_mac_from_sensor(index,dbpix,macrow,sft_top_embed,sensormac);

    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    printf("------------------------------------------------------------------\n");
    printf("Packing padding data...\n");

    int padded_index = index*2; // pessimistic: TODO need a better way
    int sample = 0;
    // ----------------------------------------------------------------------------------------
    printf ("\ttotal number of rows:        %d\n",rownb);
    printf ("\ttotal number of pixels:      %d\n",index);
    printf ("\ttotal number of pixels+pad:  %d (pessimitic allocation)\n",padded_index);
    
    // Commented by Ashutosh due to Segmentation fault error for full size image data  ---------------------------------------------------------------------------
	/*unsigned char data2tag_string[padded_index*6] ; //has to be number of characters here, *6 should do it, should be more precise
	  unsigned char data2tag_hexarr[padded_index*6] ;*/
	
	unsigned char *data2tag_string;
	data2tag_string = (unsigned char*) malloc(padded_index*6);
	if(data2tag_string == NULL){
       printf("No Memory data2tag_string\n ");
    }
	printf("------------------------------------------------------------------\n");
	unsigned char *data2tag_hexarr;
	data2tag_hexarr = (unsigned char*) malloc(padded_index*6);
	if(data2tag_hexarr == NULL){
       printf("No Memory data2tag_hexarr\n ");
    }

    // ---------------------------------------------------------------------------
    printf("------------------------------------------------------------------\n");
    printf("Processing inputs for openssl APIs...\n");
    printf ("\tcreated string and byte-array placeholders..\n");
	
    unsigned char * tmpdata;
    tmpdata = data2tag_string;

    unsigned int tmpindex;
    if(cmac) {
                pixptr = dbroipix;                          // reset pointer to first address of ROI pix database
                tmpindex = roiindex;
    }
    else    {
                pixptr = dbpix;                             // reset pointer to first address of database
                tmpindex = index;                      // if ful frame we do not take into account CMAC row
    }
    
    printf ("\tmax index is %d\n",tmpindex);
    unsigned int a,b,c;
    unsigned int p;
    unsigned int msg = 0;
    unsigned int evenpix = 0;
	printf("org tmpdata=%p", tmpdata);
    for (k=0; k<tmpindex;k++) {                 
        if (pixptr->row < macrow) {     // IGNORE last 2 MAC rows (not part of calculations)
			if (k<5 || (k > 8207 && k < 8215)) /*(k < 15)*/	printf("\n***Row:%d, Pixel val=%0x ***", pixptr->row, pixptr->val);
            a=((pixptr->val)    &0xFF); //extract first byte
            b=((pixptr->val>>8) &0xFF); //extract second byte
            c=((pixptr->val>>16)&0xFF); //extract third byte
			if (k<5 || (k > 8207 && k < 8215))/*(k < 15)*/	printf("a=%02x, b=%02x, c=%02x", a, b ,c);
            if ((packmode == mode0_8)||(packmode == mode1_8)) {          //no paccking required for eiher modes
                sprintf((char*)tmpdata,"%02x",a);
                tmpdata = tmpdata+2;
                pixptr++;
            } else if (packmode == mode0_12) {

                sprintf((char*)tmpdata,"%01x",b&0xF);     // extract nibble only to make 12bits
                tmpdata = tmpdata+1;               // 

                sprintf((char*)tmpdata,"%02x",a);
                tmpdata = tmpdata+2;
                sample++;

                if(sample == 10) {                        //every 10 pixels padding is added
                    sample = 0;
                    sprintf((char*)tmpdata,"%02x",paddingreg);  // 8 bit padding pushed-in
                    tmpdata = tmpdata+2;
                }        
                pixptr++;

            } else if ((packmode == mode0_16)||(packmode == mode1_16))  {
                sprintf((char*)tmpdata,"%02x",b);
                tmpdata = tmpdata+2;

                sprintf((char*)tmpdata,"%02x",a);
                tmpdata = tmpdata+2;
                pixptr++;

            } else if (packmode == mode0_20) {
				if (k<5 || (k > 8207 && k < 8215))/*(k < 15)*/ printf(" tmpdata=%p", tmpdata);
                sprintf((char*)tmpdata,"%01x",c&0xF);
				if (k<5 || (k > 8207 && k < 8215))/*(k < 15)*/ printf("\ttmpdata_c=%p", tmpdata);
                tmpdata = tmpdata+1;
				
                sprintf((char*)tmpdata,"%02x",b);
                if (k<5 || (k > 8207 && k < 8215))/*(k < 15)*/ printf("\ttmpdata_b=%p", tmpdata);
				tmpdata = tmpdata+2;
				
                sprintf((char*)tmpdata,"%02x",a);
                if (k<5 || (k > 8207 && k < 8215))/*(k < 15)*/ printf("\ttmpdata_a=%p\n", tmpdata);
				tmpdata = tmpdata+2;
				
                sample++;
                evenpix++;
				//printf("tmpdata final=%0x", tmpdata);

                if(sample == 6) {                        //every 6 pixels padding is added
                    sample = 0;
                    sprintf((char*)tmpdata,"%02x",paddingreg);  // 8 bit padding pushed-in
					if (k<5 || (k > 8207 && k < 8215))/*(k < 15)*/ printf("\tpaddingreg = %02x, tmpdata final=%p\n", paddingreg, tmpdata);
                    tmpdata = tmpdata+2;
                }
				//if (k < 5) printf("\ttmpdata =%06x", tmpdata);
				//getchar();
         
                if (!cmac && (compandformat > 0) && (evenpix==4)) {  // GMAC and companding, ignoring next pixel 
                    if(!msg) {
                        printf ("------------------------------------------------------------------\n"ANSI_COLOR_RED);
                        printf ("\t..ignoring next 4 adjacent pixels (1clk cycle) for GMAC & companding\n"ANSI_COLOR_RESET);
                    }
                    evenpix = 0;
                    pixptr = pixptr+1+4;
                    msg = 1; 
                }
                else {
                    pixptr++;
                }

            } else if (packmode == mode0_24) {
                sprintf((char*)tmpdata,"%02x",c);
                tmpdata = tmpdata+2;

                sprintf((char*)tmpdata,"%02x",b);
                tmpdata = tmpdata+2;

                sprintf((char*)tmpdata,"%02x",a);
                tmpdata = tmpdata+2;
                sample++;
                evenpix++;

                if(sample == 5) {                           //every 5 pixels padding is added
                    sample = 0;
                    sprintf((char*)tmpdata,"%02x",paddingreg);     // 8 bit padding pushed-in
                    tmpdata = tmpdata+2;
                }        
                if (!cmac && (compandformat > 0) && (evenpix==4)) {  // GMAC and companding, ignoring next pixel 
                    if(!msg) {
                        printf ("------------------------------------------------------------------\n"ANSI_COLOR_RED);
                        printf ("\t..ignoring next 4 adjacent pixels (1clk cycle) for GMAC & companding\n"ANSI_COLOR_RESET);
                    }
                    evenpix = 0;
                    pixptr = pixptr+1+4;
                    msg = 1;
                }
                else {
                    pixptr++;
                }

            } else if (packmode == mode1_12) {
                sprintf((char*)tmpdata,"%01x",b&0xF);
                tmpdata = tmpdata+1;

                sprintf((char*)tmpdata,"%02x",a);
                tmpdata = tmpdata+2;
                sample++;

                if(sample == 8) {                            //every 4 pixels padding is added
                    sample = 0;
                    for (p=0; p <4; p++) {
                        sprintf((char*)tmpdata,"%02x",paddingreg);  // 32 bits padding pushed-in
                        tmpdata = tmpdata+2;
                    }
                }        
                pixptr++;

            } else if (packmode == mode1_20) {
                sprintf((char*)tmpdata,"%01x",c&0xF);
                tmpdata = tmpdata+1;

                sprintf((char*)tmpdata,"%02x",b);
                tmpdata = tmpdata+2;

                sprintf((char*)tmpdata,"%02x",a);
                tmpdata = tmpdata+2;
                
				sample++;
				evenpix++;

                if(sample == 4) {                            //every 4 pixels padding is added
                    sample = 0;
                    for (p=0; p <6; p++) {
                        sprintf((char*)tmpdata,"%02x",paddingreg);  // 48 bits padding pushed-in
                        tmpdata = tmpdata+2;
                    }
                }        
                
				if (!cmac && (compandformat > 0) && (evenpix==4)) {  // GMAC and companding, ignoring next pixel 
                    if(!msg) {
                        printf ("------------------------------------------------------------------\n"ANSI_COLOR_RED);
                        printf ("\t..ignoring next 4 adjacent pixels (1clk cycle) for GMAC & companding\n"ANSI_COLOR_RESET);
                    }
                    evenpix = 0;
                    pixptr = pixptr+1+4;
                    msg = 1; 
                }
                else {
                    pixptr++;
                }
				
				
				
            } else if (packmode == mode1_24) {
                sprintf((char*)tmpdata,"%02x",c);
                tmpdata = tmpdata+2;

                sprintf((char*)tmpdata,"%02x",b);
                tmpdata = tmpdata+2;

                sprintf((char*)tmpdata,"%02x",a);
                tmpdata = tmpdata+2;
                sample++;

                if(sample == 4) {                            //every 4 pixels padding is added
                    sample = 0;
                    for (p=0; p <4; p++) {
                        sprintf((char*)tmpdata,"%02x",paddingreg);  // 32 bits padding pushed-in
                        tmpdata = tmpdata+2;
                    }
                }        
                pixptr++;
            } else {
                exit(0);
            }  
        }          
    }

    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    printf("\t-----------------------------------------------\n");
    printf("\tWriting out data string:\t"ANSI_COLOR_GREEN" ./data2string.txt"ANSI_COLOR_RESET" (includes padding)\n");

    fprintf(ofp, "%s\n", data2tag_string);
    fclose(ofp);
    printf("\t\t-> tsize data2tag_string: %d\n",(int)strlen((const char*)data2tag_string));
    int size_data2tag = asciitobytearray_faster(data2tag_string,data2tag_hexarr);
    printf("\tWriting out data byte-array:\t"ANSI_COLOR_GREEN" ./data2bytearray.txt"ANSI_COLOR_RESET" \n\t\t(in 128-bits chunks including padding)\n");
    printbytearray2file(data2tag_hexarr,"byte_array",size_data2tag,ofp2);
	

    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    //unsigned char iv_hexarr[strlen(ivplusfc)/2] ;  //On windows stack getting overriten so used malloc
	int i;
	unsigned char *iv_hexarr = (unsigned char*) malloc(strlen(ivplusfc)/2) ;
	int size_iv;
	if(cmac == 0) { //For GMAC only consider IV + Frame count ; if condition done by Mayur
		printf("\t-----------------------------------------------\n");
		printf("\tprocessing ivplusfc (%s) string (of length: %d) to byte-array..\n",(const char*)ivplusfc,(int)strlen((const char*)ivplusfc));
		size_iv =  asciitobytearray_faster((unsigned char *)ivplusfc,iv_hexarr);
		if(verbose) {
			unsigned char iv_back2string[strlen(ivplusfc)];
			bytearraytoascii(iv_hexarr,iv_back2string,size_iv,"iv_array");
			printf("[DEBUG] "); 
			comparestrings(iv_back2string,(unsigned char *)ivplusfc,1);
		}
		#if 0  //Enable for Debug purpose
		printf("ivplusfc ===>> ");
		for (i=0; i< size_iv; i++) {
			printf("%02x",ivplusfc[i]);
		}
		printf("\n");
		#endif
		printf("iv_hexarr ===>> ");
		for (i=0; i< size_iv; i++) {
			printf("%02x",iv_hexarr[i]);
		} 
	}
    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    printf("\n\t-----------------------------------------------\n");
	//unsigned char key_hexarr[strlen((const char *)key)/2] ;
	unsigned char *key_hexarr = (unsigned char*) malloc(strlen((const char *)key)/2) ;
    printf("\tprocessing key (%s) string to byte-array..\n",key);
    int size_key = asciitobytearray_faster(key,key_hexarr);
    if(verbose) {
        unsigned char key_back2string[strlen((const char *)key)];
        bytearraytoascii(key_hexarr,key_back2string,size_key,"key_array");
        printf("[DEBUG] "); 
        comparestrings(key_back2string,key,1);
    }
	printf("key_hexarr ===>> ");
    for (i=0; i< size_key; i++) {
	    printf("%02x",key_hexarr[i]);
	}
    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    printf("\n\t-----------------------------------------------\n");
	unsigned char *ptstr  		= (unsigned char *)"";
    printf("\tempty plain-text for auth only..\n");
	unsigned char pth[strlen((const char *)ptstr)/2] ;
	int ptl = asciitobytearray_faster(ptstr,pth);
	if(verbose) {
        unsigned char cmpptstr[ptl*2];
	    bytearraytoascii(pth,cmpptstr,ptl,"pt");
        printf("[DEBUG] "); 
	    comparestrings(cmpptstr,ptstr,1);
    }
	#if 0 //For Debug purpose
	printf("pth ===>> ");
	for (i=0; i< ptl; i++) {
        printf("%02x",pth[i]);
    }
	#endif
    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
	int returnVal;
    if(cmac == 0) {    // GMAC
        EVP_CIPHER_CTX *ctx;
        int outlen;
        //unsigned char outbuf[1024];
		unsigned char *outbuf = (unsigned char*) malloc(1024);
        ctx = EVP_CIPHER_CTX_new();
        printf ("\n\t->Cipher set to GCM AES 128\n");
        int ret = EVP_EncryptInit_ex(ctx, EVP_aes_128_gcm(), NULL, NULL, NULL);
        if (ret != 1) {
			printf ("\tEVP_EncryptInit_ex fails forEVP_aes_128_gcm()");
        }			

        /* Set IV length if default 96 bits is not appropriate */
        printf ("\tsetting IV length to %d \n", size_iv);
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN /*EVP_CTRL_AEAD_SET_IVLEN*/, size_iv, NULL);  //mayur

        /* Initialise key and IV */
        printf ("\tinitialise key and IV\n");
        //printf ("\tkey_hexarr: %s, \tiv_hexarr: %s\n", key_hexarr, iv_hexarr);
        printf("\tkey_hexarr ===>> ");
        for (i=0; i< size_key; i++) {
            printf("%02x",key_hexarr[i]);
        }
        printf("\n\tiv_hexarr ===>> ");
        for (i=0; i< size_iv; i++) {
            printf("%02x",iv_hexarr[i]);
        }
		printf("\n");
        ret = EVP_EncryptInit_ex(ctx, NULL, NULL, key_hexarr, iv_hexarr);
        if (ret != 1) {
			printf ("\tEVP_EncryptInit_ex fails for key_hexarr");
        }

        printf ("\tencrypting..\n");
        ret = EVP_EncryptUpdate(ctx, NULL, &outlen, data2tag_hexarr, size_data2tag);
		if (ret != 1) {
			printf ("\tEVP_EncryptUpdate fails for data2tag_hexarr");
        }
		printf ("\tsize_data2tag: %d, \toutlen: %d\n", size_data2tag, outlen);
		#if 0 //For Debug purpose
        printf("data2tag_hexarr ===>> ");
		for (i=0; i< size_data2tag; i++) {
            printf("%02x",data2tag_hexarr[i]);
        }
		print("\n");
		#endif
        ret = EVP_EncryptUpdate(ctx, outbuf, &outlen, pth, ptl);
		if (ret != 1) {
			printf ("\tEVP_EncryptUpdate fails for outbuf");
        }
		printf ("\toutbuf: %s, \toutlen: %d, \tptl: %d, \tpth: %s\n", outbuf, outlen, ptl, pth);
		#if 0 //For Debug purpose
        printf("pth ===>> ");
		for (i=0; i< ptl; i++) {
            printf("%02x",pth[i]);
        }
        printf("\noutbuf ===>> ");
		for (i=0; i< outlen; i++) {
            printf("%02x",outbuf[i]);
        }
        print("\n");
        #endif		

        int size_enct = outlen;
        unsigned char gcmctstr[] = {};
        /* ---------------------- */
        /* Output encrypted block */
        /* ---------------------- */
        printf ("\tfinalising..\n");
        ret = EVP_EncryptFinal_ex(ctx, outbuf, &outlen);
		if (ret != 1) {
			printf ("\tEVP_EncryptFinal_ex fails for outbuf");
        }
        bytearraytoascii(outbuf,gcmctstr,size_enct,"ct");  //TODO: verify why this crahses on full image
		#if 0 //For Debug purpose
        printf("\toutbuf: '%s' \n",outbuf);
        printf("\noutbuf ===>> ");
		for (i=0; i< size_enct; i++) {
            printf("%02x",outbuf[i]);
        }
        printf("\n\tgcmctstr: '%s' \tsize_enct: %d\n",gcmctstr, size_enct);
        printf("\ngcmctstr ===>> ");
		for (i=0; i< size_enct; i++) {
            printf("%02x",gcmctstr[i]);
        }
		printf("\n");
		#endif
        /* ------- */
        /* Get tag */
        /* ------- */
        printf ("\tgetting tag ...\n");
        EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG/*EVP_CTRL_AEAD_GET_TAG*/, 16, outbuf);
		printf("\ttag: '%s' \n",outbuf);
		#if 0 //For Debug purpose
        printf("\n\toutbuf ===>> ");
		for (i=0; i< 16; i++) {
            printf("%02x",outbuf[i]);
        }
		printf("\n");
		#endif
	    unsigned char gmactagstr[128/8];
        bytearraytoascii(outbuf,gmactagstr,16,"tag");
        printf("\n--------------------------------------------------------------------------\n");
        printf ("AES-GCM (GMAC) Tag calculated:\t\t'%s'\n",gmactagstr);
        printf("--------------------------------------------------------------------------\n");
        printf("--------------------------------------------------------------------------\n");
        printf ("AES-GCM (GMAC) Sensor Mac Extracted:\t'%s'\n",sensormac);
        printf("--------------------------------------------------------------------------\n");
        returnVal = comparestrings(gmactagstr,sensormac,2);
		free(outbuf);
    } 
	else {
        printf("\t->Cipher set to CBC AES 128\n");
	    size_t mactlen;
        unsigned char mact[128/2] ;

        CMAC_CTX *ctxm = CMAC_CTX_new();
        /* Set cipher type and mode */
        printf ("\tinitialise key and aes-cbc\n");
        CMAC_Init(ctxm, key_hexarr,16, EVP_aes_128_cbc(), NULL);
        printf("\ttagging..\n");
        CMAC_Update(ctxm, data2tag_hexarr, size_data2tag);
        printf ("\tfinalising and getting tag..\n");
        CMAC_Final(ctxm, mact, &mactlen);
	    unsigned char mactstr[mactlen*2];
        //printf("%02x\n", mact[0]);
        //printf("%02x\n", mact[1]);

	    bytearraytoascii(mact,mactstr,mactlen,"Generated tag");
        printf("----------------------------------------------------------------\n");
        printf ("AES-CBC (CMAC) Tag calculated:\t\t'%s'\n",mactstr);
        printf("----------------------------------------------------------------\n");
        printf("--------------------------------------------------------------------------\n");
        printf ("AES-CBC (CMAC) Sensor Mac Extracted:\t'%s'\n",sensormac);
        printf("--------------------------------------------------------------------------\n");
        returnVal = comparestrings(mactstr,sensormac,2);

    }

    /* ---------------------------------------------------------------------------------------------------------------------------------------------------------- */
    free(data2tag_hexarr);
	free(data2tag_string);
	free(iv_hexarr);
	free(key_hexarr);
    return(returnVal);
}


#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_STR_LEN 256

typedef struct parameter {
   char type[20];
   char name[20];
} param;


typedef struct function {
   char name[20];
   char rettype[20];
   param args[20];
   int ifcnt;
   int argcnt;
} func;

typedef struct tracenode {
   int FuncDef;
    int FuncDecl;
   int body;
    int block_items;
    int decl;
    int TypeDecl;
    int type0;
    int args;
    int params;
    int type1;
    int type2;
    int type3;
    int type4;
    int Decl;
    int IdentifierType;
   int PtrDecl;
} tracenode;

int preprocess();
void rec();
int check(char *input, int type);
void debugging();
void reset();

char *trim_left(char *str);
char *trim_right(char *str);
char *trim(char *str);

FILE *file;

func fuarr[1000];

char *key[] = {"\"_nodetype\"", "\"body\": ", "\"block_items\"", "\"decl\": ", "\"type\"", "\"args\"", "\"params\"", "\"declname\"", "\"names\""};
char *nodetype[] = {"\"If\"", "FuncDef", "\"FuncDecl\"", "\"Decl\"", "\"TypeDecl\"", "\"IdentifierType\"", "\"PtrDecl\""};

char dl = '{', dr = '}'; 

//funcnt : number of functions - 1 : index of fuarr
int funcnt = 0;
char line[MAX_STR_LEN];

tracenode trc;

int idx;
int arg;      // number of parameters

int debug;

// temporary save the info of function
func tmp;

int main(void)
{
   int i, j;

   tmp.ifcnt = 0;
   
   file = fopen("target.txt", "r");
   if(file == NULL) {
      printf("Txt file is not found.\nConvert json file to txt file.\n");
      if(preprocess() == 1) {
         remove("target.txt");
         return 1;
      }
      file = fopen("target.txt", "r");
   }

   printf("Success to open text file.\n");

   fgets(line, sizeof(line), file);
   
   rec();
   
   fclose(file);
   
   printf("============================== End ==============================\n");
   for(i = 0; i < funcnt; i++) {
      printf(" %2d. %s %s(", i+1, fuarr[i].rettype, fuarr[i].name);
      if(fuarr[i].argcnt > 0) printf("%s %s", fuarr[i].args[0].type, fuarr[i].args[0].name);
      for(j = 1; j < fuarr[i].argcnt; j++) {
         printf(", %s %s", fuarr[i].args[j].type, fuarr[i].args[j].name);
      }
      printf(")\n\t : has %d parameters and %d if statements\n", fuarr[i].argcnt, fuarr[i].ifcnt);
   }
   printf("\nThe number of functions : %d\n", funcnt);
   
   
    return 0;
}

void rec()
{
   /*
      depth : { -> ++, } -> --
   */
   char *tmpline;
   char quat = '"';
   int i, j = 0, isPointer, isMulti;
   int depth = 1;
   int index = 0;
   int brk;
   

   while(feof(file) == 0) {
      fgets(line, sizeof(line), file);
      idx = check(line, 1);
        if((trc.FuncDef == 0) && (check(line, 0) != 1)) {
           if(strchr(line, dl) != NULL) {
            depth += 1;
         }
         if(strchr(line, dr) != NULL) {
            if(trc.args == depth) trc.args = 0;
               if(trc.block_items == depth) trc.block_items = 0;
               if(trc.body == depth) trc.body = 0;
               if(trc.Decl == depth) trc.Decl = 0;
               if(trc.decl == depth) trc.decl = 0;
               if(trc.FuncDecl == depth) trc.FuncDecl = 0;
               if(trc.FuncDef == depth) trc.FuncDef = 0;
               if(trc.IdentifierType == depth) trc.IdentifierType = 0;
               if(trc.params == depth) trc.params = 0;
               if(trc.type0 == depth) trc.type0 = 0;
               if(trc.type1 == depth) trc.type1 = 0;
               if(trc.type2 == depth) trc.type2 = 0;
               if(trc.type3 == depth) trc.type3 = 0;
               if(trc.type4 == depth) trc.type4 = 0;
               if(trc.TypeDecl == depth) trc.TypeDecl = 0;
               depth--;
         }
           
         continue;
      }

      switch (idx)
      {
         case 0:
            //printf("case 0\n");
            idx = check(line, 0);
            switch (idx)
            {
               case 0:     // If  statement
                  //printf("case 00\n");
                  if(trc.block_items != 0) tmp.ifcnt += 1;
                  break;
               case 1:     // FuncDef
                  //printf("case 01\n");
                        trc.FuncDef = depth;
                  break;
               case 2:     // FuncDecl
                  //printf("case 02\n");
                  if((trc.type0 == depth-1) && (trc.FuncDecl == 0)) trc.FuncDecl = depth;
                  break;
               case 3:     // Decl
                  //printf("case 03\n");
                  if((trc.params == depth-1) && (trc.Decl == 0)) trc.Decl = depth;
                  break;
               case 4:     // TypeDecl
                  //printf("case 04\n");
                  if(((trc.type3 == depth-1) || (trc.PtrDecl == depth - 1)) && (trc.TypeDecl == 0)) trc.TypeDecl = depth;
                        else if(((trc.type1 == depth-1) || (trc.PtrDecl == depth - 1)) && (trc.TypeDecl == 0)) trc.TypeDecl = depth;
                  break;
               case 5:     // IdentifierType
                  //printf("case 05\n");
                  if((trc.type4 == depth-1) && (trc.IdentifierType == 0)) trc.IdentifierType = depth;
                        else if((trc.type2 == depth-1) && (trc.IdentifierType == 0)) trc.IdentifierType = depth;
                  break;
               case 6:     // PtrDecl
                  //printf("case 06\n");
                  if((trc.type3 == depth-1) && (trc.PtrDecl == 0)) trc.PtrDecl = depth;
                        else if((trc.type1 == depth-1) && (trc.PtrDecl == 0)) trc.PtrDecl = depth;
                  isPointer = 1;
                  break;
               default:
                  break;
            }
            break;
         case 1:     // body
            //printf("case 1\n");
                if((trc.body == 0) && (trc.FuncDef == depth)) trc.body = depth;
            break;
         case 2:     // block_items
            //printf("case 2\n");
                if(trc.body != 0) trc.block_items = depth;
                break;
         case 3:     // decl
            //printf("case 3\n");
            if((trc.decl == 0) &&(trc.FuncDef == depth)) trc.decl = depth;
            break;
         case 4:     // type
            //printf("case 4\n");
                if((trc.type0 == 0) && (trc.FuncDef == depth-1)) trc.type0 = depth;
                if((trc.Decl == depth) && (trc.type3 == 0)) trc.type3 = depth;
                if((trc.params != 0) && (trc.TypeDecl == depth) && (trc.type4 == 0)) trc.type4 = depth;
                if((trc.FuncDecl == depth) && (trc.type1 == 0)) trc.type1 = depth;
                if((trc.TypeDecl == depth) && (trc.type2 == 0)) trc.type2 = depth;
            break;
         case 5:     // args
            //printf("case 5\n");
                if((trc.FuncDecl == depth) && (trc.args == 0) && (strstr(line, "null") == NULL)) trc.args = depth;
            break;
         case 6:     // params
            //printf("case 6\n");
                if((trc.args == depth-1) && (trc.params == 0)) trc.params = depth;
            break;
            case 7:     // declname
                //printf("case 7\n");
                //"declname": "", or "declname": null
                if((trc.type3 != 0) && (trc.TypeDecl == depth)) {
                    //name of value
                    tmpline = &(strstr(line, key[7])[11]);
                    if(strncmp(tmpline, "null", 4) == 0) {
                        strcpy(tmp.args[index].name, "\0");
                  break;
                    }
               tmpline = &(strchr(tmpline, quat)[1]);
                    tmpline = trim(tmpline);
                    if(strchr(tmpline, '\"') != NULL) *strchr(tmpline, '\"') = '\0';
               if((index != 0) && (strcmp(tmp.args[index-1].name, tmpline) == 0)) break;
                    strcpy(tmp.args[index].name, tmpline);               
                } else if((trc.type1 != 0) && (trc.TypeDecl == depth)) {
                    //name of function
                    tmpline = &(strstr(line, key[7])[11]);
                    if(strncmp(tmpline, "null", 4) == 0) {
                        strcpy(tmp.name, "");
                  break;
                    }
               tmpline = &(strchr(tmpline, quat)[1]);
                    tmpline = trim(tmpline);
                    if(strchr(tmpline, '\"') != NULL) *strchr(tmpline, '\"') = '\0';
               strcpy(tmp.name, tmpline);
                }
            break;
            case 8:     // names
                //printf("case 8\n");
                if((trc.type4 == depth-1) && (trc.IdentifierType == depth)) {
                    //type of value
                    fgets(line, sizeof(line), file);
                    debug += 1;
                    tmpline = &(strchr(line, quat)[1]);
               tmpline = trim(tmpline);
                    if(strchr(tmpline, '\"') != NULL) {
                  if(isPointer) *strchr(tmpline, '\"') = '*';
                  else *strchr(tmpline, '\"') = '\0';
               }
                    strcpy(tmp.args[index].type, tmpline);

               index++;
                } else if((trc.type2 == depth-1) && (trc.IdentifierType == depth)) {
                    //type of function
                    fgets(line, sizeof(line), file);
                    debug += 1;
                    tmpline = &(strchr(line, quat)[1]);
               tmpline = trim(tmpline);
                    if(strchr(tmpline, '\"') != NULL) {
                  if(isPointer) {
                     *strchr(tmpline, '\"') = '*';
                  } else {
                     *strchr(tmpline, '\"') = '\0';
                  }
               }
                    strcpy(tmp.rettype, tmpline);
                }
                fgets(line, sizeof(line), file);
                debug += 1;
            continue;
                break;
         default:
            break;
      }
   
      if(strchr(line, dl) != NULL) {
         depth += 1;
      }
      if(strchr(line, dr) != NULL) {
         if(trc.args == depth) trc.args = 0;
            if(trc.block_items == depth) trc.block_items = 0;
            if(trc.body == depth) trc.body = 0;
            if(trc.Decl == depth) trc.Decl = 0;
            if(trc.decl == depth) trc.decl = 0;
            if(trc.FuncDecl == depth) trc.FuncDecl = 0;
            if(trc.FuncDef == depth) trc.FuncDef = 0;
            if(trc.IdentifierType == depth) trc.IdentifierType = 0;
            if(trc.params == depth) {
            trc.params = 0;
            if(arg == 0) arg = index;
            index = 0;
         }
            if(trc.type0 == depth) trc.type0 = 0;
            if(trc.type1 == depth) trc.type1 = 0;
            if(trc.type2 == depth) trc.type2 = 0;
            if(trc.type3 == depth) trc.type3 = 0;
            if(trc.type4 == depth) trc.type4 = 0;
            if(trc.TypeDecl == depth) trc.TypeDecl = 0;
         if(trc.PtrDecl == depth) {
            trc.PtrDecl = 0;
            isPointer = 0;
         }
            depth--;
         
         if((depth == 2) && (strlen(tmp.name) != 0)) {
            fuarr[funcnt].ifcnt = tmp.ifcnt;
            strcpy(fuarr[funcnt].name, tmp.name);
            strcpy(fuarr[funcnt].rettype, tmp.rettype);
            
            for(i = 0; i < arg; i++) {
               strcpy(fuarr[funcnt].args[i].name, tmp.args[i].name);
               strcpy(fuarr[funcnt].args[i].type, tmp.args[i].type);
            }
            fuarr[funcnt].argcnt = arg;                     
            funcnt++;
            arg = 0;
            reset();
         }
         
      }
   }
   return;
}

void reset()
{
   int i;
   for(i = 0; i < tmp.argcnt; i++) {
      strcpy(tmp.args[i].name, "\0");
      strcpy(tmp.args[i].type, "\0");
   }
   tmp.argcnt = 0;
   tmp.ifcnt = 0;
   strcpy(tmp.name, "");
   strcpy(tmp.rettype, "");
   
}

void debugging()
{
   int i, j;
   printf("=============== Debug =================\n");
   printf("%s %s(", tmp.rettype, tmp.name);
   if(arg > 0) printf("%s %s", tmp.args[0].type, tmp.args[0].name);
   for(j = 0; j < arg; j++) {
      printf(", %s %s", tmp.args[j].type, tmp.args[j].name);
   }
   printf(") : has %d if statements\n", tmp.ifcnt);

   printf("========= flags ==========\n");
   printf("trc.args : %d\ntrc.block_items : %d\ntrc.body : %d\ntrc.Decl : %d\ntrc.decl : %d\ntrc.FuncDecl : %d\ntrc.FuncDef : %d\ntrc.IdentifierType : %d\ntrc.params : %d\ntrc.type0 : %d\ntrc.type1 : %d\ntrc.type2 : %d\ntrc.type3 : %d\ntrc.type4 : %d\ntrc.TypeDecl : %d\n", trc.args, trc.block_items, trc.body, trc.Decl, trc.decl, trc.FuncDecl, trc.FuncDef, trc.IdentifierType, trc.params, trc.type0, trc.type1, trc.type2, trc.type3, trc.type4, trc.TypeDecl);
}

int check(char *input, int type)
{
   int i = 0, brk;
   //printf("incheck %s\n", input);
   if(type == 1) {
      //printf("in check 1 bp : ");
      //scanf("%d", &brk);
      for(i = 0; i < 9; i++) {
         if(strstr(input, key[i]) != NULL) return i;
      }
   } else {
      //printf("in check 0 bp : ");
      //scanf("%d", &brk);
      for(i = 0; i < 7; i++) {
         if(strstr(input, nodetype[i]) != NULL) return i;
      }
   }
   
   return -1;
}

int preprocess()
{
   char line[2];
   char lich[2];
   int i = 0;
   
   FILE *fp = fopen("ast.json", "rb");
   FILE *re = fopen("target.txt", "wb");

   if(fp == NULL){
      printf("fail to open target.json\n");
      fclose(fp);
      fclose(re);
      return 1;
   } else {
      printf("success to open target.json\n");
   }
   
   if(re == NULL){
      printf("fail to open target.txt\n");
      fclose(fp);
      fclose(re);
      return 1;
   } else {
      printf("success to open target.txt\n");
   }

   while (fgets(line, sizeof(line), fp) != NULL ) {
      if((line[0] >= 32) && (line[0] <= 126)) fputs(line, re);
      else if(line[0] == '\n') fputs(line, re);

   }
   fclose(fp);
   fclose(re);
   
   return 0;
}

char *trim_left(char *str) {
    while (*str) {
        if (isspace(*str)) {
            str++;
        } else {
            break;
        }
    }
    return str;
}

char *trim_right(char *str) {
    int len = (int)strlen(str) - 1;

    while (len >= 0) {
        if (isspace(*(str + len))) {
            len--;
        } else {
            break;
        }
    }
    *(str + ++len) = '\0';
    return str;
}

char *trim(char *str) {
    return trim_left(trim_right(str));
}

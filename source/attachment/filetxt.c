#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <dirent.h>
#include <unistd.h>
#include <vector>
#include <iostream>


using namespace std;

//struct dirent
//{
//   long d_ino; /* inode number 索引节点号 */
//   off_t d_off; /* offset to this dirent 在目录文件中的偏移 */
//   unsigned short d_reclen; /* length of this d_name 文件名长 */
//   unsigned char d_type; /* the type of d_name 文件类型 */
//   char d_name [NAME_MAX+1]; /* file name (null-terminated) 文件名，最长255字符 */
//}

vector < string > readFileList(char *basePath)
{
    DIR *dir;
    struct dirent *ptr;
    char base[1000];
    char temp[1000];
    vector<string> str;
    if ((dir=opendir(basePath)) == NULL)
    {
        perror("Open dir error...");
        exit(1);
    }

    while ((ptr=readdir(dir)) != NULL)
    {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    ///current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8)    ///file
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
        {
            memset(temp,'\0',sizeof(temp));
            strcpy(temp,basePath);
            strcat(temp,"/");
            strcat(temp,ptr->d_name);
            strcat(temp,"\n");
            str.push_back(temp);
        }
        else if(ptr->d_type == 10)    ///link file
            //printf("d_name:%s/%s\n",basePath,ptr->d_name);
        {
            memset(temp,'\0',sizeof(temp));
            strcpy(temp,basePath);
            strcat(temp,"/");
            strcat(temp,ptr->d_name);
            strcat(temp,"\n");
            str.push_back(temp);
        }
        else if(ptr->d_type == 4)    ///dir
        {
            vector<string> x;
            int i;
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            x=readFileList(base);
            for(i=0;i<(int)x.size();i++) str.push_back(x[i]);
        }
    }
    closedir(dir);
    return str;
}

int main(void)
{
    DIR *dir;
    char basePath[1000];
    vector<string> str;
    int i;
    ofstream file;
    ///get the current absoulte path
    memset(basePath,'\0',sizeof(basePath));
    getcwd(basePath, 999);
    printf("the current dir is : %s\n",basePath);

    ///get the file list
//    memset(basePath,'\0',sizeof(basePath));
//    strcpy(basePath,"./XL");
    str=readFileList(basePath);
    file.open("t.txt");
    for(i=0;i<(int)str.size();i++){
        string te=str[i].c_str();
        string temp=te.substr(0,te.size()-1);
	if(temp.find(".jpg")==-1) continue;
        file << temp.substr(temp.find('/',temp.find(".jpg")-10)) << ' ' << temp[temp.find('/',temp.find("jpg")-10)+1] << endl;
    }
    file.close();
    return 0;
}

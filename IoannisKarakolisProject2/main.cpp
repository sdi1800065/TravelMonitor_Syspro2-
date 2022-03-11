#include "modules.h"
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <math.h>
#include <signal.h>


  static int fd;
  static int fd2;
  static int buffersize;
  static int bloomsize;


  static HTHash filetable=HTCreate(7867);//Initialize with a size /prime numbers are better as a size/the hashtable is dynamic so if it need more it will allocate by itself.
  static HTHash virustable=HTCreate(23);
  static HTHash countriestable=HTCreate(23);
  static std::string id,firstname,lastname,country,age,virus,vacinated,date;
  static std::string line;
  static int countriestracker=0;

template <typename Temp>    
void bufferwrite(int fd,int bufferSize,int sizeofelement,Temp send_element)
{
  int parts=0;
  if(bufferSize>=sizeofelement)parts=1;
  else parts=ceil((float)sizeofelement/(float)bufferSize);
  int i;
  if(write(fd,&parts,sizeof(int))==-1)
  {
    perror("Parting Write parts");
    exit(0);
  }
  int t=(sizeofelement/parts);
  if(write(fd,&t,sizeof(int))==-1)
  {
    perror("Parting Write parts");
    exit(0);
  }
  int lastpart=sizeofelement%parts;
  if(write(fd,&lastpart,sizeof(int))==-1)
  {
    perror("Parting Write parts");
    exit(0);
  }
  for(i=0;i<parts;i++)
  {
    if(write(fd,send_element+i*t,t)==-1)
    {
      perror("Parting Write");
      exit(0);
    }
  }
  if(lastpart!=0)
  {
    if(write(fd,send_element+i*t,lastpart)==-1)
    {
      perror("Parting Write");
      exit(0);
    }
  }
}

template<typename Temp>
void bufferread(int fd,int bufferSize,Temp temp)
{
  int parts;
  while(read(fd,&parts,sizeof(int))==-1)
  {
  //  perror("READ PARTING parts");
    //exit(0);
  }
  int partsize;
  while(read(fd,&partsize,sizeof(int))==-1)
  {
    //perror("READ PARTING partssize");
    //exit(0);
  }
  int lastpartsize;
  while(read(fd,&lastpartsize,sizeof(int))==-1)
  {
    //perror("READ PARTING partssize");
    //exit(0);
  }
  int i;
  memset(temp,0,partsize*parts+lastpartsize);
  for(i=0;i<parts;i++)
  {
    while(read(fd,temp+i*partsize,partsize)==-1)
    {
      //perror("READ PARTING Inside");
      //exit(0);
    }
  }
  if(lastpartsize!=0)
  {
    if(read(fd,temp+i*partsize,lastpartsize)==-1)
    {
      perror("READ PARTING Inside");
      exit(0);
    }
  }
}

inline bool exists (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

static char** countries=0;
static int countries_counter=0;

static char** already_inserted=0;
static int already_inserted_counter=0;
void catchinterrupt ( int signo ) {
    return;                                 //ignoring control c so it will be handled ccountrorrectly from the parent .
}
void catchsigurs ( int signo ) {

  int i,j;
  char*bf2=new char[200];
  for(i=0;i<countries_counter;i++)
  {
    memset(bf2,0,200);
    strcpy(bf2,countries[i]);
    struct dirent *lv;
    DIR *dirp;
    
    dirp = opendir(bf2);
    if (dirp == NULL) {
        perror("Directory Error2");
        exit(-2);
    }
    
    while ((lv = readdir(dirp))){
        if(strcmp(lv->d_name,"..")==0 || strcmp(lv->d_name,".")==0  )
        {
            continue;
        }
        int flaga=0;
        for(j=0;j<already_inserted_counter;j++)
        {
          if(strcmp(already_inserted[j],lv->d_name)==0)
          {
            flaga=1;
            break;
          }
        }
        
        if(flaga==1)continue;
        std::string inputfile(bf2);
        inputfile=inputfile+"/"+lv->d_name;
        std::ifstream myfile(inputfile);
        if(exists(inputfile)==0)
        {
            std::cout<<"ERROR inputfile doesnt exist \n";
            continue;
        }
        
        already_inserted=(char**) realloc(already_inserted, (already_inserted_counter + 1) * sizeof(*already_inserted));
        already_inserted[already_inserted_counter]=new char[100];
        strcpy(already_inserted[already_inserted_counter],lv->d_name);
        already_inserted_counter++;
        while (std::getline(myfile, line))
        {
            
            std::istringstream iss(line);
            if (!(iss >>id>>firstname>>lastname>>country>>age>>virus>>vacinated)) { break; } // error

            if(vacinated=="NO")
            {
            if(iss>>date)
            {
                std::cout<<"ERROR IN RECORD "<<line<<" Date after NO\n";
                continue;
            }else date=" ";
            }else if(!(iss>>date)){std::cout<<"ERROR:EXPECTED DATE AFTER YES\n";continue;};
            insert_records_file(filetable,virustable,countriestable,bloomsize,id,firstname,lastname,country,age,virus,vacinated,date);
        }

        myfile.close();
    }
    closedir(dirp);
  }
    int a=virustable->size;
    for(i=0;i<a;i++)
    {
        Hashnode *templist=virustable[i].list;
        while(templist!=NULL)
        {
            bufferwrite(fd2,buffersize,sizeof(char)*strlen((templist->key).c_str()),(templist->key).c_str());

            Virus_structs* getbloom;
            getbloom=(Virus_structs*)templist->item;
            unsigned int* t=getbloom->bloom->getbm();
            for(int j=0;j<getbloom->bloom->getbms();j++)
                bufferwrite(fd2,buffersize,sizeof(unsigned int),&t[j]);
            templist=templist->Link;
        }
    }
    memset(bf2,0,100);
    strcpy(bf2,"DONEREADINGVIRUSES");
    bufferwrite(fd2,buffersize,sizeof(char)*strlen(bf2)+1,bf2);
  delete[] bf2;
}


int main(int argc, char** argv)
{
    static struct sigaction act ;
    act . sa_handler = catchinterrupt ;
    sigfillset (&( act . sa_mask ) ) ;
    sigaction ( SIGINT , & act , NULL ) ;
    sigaction ( SIGQUIT , & act , NULL ) ;

    static struct sigaction act2 ;
    act2 . sa_handler = catchsigurs ;
    sigfillset (&( act2 . sa_mask ) ) ;
    sigaction ( SIGUSR1 , & act2 , NULL ) ;


    int i;
    if(argc!=3)
    {
        std::cout<<"ERROR missing arguments or too many arguments \n";
        return 1;
    }
    char Pipe[20];
    char Piper[20];
    strcpy(Pipe,argv[1]);
    strcpy(Piper,argv[2]);
    if((fd=open(Pipe,O_RDONLY))==-1)
    {
        std::cout<<"ERROR OPENING PIPE\n";
        return -1;
    }
    int r;
    int r2;
    if(read(fd,&r,sizeof(int))==-1)
    {
        std::cout<<"ERROR READING FROM PIPE\n";
        return -1;
    }
    if(read(fd,&r2,sizeof(int))==-1)
    {
        std::cout<<"ERROR READING FROM PIPE\n";
        return -1;
    }
  buffersize=r;
  bloomsize =r2;
  
  char *bf=new char[100];
  while(1){
    
    memset(bf,0,100);
    bufferread(fd,buffersize,bf);
    if(strcmp(bf,"DONEREADINGFILES")==0)break;

    countries=(char**) realloc(countries, (countries_counter + 1) * sizeof(*countries));
    countries[countries_counter]=new char[100];
    strcpy(countries[countries_counter],bf);
    countries_counter++;
    struct dirent *lv;
    DIR *dirp;

    dirp = opendir(bf);
    if (dirp == NULL) {
        perror("Directory Error");
        exit(-2);
    }
    while ((lv = readdir(dirp))){
        if(strcmp(lv->d_name,"..")==0 || strcmp(lv->d_name,".")==0)
        {
            continue;
        }
        std::string inputfile(bf);
        inputfile=inputfile+"/"+lv->d_name;
        std::ifstream myfile(inputfile);
        if(exists(inputfile)==0)
        {
            std::cout<<"ERROR inputfile doesnt exist \n";
            continue;
        }
        already_inserted=(char**) realloc(already_inserted, (already_inserted_counter + 1) * sizeof(*already_inserted));
        already_inserted[already_inserted_counter]=new char[100];
        strcpy(already_inserted[already_inserted_counter],lv->d_name);
        already_inserted_counter++;
        while (std::getline(myfile, line))
        {
            
            std::istringstream iss(line);
            if (!(iss >>id>>firstname>>lastname>>country>>age>>virus>>vacinated)) { break; } // error

            if(vacinated=="NO")
            {
            if(iss>>date)
            {
                std::cout<<"ERROR IN RECORD "<<line<<" Date after NO\n";
                continue;
            }else date=" ";
            }else if(!(iss>>date)){std::cout<<"ERROR:EXPECTED DATE AFTER YES\n";continue;};
            insert_records_file(filetable,virustable,countriestable,bloomsize,id,firstname,lastname,country,age,virus,vacinated,date);
        }
        myfile.close();
    }
    closedir(dirp);
  }
  
  if((fd2=open(Piper,O_WRONLY))==-1)
  {
      perror("OPENING PIPE");
      return -1;
  }
    int a=virustable->size;
    for(i=0;i<a;i++)
    {
        Hashnode *templist=virustable[i].list;
        while(templist!=NULL)
        {
            bufferwrite(fd2,buffersize,sizeof(char)*strlen((templist->key).c_str()),(templist->key).c_str());
            Virus_structs* getbloom;
            getbloom=(Virus_structs*)templist->item;
            unsigned int* t=getbloom->bloom->getbm();
            for(int j=0;j<getbloom->bloom->getbms();j++)
                bufferwrite(fd2,buffersize,sizeof(unsigned int),&t[j]);


            templist=templist->Link;
        }
    }
    memset(bf,0,100);
    strcpy(bf,"DONEREADINGVIRUSES");
    bufferwrite(fd2,buffersize,sizeof(char)*strlen(bf)+1,bf);


  while(1)
  {    
    memset(bf,0,100);
    bufferread(fd,buffersize,bf);
      
    if(strcmp(bf,"exit")==0) //prepares the monitor to be killed , its not really neaded but valgrind gives errors withought it.
    {       
      close(fd);
      close(fd2);
      HTDestroy<records *>(filetable);
      HTDestroy<Virus_structs *>(virustable);
      HTDestroy<int*>(countriestable);//Doesnt matter what template you give cause item->data will be null for this case just need to delete pointers and key.
      delete[] bf; 
      pause();//waits to be killed by parent;
    }
    else if(strcmp(bf,"EVERYTHING")==0)
    {
      memset(bf,0,100);
      bufferread(fd,buffersize,bf);
           
      std::string citizenID(bf);
      void*recv;
      if(!HTGet(filetable,citizenID,&recv)){memset(bf,0,100);strcpy(bf,"NOSUCHID");bufferwrite(fd2,buffersize,sizeof(char)*strlen(bf),bf);continue;};
          
      records*rec=(records*)recv;
      int flaga=0;
      int i,a=virustable->size;
      int id=std::stoi(citizenID);
      for(i=0;i<a;i++)
      {
        Hashnode *templist=virustable[i].list;
        while(templist!=NULL)
        {   
          Virus_structs* getvirus;
          getvirus=(Virus_structs*)templist->item;
          listnode *temp;
                    
          if(temp=getvirus->vaccinated->search(id))
          {
            std::string sendstr(" VACCINATED ON ");
            sendstr=templist->key+sendstr+temp->data;
            memset(bf,0,100);
            strcpy(bf,sendstr.c_str());
            bufferwrite(fd2,buffersize,sizeof(char)*strlen(bf),bf);
            if(flaga==0)
            {
              char namelast[100];
              memset(namelast,0,100);
              strcpy(namelast,(rec->getnamelastname()).c_str());
              bufferwrite(fd2,buffersize,sizeof(char)*strlen(namelast),namelast);
              memset(namelast,0,100);
              strcpy(namelast,(std::to_string(rec->getage())).c_str());
              bufferwrite(fd2,buffersize,sizeof(char)*strlen(namelast)+1,namelast);
              flaga=1;
            }
          }
          else if(temp=getvirus->not_vaccinated->search(id))
          {
            std::string sendstr(" NOT YET VACCINATED");
            sendstr=templist->key+sendstr;
            memset(bf,0,100);
            strcpy(bf,sendstr.c_str());
            bufferwrite(fd2,buffersize,sizeof(char)*strlen(bf),bf);
            if(flaga==0)
            {
              char namelast[100];
              memset(namelast,0,100);
              strcpy(namelast,(rec->getnamelastname()).c_str());
              bufferwrite(fd2,buffersize,sizeof(char)*strlen(namelast),namelast);
              memset(namelast,0,100);
              strcpy(namelast,(std::to_string(rec->getage())).c_str());
              bufferwrite(fd2,buffersize,sizeof(char)*strlen(namelast)+1,namelast);
              flaga=1;
            }
          }
          templist=templist->Link;
        }
      }
      memset(bf,0,100);
      strcpy(bf,"NOSUCHID");
      bufferwrite(fd2,buffersize,sizeof(char)*strlen(bf),bf);
    }
    else if(strcmp(bf,"CHECKPOSITIVE")==0){
      memset(bf,0,100);
      bufferread(fd,buffersize,bf);
      std::string id(bf);
      memset(bf,0,100);
      bufferread(fd,buffersize,bf);
      std::string virusname(bf);
      void*getvoid;
      Virus_structs*getvirus;
      HTGet(virustable,virusname,&getvoid);      
      getvirus=(Virus_structs*)getvoid;
      listnode*temp;
      if(temp=getvirus->vaccinated->search(std::stoi(id))){
        memset(bf,0,100);
        strcpy(bf,"YES");
        bufferwrite(fd2,buffersize,sizeof(char*)*strlen(bf),bf);
        memset(bf,0,100);
        strcpy(bf,(temp->data).c_str());
        bufferwrite(fd2,buffersize,sizeof(char*)*strlen(bf),bf);
        continue;
      }
      else
      {
        memset(bf,0,100);
        strcpy(bf,"NO");
        bufferwrite(fd2,buffersize,sizeof(char*)*strlen(bf),bf);
        continue;
      }
      continue;
    }
    else
    {
      std::cout<<"WRONG INPUT GIVEN TO MONITORAKI\n";
    }
  }
}

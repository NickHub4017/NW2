
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <dirent.h>
#include <bits/errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/sendfile.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc , char *argv[])
{
    int  client_sock , read_size,server_socket_desc;
    struct sockaddr_in client;
    char client_message[500];
    char filerequested[200];
    char extensionoffile[10];
    char htmlext[]=".html";

    client_sock = waittoconnect(client,&server_socket_desc);
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
    puts("Connection accepted");

    //Receive a message from client
    if( (read_size = recv(client_sock , client_message , 750 , 0)) > 0 )
    {
        //Send the message back to client
        printf("readed %d\n",read_size);//Get the size of header
        puts(client_message);//Print header

        filerequested[0]=NULL;
        extensionoffile[0]=NULL;

        int x=getrequestfile(client_message,filerequested,extensionoffile);
        puts(filerequested);

        int isfileexsist=findfile(filerequested);
        if(isfileexsist>0) {

            if (strcmp(extensionoffile, htmlext) == 0) {//when it is html file
                sendfiletosocket(client_sock, filerequested);

            }
            else{
                sendfiletosocket(client_sock, filerequested);
            }
            printf("hiiii %d",isfileexsist);
            printf("file ext:- %s \n",extensionoffile);
        }
        else{
            writeheader(client_sock,0,NULL,404);
        }

       // write(client_sock , "<h1>hello world</h1>" , strlen("<h1>hello world</h1>"));

       //writeheader(client_sock,0,"text",404);




       // FILE *fp;
       // fp=fopen("/home/nrv/public/index.html", "r");
       // char ch;



        //FILE* fpsock = fdopen(client_sock, "w");
        //sendfile(client_sock, fp, 0, 1000);
        //ssize_t k=sendfile(fp, fp2, 0,100);
        //fclose(fp2);
        //printf("--> %d",k);
        close(client_sock);

        close(server_socket_desc);

    }

    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }

    return 0;
}

int sendfiletosocket(int desc,char file[]){
    struct stat stat_buf;
    /* open the file to be sent */
    char basedir[]="/home/nrv/public";
    char fullpath[strlen(basedir)+strlen(file)];

    strcpy(fullpath,basedir);
    strcpy(fullpath+strlen(fullpath),file);
    printf("sendfilesocket data fullpath %s",fullpath);
    int fd = open(fullpath, O_RDONLY);

    /* get the size of the file to be sent */
    fstat(fd, &stat_buf);


    /* copy file using sendfile */
    off_t offset = 0;
    int rc = sendfile (desc, fd, &offset, stat_buf.st_size);
    close(fd);


    return 0;
}
int phpread(){
    FILE *fp;
    char path[10];
    fp = popen("php -f /home/nrv/public/a.php", "r");
    while (fgets(path, sizeof(path)-1, fp) != NULL) {
        printf("%s", path);
    }
}


int waittoconnect(struct sockaddr_in client,int *server_socket_desc){
    int size_sockaddrin;
    struct sockaddr_in server;

    server_socket_desc=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket_desc==-1){
        return -1;//server socket create error
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8889 );

    int bindval=bind(server_socket_desc,(struct sockaddr *)&server , sizeof(server));
    if(bindval<0){
        return -2;//bind fails
    }

    listen(server_socket_desc , 3);
    size_sockaddrin = sizeof(struct sockaddr_in);
    int cleint_sock=accept(server_socket_desc, (struct sockaddr *)&client, (socklen_t*)&size_sockaddrin);
    if(cleint_sock<0){
        return -3;//accept fails
    }

    return cleint_sock;

}

 int getrequestfile(char header[] ,char *filerequested,char *extensionoffile){
    char *frstline = strtok (header,"\n");
    puts(frstline);
    char *frstword =strtok (frstline," ");
    char *secondword =strtok (NULL," ");
     char defultpage[]="/index.html";
     if(strcmp("/",secondword)==0){
         strcpy(filerequested,defultpage);
     }
     else {
         strcpy(filerequested, secondword);
     }
     puts(secondword);
     char temp_ext[5];
     int i=strlen(filerequested);
     int passlen=0;
     for (;i>0;i--){
         passlen=passlen+1;
         if(filerequested[i]=='.'){

             int j=0;
             for (j=0;j<passlen-1;j++){
                 temp_ext[j]=filerequested[i+j];
             }
             temp_ext[j]='\0';
             strcpy(extensionoffile,temp_ext);
             return 1;
         }
     }

    return 0;

}

int findfile(char orifname[]){
    FILE *fp;
    char ch;
    char fname[strlen(orifname-1)];
    strcpy(fname,orifname+1);
    DIR *dp;
    struct dirent *ep;
    dp = opendir ("/home/nrv/public/");
    if (dp != NULL) {

        while (ep = readdir(dp)){
            puts(ep->d_name);

        if (strcmp(ep->d_name, fname) == 0) {
            (void) closedir(dp);
            struct stat st;
            stat(ep->d_name, &st);
            printf("size %d",st.st_size);
            return st.st_size;
        }
    }
        (void) closedir (dp);
    }

    return 0;

}

int writeheader(int clientsock,int contentlen,char contenttype[],int responsecode){

    char end[]="\n";
   /* char head1[]="HTTP/1.0 400 Bad Request\n"
            "Content-Type: text/html\n"
            "Content-Length: 349\n"
            "Connection: close\n"
            "Date: Sun, 10 Apr 2016 02:59:42 GMT\n"
            "Server: ECSF (ams/4991)\n"
            "\n"
            "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?>\n"
            "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\"\n"
            "         \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n"
            "<html xmlns=\"http://www.w3.org/1999/xhtml\" xml:lang=\"en\" lang=\"en\">\n"
            "\t<head>\n"
            "\t\t<title>400 - Bad Request</title>\n"
            "\t</head>\n"
            "\t<body>\n"
            "\t\t<h1>400 - Bad Request</h1>\n"
            "\t</body>\n"
            "</html>\n";*/
    if(responsecode==200) {
        char statusres[] = "HTTP/1.1 200 OK\n";

        char contlen[20];
        char contenttype_first[]="Content-Type: ";
        sprintf(contlen, "Content-Length: %d\n", contentlen);
        write(clientsock , statusres , strlen(statusres));
        write(clientsock , contlen , strlen(contlen));

        write(contenttype_first , contlen , strlen(contenttype_first));
        write(contenttype , contlen , strlen(contenttype));
        write(end , contlen , strlen(end));
    }
    else {
        char statusres[] = "HTTP/1.1 404 Not Found\n";
        write(clientsock , statusres , strlen(statusres));
    }


    char connection[]="Connection: close\n";
    char dateoffile[]="Date: Sun, 10 Apr 2016 02:59:42 GMT\n";
    char server[]="Server: ECSF (ams/4991)\n";
    char newline[]="\n";
    write(clientsock , connection , strlen(connection));
    write(clientsock , dateoffile , strlen(dateoffile));
    write(clientsock , server , strlen(server));
    write(clientsock , newline , strlen(newline));

}

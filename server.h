//
// Created by nrv on 4/6/16.
//

#ifndef SERVER_SERVER_H
#define SERVER_SERVER_H

#endif //SERVER_SERVER_H

int sendfiletosocket(int desc,char fullpath[]);
int getrequestfile(char header[] ,char *filerequested,char *extensionoffile);
int findfile(char fullpath[]);
int writeheader(int clientsock,int contentlen,char contenttype[],int responsecode);
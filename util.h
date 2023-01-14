#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int CompareMagicBytes(unsigned char* cli, unsigned char* target);
int IPIsExist(char* ip);
int WriteIP(char* ip);
int GetIPs(char* ip, char** ips);

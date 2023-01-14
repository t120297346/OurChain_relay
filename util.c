#include "util.h"

int CompareMagicBytes(unsigned char* cli, unsigned char* target){
    return memcmp(cli, target, 4);
};

int IPIsExist(char* ip){
    char* line = NULL;
    size_t len = 0;
    FILE* ids = fopen("./ourchain_ips.txt", "r");

    if(ids == NULL) {
        printf("Error!\n");
        return -1;
    }
    
    while(getline(&line, &len, ids) != -1) {
        if(strcmp(ip, strtok(line, "\n")) == 0)
            return 1;
    }
    
    fclose(ids);
    return 0;
}

int WriteIP(char* ip){
    char* line = NULL;
    size_t len = 0;
    FILE* ids = fopen("./ourchain_ips.txt", "a+");
    
    if(ids == NULL) {
        printf("Error!\n");
        return -1;
    }

    fprintf(ids, "%s\n", ip);
    fclose(ids);
    return 1;
}

int GetIPs(char* ip, char** ips){
    char* line = NULL;
    size_t len = 0;
    FILE* ids = fopen("./ourchain_ips.txt", "r");
    int len_ips, len_line;

    if(ids == NULL) {
        printf("Error!\n");
        return -1;
    }
    
    while(getline(&line, &len, ids) != -1) {
        if(strcmp(ip, strtok(line, "\n")) == 0)
            continue;
        else {
            len_ips = strlen(*ips);
            len_line = strlen(line);
            char* temp = (char*) malloc(len_ips + len_line + 2);
            memcpy(temp, *ips, len_ips);
            memcpy(temp + len_ips, line, len_line);
            memcpy(temp + len_ips + len_line, ",", 1);
            *ips = (char*) malloc(strlen(temp) + 1);
            memcpy(*ips, temp, strlen(temp));
        }
    }
    return 1;
}





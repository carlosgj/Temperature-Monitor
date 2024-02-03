#include "datetime.h"

void unix2Datetime(uint32_t *unix, struct datetime_t *dt){
    struct tm* thistm;
    time_t t = (time_t)(*unix);
    thistm = gmtime(&t);
    
    //printf("\t%04d-%02d-%02d %02d:%02d:%02d\n", thistm->tm_year, thistm->tm_mon, thistm->tm_mday, thistm->tm_hour, thistm->tm_min, thistm->tm_sec);
    
    dt->year = 1900 + (uint16_t)thistm->tm_year;
    dt->month = 1 + (uint8_t)thistm->tm_mon;
    dt->dom = (uint8_t)thistm->tm_mday;
    dt->hour = (uint8_t)thistm->tm_hour;
    dt->minute = (uint8_t)thistm->tm_min;
    dt->second = (uint8_t)thistm->tm_sec;
}

void datetime2Unix(struct datetime_t *dt, uint32_t *unix){
    struct tm thistm;
    memset(&thistm, 0, sizeof(struct tm));
    
    thistm.tm_year = (int16_t)(dt->year - 1900);
    thistm.tm_mon = dt->month - 1;
    thistm.tm_mday = dt->dom;
    thistm.tm_hour = dt->hour;
    thistm.tm_min = dt->minute;
    thistm.tm_sec = dt->second;
    
    //printf("\t%04d-%02d-%02d %02d:%02d:%02d\n", thistm->tm_year, thistm->tm_mon, thistm->tm_mday, thistm->tm_hour, thistm->tm_min, thistm->tm_sec);
    
    *unix = (uint32_t)mktime(&thistm);
}

void datetime_tests(void){
    printf("Running datetime routine tests...\n");
    
    uint8_t i,j;
    printf("\tMonth names:");
    for(i=0; i<12; i++){
        for(j=0; j<3; j++){
            printf("%c", MONTHNAMES[i][j]);
        }
        printf(" ");
    }
    printf("\n");
    
    printf("\tDoW names:");
    for(i=0; i<8; i++){
        for(j=0; j<3; j++){
            printf("%c", DAYNAMES[i][j]);
        }
        printf(" ");
    }
    printf("\n");
    
    struct datetime_t dt;
    uint32_t unix = 0;
    unix2Datetime(&unix, &dt);
    printf("\tEpoch start: %04d-%02d-%02d %02d:%02d:%02d\n", dt.year, dt.month, dt.dom, dt.hour, dt.minute, dt.second);
    if(dt.year != 1970 || 
       dt.month != 1 ||
       dt.dom != 1 ||
       dt.hour != 0 ||
       dt.minute != 0 ||
       dt.second != 0){
        printf("\tIncorrect epoch start!\n");
    }
    
    memset(&dt, 0, sizeof(struct datetime_t));
    
    dt.year = 2024;
    dt.month = 2;
    dt.dom = 3;
    dt.hour = 4;
    dt.minute = 5;
    dt.second = 6;
    
    datetime2Unix(&dt, &unix);
    //Should be 1706933106
    printf("\tUnix timestamp: %lu\n", unix);
    if(unix != 1706933106L){
        printf("\tIncorrect Unix timestamp; s.b. 1706933106\n");
    }
    
    
    printf("Datetime tests done.\n\n");
}
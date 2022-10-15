
#define L2CacheSize 1000; //se MB

int TableFitsCache(int cacheSize, int tableSize){
    
    if(tableSize>0 && cacheSize*1024*1024>=tableSize*sizeof(int))
        return 1;
    else
        return 0;
};

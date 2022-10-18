int TableFitsCache(int cacheSize, int tableSize){
    
    if(tableSize>0 && cacheSize*1024*1024>=tableSize*sizeof(struct tuple))
        return 1;
    else
        return 0;
};

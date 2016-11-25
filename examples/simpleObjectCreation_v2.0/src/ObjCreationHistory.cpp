

/*******************************************************************************
 *                                                                             *
 *                                                                             *
 *        Simple concurrent tag, class and time of creation register           *
 *                                                                             *
 * Adds a (tag,class,time,thread) entry for each object creation with simple   *
 * concurrency support                                                         *
 *                                                                             *
 *                                                                             *
 * ****************************************************************************/


#include <ObjCreationHistory.hpp>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <libkern/OSAtomic.h>
#include <iostream>
#include <cmath>
#define START_ENTRIES 4

ObjCreationHistory::_threadLoadEntry::
_threadLoadEntry(uint32_t ts, const char *cn)
{
    timestamp = ts;
    if(cn){
        className = (char *)malloc(strlen(cn)+1);
        strcpy(className, cn);
    }else{
        className = NULL;
    }
}
ObjCreationHistory::_threadLoadEntry::~_threadLoadEntry()
{
    if(className){
        free(className);
        className = NULL;
    }
}

ObjCreationHistory::ObjCreationHistory()
{
    _globalAccess       = 0;
    _readersCounter     = 0;
    _validEntriesCount  = -1;
    _entriesSize        = START_ENTRIES;
    _globalClock        = 0;
    _loadingEventLock   = 0;
    _entriesForThreads = (struct _threadEntry *)
    malloc(sizeof(struct _threadEntry)*START_ENTRIES);
    for(unsigned int i=0; i != START_ENTRIES; i++){
        _entriesForThreads[i].threadId = -1;
        _entriesForThreads[i].tags =
        new std::vector<std::pair<uint32_t, uint32_t> >();
    }
    _loadingEvents = new std::unordered_map<uint32_t,
    std::pair<int64_t, struct _threadLoadEntry *> >();
}
ObjCreationHistory::~ObjCreationHistory()
{
    for(unsigned int i=0; i != _entriesSize; i++)
    {
        delete _entriesForThreads[i].tags;
    }
    free(_entriesForThreads);
    _entriesForThreads = NULL;
    std::unordered_map<uint32_t, std::pair<int64_t,
    struct _threadLoadEntry *> >::iterator load_it = _loadingEvents ->begin();
    while(load_it != _loadingEvents ->end())
    {
        delete (*load_it).second.second;
        load_it++;
    }
    delete _loadingEvents;
    
}
void ObjCreationHistory::addNewLoadingEntry(int64_t threadId, uint32_t newTag,
                                            const char *name)
{
    while(!OSAtomicCompareAndSwap64(0, threadId, &_loadingEventLock)){
        struct timespec req, rem;
        req.tv_sec  = 0;
        req.tv_nsec = 500;
        nanosleep(&req, &rem);
    }
    uint32_t timestamp = (uint32_t)OSAtomicIncrement32((volatile int32_t *)
                                                       &_globalClock);
    struct _threadLoadEntry *nle = new _threadLoadEntry(timestamp, name);
    _loadingEvents ->insert(std::make_pair<uint32_t, std::pair
                            <int64_t, struct _threadLoadEntry *> >(newTag,
                                           std::make_pair<int64_t,
                                struct _threadLoadEntry *>(threadId, nle)));
    
    OSAtomicCompareAndSwap64(threadId,0, &_loadingEventLock);
}

void ObjCreationHistory::addNewEntry(int64_t threadId, uint32_t tag)
{
    //Check if the whole array in not blocked
    while(!OSAtomicCompareAndSwap32(0,0, (volatile int32_t *)&_globalAccess)){
        struct timespec req, rem;
        req.tv_sec  = 0;
        req.tv_nsec = 500;
        nanosleep(&req, &rem);
    }
    //Set that we are using the array
    OSAtomicIncrement32((volatile int32_t *)&_readersCounter);
    //Look if the threadId is already registered
    uint32_t curntCount = (uint32_t)OSAtomicOr32(0,
                                    &_validEntriesCount) + 1;
    
    uint32_t index = -1;
    if(curntCount != -1)
        for(unsigned int i =0; i != curntCount; i++){
            if(_entriesForThreads[i].threadId == threadId){
                index = i;
                break;
            }
        }
    if(index == -1){
        //We need to register this thread
        uint32_t newIndex = (uint32_t)OSAtomicIncrement32((volatile int32_t *)
                                                          &_validEntriesCount);
        index = newIndex;
        if(newIndex >= _entriesSize){
            //We dont have space
            //Try to set the _globalAccess flag
            OSAtomicDecrement32((volatile int32_t *)&_readersCounter);
            while(!OSAtomicCompareAndSwap64(0,threadId,
                                            (volatile int64_t*)&_globalAccess))
            {
                struct timespec req, rem;
                req.tv_sec  = 0;
                req.tv_nsec = 500;
                nanosleep(&req, &rem);
            }
            //Check if someone already made space
            if(newIndex < _entriesSize)
            {
                OSAtomicIncrement32((volatile int32_t *)&_readersCounter);
                OSAtomicCompareAndSwap64(threadId,0,
                                         (volatile int64_t*)&_globalAccess);
            }else{
                //Wait for the readers to exit
                while(OSAtomicOr32(0,&_readersCounter))
                {
                    struct timespec req, rem;
                    req.tv_sec  = 0;
                    req.tv_nsec = 500;
                    nanosleep(&req, &rem);
                }
                _entriesSize *= 2;
                struct _threadEntry *backup = _entriesForThreads;
                _entriesForThreads = (struct _threadEntry *)
                malloc(sizeof(struct _threadEntry)*_entriesSize);
                for(unsigned int i=0; i != _entriesSize >> 1; i++)
                {
                    _entriesForThreads[i].threadId = backup[i].threadId;
                    _entriesForThreads[i].tags = backup[i].tags;
                }
                for(unsigned int i=_entriesSize>>1; i != _entriesSize; i++){
                    _entriesForThreads[i].threadId = -1;
                    _entriesForThreads[i].tags =
                    new std::vector<std::pair<uint32_t, uint32_t> >();
                }
                free(backup);
                OSAtomicIncrement32((volatile int32_t *)&_readersCounter);
                OSAtomicCompareAndSwap64(threadId,0,
                                         (volatile int64_t*)&_globalAccess);
            }
        }
        _entriesForThreads[index].threadId = threadId;
    }
    //Add the new entry
    uint32_t timestamp = (uint32_t)OSAtomicIncrement32((volatile int32_t *)
                                                       &_globalClock);
    _entriesForThreads[index].tags->push_back(std::make_pair(timestamp,tag));
    OSAtomicDecrement32((volatile int32_t *)&_readersCounter);
}

int ObjCreationHistory::eventsCMP(const void *a, const void *b)
{
    ObjCreationHistory::_logByTimeEntry *e1 =
    (ObjCreationHistory::_logByTimeEntry *)a;
    ObjCreationHistory::_logByTimeEntry *e2 =
    (ObjCreationHistory::_logByTimeEntry *)b;
    if(e1 ->timestamp < e2 ->timestamp)
        return -1;
    if(e1 ->timestamp > e2 ->timestamp)
        return 1;
    return 0;
}

//Retunrs a string representing number centered in a slot of size padding
char *formatNumberWithPadding(uint32_t number, uint32_t padding)
{
    short numChars = (short)log10(number);
    if(padding < numChars)
        padding = numChars;
    char *retStr = (char *)malloc(padding+1);
    short pad = padding - numChars;
    pad = pad >> 1;
    memset(retStr, ' ', padding);
    sprintf(retStr + pad, "%d", number);
    return retStr;
}

//Returns a string centered with padding
char *formatStringWithPadding(const char *str, uint32_t padding)
{
    if(!str)
        return NULL;
    uint32_t strLen = strlen(str);
    padding = (strLen > padding)?strLen:padding;
    char *retStr = (char *)malloc(padding);
    memset(retStr, ' ', padding);
    short pad = padding - strLen;
    pad = pad >> 1;
    sprintf(retStr + pad, "%s", str);
    return retStr;
}

void ObjCreationHistory::writeLogToFile(const char *filename)
{
    FILE *out = fopen(filename, "w");
    
    uint32_t numberOfEvents = _loadingEvents ->size();
    for(uint32_t i=0; i != _validEntriesCount+1; i++){
        numberOfEvents += _entriesForThreads[i].tags ->size();
    }
    
    
    struct _logByTimeEntry *events = (struct _logByTimeEntry *)malloc(
    sizeof(struct _logByTimeEntry)*numberOfEvents);
    uint32_t eventIndex = 0;
    //Write the loading history
    std::unordered_map<uint32_t, std::pair<int64_t,
    struct _threadLoadEntry *> >::iterator load_it = _loadingEvents ->begin();
    while(load_it != _loadingEvents ->end())
    {
        events[eventIndex].timestamp = (*load_it).second.second ->timestamp;
        events[eventIndex].classTag = (*load_it).first;
        events[eventIndex].threadId = (*load_it).second.first;
        events[eventIndex].type = ClassLoading;
        load_it++;
        eventIndex++;
    }
    fprintf(out, "Number of loaded classes: %u\n", eventIndex);
    fprintf(out, "Number of threads %u\n", _validEntriesCount+1);
    for(uint32_t i=0; i != _validEntriesCount+1; i++){
        std::vector<std::pair<uint32_t, uint32_t> >::iterator entries_it =
        _entriesForThreads[i].tags ->begin();
        uint32_t constCalled = 0;
        uint32_t objCreated = 0;
        while (entries_it != _entriesForThreads[i].tags ->end()) {
            events[eventIndex].timestamp = (*entries_it).first;
            events[eventIndex].classTag = (*entries_it).second;
            events[eventIndex].threadId = _entriesForThreads[i].threadId;
            events[eventIndex].type = ConstructorCall;
            if (events[eventIndex].classTag == 0) {
                objCreated++;
            }else{
                constCalled++;
            }
            eventIndex++;
            entries_it++;
        }
        fprintf(out, "Thread Id: 0x%llx\n", _entriesForThreads[i].threadId);
        fprintf(out, "\tObjects created: %u\n", objCreated);
        fprintf(out, "\tConstructors called: %u\n", constCalled);
        
    }
    fprintf(out, "\n----------------Events List----------------\n");
    fprintf(out, "CL = Class Loaded\nOC = Object Created\nCC = Constructor Called\n\n");
    //Now sort all the events according to the timestamp
    qsort(events, numberOfEvents, sizeof(struct _logByTimeEntry),
          ObjCreationHistory::eventsCMP);
    
    char stringBuff[100];
    for(uint32_t i=0; i != numberOfEvents; i++){
        if(events[i].type == ClassLoading)
        {
            sprintf(stringBuff,"%u 0x%llx CL %u\n",
                    events[i].timestamp,events[i].threadId,events[i].classTag);
        }else{
            if(events[i].classTag == 0){
                sprintf(stringBuff,"%u 0x%llx OC\n",
                        events[i].timestamp,events[i].threadId);
            }else{
                sprintf(stringBuff,"%u 0x%llx CC %u\n",
                        events[i].timestamp,events[i].threadId,
                        events[i].classTag);
            }
        }
        fprintf(out, "%s", stringBuff);
    }
    
    fclose(out);
    free(events);
}


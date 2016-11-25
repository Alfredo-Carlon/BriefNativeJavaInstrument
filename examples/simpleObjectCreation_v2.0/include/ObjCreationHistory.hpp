

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

#include <vector>
#include <unordered_map>


class  ObjCreationHistory {
    
private:
    
    enum eventType
    {
        ClassLoading,
        ConstructorCall
    };
    
    struct _logByTimeEntry
    {
        uint32_t timestamp;
        uint32_t classTag;
        uint64_t threadId;
        enum eventType type;
    };
    
    struct _threadEntry
    {
        //The id of the thread for the entry
        int64_t threadId;
        //The (tag,time) entries registered by this thread
        std::vector<std::pair<uint32_t, uint32_t> > *tags;
    };
    
    struct _threadLoadEntry
    {
        uint32_t timestamp;
        char *className;
        _threadLoadEntry(uint32_t ts, const char *cn);
        ~_threadLoadEntry();
    };
    
    //Global flag for updates
    volatile uint64_t _globalAccess;
    //Global counter for readers
    volatile uint32_t _readersCounter;
    
    //Fixed size array for entries. It can grow by blocking the
    //globalAccess.
    //Good: Adding new entries (when size allows it)
    //without blocking the whole array.
    //Bad: Linear time search, possible starvation (if the instrumented code
    //is not starvation free?)
    struct _threadEntry *_entriesForThreads;
    
    uint32_t _validEntriesCount; //Number of valid entries
    uint32_t _entriesSize; //Number of entries at _entriesForThreads
    
    //Global 'clock' for the entries
    volatile uint32_t _globalClock;
    
    //Loading events logger
    std::unordered_map<uint32_t, std::pair<int64_t,
    struct _threadLoadEntry *> > *_loadingEvents;
    //Loading events logger access flag
    volatile int64_t _loadingEventLock;
    
    
    static int eventsCMP(const void *a, const void *b);
    
public:
    ObjCreationHistory();
    
    ~ObjCreationHistory();
    
    //Adds a new 'Loading Event' to the log.
    //That is, a thread loads a new class into the JVM
    void addNewLoadingEntry(int64_t threadId, uint32_t newTag,
                            const char *name);
    
    //Adds a new even to the log, in this case the constructor
    //of the class with tag is called by the threadId
    void addNewEntry(int64_t threadId, uint32_t tag);
    
    //Writes all the events to a file
    void writeLogToFile(const char *filename);
    
    
};


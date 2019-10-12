#ifndef DEBUGMALLOC_IMPL_H
#define DEBUGMALLOC_IMPL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>


/* make getpid and putenv "crossplatform". deprecated on windows but they work just fine,
 * however not declared. */
#ifdef _WIN32
    /* windows */
    #include <process.h>
    #ifdef _MSC_VER
        /* visual studio, getenv/getpid deprecated warning */
        #pragma warning(disable: 4996)
    #else
        /* other windows. the declaration is unfortunately hidden
         * in mingw header files by ifdefs. */
        int putenv(const char *);
    #endif
#else
    /* posix */
    #include <unistd.h>
#endif


/* size of canary in bytes. should be multiple of largest alignment
 * required by any data type (usually 8 or 16) */
enum { debugmalloc_canary_size = 64 };

/* canary byte */
enum { debugmalloc_canary_char = 'K' };

/* hash table size for allocated entries */
enum { debugmalloc_tablesize = 256 };


/* linked list entry for allocated blocks */
typedef struct DebugmallocElem {
    void *real_mem;     /* the address of the real allocation */
    void *user_mem;     /* address shown to the user */
    size_t size;        /* size of block requested by user */

    char file[64];      /* malloc called in this file */
    unsigned line;      /* malloc called at this line in file */
    char func[32];      /* allocation function called (malloc, calloc, realloc) */
    char expr[128];     /* expression calculating the size of allocation */

    struct DebugmallocElem *prev, *next;  /* for doubly linked list */
} DebugmallocElem;


/* debugmalloc singleton, storing all state */
typedef struct DebugmallocData {
    char logfile[256];    /* log file name or empty string */
    DebugmallocElem head[debugmalloc_tablesize], tail[debugmalloc_tablesize];  /* head and tail elements of allocation lists */
} DebugmallocData;


/* this forward declaration is required by the singleton manager function */
static DebugmallocData * debugmalloc_create(void);


/* creates singleton instance. as this function is static included to different
 * translation units, multiple instances of the static variables are created.
 * to make sure it is really a singleton, these instances must know each other
 * somethow. an environment variable is used for that purpose, ie. the address
 * of the singleton allocated is stored by the operating system.
 * this implementation is not thread-safe. */
static DebugmallocData * debugmalloc_singleton(void) {
    static char envstr[100];
    static void *instance = NULL;

    /* if we do not know the address of the singleton:
     * - maybe we are the one to create it (env variable also does not exist)
     * - or it is already created, and stored in the env variable. */
    if (instance == NULL) {
        char envvarname[100] = "";
        sprintf(envvarname, "%s%d", "debugmallocsingleton", (int) getpid());
        char *envptr = getenv(envvarname);
        if (envptr == NULL) {
            /* no env variable: create singleton. */
            instance = debugmalloc_create();
            sprintf(envstr, "%s=%p", envvarname, instance);
            putenv(envstr);
        } else {
            /* another copy of this function already created it. */
            int ok = sscanf(envptr, "%p", &instance);
            if (ok != 1) {
                fprintf(stderr, "debugmalloc: nem lehet ertelmezni: %s!\n", envptr);
                abort();
            }
        }
    }

    return (DebugmallocData *) instance;
}


/* better version of strncpy, always terminates string with \0. */
static void debugmalloc_strlcpy(char *dest, char const *src, size_t destsize) {
    strncpy(dest, src, destsize);
    dest[destsize - 1] = '\0';
}


/* set the name of the log file for debugmalloc. empty filename
 * means logging to stderr. */
static void debugmalloc_log_file(char const *logfilename) {
    if (logfilename == NULL)
        logfilename = "";
    DebugmallocData *instance = debugmalloc_singleton();
    debugmalloc_strlcpy(instance->logfile, logfilename, sizeof(instance->logfile));
}



/* printf to the log file, or stderr. */
static void debugmalloc_log(char const *format, ...) {
    DebugmallocData *instance = debugmalloc_singleton();
    FILE *f = stderr;
    if (strcmp(instance->logfile, "") != 0) {
        f = fopen(instance->logfile, "at");
        if (f == NULL) {
            f = stderr;
            fprintf(stderr, "debugmalloc: nem tudom megnyitni a %s fajlt irasra!\n", instance->logfile);
            strcpy(instance->logfile, "");
        }
    }

    va_list ap;
    va_start(ap, format);
    vfprintf(f, format, ap);
    va_end(ap);

    if (f != stderr)
        fclose(f);
}


/* initialize a memory block allocated for the user. the start and the end
 * of the block is initialized with the canary characters. if 'zero' is
 * true, the user memory area is zero-initialized, otherwise it is also
 * filled with the canary character to simulate garbage in memory. */
static void debugmalloc_memory_init(DebugmallocElem *elem, bool zero) {
    unsigned char *real_mem = (unsigned char *) elem->real_mem;
    unsigned char *user_mem = (unsigned char *) elem->user_mem;
    unsigned char *canary1 = real_mem;
    unsigned char *canary2 = real_mem + debugmalloc_canary_size + elem->size;
    memset(canary1, debugmalloc_canary_char, debugmalloc_canary_size);
    memset(canary2, debugmalloc_canary_char, debugmalloc_canary_size);
    memset(user_mem, zero ? 0 : debugmalloc_canary_char, elem->size);
}

/* check canary, return true if ok, false if corrupted. */
static bool debugmalloc_canary_ok(DebugmallocElem const *elem) {
    unsigned char *real_mem = (unsigned char *) elem->real_mem;
    unsigned char *canary1 = real_mem;
    unsigned char *canary2 = real_mem + debugmalloc_canary_size + elem->size;
    for (size_t i = 0; i < debugmalloc_canary_size; ++i) {
        if (canary1[i] != debugmalloc_canary_char)
            return false;
        if (canary2[i] != debugmalloc_canary_char)
            return false;
    }
    return true;
}

/* dump memory contents to log file. */
static void debugmalloc_dump_memory(char const *mem, size_t size) {
    for (unsigned y = 0; y < (size + 15) / 16; y++) {
        char line[80];
        int pos = 0;
        pos += sprintf(line + pos, "      %04x  ", y * 16);
        for (unsigned x = 0; x < 16; x++) {
            if (y * 16 + x < size)
                pos += sprintf(line + pos, "%02x ", mem[y * 16 + x]);
            else
                pos += sprintf(line + pos, "   ");
        }
        pos += sprintf(line + pos, "  ");
        for (unsigned x = 0; x < 16; x++) {
            if (y * 16 + x < size) {
                unsigned char c = mem[y * 16 + x];
                pos += sprintf(line + pos, "%c", isprint(c) ? c : '.');
            }
            else {
                pos += sprintf(line + pos, " ");
            }
        }
        debugmalloc_log("%s\n", line);
    }
}

/* dump data of allocated memory block.
 * if the canary is corrupted, it is also written to the log. */
static void debugmalloc_dump_elem(DebugmallocElem const *elem) {
    bool canary_ok = debugmalloc_canary_ok(elem);

    debugmalloc_log("  MEMORIATERULET: %p, %u bajt, kanari: %s\n"
                    "    foglalas: %s:%u, %s(%s)\n",
                       elem->user_mem, (unsigned) elem->size, canary_ok ? "ok" : "**SERULT**",
                       elem->file, elem->line,
                       elem->func, elem->expr);

    if (!canary_ok) {
        debugmalloc_log("    ELOTTE kanari: \n");
        debugmalloc_dump_memory((char const *) elem->real_mem, debugmalloc_canary_size);
    }

    debugmalloc_log("    memoria eleje: \n");
    debugmalloc_dump_memory((char const *) elem->user_mem, elem->size > 64 ? 64 : elem->size);

    if (!canary_ok) {
        debugmalloc_log("    UTANA kanari: \n");
        debugmalloc_dump_memory((char const *) elem->real_mem + debugmalloc_canary_size + elem->size, debugmalloc_canary_size);
    }
}


/* dump data of all memory blocks allocated. */
static void debugmalloc_dump(void) {
    DebugmallocData *instance = debugmalloc_singleton();
    debugmalloc_log("** DEBUGMALLOC DUMP ************************************\n");
    for (size_t i = 0; i < debugmalloc_tablesize; i++) {
        DebugmallocElem *head = &instance->head[i];
        for (DebugmallocElem *iter = head->next; iter->next != NULL; iter = iter->next)
            debugmalloc_dump_elem(iter);
    }
    debugmalloc_log("** DEBUGMALLOC DUMP VEGE *******************************\n");
}


/* called at program exit to dump data if there is a leak,
 * ie. allocated block remained. */
static void debugmalloc_atexit_dump(void) {
    DebugmallocData *instance = debugmalloc_singleton();
    bool ok = true;
    for (size_t i = 0; i < debugmalloc_tablesize; i++) {
        if (instance->head[i].next != &instance->tail[i]) {
            ok = false;
            break;
        }
    }

    if (!ok) {
        debugmalloc_log("********************************************************\n"
                        "* MEMORIASZIVARGAS VAN A PROGRAMBAN!!!\n"
                        "********************************************************\n");
        debugmalloc_dump();
    }
}


/* hash function for bucket hash. */
static size_t debugmalloc_hash(void *address) {
    /* the last few bits are ignored, as they are usually zero, because of
     * alignment purposes. all tested architectures used 16 byte allocation. */
    size_t cut = (size_t)address >> 4;
    return cut % debugmalloc_tablesize;
}


/* insert element to hash table. */
static void debugmalloc_insert(DebugmallocElem *entry) {
    DebugmallocData *instance = debugmalloc_singleton();
    size_t idx = debugmalloc_hash(entry->user_mem);
    DebugmallocElem *head = &instance->head[idx];
    entry->prev = head;
    entry->next = head->next;
    head->next->prev = entry;
    head->next = entry;
}


/* find element in hash table, given with the memory address that the user sees.
 * @return the linked list entry, or null if not found. */
static DebugmallocElem * debugmalloc_find(void *mem) {
    DebugmallocData *instance = debugmalloc_singleton();
    size_t idx = debugmalloc_hash(mem);
    DebugmallocElem *head = &instance->head[idx];
    for (DebugmallocElem *iter = head->next; iter->next != NULL; iter = iter->next)
        if (iter->user_mem == mem)
            return iter;
    return NULL;
}


/* count number of blocks allocated */
static int debugmalloc_allocated_count(void) {
    DebugmallocData *instance = debugmalloc_singleton();
    int count = 0;
    for (size_t i = 0; i < debugmalloc_tablesize; i++) {
        for (DebugmallocElem *iter = instance->head[i].next; iter != &instance->tail[i]; iter = iter->next)
            count += 1;
    }   
    return count;
}


/* allocate memory. this function is called via the macro. */
static void *debugmalloc_malloc_full(size_t size, char const *func, char const *expr, char const *file, unsigned line, bool zero) {
    /* imitate standard malloc: return null if size is zero */
    if (size == 0)
        return NULL;

    /* allocate more memory, make room for canary */
    void *real_mem = malloc(size + 2 * debugmalloc_canary_size);
    if (real_mem == NULL) {
        debugmalloc_log("debugmalloc: %s @ %s:%u: nem sikerult %u meretu memoriat foglalni!\n", func, file, line, (unsigned) size);
        return NULL;
    }

    /* allocate memory for linked list element */
    DebugmallocElem *newentry = (DebugmallocElem *) malloc(sizeof(DebugmallocElem));
    if (newentry == NULL) {
        free(real_mem);
        debugmalloc_log("debugmalloc: %s @ %s:%u: le tudtam foglalni %u memoriat, de utana a sajatnak nem, sry\n", func, file, line, (unsigned) size);
        abort();
    }

    /* metadata of allocation: caller function, code line etc. */
    debugmalloc_strlcpy(newentry->func, func, sizeof(newentry->func));
    debugmalloc_strlcpy(newentry->expr, expr, sizeof(newentry->expr));
    debugmalloc_strlcpy(newentry->file, file, sizeof(newentry->file));
    newentry->line = line;

    /* address of allocated memory chunk */
    newentry->real_mem = real_mem;
    newentry->user_mem = (unsigned char *) real_mem + debugmalloc_canary_size;
    newentry->size = size;
    debugmalloc_memory_init(newentry, zero);

    /* store in list and return pointer to user area */
    debugmalloc_insert(newentry);
    return newentry->user_mem;
}


/* free memory and remove list item. before deleting, the chuck is filled with
 * the canary byte to make sure that the user will see garbage if the memory
 * is accessed after freeing. */
static void debugmalloc_free_inner(DebugmallocElem *deleted) {
    /* fill with garbage */
    memset(deleted->real_mem, debugmalloc_canary_char, deleted->size + 2 * debugmalloc_canary_size);

    /* free memory and remove from linked list */
    free(deleted->real_mem);
    deleted->next->prev = deleted->prev;
    deleted->prev->next = deleted->next;
    free(deleted);
}


/* free memory - called via the macro.
 * as all allocations are tracked in the list, this function can call abort()
 * if a block is freed twice or the free function is called with an invalid address. */
static void debugmalloc_free_full(void *mem, char const *func, char const *file, unsigned line) {
    /* imitate standard free function: if ptr is null, no operation is performed */
    if (mem == NULL)
        return;

    /* find allocation, abort if not found */
    DebugmallocElem *deleted = debugmalloc_find(mem);
    if (deleted == NULL) {
        debugmalloc_log("debugmalloc: %s @ %s:%u: olyan teruletet akarsz felszabaditani, ami nincs lefoglalva!\n", func, file, line);
        abort();
    }

    /* check canary and then free memory */
    if (!debugmalloc_canary_ok(deleted)) {
        debugmalloc_log("debugmalloc: %s @ %s:%u: a %p memoriateruletet tulindexelted!\n", func, file, line, mem);
        debugmalloc_dump_elem(deleted);
    }
    debugmalloc_free_inner(deleted);
}


/* realloc-like function. */
static void *debugmalloc_realloc_full(void *oldmem, size_t newsize, char const *func, char const *expr, char const *file, unsigned line) {
    /* imitate standard realloc: equivalent to malloc if first param is NULL */
    if (oldmem == NULL)
        return debugmalloc_malloc_full(newsize, func, expr, file, line, 0);

    /* imitate standard realloc: equivalent to free if size is null. */
    if (newsize == 0) {
        debugmalloc_free_full(oldmem, func, file, line);
        return NULL;
    }

    /* find old allocation. abort if not found. */
    DebugmallocElem *entry = debugmalloc_find(oldmem);
    if (entry == NULL) {
        debugmalloc_log("debugmalloc: %s @ %s:%u: olyan teruletet akarsz atmeretezni, ami nincs lefoglalva!\n", func, file, line);
        abort();
    }

    /* create new allocation */
    void *newmem = debugmalloc_malloc_full(newsize, func, expr, file, line, false);
    if (newmem == NULL) {
        debugmalloc_log("debugmalloc: %s @ %s:%u: nem sikerult uj memoriat foglalni az atmeretezeshez!\n", func, file, line);
        /* imitate standard realloc: original block is untouched, but return NULL */
        return NULL;
    }

    /* copy old data */
    size_t smaller = entry->size < newsize ? entry->size : newsize;
    memcpy(newmem, oldmem, smaller);
    debugmalloc_free_inner(entry);
    return newmem;
}


/* initialize debugmalloc singleton. returns the newly allocated instance */
static DebugmallocData * debugmalloc_create(void) {
    /* config check */
    if (debugmalloc_canary_size % 16 != 0) {
        debugmalloc_log("debugmalloc: a kanari merete legyen 16-tal oszthato\n");
        abort();
    }
    if (debugmalloc_canary_char == 0) {
        debugmalloc_log("debugmalloc: a kanari legyen 0-tol kulonbozo\n");
        abort();
    }
    /* avoid compiler warning if these functions are not used */
    (void) debugmalloc_realloc_full;
    (void) debugmalloc_log_file;
    (void) debugmalloc_allocated_count;

    /* create and initialize instance */
    DebugmallocData *instance = (DebugmallocData *) malloc(sizeof(DebugmallocData));
    if (instance == NULL) {
        debugmalloc_log("debugmalloc: nem sikerult elinditani a memoriakezelest\n");
        abort();
    }
    memset(instance, 0, sizeof(DebugmallocData));
    for (size_t i = 0; i < debugmalloc_tablesize; i++) {
        instance->head[i].prev = NULL;
        instance->head[i].next = &instance->tail[i];
        instance->tail[i].next = NULL;
        instance->tail[i].prev = &instance->head[i];
    }

    atexit(debugmalloc_atexit_dump);
    return instance;
}

#endif

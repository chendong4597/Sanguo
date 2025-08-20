#ifndef __SYSTEM_INFO_ANDROID_H__
#define __SYSTEM_INFO_ANDROID_H__

#include <malloc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

struct stats_t {
    int dalvikPss;
    int dalvikPrivateDirty;
    int dalvikSharedDirty;

    int nativePss;
    int nativePrivateDirty;
    int nativeSharedDirty;

    int otherPss;
    int otherPrivateDirty;
    int otherSharedDirty;
};

struct proc_info {
    long VmHWM;
};


static void read_mapinfo(FILE *fp, stats_t* stats)
{
    char line[1024];
    int len;
    bool skip, done = false;

    unsigned start = 0, size = 0, resident = 0, pss = 0;
    unsigned shared_clean = 0, shared_dirty = 0;
    unsigned private_clean = 0, private_dirty = 0;
    unsigned referenced = 0;
    unsigned temp;

    int isNativeHeap;
    int isDalvikHeap;
    int isSqliteHeap;

    if(fgets(line, 1024, fp) == 0) return;
    while (!done) {
        isNativeHeap = 0;
        isDalvikHeap = 0;
        isSqliteHeap = 0;
        skip = false;

        len = strlen(line);
        if (len < 1) return;
        line[--len] = 0;

        /* ignore guard pages */
        if (len > 18 && line[17] == '-') skip = true;
        start = strtoul(line, 0, 16);
        if (strstr(line, "[heap]")) {
            isNativeHeap = 1;
        } else if (strstr(line, "/dalvik-LinearAlloc")) {
            isDalvikHeap = 1;
        } else if (strstr(line, "/mspace/dalvik-heap")) {
            isDalvikHeap = 1;
        } else if (strstr(line, "/dalvik-heap-bitmap/")) {
            isDalvikHeap = 1;
        } else if (strstr(line, "/data/dalvik-cache/")) {
            isDalvikHeap = 1;
        } else if (strstr(line, "/tmp/sqlite-heap")) {
            isSqliteHeap = 1;
        }

        //LOGI("native=%d dalvik=%d sqlite=%d: %s\n", isNativeHeap, isDalvikHeap,

        //    isSqliteHeap, line);

       while (true) {
             if (fgets(line, 1024, fp) == 0) {
                done = true;
                break;
            }

            if (sscanf(line, "Size: %d kB", &temp) == 1) {
                size = temp;
            } else if (sscanf(line, "Rss: %d kB", &temp) == 1) {
                resident = temp;
            } else if (sscanf(line, "Pss: %d kB", &temp) == 1) {
                pss = temp;
            } else if (sscanf(line, "Shared_Clean: %d kB", &temp) == 1) {
                shared_clean = temp;
            } else if (sscanf(line, "Shared_Dirty: %d kB", &temp) == 1) {
                shared_dirty = temp;
            } else if (sscanf(line, "Private_Clean: %d kB", &temp) == 1) {
                private_clean = temp;
            } else if (sscanf(line, "Private_Dirty: %d kB", &temp) == 1) {
                private_dirty = temp;
            } else if (sscanf(line, "Referenced: %d kB", &temp) == 1) {
                referenced = temp;
            } else if (strlen(line) > 30 && line[8] == '-' && line[17] == ' ') {
                // looks like a new mapping
                // example: "10000000-10001000 ---p 10000000 00:00 0"
                break;
            }
        }

        if (!skip) {
            if (isNativeHeap) {
                stats->nativePss += pss;
                stats->nativePrivateDirty += private_dirty;
                stats->nativeSharedDirty += shared_dirty;
            } else if (isDalvikHeap) {
                stats->dalvikPss += pss;
                stats->dalvikPrivateDirty += private_dirty;
                stats->dalvikSharedDirty += shared_dirty;
            } else if ( isSqliteHeap) {
                // ignore
            } else {
                stats->otherPss += pss;
                stats->otherPrivateDirty += private_dirty;
                stats->otherSharedDirty += shared_dirty;
            }
        }
    }
}

static void load_maps(int pid, stats_t* stats)
{
    char tmp[128];
    FILE *fp;

    sprintf(tmp, "/proc/%d/smaps", pid);
    fp = fopen(tmp, "r");
    if (fp == 0) return;

    read_mapinfo(fp, stats);
    fclose(fp);
}

static long getFreeMemoryImpl(const char* const sums[], const size_t sumsLen[], size_t num)
{
   int fd = open("/proc/meminfo", O_RDONLY);

   if (fd < 0) {
		//LOGD(("Unable to open /proc/meminfo"));
        return -1;
    }

    char buffer[256];
    const int len = read(fd, buffer, sizeof(buffer)-1);
    close(fd);

    if (len < 0) {
		//LOGD(("Unable to open /proc/meminfo"));
        return -1;
    }
    buffer[len] = 0;

    size_t numFound = 0;
    long mem = 0;

    char* p = buffer;
    while (*p && numFound < num) {
    int i = 0;

    while (sums[i]) {
        if (strncmp(p, sums[i], sumsLen[i]) == 0) {
               p += sumsLen[i];
               while (*p == ' ') p++;
               char* num = p;
               while (*p >= '0' && *p <= '9') p++;
                if (*p != 0) {
                   *p = 0;
                    p++;
                    if (*p == 0) p--;
               }
               mem += atoll(num);
               numFound++;
               break;
            }
            i++;
       }
        p++;
    }

    return numFound > 0 ? mem : -1;
}

static int read_status(FILE *fp, proc_info* proc)
{
    char line[1024] = {0x00};
    unsigned int nVmHWM = 0;

    while (fgets(line, 1024, fp)) {
        sscanf(line, "VmHWM: %u", &nVmHWM);
    }
    proc->VmHWM = nVmHWM;
    return 0;
}

static void load_status(int pid, proc_info* proc)
{
    char tmp[128] = {0x00};
    FILE *fp;

    sprintf(tmp, "/proc/%d/status", pid);
    fp = fopen(tmp, "r");
    if (fp == 0) return;

    read_status(fp, proc);
    fclose(fp);
}



#endif

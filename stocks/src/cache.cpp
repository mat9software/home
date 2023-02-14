#include "cache.h"

#include "common/log.h"
#include "utils.h"

#include "emscripten.h"

#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

#ifdef ENABLE_CACHE
//--------------------------------------------------
void cache_init() {
    // EM_ASM is a macro to call in-line JavaScript code.
    EM_ASM(
        // Make a directory other than '/'
        FS.mkdir("/offline");
        // Then mount with IDBFS type
        FS.mount(IDBFS, {}, "/offline");

        // Then sync
        FS.syncfs(true, function (err) {
          if(err) {
            console.log("cache_init : Sync failed : " + err);
          }
        });
    );
}

//--------------------------------------------------
std::optional<std::string> cache_get(const char* key) {
  FMT_STACK_STR(filepath, 50, "/offline/%s", key);
  FILE* fp = fopen(filepath, "r");
  if(fp == nullptr) {
    //file not found, no cache
    return std::nullopt;
  }

  std::string content;
  fseek(fp, 0, SEEK_END);
  content.resize(ftell(fp));
  rewind(fp);
  fread(&content[0], 1, content.size(), fp);
  fclose(fp);
  return content; 
}

//--------------------------------------------------
void cache_put(const char* key, const char* buffer) {
  FMT_STACK_STR(filepath, 50, "/offline/%s", key);
  FILE* file = fopen(filepath, "w");
  if (file == nullptr) {
    LOG_ERROR("Failed to open.");
    return;
  }

  fputs(buffer, file);

  int ret = fclose(file);
  if (ret != 0) {
    LOG_ERROR("Failed to close.");
  }
  EM_ASM(FS.syncfs(function(err) {
    if (err) {
      console.log("cache_put : Sync failed" + err);
    }
  }););
}

//--------------------------------------------------
void cache_rm_all() {
    struct dirent *de;  // Pointer for directory entry
  
    // opendir() returns a pointer of DIR type. 
    DIR *dr = opendir("/offline");
  
    if (dr == NULL)  // opendir returns NULL if couldn't open directory
    {
        LOG_ERROR("Could not open current directory" );
        return;
    }
  
    while ((de = readdir(dr)) != NULL) {
      if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
        continue;
      }

      FMT_STACK_STR(filepath, 50, "/offline/%s", de->d_name);
      if(remove(filepath) != 0) {
        LOG_ERROR("Failed to remove %s.", de->d_name);
      } else {
        LOG_TRACE("Removed file from cache : %s", filepath);
      }
    }
  
    closedir(dr);  

  EM_ASM(FS.syncfs(function(err) {
    if (err) {
      console.log("cache_rm_all : Sync failed" + err);
    }
  }););
}
#else
// Define empty function when cache is disabled.
void cache_init(){}
std::optional<std::string> cache_get(const char* key){return std::nullopt;}
void cache_put(const char* key, const char* buffer){}
void cache_rm_all(){}

#endif //ENABLE_CACHE

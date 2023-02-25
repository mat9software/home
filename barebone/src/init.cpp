#include "init.h"
#include "emscripten.h"

void init()
{
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


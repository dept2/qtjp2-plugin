SET (CMAKE_CXX_FLAGS_DEBUG   "-std=gnu++0x -pipe -W -Wall -Wextra -O0 -g -fPIC")
SET (CMAKE_CXX_FLAGS_RELEASE "-std=gnu++0x -pipe -W -Wall -Wextra -O2 -fomit-frame-pointer -fPIC -fvisibility-inlines-hidden -DQT_NO_DEBUG")
SET (CMAKE_CXX_FLAGS_RELWITHDEBINFO "-std=gnu++0x -pipe -W -Wall -Wextra -O2 -g -fomit-frame-pointer -fPIC -fvisibility-inlines-hidden -DQT_NO_DEBUG")

SET (CMAKE_EXE_LINKER_FLAGS "-Wl,--no-undefined")
SET (CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-undefined")

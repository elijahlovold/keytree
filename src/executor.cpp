#include "executor.hpp"
#include <spawn.h>

extern char** environ;

void execute(const std::string& cmd) {
    pid_t pid;
    const char* sh = "/bin/sh";
    char* argv[] = {
        const_cast<char*>("sh"),
        const_cast<char*>("-c"),
        const_cast<char*>(cmd.c_str()),
        nullptr
    };

    posix_spawnattr_t attr;
    posix_spawnattr_init(&attr);
    // Detach child into its own session so it outlives keytree's exit
    posix_spawnattr_setflags(&attr, POSIX_SPAWN_SETSID);
    posix_spawn(&pid, sh, nullptr, &attr, argv, environ);
    posix_spawnattr_destroy(&attr);
}

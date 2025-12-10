alias b := build
alias r := run
alias br := build-run 

host := `uname -a`

build:
    @clang src/main.c              \
        /lib/libwayland-client.so \
        /lib/libvulkan.so         \
        -I lib                    \
        -D __DEBUG                \
        -D LOG_LEVEL_VERBOSE      \
        -D LOG_LEVEL_INFO         \
        -D LOG_LEVEL_DEBUG        \
        -D LOG_LEVEL_WARNING      \
        -D LOG_LEVEL_ERROR        \
        -D LOG_LEVEL_CRITICAL     \
        -o ritual

build-release:
    @clang -O3 -flto src/main.c -I lib -D __RELEASE -o ritual /lib/libwayland-client.so /lib/libvulkan.so

run:
    @./ritual

build-run:
    @just build
    @just run

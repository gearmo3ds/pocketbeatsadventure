#/usr/share/qtcreator/debugger/gdbbridge.py

set $ipend = 80
set $progname = "pocketbeatsadventure.3dsx"

define 3ds-select
    set $ipend = $arg0
end

# Define a custom command that takes one argument
define 3ds-upload
    eval "shell 3dslink -r 10 --address 192.168.2.%i %s", $ipend, $progname
end

define 3ds-attach
    eval "target remote 192.168.2.%i:4003", $ipend
end

define 3ds-debug
    3ds-upload
    shell sleep 1
    3ds-attach
    continue
end

define 3ds-citra
    shell ./runcdebug
    target remote 127.0.0.1:24689
    continue
end

define citra-attach
    target remote 127.0.0.1:24689
end

define getbreaks
    delete
    source /tmp/breakpoints.txt
end

define 3ds-quit
    set variable doExit = 1
    continue
end

# Home
#target remote 192.168.2.67:4003

# White 3DS
#target remote 192.168.2.80:4003

# Alflen
#target remote 192.168.178.47:4003

# Citra
#target remote localhost:24689


tui enable
set history save on

set print pretty on
set print object on
set print static-members on
set print vtbl on
set print demangle on
set demangle-style gnu-v3

break abort


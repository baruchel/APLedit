APLedit
=======

A line-editor with a pop-up menu for easely inserting special APL characters
It is able to interact with any interpreter using readline (like GNU APL).

Compile it with:

    gcc -Wall -fPIC -shared -o apledit.so apledit.c -ldl

The key bindings must be set in the ~/.inputrc configuration file of each user.
Example:

    $if APLedit
    "\e[2~": apledit-mode             # key "insert"
    "\e[5~": apledit-category-up      # key "page up"
    "\e[6~": apledit-category-down    # key "page down"
    #"\e[5~": apledit-item-up         # key "page up"
    "\e[3~": apledit-item-down        # key "suppr"
    "\e/": apledit-insert-char        # key "menu" (below, next to right-ctrl)
    $endif

It has to be run through the LD__PRELOAD mechanism.

    LD_PRELOAD=./apledit.so apl --noCIN

You can enter the editor whenever you want with the apledit-mode function.

You can customize the menu with something like:

    export APLEDIT_PROMPT='%s [%s] '

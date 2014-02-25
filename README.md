APLedit
=======

A line-editor with a pop-up menu for easely inserting special APL characters.
It is able to interact with GNU APL.

##Installation

Compile it with:

    gcc -Wall -fPIC -shared -o apledit.so apledit.c -ldl

##Usage

The program has to be run through the LD_PRELOAD mechanism.

    LD_PRELOAD=./apledit.so apl --noCIN

Of course, once you have found a good location for the binary file, you should create some alias for your shell in order to type only `apl` or some easy to remeber command.

You can enter the editor whenever you want with the apledit-mode function.

##Settings

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

If for some reason another file is wanted, its path has to be stored in the
APLEDIT_INITFILE environment variable.

You can customize the menu with something like:

    export APLEDIT_PROMPT='%s [%s] '
    export APLEDIT_COLOR=$'\033[0;1m'

You can remove the banner concerning APLedit with:

    export APLEDIT_NOBANNER="True"

##Demo
![Demo](https://raw.github.com/baruchel/APLedit/master/apledit.gif)

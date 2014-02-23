APLedit
=======

A line-editor with a pop-up menu for easely inserting special APL characters
It will be able to interact with various APL interpreters with a special focus
on GNU APL.

Since it uses the readline library, the key bindings can be set in the ~/.inputrc
configuration file of each user. Example:

    $if APLedit
    "\e[2~": apledit-mode             # key "insert"
    "\e[5~": apledit-category-up      # key "page up"
    "\e[6~": apledit-category-down    # key "page down"
    #"\e[5~": apledit-item-up         # key "page up"
    "\e[3~": apledit-item-down        # key "suppr"
    "\e/": apledit-insert-char        # key "menu" (below, next to right-ctrl)
    $endif

The user can customize both prompt (normal prompt and pop-up menu). Using ANSI
escape sequences for color/bold/etc. can also be done:

    ./apledit -p2 "^[[1;33m[%s %s]^[[0m "
    ./apledit -p2 "^[[37;44;1m%s ^[[33;45m%s^[[0m " -q ")OFF" -p1 "% "

But escape sequence has to be typed as directly.

A few other flags will be described later.

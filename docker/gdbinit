define fs
finish
step
end

set print pretty on
set print object on
set print static-members off

set listsize 20
define lc
l *$pc
end

set pagination off

set logging overwrite
set logging file /tmp/gdb.out
set logging on

define updateinit
   source ~/.gdbinit
end

define bsave
save breakpoints ~/.gdbbreak
end

define bload
source ~/.gdbbreak
end

define bclear
shell rm -f ~/.gdbbreak
end

define stackContinue
where
continue
end

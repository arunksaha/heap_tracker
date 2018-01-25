find src test -type f -name '*.h' -o -name '*.cc' | xargs clang-format -i
# git difftool --no-prompt --tool=meld

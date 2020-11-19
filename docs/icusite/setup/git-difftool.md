# Setup: git difftool & mergetool

The `git diff` command prints changes to stdout, normally to the terminal
screen.

Set up a visual diff and merge program for use with `git difftool` and `git
mergetool`.

Changes in binary files do not show well in common diff tools and can take a
long time for them to compute visual diffs.

This is easily avoided using the -d option: `git difftool -d`

This shows all changed files in the diff program, and you can view and skip
files there as appropriate.

## Linux example

[stackoverflow/.../setting-up-and-using-meld-as-your-git-difftool-and-mergetool](https://stackoverflow.com/questions/34119866/setting-up-and-using-meld-as-your-git-difftool-and-mergetool)

### Linux meld

`gedit ~/.gitconfig` →

```none
[diff]
```

```none
    tool = meld
```

```none
[difftool]
```

```none
    prompt = false
```

```none
[difftool "meld"]
```

```none
    cmd = meld "$LOCAL" "$REMOTE"
```

```none
[merge]
```

```none
    tool = meld
```

```none
[mergetool "meld"]
```

```none
    cmd = meld "$LOCAL" "$MERGED" "$REMOTE" --output "$MERGED"
```

#!/usr/bin/env python3
"""Claude Code `PreToolUse` hook: refuse a `git commit` that skips the git hooks.

The pre-commit hooks are how this repository checks a change before it is
committed, so an agent must fix what they report rather than bypass them. This
refuses `--no-verify`, `-n` (also inside a cluster such as `-an`), and an
override of `core.hooksPath`, which skips them just as well.
"""

import json
import shlex
import sys

# `git commit` short options that take a value, which ends the option cluster
TAKES_VALUE = set("mFcCt")
LONG_TAKES_VALUE = {
    "--message",
    "--file",
    "--author",
    "--date",
    "--reuse-message",
    "--reedit-message",
    "--fixup",
    "--squash",
    "--template",
    "--cleanup",
    "--trailer",
    "--pathspec-from-file",
}


def bypasses(words):
    """Why this `git` invocation skips the hooks, or None."""
    i = 1
    while i < len(words) and words[i].startswith("-"):  # git's own options
        opt = words[i]
        if opt in ("-c", "-C") and i + 1 < len(words):
            if opt == "-c" and words[i + 1].lower().startswith("core.hookspath"):
                return "overrides core.hooksPath"
            i += 1
        i += 1
    if i >= len(words) or words[i] != "commit":
        return None
    args = iter(words[i + 1 :])
    for arg in args:
        if arg == "--":
            break
        if arg == "--no-verify":
            return "passes --no-verify"
        if arg in LONG_TAKES_VALUE:
            next(args, None)  # its value, even one that starts with a dash
        elif arg.startswith("-") and not arg.startswith("--"):
            for pos, letter in enumerate(arg[1:], 1):
                if letter == "n":
                    return f"passes -n (in '{arg}')"
                if letter == "S":  # -S<keyid>: an optional value, attached only
                    break
                if letter in TAKES_VALUE:
                    if pos == len(arg) - 1:
                        next(args, None)  # the value is the next word
                    break
    return None


def main():
    command = json.load(sys.stdin).get("tool_input", {}).get("command", "")
    try:
        lexer = shlex.shlex(command, posix=True, punctuation_chars=";&|()")
        lexer.whitespace_split = True
        tokens = list(lexer)
    except ValueError:  # unbalanced quotes: let the shell report it
        return 0
    # split into simple commands and look at each one that runs git
    segment = []
    for token in tokens + [";"]:
        if token and set(token) <= set(";&|()"):
            while segment and "=" in segment[0] and not segment[0].startswith("-"):
                segment.pop(0)  # an environment assignment ahead of the command
            if segment and segment[0].rsplit("/", 1)[-1] == "git":
                why = bypasses(segment)
                if why:
                    print(
                        f"Refused: this `git commit` {why}, which skips the pre-commit hooks. "
                        "Do not bypass them: fix what the hooks report, re-stage, and commit again.",
                        file=sys.stderr,
                    )
                    return 2
            segment = []
        else:
            segment.append(token)
    return 0


if __name__ == "__main__":
    sys.exit(main())

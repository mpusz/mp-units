#!/bin/bash

set -euo pipefail

if ! command -v code >/dev/null 2>&1; then
  echo "VS Code CLI not found; skipping extension installation." >&2
  exit 0
fi

extensions=(
  ms-vscode.cpptools
  ms-vscode.cpptools-extension-pack
  ms-vscode.cmake-tools
  fredericbonnet.cmake-test-adapter
  matepek.vscode-catch2-test-adapter
  xaver.clang-format
  bierner.markdown-mermaid
  darkriszty.markdown-table-prettify
  DavidAnson.vscode-markdownlint
  ms-vscode.vscode-json
  redhat.vscode-yaml
  ms-python.python
  ms-python.isort
  ms-python.black-formatter
  hbenl.vscode-test-explorer
  mhutchie.git-graph
  Gruntfuggly.todo-tree
  streetsidesoftware.code-spell-checker
  vivaxy.vscode-conventional-commits
  github.vscode-github-actions
  github.copilot
  github.copilot-chat
  ms-vsliveshare.vsliveshare
)

for extension in "${extensions[@]}"; do
  echo "Installing ${extension}..."
  code --install-extension "${extension}" --force
done
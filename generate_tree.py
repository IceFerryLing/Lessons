import os
import re

README_NAME = "README.md"
SECTION_HEADING = "## 文件目录"
FENCE_LANG = "text"

EXCLUDE_DIRS = ['.venv', '.git', '.vscode', '__pycache__']
EXCLUDE_FILES = [os.path.basename(__file__), 'file_tree.txt', README_NAME]


def build_tree(startpath, exclude_dirs, exclude_files):
    """Return the directory tree as a list of lines."""
    lines = [os.path.basename(startpath)]
    _collect_tree(startpath, lines, "", exclude_dirs, exclude_files)
    return lines


def _collect_tree(directory, lines, prefix, exclude_dirs, exclude_files):
    try:
        items = os.listdir(directory)
    except (FileNotFoundError, PermissionError):
        return

    dirs = sorted(
        d for d in items
        if d not in exclude_dirs and os.path.isdir(os.path.join(directory, d))
    )
    files = sorted(
        f for f in items
        if f not in exclude_files and os.path.isfile(os.path.join(directory, f))
    )

    all_items = dirs + files
    for i, item_name in enumerate(all_items):
        path = os.path.join(directory, item_name)
        is_last = (i == len(all_items) - 1)
        lines.append(f"{prefix}{'└── ' if is_last else '├── '}{item_name}")

        if os.path.isdir(path):
            new_prefix = prefix + ("    " if is_last else "│   ")
            _collect_tree(path, lines, new_prefix, exclude_dirs, exclude_files)


def _render_block(tree_lines):
    return "```%s\n%s\n```" % (FENCE_LANG, "\n".join(tree_lines))


def _line_start(text, index):
    """Index of the first character of the line containing `index`."""
    return text.rfind("\n", 0, index) + 1


def _next_line_start(text, index):
    """Index of the first character after the line containing `index`."""
    end = text.find("\n", index)
    return len(text) if end == -1 else end + 1


def update_readme(readme_path, tree_lines, heading=SECTION_HEADING):
    """Replace the tree code block under `heading` in the README, in place.

    Returns one of: "replaced", "created", "appended".
    """
    with open(readme_path, "r", encoding="utf-8", newline="") as f:
        text = f.read()

    newline = "\r\n" if "\r\n" in text else "\n"
    block = _render_block(tree_lines).replace("\n", newline)

    match = re.search(r"^" + re.escape(heading) + r"[ \t]*\r?$", text, re.MULTILINE)
    heading_idx = match.start() if match else -1
    if heading_idx == -1:
        # No section yet: append it at the bottom of the file.
        stripped = text.rstrip("\r\n")
        separator = "" if not stripped else newline + newline
        text = stripped + separator + heading + newline + block + newline
        status = "created"
    else:
        open_idx = text.find("```", heading_idx)
        if open_idx == -1:
            # Heading exists but no code block: insert one right after the heading line.
            insert_at = _next_line_start(text, heading_idx)
            text = text[:insert_at] + block + newline + text[insert_at:]
            status = "appended"
        else:
            open_line_start = _line_start(text, open_idx)
            close_idx = text.find("```", _next_line_start(text, open_idx))
            if close_idx == -1:
                # Unclosed block: rewrite everything from the dangling opening fence onward.
                text = text[:open_line_start] + block + newline
                status = "appended"
            else:
                # Replace the whole block, one full line per fence, so CRLF stays untouched.
                text = text[:open_line_start] + block + newline \
                    + text[_next_line_start(text, close_idx):]
                status = "replaced"

    with open(readme_path, "w", encoding="utf-8", newline="") as f:
        f.write(text)
    return status


def main():
    start_path = os.path.abspath(os.path.dirname(__file__))
    readme_path = os.path.join(start_path, README_NAME)

    if not os.path.isfile(readme_path):
        raise SystemExit(f"{README_NAME} not found next to this script: {readme_path}")

    tree_lines = build_tree(start_path, EXCLUDE_DIRS, EXCLUDE_FILES)
    status = update_readme(readme_path, tree_lines)

    print(f"{SECTION_HEADING} {status} in {README_NAME} ({len(tree_lines)} lines of tree)")


if __name__ == "__main__":
    main()

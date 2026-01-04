import os

def generate_tree(startpath, file_output, exclude_dirs=None, exclude_files=None):
    if exclude_dirs is None:
        exclude_dirs = []
    if exclude_files is None:
        exclude_files = []

    with open(file_output, "w", encoding="utf-8") as f:
        # First, write the root directory name
        f.write(f"{os.path.basename(startpath)}\n")
        
        # Use a recursive helper function to build the tree
        _generate_tree_recursive(startpath, f, "", exclude_dirs, exclude_files)

def _generate_tree_recursive(directory, f, prefix, exclude_dirs, exclude_files):
    # Get all items in the directory, separating dirs and files
    try:
        items = os.listdir(directory)
    except FileNotFoundError:
        return
        
    dirs = sorted([d for d in items if os.path.isdir(os.path.join(directory, d)) and d not in exclude_dirs])
    files = sorted([file for file in items if os.path.isfile(os.path.join(directory, file)) and file not in exclude_files])
    
    # Combine dirs and files for iteration
    all_items = dirs + files
    
    for i, item_name in enumerate(all_items):
        path = os.path.join(directory, item_name)
        is_last = (i == len(all_items) - 1)
        
        # Determine the connector
        connector = "└── " if is_last else "├── "
        f.write(f"{prefix}{connector}{item_name}\n")
        
        # If it's a directory, recurse
        if os.path.isdir(path):
            new_prefix = prefix + ("    " if is_last else "│   ")
            _generate_tree_recursive(path, f, new_prefix, exclude_dirs, exclude_files)


if __name__ == "__main__":
    # Get the absolute path of the script's directory
    start_path = os.path.abspath(os.path.dirname(__file__))
    
    # Define excludes
    exclude_dirs_list = ['.venv', '.git', '.vscode', '__pycache__']
    exclude_files_list = ['generate_tree.py', 'file_tree.txt', 'README.md']
    
    # Define the output file name
    output_file = "file_tree.txt"
    
    generate_tree(start_path, output_file, exclude_dirs=exclude_dirs_list, exclude_files=exclude_files_list)
    
    print(f"File tree has been generated and saved to {output_file}")

import os

def generate_tree(startpath, excludes, file_output):
    with open(file_output, "w", encoding="utf-8") as f:
        for root, dirs, files in os.walk(startpath):
            # Exclude directories
            dirs[:] = [d for d in dirs if d not in excludes]
            
            level = root.replace(startpath, '').count(os.sep)
            indent = ' ' * 4 * (level)
            f.write(f'{indent}{os.path.basename(root)}/\n')
            subindent = ' ' * 4 * (level + 1)
            for file in files:
                if file not in excludes:
                    f.write(f'{subindent}{file}\n')

if __name__ == "__main__":
    # Get the absolute path of the script's directory
    start_path = os.path.abspath(os.path.dirname(__file__))
    
    # Define excludes at the root level
    exclude_list = ['.venv', 'generate_tree.py', 'file_tree.txt', 'README.md', '.git']
    
    # Define the output file name
    output_file = "file_tree.txt"
    
    generate_tree(start_path, exclude_list, output_file)

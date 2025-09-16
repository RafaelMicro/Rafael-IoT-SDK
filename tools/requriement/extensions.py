
import os

def install_extensions_from_file(file_path):
    try:
        with open(file_path, 'r') as file:
            extensions = file.readlines()
        
        for extension in extensions:
            extension = extension.strip()
            if extension:
                os.system(f"code --install-extension {extension}")
                print(f"Installed extension: {extension}")
    except Exception as e:
        print(f"Error: {e}")

# 指定txt文件的路徑
file_path = "extensions.txt"
install_extensions_from_file(file_path)


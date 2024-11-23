# cupid-clip

CupidClip is a terminal-based clipboard utility written in C. It reads the contents of files and directories, with an optional recursive mode, and copies the concatenated content to the system clipboard using `xclip`. 

This tool is designed for Linux systems, offering a simple way to batch copy text data from files or directories into the clipboard.

## Features

- **Single File Support:** Read and copy the content of a single file.
- **Directory Support:** Read and copy the content of all files in a specified directory.
- **Recursive Mode (`-R`):** Traverse directories recursively, reading and copying content from all nested files.
- **Clipboard Integration:** Uses `xclip` to copy content directly to the system clipboard.

## Prerequisites

To build and run CupidClip, you need the following:

- `gcc` (GNU Compiler Collection)
- `make` (optional but recommended for build automation)
- `xclip` (clipboard utility for Linux)

### Installing Dependencies on Arch Linux

Run the following command to install the necessary packages:

```bash
sudo pacman -S base-devel xclip
```

## Building the Project

To compile the project, run:

```bash
gcc -o cupid-clip cupid-clip.c
```

This will produce an executable named `cupid-clip`.

## Running the Program

### Basic Usage

```bash
./cupid-clip [options] <path>
```

### Options

- **No Options:** Process the specified file or directory non-recursively.
- **`-R`:** Enable recursive processing of directories.

### Examples

- **Copy a Single File to Clipboard:**
  ```bash
  ./cupid-clip /path/to/file.txt
  ```

- **Copy All Files in a Directory (Non-Recursive):**
  ```bash
  ./cupid-clip /path/to/directory
  ```

- **Copy All Files in a Directory and Subdirectories (Recursive):**
  ```bash
  ./cupid-clip -R /path/to/directory
  ```

## File Structure

- `cupid-clip.c`: Main source code for the clipboard utility.

## How It Works

1. **File or Directory Validation:** 
   - Verifies if the input is a valid file or directory.
   - Handles errors gracefully if the path is invalid.

2. **Content Aggregation:**
   - Reads the content of the specified file or files in a directory.
   - Appends all content into a temporary file (`/tmp/cupid_clip_temp.txt`).

3. **Clipboard Copying:**
   - Uses `xclip` to copy the concatenated content to the system clipboard.

4. **Cleanup:**
   - Deletes the temporary file after the operation.

## Example Workflow

### Scenario 1: Single File
1. Input: `/home/user/documents/example.txt`
2. Result: The content of `example.txt` is copied to the clipboard.

### Scenario 2: Directory (Non-Recursive)
1. Input: `/home/user/documents/`
2. Result: All files in the `documents` directory are read, and their content is concatenated and copied to the clipboard.

### Scenario 3: Directory (Recursive)
1. Input: `-R /home/user/documents/`
2. Result: All files in `documents` and its subdirectories are read, concatenated, and copied to the clipboard.

## Error Handling

- Displays an error if the specified path does not exist.
- Skips unreadable files while processing directories and logs warnings to the terminal.

## Requirements

- A Linux-based operating system
- Installed `xclip` utility
- Read permissions for files and directories

## Contributing

Contributions are welcome! If you'd like to improve CupidClip, feel free to:

1. Fork the repository.
2. Create a feature branch.
3. Submit a pull request.

## License

This project is licensed under the GNU General Public License v3.0.

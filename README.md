# cupid-clip

cupid-clip is a terminal-based clipboard utility written in C. It reads the contents of multiple files and directories, with optional recursive and verbose modes, and copies the concatenated content to the system clipboard using `xclip`.

This tool is designed for Linux systems, offering a simple way to batch copy text data from multiple files or directories into the clipboard.

## Features

- **Multiple File Support:** Read and copy the content of multiple files.
- **Directory Support:** Read and copy the content of all files in specified directories.
- **Recursive Mode (`-R`):** Traverse directories recursively, reading and copying content from all nested files.
- **Verbose Mode (`-V`):** Provides detailed output during processing.
- **Clipboard Integration:** Uses `xclip` to copy content directly to the system clipboard.
- **Completion Messages:** Always displays output upon completion to inform the user.

## Prerequisites

To build and run cupid-clip, you need the following:

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
./cupid-clip [options] <path1> [path2] [path3] ...
```

### Options

- **`-R`:** Enable recursive processing of directories.
- **`-V`:** Enable verbose mode for detailed output during processing.

### Examples

- **Copy Multiple Files to Clipboard:**

  ```bash
  ./cupid-clip file1.txt file2.txt file3.txt
  ```

- **Copy Multiple Directories (Non-Recursive):**

  ```bash
  ./cupid-clip dir1 dir2 dir3
  ```

- **Copy Multiple Files and Directories with Verbose Output:**

  ```bash
  ./cupid-clip -V file1.txt dir1 file2.txt
  ```

- **Copy Multiple Directories Recursively:**

  ```bash
  ./cupid-clip -R dir1 dir2
  ```

- **Combined Options and Multiple Paths:**

  ```bash
  ./cupid-clip -R -V file1.txt dir1 dir2 file2.txt
  ```

**Note:** Options can be placed anywhere in the command line.

## File Structure

- `cupid-clip.c`: Main source code for the clipboard utility.

## How It Works

1. **Argument Parsing:**

   - Parses command-line arguments to identify options (`-R`, `-V`) and collect paths to files or directories.
   - Supports multiple files and directories as input.

2. **File or Directory Validation:**

   - Verifies if each input path is a valid file or directory.
   - Handles errors gracefully if a path is invalid, and continues processing other paths.

3. **Content Aggregation:**

   - Reads the content of each specified file or files in directories.
   - Appends all content into a temporary file (`/tmp/cupid_clip_temp.txt`).

4. **Clipboard Copying:**

   - Uses `xclip` to copy the concatenated content to the system clipboard.

5. **Cleanup:**

   - Deletes the temporary file after the operation.

6. **Completion Message:**

   - Always displays a message upon completion, informing the user that the operation was successful.

## Example Workflows

### Scenario 1: Multiple Files

1. **Input:**

   ```bash
   ./cupid-clip /home/user/documents/file1.txt /home/user/documents/file2.txt
   ```

2. **Output:**

   ```
   Content copied to clipboard successfully.
   Operation completed successfully.
   ```

3. **Result:**

   - The content of `file1.txt` and `file2.txt` is concatenated and copied to the clipboard.

### Scenario 2: Multiple Directories (Non-Recursive)

1. **Input:**

   ```bash
   ./cupid-clip /home/user/documents/ /home/user/projects/
   ```

2. **Output:**

   ```
   Content copied to clipboard successfully.
   Operation completed successfully.
   ```

3. **Result:**

   - All files in `documents` and `projects` directories are read, concatenated, and copied to the clipboard.

### Scenario 3: Mixed Inputs with Verbose Mode

1. **Input:**

   ```bash
   ./cupid-clip -V /home/user/documents/file1.txt /home/user/projects/
   ```

2. **Output:**

   ```
   Processing file: /home/user/documents/file1.txt
   Processing directory: /home/user/projects/
   Reading file: /home/user/projects/project1.txt
   Reading file: /home/user/projects/project2.txt
   Content copied to clipboard successfully.
   Operation completed successfully.
   ```

3. **Result:**

   - Provides detailed output during processing.

## Error Handling

- Displays an error if a specified path does not exist, but continues processing other valid paths.
- Skips unreadable files while processing directories and logs warnings to the terminal.
- Provides informative messages if `xclip` is not installed or if there are issues copying to the clipboard.

## Requirements

- A Linux-based operating system
- Installed `xclip` utility
- Read permissions for files and directories

## Contributing

Contributions are welcome! If you'd like to improve cupid-clip, feel free to:

1. Fork the repository.
2. Create a feature branch.
3. Submit a pull request.

## License

This project is licensed under the GNU General Public License v3.0.

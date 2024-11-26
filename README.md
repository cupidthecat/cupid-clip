# cupid-clip

cupid-clip is a terminal-based clipboard utility written in C. It reads the contents of files and directories, with optional recursive and verbose modes, and copies the concatenated content to the system clipboard using `xclip`.

This tool is designed for Linux systems, offering a simple way to batch copy text data from files or directories into the clipboard.

## Features

- **Single File Support:** Read and copy the content of a single file.
- **Directory Support:** Read and copy the content of all files in a specified directory.
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
./cupid-clip [options] <path>
```

### Options

- **`-R`:** Enable recursive processing of directories.
- **`-V`:** Enable verbose mode for detailed output during processing.

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

- **Copy with Verbose Output:**

  ```bash
  ./cupid-clip -V /path/to/file_or_directory
  ```

- **Recursive Copy with Verbose Output:**

  ```bash
  ./cupid-clip -R -V /path/to/directory
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

5. **Completion Message:**

   - Always displays a message upon completion, informing the user that the operation was successful.

## Example Workflow

### Scenario 1: Single File

1. **Input:**

   ```bash
   ./cupid-clip /home/user/documents/example.txt
   ```

2. **Output:**

   ```
   Content copied to clipboard successfully.
   Operation completed successfully.
   ```

3. **Result:**

   - The content of `example.txt` is copied to the clipboard.

### Scenario 2: Directory (Non-Recursive)

1. **Input:**

   ```bash
   ./cupid-clip /home/user/documents/
   ```

2. **Output:**

   ```
   Content copied to clipboard successfully.
   Operation completed successfully.
   ```

3. **Result:**

   - All files in the `documents` directory are read, and their content is concatenated and copied to the clipboard.

### Scenario 3: Directory (Recursive)

1. **Input:**

   ```bash
   ./cupid-clip -R /home/user/documents/
   ```

2. **Output:**

   ```
   Content copied to clipboard successfully.
   Operation completed successfully.
   ```

3. **Result:**

   - All files in `documents` and its subdirectories are read, concatenated, and copied to the clipboard.

### Scenario 4: Verbose Mode

1. **Input:**

   ```bash
   ./cupid-clip -V /home/user/documents/
   ```

2. **Output:**

   ```
   Processing directory: /home/user/documents/
   Reading file: /home/user/documents/file1.txt
   Reading file: /home/user/documents/file2.txt
   Content copied to clipboard successfully.
   Operation completed successfully.
   ```

3. **Result:**

   - Provides detailed output during processing.

## Error Handling

- Displays an error if the specified path does not exist.
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

# cupid-clip

**cupid-clip** is a terminal-based clipboard utility written in C. It reads the contents of multiple files and directories, with optional recursive and verbose modes, and copies the concatenated content to the system clipboard using `xclip`.

This tool is designed for Linux systems, offering a simple way to batch copy text data from multiple files or directories into the clipboard.

## Features

- **Multiple File Support:** Read and copy the content of multiple files.
- **Directory Support:** Read and copy the content of all files in specified directories.
- **Recursive Mode (`-R`):** Traverse directories recursively, reading and copying content from all nested files.
- **Verbose Mode (`-V`):** Provides detailed output during processing.
- **Ignore Paths (`-I`):** Exclude specific files or directories from processing.
- **Clipboard Integration:** Uses `xclip` to copy content directly to the system clipboard.
- **Completion Messages:** Always displays output upon completion to inform the user.

## Todo

- **Config**: Configure files or directories to ignore instead of using the -I argument
- **Make install**: Add make install to move executable to bin folder to use terminal wide 

## Prerequisites

To build and run **cupid-clip**, you need the following:

- `gcc` (GNU Compiler Collection)
- `make` (optional but recommended for build automation)
- `xclip` (clipboard utility for Linux)

### Installing Dependencies on Arch Linux

Run the following command to install the necessary packages:

```bash
sudo pacman -S base-devel xclip
```

## Building with Makefile

1. **Build the Executable**

   In your project directory, run:

   ```bash
   make
   ```

   This compiles `cupid-clip.c` into the `cupid-clip` executable.

2. **Install the Executable**

   To install the executable to `/usr/bin` (so you can run it from anywhere), run:

   ```bash
   sudo make install
   ```

   *Note: `sudo` is required because writing to `/usr/bin` needs root privileges.*

3. **Clean the Build**

   To remove the compiled executable from your project directory, run:

   ```bash
   make clean
   ```

4. **Uninstall the Executable**

   If you wish to remove the installed executable from `/usr/bin`, run:

   ```bash
   sudo make uninstall
   ```

## Building with GCC

To compile the project, run:

```bash
gcc -o cupid-clip cupid-clip.c
```

This will produce an executable named `cupid-clip`.

To make `cupid-clip` executable from anywhere in the terminal, move the compiled binary to `/usr/bin`:

```bash
sudo mv cupid-clip /usr/bin
```

## Running the Program

### Basic Usage

```bash
./cupid-clip [options] <path1> [path2] [path3] ...
```

### Options

- **`-R`:** Enable recursive processing of directories.
- **`-V`:** Enable verbose mode for detailed output during processing.
- **`-I <file_or_dir>`:** **Ignore** a specific file or directory (and its subcontents, if it’s a directory).
  - **Important:** Each `-I` flag accepts **only one** path. To ignore multiple files or directories, use multiple `-I` flags, each followed by a single path.
  - By default, the ignore check uses *prefix* or *exact* matches, so any subdirectory or file within the ignored path is skipped.

> **Note:** Options can be placed anywhere in the command line. If you provide a path for `-I`, ensure you include an argument immediately after the `-I` flag. For example:
>
> ```bash
> ./cupid-clip -I /home/user/secret file1.txt
> ```

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

- **Ignore Specific Paths While Recursing:**

  ```bash
  ./cupid-clip -R -I "/home/frank/frankhagan.online/better/music/" -I "/home/frank/frankhagan.online/.idea/" /home/frank/frankhagan.online/better/
  ```

  Here, the directories `/home/frank/frankhagan.online/better/music/` and `/home/frank/frankhagan.online/.idea/` are **ignored** during the recursive processing of `/home/frank/frankhagan.online/better/`.

- **Combined Options and Multiple Paths:**

  ```bash
  ./cupid-clip -R -V -I "/path/to/ignore1" -I "/path/to/ignore2" file1.txt dir1 dir2
  ```

## File Structure

- `cupid-clip.c`: Main source code for the clipboard utility.

## How It Works

1. **Argument Parsing:**

   - Parses command-line arguments to identify options (`-R`, `-V`, `-I`) and collect paths to files or directories.
   - Supports multiple files and directories as input.
   - Each `-I` flag must be followed by **one** path to ignore.

2. **File or Directory Validation:**

   - Verifies if each input path is a valid file or directory.
   - Handles errors gracefully if a path is invalid and continues processing other paths.

3. **Ignoring Files or Directories (`-I`):**

   - If the path of a file or directory matches any ignore pattern (using prefix matching or exact match), it is skipped.
   - Useful for skipping large or sensitive directories when performing recursive operations.

4. **Content Aggregation:**

   - Reads the content of each specified file or files in directories (unless ignored).
   - Appends all content into a temporary file (`/tmp/cupid_clip_temp.txt`).

5. **Clipboard Copying:**

   - Uses `xclip` to copy the concatenated content to the system clipboard.

6. **Cleanup:**

   - Deletes the temporary file after the operation.

7. **Completion Message:**

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

### Scenario 4: Ignoring Specific Directories or Files

1. **Input (Recursive + Ignore):**

   ```bash
   ./cupid-clip -R -I "/home/user/docs/ignore_this/" -I "/home/user/docs/subdir/secret.txt" /home/user/docs/
   ```

2. **Output:**

   ```
   Processing directory: /home/user/docs (recursively)
   Ignoring: /home/user/docs/ignore_this
   Reading file: /home/user/docs/file1.txt
   Reading file: /home/user/docs/file2.txt
   Ignoring: /home/user/docs/subdir/secret.txt
   Content copied to clipboard successfully.
   Operation completed successfully.
   ```

3. **Result:**

   - Skips the entire `ignore_this/` directory and the `secret.txt` file while copying the rest.

## Error Handling

- **Missing Arguments:**
  
  - Displays an error if a specified path does not exist but continues processing other valid paths.
  - Example:
    
    ```
    Error: '/invalid/path' does not exist.
    ```

- **Unreadable Files:**
  
  - Skips unreadable files while processing directories and logs warnings to the terminal.
  - Example:
    
    ```
    Failed to open file: /path/to/unreadable_file.txt
    ```

- **Missing `xclip`:**
  
  - Provides an informative message if `xclip` is not installed or if there are issues copying to the clipboard.
  - Example:
    
    ```
    Failed to copy content to clipboard. Ensure xclip is installed.
    ```

- **Incorrect `-I` Usage:**
  
  - Alerts the user if the `-I` flag is provided without a corresponding path.
  - Example:
    
    ```
    Error: -I option requires an argument.
    ```

## Requirements

- A Linux-based operating system
- Installed `xclip` utility
- Read permissions for files and directories

## Todo

- Add a config for files that cupid-clip will always ignore

## Contributing

Contributions are welcome! If you'd like to improve **cupid-clip**, feel free to:

1. Fork the repository.
2. Create a feature branch.
3. Submit a pull request.

## License

This project is licensed under the GNU General Public License v3.0.

---

## Additional Recommendations

To further enhance the usability and robustness of **cupid-clip**, consider the following improvements:

1. **Use of `getopt` for Argument Parsing:**
   
   Implementing `getopt` or `getopt_long` can provide more flexible and error-resistant command-line argument parsing. This approach can simplify handling options and their associated arguments.

2. **Enhanced Logging:**
   
   Incorporate more detailed logging, especially when in verbose mode, to assist users in understanding which files are being processed or ignored.

3. **Pattern-Based Ignoring:**
   
   Allow users to specify patterns or wildcards for the `-I` option to ignore multiple files or directories that match certain criteria.

4. **Configuration Files:**
   
   Support reading ignore paths from a configuration file (similar to `.gitignore`), providing users with an alternative method to specify paths to exclude.

5. **Error Reporting Enhancements:**
   
   Provide more context in error messages, such as reasons for failures (e.g., permission issues) to aid in troubleshooting.

6. **Performance Optimizations:**
   
   Optimize file reading and writing operations for better performance, especially when dealing with large files or deeply nested directories.

By implementing these recommendations, **cupid-clip** can become even more powerful and user-friendly, catering to a broader range of use cases and enhancing overall user experience.

# SQLite CRUD demo (for Tenge integration)

This is a minimal SQLite-backed TODO CLI to accompany the Tenge project.  
It demonstrates how a small C “host” program can work with a simple schema and CLI,
paving the way for generating such hosts from Tenge (e.g., `crud_cli.tng`) later.

## Prerequisites

- SQLite dev library installed
  - **macOS (Homebrew):**
    ```bash
    brew install sqlite
    ```
  - **Linux (Debian/Ubuntu):**
    ```bash
    sudo apt-get update && sudo apt-get install -y sqlite3 libsqlite3-dev
    ```

## Build

From the project root:
```bash
make -C examples/crud_todos
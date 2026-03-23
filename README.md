# MetaCall C API MCP Server (Proof of Concept)

A lightweight, C-based Model Context Protocol (MCP) server that links directly against the MetaCall C API. This allows AI coding assistants (like GitHub Copilot) to load scripts and execute functions across multiple languages (Python, Node.js, Ruby, etc.) locally, without requiring a FaaS or HTTP server.

## 🎥 Video Demo

[https://github.com/user-attachments/assets/0cc0e453-0713-4103-a1df-74c81343459e](https://github.com/user-attachments/assets/0cc0e453-0713-4103-a1df-74c81343459e)

---

## ⚙️ Prerequisites

**For All Setups:**
* **VS Code & GitHub Copilot**: For testing the MCP host integration.

**For Docker Mode (Recommended/Cross-Platform):**
* **Docker**: Required to run the isolated MetaCall core environment and avoid local library path configurations.

**For Native Mode (Currently optimized for Ubuntu):**
* **gcc & make**: Standard C build tools.
* **MetaCall Core**: Installed locally via the official install script (which places headers and libraries in `/gnu/include` and `/gnu/lib`).

---

## 🛠️ Build Instructions

You can build the MCP server either as a containerized Docker image or as a native bare-metal binary.

### Option A: Docker Build
Builds a self-contained environment that works on any machine.
```bash
docker build -t metacall-mcp .
```

### Option B: Native Build
Compiles directly on your host machine (currently configured for GNU Guix paths on Ubuntu).
```bash
make clean && make
```

---

## 🧪 Testing via Terminal

You can verify that the JSON-RPC parsing, the MetaCall C API, and the type conversions work correctly by running the automated test script. This script feeds raw JSON payloads into the server via standard input.

Ensure the script is executable:
```bash
chmod +x test/scripts/test.sh
```

**Run the Docker version:**
```bash
./test/scripts/test.sh --docker
```

**Run the Native version:**
```bash
./test/scripts/test.sh --native
```

**What this does:** It tests the `initialize` handshake, requests `tools/list`, and executes the `load_script` and `call_function` tools against the included `math_utils.py` and `hello.py` test files.

---

## 🚀 Using with VS Code Copilot (MCP Host)

To give GitHub Copilot Agent autonomous access to your MetaCall runtime, you need to register the server using an `mcp.json` file. Our configuration supports both Docker and Native execution.

1. Create a `.vscode` folder in your workspace if it doesn't already exist.
2. Copy the sample configuration into your `.vscode` folder:
   ```bash
   mkdir -p .vscode
   cp .vscode.sample/mcp.json .vscode/mcp.json
   ```
3. Reload your VS Code window (`Ctrl+Shift+P` -> type `Developer: Reload Window`).
4. Open the Command Palette (`Ctrl+Shift+P`), type **`MCP: List Servers`**. You will see two options:
   * `metacall-mcp-docker`
   * `metacall-mcp-native`
5. Click the **Start** button next to the version you built. Click "Trust" when prompted by VS Code.
6. Open the **Copilot Chat panel**, switch to **Agent Mode**, and click the **Tools (wrench) icon**. Ensure both `load_script` and `call_function` are checked.

### Example Prompt
Once the server is running, you can test the end-to-end integration by sending Copilot a prompt like this:

> "Use your tools to load the script `math_utils.py`. Once it is loaded, call the `multiply` function with the arguments 5 and 10."

---

## 🧰 Available Tools

* **`load_script`**: Instructs MetaCall to ingest a file.
  * Arguments: `language` (e.g., "py", "node") and `files` (array of file paths).
* **`call_function`**: Executes a function currently loaded in MetaCall's memory.
  * Arguments: `function` (string name) and `args` (array of arguments to pass).
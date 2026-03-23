
# MetaCall C API MCP Server (Proof of Concept)

A lightweight, C-based Model Context Protocol (MCP) server that links directly against the MetaCall C API. This allows AI coding assistants (like GitHub Copilot) to load scripts and execute functions across multiple languages (Python, Node.js, Ruby, etc.) locally, without requiring a FaaS or HTTP server.


## ⚙️ Prerequisites

* **Docker**: Required to run the isolated MetaCall core environment and avoid complex local library path configurations.
* **VS Code & GitHub Copilot**: For testing the MCP host integration.

---

## 🛠️ Build Instructions

Because the C server links against `libmetacall.so`, we compile and run it inside a custom Docker image. Run this command in the root of the project to build the image:

docker build -t metacall-mcp .

---

## 🧪 Testing via Terminal

You can verify that the JSON-RPC parsing, the MetaCall C API, and the type conversions work correctly by running the automated test script. This script feeds raw JSON payloads into the Docker container via standard input.

Ensure the script is executable and run it:

chmod +x test/scripts/test.sh
./test/scripts/test.sh

**What this does:** It tests the `initialize` handshake, requests `tools/list`, and executes the `load_script` and `call_function` tools against the included `math_utils.py` and `hello.py` files.

---

## 🚀 Using with VS Code Copilot (MCP Host)

To give GitHub Copilot Agent autonomous access to your MetaCall runtime, you need to register the server using an `mcp.json` file. 

1. Create a `.vscode` folder in your workspace if it doesn't already exist.
2. Copy the sample configuration into your `.vscode` folder:
   
   mkdir -p .vscode
   cp .vscode.sample/mcp.json .vscode/mcp.json
   
3. Reload your VS Code window (`Ctrl+Shift+P` -> type `Developer: Reload Window`).
4. Open the Command Palette (`Ctrl+Shift+P`), type **`MCP: List Servers`**, and click the **Start** button next to `metacall-mcp`. Click "Trust" when prompted by VS Code.
5. Open the **Copilot Chat panel**, switch to **Agent Mode**, and click the **Tools (wrench) icon**. Ensure both `load_script` and `call_function` are checked.

### Example Prompt
Once the server is running, you can test the end-to-end integration by sending Copilot a prompt like this:

> "Use your tools to load the script `math_utils.py`. Once it is loaded, call the `multiply` function with the arguments 5 and 10."

---

## 🧰 Available Tools

* **`load_script`**: Instructs MetaCall to ingest a file.
  * Arguments: `language` (e.g., "py", "node") and `files` (array of file paths).
* **`call_function`**: Executes a function currently loaded in MetaCall's memory.
  * Arguments: `function` (string name) and `args` (array of arguments to pass).

## 🎥 Video Demo

https://github.com/user-attachments/assets/0cc0e453-0713-4103-a1df-74c81343459e

#!/bin/bash
# Usage: 
#   ./test/scripts/test.sh --docker  (Runs the containerized version)
#   ./test/scripts/test.sh --native  (Runs the local compiled binary)

set -e

if [ "$1" == "--native" ]; then
    echo "[info] Running in NATIVE mode..."
    export LD_LIBRARY_PATH=/gnu/lib:/usr/local/lib:${LD_LIBRARY_PATH:-}
    export LOADER_SCRIPT_PATH=$(pwd)/test/scripts
    BIN="./metacall-mcp"
else
    echo "[info] Running in DOCKER mode..."
    BIN="docker run -i --rm -e LOADER_SCRIPT_PATH=/app/test/scripts metacall-mcp"
fi

echo "=== Test 1: initialize ==="
echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"test"}}}' | $BIN

echo ""
echo "=== Test 2: tools/list ==="
echo '{"jsonrpc":"2.0","id":2,"method":"tools/list"}' | $BIN

echo ""
echo "=== Test 3: load + call (math_utils) ==="
printf '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"test"}}}\n{"jsonrpc":"2.0","id":2,"method":"notifications/initialized"}\n{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"load","arguments":{"language":"py","files":["math_utils.py"]}}}\n{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"call","arguments":{"function":"add","args":[2,4]}}}\n' | $BIN

echo ""
echo "=== Test 4: load + call (hello) ==="
printf '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"test"}}}\n{"jsonrpc":"2.0","id":2,"method":"notifications/initialized"}\n{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"load","arguments":{"language":"py","files":["hello.py"]}}}\n{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"call","arguments":{"function":"greet","args":["viferga"]}}}\n' | $BIN

echo ""
echo "done"
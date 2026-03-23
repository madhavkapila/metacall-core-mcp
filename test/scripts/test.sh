#!/bin/bash
# Pipe JSON-RPC messages to the Dockerized PoC and see if it responds correctly.
# Usage: ./test/scripts/test.sh

set -e

BIN="docker run -i --rm metacall-mcp"

echo "=== Test 1: initialize ==="
echo '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"test"}}}' | $BIN

echo ""
echo "=== Test 2: tools/list ==="
echo '{"jsonrpc":"2.0","id":2,"method":"tools/list"}' | $BIN

echo ""
echo "=== Test 3: load + call (math_utils) ==="
printf '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"test"}}}\n{"jsonrpc":"2.0","id":2,"method":"notifications/initialized"}\n{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"load_script","arguments":{"language":"py","files":["math_utils.py"]}}}\n{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"call_function","arguments":{"function":"add","args":[2,4]}}}\n' | $BIN

echo ""
echo "=== Test 4: load + call (hello) ==="
printf '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2024-11-05","capabilities":{},"clientInfo":{"name":"test"}}}\n{"jsonrpc":"2.0","id":2,"method":"notifications/initialized"}\n{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"load_script","arguments":{"language":"py","files":["hello.py"]}}}\n{"jsonrpc":"2.0","id":4,"method":"tools/call","params":{"name":"call_function","arguments":{"function":"greet","args":["Madhav"]}}}\n' | $BIN

echo ""
echo "done"
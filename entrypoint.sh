#!/bin/bash
# entrypoint.sh — Sets up MetaCall environment and runs the MCP server

export LD_LIBRARY_PATH=/usr/local/lib:${LD_LIBRARY_PATH:-}

export LOADER_SCRIPT_PATH=/app/test/scripts

echo "[entrypoint] LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >&2
echo "[entrypoint] LOADER_SCRIPT_PATH=$LOADER_SCRIPT_PATH" >&2
echo "[entrypoint] Starting metacall-mcp..." >&2

exec /app/metacall-mcp
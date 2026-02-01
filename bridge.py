#!/usr/bin/env python3
"""
Arctic Analytics - Serial to MongoDB Bridge
============================================

A dual-purpose server that:
1. Bridges Arduino sensor data to MongoDB Atlas via serial connection
2. Serves a REST API for the React dashboard to consume

The bridge reads JSON-formatted sensor data from an Arduino over serial,
processes it, and uploads session documents to MongoDB. It also provides
API endpoints for the dashboard to fetch stored session data.

UTRA Hacks 2026

Usage Examples:
    # Interactive mode - connect to Arduino and upload data
    python bridge.py

    # Specify serial port
    python bridge.py --port COM3

    # API server only (no serial connection)
    python bridge.py --api

    # Custom API port
    python bridge.py --api --api-port 8080

Author: Arctic Analytics Team
Version: 1.0.0
"""

# =============================================================================
# IMPORTS
# =============================================================================

import json
import time
import argparse
import os
from datetime import datetime

# Flask for REST API
from flask import Flask, jsonify, request
from flask_cors import CORS

# MongoDB driver
from pymongo import MongoClient

# Serial communication
import serial
import serial.tools.list_ports

# Threading for concurrent API server
import threading


# =============================================================================
# ENVIRONMENT CONFIGURATION
# =============================================================================

# Load environment variables from .env file if python-dotenv is installed
try:
    from dotenv import load_dotenv
    load_dotenv()
except ImportError:
    pass  # dotenv not installed, use system environment variables


# =============================================================================
# CONFIGURATION CONSTANTS
# =============================================================================

# MongoDB Atlas connection string (from .env or default)
# Removed URL 

# Database and collection names
DATABASE_NAME = "arctic_analytics"
COLLECTION_NAME = "sessions"

# Serial communication baud rate (must match Arduino firmware)
BAUD_RATE = 115200


# =============================================================================
# FLASK APPLICATION SETUP
# =============================================================================

# Initialize Flask app
app = Flask(__name__)

# Enable CORS for all routes (allows dashboard to make cross-origin requests)
CORS(app)


# =============================================================================
# MONGODB CONNECTION
# =============================================================================

# Global MongoDB connection objects
mongo_client = None
db = None
sessions_collection = None


def connect_mongo():
    """
    Establish connection to MongoDB Atlas.

    Uses TLS/SSL with certifi for secure connection on Windows.
    Sets up global database and collection references.

    Returns:
        bool: True if connection successful, False otherwise
    """
    global mongo_client, db, sessions_collection

    try:
        # Import certifi for SSL certificate handling on Windows
        import certifi

        # Create MongoDB client with TLS enabled
        mongo_client = MongoClient(
            MONGO_URI,
            tls=True,
            tlsCAFile=certifi.where()
        )

        # Get database and collection references
        db = mongo_client[DATABASE_NAME]
        sessions_collection = db[COLLECTION_NAME]

        # Test connection with ping command
        mongo_client.admin.command('ping')
        print("[MongoDB] Connected successfully!")
        return True

    except Exception as e:
        print(f"[MongoDB] Connection failed: {e}")
        print("[MongoDB] Running in offline mode")
        return False


# =============================================================================
# SERIAL COMMUNICATION FUNCTIONS
# =============================================================================

def list_ports():
    """
    List all available serial ports on the system.

    Prints a numbered list of ports for user selection.

    Returns:
        list: List of serial port objects
    """
    ports = serial.tools.list_ports.comports()

    if not ports:
        print("No serial ports found!")
        return []

    print("\nAvailable ports:")
    for i, port in enumerate(ports):
        print(f"  [{i}] {port.device} - {port.description}")

    return ports


def read_json_from_serial(ser):
    """
    Read and parse JSON data from Arduino over serial.

    Waits for JSON data delimited by "END_JSON" marker.
    Handles multi-line JSON and parsing errors.

    Args:
        ser: Serial port object

    Returns:
        dict: Parsed JSON data, or None if parsing failed
    """
    print("[Serial] Waiting for JSON data...")
    json_lines = []
    capturing = False

    # Read lines until END_JSON marker
    while True:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8').strip()

            # Check for end marker
            if line == "END_JSON":
                break
            # Start capturing at opening brace
            elif line.startswith("{"):
                capturing = True
                json_lines.append(line)
            # Continue capturing after start
            elif capturing:
                json_lines.append(line)

        time.sleep(0.01)  # Prevent CPU spinning

    # Join lines and parse JSON
    json_str = "\n".join(json_lines)
    try:
        return json.loads(json_str)
    except json.JSONDecodeError as e:
        print(f"[Serial] JSON parse error: {e}")
        return None


def upload_session(data, robot_id="arctic-1", notes=""):
    """
    Upload a sensor session to MongoDB.

    Constructs a session document from Arduino data and inserts
    it into the sessions collection.

    Args:
        data: Parsed JSON data from Arduino containing:
            - duration_ms: Session duration in milliseconds
            - reading_count: Number of sensor readings
            - color_summary: Dict with red/green/blue/unknown counts
            - line_summary: Dict with white/black counts
            - readings: Array of individual sensor readings
        robot_id: Identifier for the robot (default: "arctic-1")
        notes: Optional session notes

    Returns:
        str: Inserted document ID, or None if upload failed
    """
    # Check database connection
    if sessions_collection is None:
        print("[MongoDB] Not connected - skipping upload")
        return None

    # Build session document matching dashboard schema
    session_doc = {
        "robot_id": robot_id,
        "timestamp": datetime.utcnow(),
        "duration_ms": data.get("duration_ms", 0),
        "reading_count": data.get("reading_count", 0),
        "notes": notes,

        # Color detection summary for pie chart
        "color_summary": data.get("color_summary", {
            "red": 0, "green": 0, "blue": 0, "unknown": 0
        }),

        # Line following summary for pie chart
        "line_summary": data.get("line_summary", {
            "white": 0, "black": 0
        }),

        # Raw sensor readings array for time-series charts
        # Each reading: {t, r, g, b, color, line, dist, ml, mr}
        "readings": data.get("readings", [])
    }

    # Insert document and return ID
    result = sessions_collection.insert_one(session_doc)
    print(f"[MongoDB] Uploaded session: {result.inserted_id}")
    return str(result.inserted_id)


# =============================================================================
# INTERACTIVE MODE
# =============================================================================

def interactive_mode(port=None):
    """
    Run interactive serial communication with Arduino.

    Provides a command-line interface for controlling Arduino
    sessions and uploading data to MongoDB.

    Commands:
        s - Start recording session on Arduino
        e - End recording session
        d - Dump data and upload to MongoDB
        c - Clear Arduino memory
        q - Quit

    Args:
        port: Serial port device name (e.g., "COM3"), or None for selection
    """
    # List available ports
    ports = list_ports()

    # Port selection
    if port is None:
        if not ports:
            return
        choice = input("\nSelect port number (or Enter for first): ").strip()
        port = ports[int(choice) if choice else 0].device

    print(f"\n[Serial] Connecting to {port}...")

    try:
        # Open serial connection
        ser = serial.Serial(port, BAUD_RATE, timeout=1)
        time.sleep(2)  # Wait for Arduino to reset
        print("[Serial] Connected!")

        # Flush any pending output
        while ser.in_waiting:
            print(f"[Arduino] {ser.readline().decode('utf-8').strip()}")

        # Print command help
        print("\nCommands:")
        print("  s - Start session")
        print("  e - End session")
        print("  d - Dump & upload data")
        print("  c - Clear data")
        print("  q - Quit")

        # Get robot identifier
        robot_id = input("\nRobot ID [arctic-1]: ").strip() or "arctic-1"

        # Main command loop
        while True:
            cmd = input("\n> ").strip().lower()

            if cmd == 'q':
                # Quit
                break

            elif cmd == 's':
                # Start session - send 'S' command to Arduino
                ser.write(b'S')
                print("[Arduino] Starting session...")

            elif cmd == 'e':
                # End session - send 'E' command to Arduino
                ser.write(b'E')
                print("[Arduino] Ending session...")

            elif cmd == 'd':
                # Dump data - request JSON from Arduino and upload
                ser.write(b'D')
                data = read_json_from_serial(ser)

                if data:
                    # Display summary
                    print(f"\n[Data] Received {data.get('reading_count', 0)} readings")
                    print(f"  Colors: R={data.get('color_summary', {}).get('red', 0)}, "
                          f"G={data.get('color_summary', {}).get('green', 0)}, "
                          f"B={data.get('color_summary', {}).get('blue', 0)}")
                    print(f"  Line: W={data.get('line_summary', {}).get('white', 0)}, "
                          f"B={data.get('line_summary', {}).get('black', 0)}")

                    # Get optional notes and upload
                    notes = input("Session notes (optional): ").strip()
                    session_id = upload_session(data, robot_id, notes)
                    if session_id:
                        print(f"[Success] Session saved: {session_id}")

            elif cmd == 'c':
                # Clear Arduino memory
                ser.write(b'C')
                print("[Arduino] Clearing data...")

            # Print any Arduino responses
            time.sleep(0.1)
            while ser.in_waiting:
                print(f"[Arduino] {ser.readline().decode('utf-8').strip()}")

    except serial.SerialException as e:
        print(f"[Serial] Error: {e}")

    finally:
        # Close serial connection
        if 'ser' in locals():
            ser.close()


# =============================================================================
# REST API ENDPOINTS
# =============================================================================

@app.route('/api/sessions', methods=['GET'])
def get_sessions():
    """
    Get list of all sessions (without full readings array).

    Returns last 50 sessions sorted by timestamp descending.
    Excludes the readings array for performance.

    Returns:
        JSON array of session objects
    """
    if sessions_collection is None:
        return jsonify({"error": "Database not connected"}), 503

    # Query sessions, excluding large readings array
    sessions = list(sessions_collection.find(
        {},
        {"readings": 0}  # Projection: exclude readings
    ).sort("timestamp", -1).limit(50))

    # Convert ObjectId to string
    for s in sessions:
        s["_id"] = str(s["_id"])

    return jsonify(sessions)


@app.route('/api/sessions/<session_id>', methods=['GET'])
def get_session(session_id):
    """
    Get a single session with full data including readings.

    Args:
        session_id: MongoDB ObjectId as string

    Returns:
        JSON session object with full readings array
    """
    if sessions_collection is None:
        return jsonify({"error": "Database not connected"}), 503

    from bson import ObjectId

    # Find session by ID
    session = sessions_collection.find_one({"_id": ObjectId(session_id)})
    if not session:
        return jsonify({"error": "Session not found"}), 404

    session["_id"] = str(session["_id"])
    return jsonify(session)


@app.route('/api/latest', methods=['GET'])
def get_latest():
    """
    Get the most recent session.

    Used by the dashboard to display current/latest data.

    Returns:
        JSON session object with full data
    """
    if sessions_collection is None:
        return jsonify({"error": "Database not connected"}), 503

    # Find most recent session
    session = sessions_collection.find_one(sort=[("timestamp", -1)])
    if not session:
        return jsonify({"error": "No sessions found"}), 404

    session["_id"] = str(session["_id"])
    return jsonify(session)


@app.route('/api/stats', methods=['GET'])
def get_stats():
    """
    Get aggregated statistics across all sessions.

    Uses MongoDB aggregation pipeline to sum up:
    - Total sessions count
    - Total duration
    - Color detection totals
    - Line detection totals

    Returns:
        JSON object with aggregated statistics
    """
    if sessions_collection is None:
        return jsonify({"error": "Database not connected"}), 503

    # Get total session count
    total = sessions_collection.count_documents({})

    # Aggregate totals using pipeline
    pipeline = [
        {"$group": {
            "_id": None,
            "total_red": {"$sum": "$color_summary.red"},
            "total_green": {"$sum": "$color_summary.green"},
            "total_blue": {"$sum": "$color_summary.blue"},
            "total_white": {"$sum": "$line_summary.white"},
            "total_black": {"$sum": "$line_summary.black"},
            "total_duration": {"$sum": "$duration_ms"}
        }}
    ]

    result = list(sessions_collection.aggregate(pipeline))
    stats = result[0] if result else {}

    return jsonify({
        "total_sessions": total,
        "total_duration_ms": stats.get("total_duration", 0),
        "color_totals": {
            "red": stats.get("total_red", 0),
            "green": stats.get("total_green", 0),
            "blue": stats.get("total_blue", 0)
        },
        "line_totals": {
            "white": stats.get("total_white", 0),
            "black": stats.get("total_black", 0)
        }
    })


# =============================================================================
# MAIN ENTRY POINT
# =============================================================================

if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Arctic Analytics Bridge - Serial to MongoDB bridge and API server"
    )
    parser.add_argument(
        "--port",
        help="Serial port device (e.g., COM3, /dev/ttyUSB0)"
    )
    parser.add_argument(
        "--api",
        action="store_true",
        help="Run API server only (no serial connection)"
    )
    parser.add_argument(
        "--api-port",
        type=int,
        default=5000,
        help="Port for API server (default: 5000)"
    )
    args = parser.parse_args()

    # Print banner
    print("=" * 50)
    print("  Arctic Analytics - Bridge Server")
    print("  UTRA Hacks 2026")
    print("=" * 50)

    # Connect to MongoDB
    connect_mongo()

    if args.api:
        # API-only mode: just run Flask server
        print(f"\n[API] Starting on http://localhost:{args.api_port}")
        app.run(host="0.0.0.0", port=args.api_port, debug=True)
    else:
        # Full mode: run API server in background thread, interactive in foreground
        api_thread = threading.Thread(
            target=lambda: app.run(
                host="0.0.0.0",
                port=args.api_port,
                debug=False,
                use_reloader=False
            ),
            daemon=True  # Thread dies when main program exits
        )
        api_thread.start()
        print(f"[API] Running on http://localhost:{args.api_port}")

        # Start interactive serial mode
        interactive_mode(args.port)

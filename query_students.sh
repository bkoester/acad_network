#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

SEGMENT_BINARY="${SCRIPT_DIR}/build/src/query"
STUDENT_FILE="${SCRIPT_DIR}/../data/student.tab"
ENROLLMENT_FILE="${SCRIPT_DIR}/../data/enrollment.tab"


$SEGMENT_BINARY --student_file="$STUDENT_FILE" \
	--enrollment_file="$ENROLLMENT_FILE"

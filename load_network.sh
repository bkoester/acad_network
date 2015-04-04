#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

LOAD_BINARY="${SCRIPT_DIR}/build/src/load"
STUDENT_ARCHIVE_PATH="${SCRIPT_DIR}/../data/student_archive.txt"
COURSE_ARCHIVE_PATH="${SCRIPT_DIR}/../data/course_archive.txt"
STUDENT_NETWORK_ARCHIVE_PATH="${SCRIPT_DIR}/../data/student_network.txt"
COURSE_NETWORK_ARCHIVE_PATH="${SCRIPT_DIR}/../data/course_network.txt"

$LOAD_BINARY ----student_archive_path="${STUDENT_ARCHIVE_PATH}" \
	--course_archive_path="${COURSE_ARCHIVE_PATH}" --network_to_load=student \
	--student_network_archive_path="$STUDENT_NETWORK_ARCHIVE_PATH" \
	--course_network_archive_path="$COURSE_NETWORK_ARCHIVE_PATH"

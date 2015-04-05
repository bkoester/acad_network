#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

SEGMENT_BINARY="${SCRIPT_DIR}/build/src/query"
STUDENT_ARCHIVE_PATH="${SCRIPT_DIR}/../data/student_archive.txt"
COURSE_ARCHIVE_PATH="${SCRIPT_DIR}/../data/course_archive.txt"


$SEGMENT_BINARY --student_archive_path="${STUDENT_ARCHIVE_PATH}" \
	--course_archive_path="${COURSE_ARCHIVE_PATH}"

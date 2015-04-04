#! /bin/bash

SCRIPT_DIR=$(cd "$(dirname "$0")" && pwd)

SERIALIZE_BINARY="${SCRIPT_DIR}/build/src/serialize"
STUDENT_FILE="${SCRIPT_DIR}/../data/student.tab"
ENROLLMENT_FILE="${SCRIPT_DIR}/../data/enrollment.tab"
STUDENT_ARCHIVE_PATH="${SCRIPT_DIR}/../data/student_archive.txt"
COURSE_ARCHIVE_PATH="${SCRIPT_DIR}/../data/course_archive.txt"


$SERIALIZE_BINARY --student_file="$STUDENT_FILE" \
	--enrollment_file="$ENROLLMENT_FILE" \
	--student_archive_path="${STUDENT_ARCHIVE_PATH}" \
	--course_archive_path="${COURSE_ARCHIVE_PATH}"

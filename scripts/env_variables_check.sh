CHECK=1
if [[ -z "${JAVA_HOME}" ]]; then
  echo "JAVA_HOME environment variable is not set"
  CHECK=0
else
  echo $JAVA_HOME
fi
if [[ -z "${DOCUMENTDB_HOME}" ]]; then
  echo "DOCUMENTDB_HOME environment variable is not set"
  CHECK=0
else
  echo $DOCUMENTDB_HOME
fi
if [[ -z "${ODBC_LIB_PATH}" ]]; then
  echo "ODBC_LIB_PATH environment variable is not set"
  CHECK=0
else
  echo $ODBC_LIB_PATH
fi

if [[ -z "${REPOSITORY_ROOT}" ]]; then
  echo "REPOSITORY_ROOT environment variable is not set"
  CHECK=0
else
  echo "REPOSITORY_ROOT=${ROOT_DIRECTORY}"
fi

if [[ "${CHECK}" -eq "0" ]]; then
  echo "Missing envrionment variables, please set them accordingly."
else
  echo "Environment variables are all set"
fi

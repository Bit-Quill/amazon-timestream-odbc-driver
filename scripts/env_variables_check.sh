CHECK=1
if [[ -z "${JAVA_HOME}" ]]; then
  echo "JAVA_HOME environment variable is not set"
  CHECK=0
else
  echo "JAVA_HOME=${JAVA_HOME}"
fi

if [[ -z "${AWS_ACCESS_KEY_ID}" ]]; then
  echo "AWS_ACCESS_KEY_ID environment variable is not set"
  CHECK=0
else
  echo "AWS_ACCESS_KEY_ID=${AWS_ACCESS_KEY_ID}"
fi

if [[ -z "${AWS_SECRET_ACCESS_KEY}" ]]; then
  echo "AWS_SECRET_ACCESS_KEY environment variable is not set"
  CHECK=0
else
  echo "AWS_SECRET_ACCESS_KEY=***"
fi

if [[ -z "${ODBC_LIB_PATH}" ]]; then
  echo "ODBC_LIB_PATH environment variable is not set"
  CHECK=0
else
  echo "ODBC_LIB_PATH=${ODBC_LIB_PATH}"
fi

if [[ "${CHECK}" -eq "0" ]]; then
  echo "Missing envrionment variables, please set them accordingly."
else
  echo "Environment variables are all set"
fi

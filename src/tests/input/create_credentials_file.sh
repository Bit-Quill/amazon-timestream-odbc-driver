REPO_ROOT=$REPOSITORY_ROOT
ENV_ACCESS_ID=$AWS_ACCESS_KEY_ID
ENV_SECRET_KEY=$AWS_SECRET_ACCESS_KEY

if [ -z "$ENV_ACCESS_ID" ]; then
   echo "Environment variable AWS_ACCESS_KEY_ID is null or empty."
   MISSING_VARS=1
fi

if [ -z "$ENV_SECRET_KEY" ]; then
   echo "Environment variable AWS_SECRET_ACCESS_KEY is null or empty."
   MISSING_VARS=1
fi

if [ -z "$REPO_ROOT" ]; then
   echo "Environment variable REPOSITORY_ROOT is null or empty."
   MISSING_VARS=1
fi

if [[ "${MISSING_VARS}" -eq "1" ]]; then
  echo "Missing envrionment variables, please set them accordingly. Exiting"
  exit 1
fi

# write credentials files

echo "[default]" > $REPO_ROOT/src/tests/input/credentials
echo "aws_access_key_id = $ENV_ACCESS_ID" >> $REPO_ROOT/src/tests/input/credentials
echo "aws_secret_access_key = $ENV_SECRET_KEY" >> $REPO_ROOT/src/tests/input/credentials

echo "[default]" > $REPO_ROOT/src/tests/input/incomplete_credentials
echo "aws_access_key_id = $ENV_ACCESS_ID" >> $REPO_ROOT/src/tests/input/incomplete_credentials

# retrieve access key id and secret key from environment variables
$REPO_ROOT = [System.Environment]::GetEnvironmentVariable('REPOSITORY_ROOT')
$ENV_ACCESS_ID = [System.Environment]::GetEnvironmentVariable('AWS_ACCESS_KEY_ID')
$ENV_SECRET_KEY = [System.Environment]::GetEnvironmentVariable('AWS_SECRET_ACCESS_KEY')

If ($ENV_ACCESS_ID -eq $null -or $ENV_ACCESS_ID -eq "") {
    Write-Host "Environment variable AWS_ACCESS_KEY_ID is null or empty."
    $MISSING_VARS = 1
}

If ($ENV_SECRET_KEY -eq $null -or $ENV_SECRET_KEY -eq "") {
    Write-Host "Environment variable AWS_SECRET_ACCESS_KEY is null or empty."
    $MISSING_VARS = 1
}

If ($REPO_ROOT -eq $null -or $REPO_ROOT -eq "") {
    Write-Host "Environment variable REPOSITORY_ROOT is null or empty."
    $MISSING_VARS = 1
}

If ($MISSING_VARS -eq 1) {
    Write-Host "Missing environment variables, please set them accordingly. Exiting."
    Exit 1
}

Write-Output "[default]" > $REPO_ROOT\src\tests\input\credentials
Write-Output "aws_access_key_id = $ENV_ACCESS_ID" >> $REPO_ROOT\src\tests\input\credentials
Write-Output "aws_secret_access_key = $ENV_SECRET_KEY" >> $REPO_ROOT\src\tests\input\credentials

Write-Output "[default]" > $REPO_ROOT\src\tests\input\incomplete_credentials
Write-Output "aws_access_key_id = $ENV_ACCESS_ID" >> $REPO_ROOT\src\tests\input\incomplete_credentials

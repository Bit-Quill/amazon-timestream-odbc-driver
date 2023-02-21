$CONFIGURATION = $args[0]
$WIN_ARCH = $args[1]
$SRC_DIR = $args[2]
$BUILD_DIR = $args[3]
$INSTALL_DIR = $args[4]

Write-Host $args

# Clone the AWS SDK CPP repo
git config --global core.longpaths true
git clone --recurse-submodules -b "1.9.220" "https://github.com/aws/aws-sdk-cpp.git" $SRC_DIR

# Make and move to build directory
New-Item -Path $BUILD_DIR -ItemType Directory -Force | Out-Null
Set-Location $BUILD_DIR

# Configure and build 
cmake $SRC_DIR `
    -A $WIN_ARCH `
    -D CMAKE_INSTALL_PREFIX=$INSTALL_DIR `
    -D CMAKE_BUILD_TYPE=$CONFIGURATION `
    -D BUILD_ONLY="core;sts;timestream-query;timestream-write" `
    -D ENABLE_UNITY_BUILD="ON" `
    -D CUSTOM_MEMORY_MANAGEMENT="OFF" `
    -D ENABLE_RTTI="OFF" `
    -D ENABLE_TESTING="OFF" `
    -D CPP_STANDARD="17"

# Build AWS SDK and install to $INSTALL_DIR 
$msbuild = &"${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe" -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe | select-object -first 1
&$msbuild ALL_BUILD.vcxproj /m /p:Configuration=$CONFIGURATION
&$msbuild INSTALL.vcxproj /m /p:Configuration=$CONFIGURATION
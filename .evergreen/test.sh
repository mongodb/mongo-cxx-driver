#!/usr/bin/env bash

set -o errexit
set -o pipefail

: "${build_type:?}"
: "${cse_aws_access_key_id:?}"
: "${cse_aws_secret_access_key:?}"
: "${cse_azure_client_id:?}"
: "${cse_azure_client_secret:?}"
: "${cse_azure_tenant_id:?}"
: "${cse_gcp_email:?}"
: "${cse_gcp_privatekey:?}"
: "${distro_id:?}"

: "${CRYPT_SHARED_LIB_PATH:-}"
: "${disable_slow_tests:-}"
: "${example_projects_cc:-}"
: "${example_projects_cxx_standard:-}"
: "${example_projects_cxx:-}"
: "${example_projects_cxxflags:-}"
: "${example_projects_ldflags:-}"
: "${generator:-}"
: "${lib_dir:-}"
: "${MONGODB_API_VERSION:-}"
: "${TEST_WITH_ASAN:-}"
: "${TEST_WITH_UBSAN:-}"
: "${TEST_WITH_VALGRIND:-}"
: "${use_mongocryptd:-}"
: "${USE_STATIC_LIBS:-}"

# Grabs the mongocryptd path
pushd ..
export MONGOCRYPTD_PATH=$(pwd)/
if [[ "$OSTYPE" =~ cygwin ]]; then
  export MONGOCRYPTD_PATH=$(cygpath -m $MONGOCRYPTD_PATH)
fi
popd # ..

# Add MSBuild.exe to path.
if [[ "$OSTYPE" == "cygwin" ]]; then
  case "${generator}" in
  *2015*)
    PATH="/cygdrive/c/cmake/bin:/cygdrive/c/Program Files (x86)/MSBuild/14.0/Bin:$PATH"
    ;;
  *2017*)
    PATH="/cygdrive/c/cmake/bin:/cygdrive/c/Program Files (x86)/Microsoft Visual Studio/2017/Professional/MSBuild/15.0/Bin:$PATH"
    ;;
  esac
fi
export PATH

pushd build
export PREFIX=$(pwd)/../../mongoc

# Use PATH / LD_LIBRARY_PATH / DYLD_LIBRARY_PATH to inform the tests where to find
# mongoc library dependencies on Windows / Linux / Mac OS, respectively.
# Additionally, on Windows, we also need to inform the tests where to find
# mongocxx library dependencies.
if [ -n "${lib_dir}" ]; then
  export LD_LIBRARY_PATH=.:$PREFIX/${lib_dir}/
  export DYLD_LIBRARY_PATH=.:$PREFIX/${lib_dir}/
else
  export LD_LIBRARY_PATH=.:$PREFIX/lib/
  export DYLD_LIBRARY_PATH=.:$PREFIX/lib/
fi
PATH="$(pwd)/src/mongocxx/test/${build_type}:$PATH"
PATH="$(pwd)/src/bsoncxx/test/${build_type}:$PATH"
PATH="$(pwd)/src/mongocxx/${build_type}:$PATH"
PATH="$(pwd)/src/bsoncxx/${build_type}:$PATH"
PATH="$PREFIX/bin:$PATH"
PATH="$(pwd)/install/bin:$PATH"

export CHANGE_STREAMS_UNIFIED_TESTS_PATH="$(pwd)/../data/change-streams/unified"
export CLIENT_SIDE_ENCRYPTION_LEGACY_TESTS_PATH="$(pwd)/../data/client_side_encryption/legacy"
export CLIENT_SIDE_ENCRYPTION_TESTS_PATH="$(pwd)/../data/client_side_encryption"
export CLIENT_SIDE_ENCRYPTION_UNIFIED_TESTS_PATH="$(pwd)/../data/client_side_encryption/unified"
export COLLECTION_MANAGEMENT_TESTS_PATH="$(pwd)/../data/collection-management"
export COMMAND_MONITORING_TESTS_PATH="$(pwd)/../data/command-monitoring"
export CRUD_LEGACY_TESTS_PATH="$(pwd)/../data/crud/legacy"
export CRUD_UNIFIED_TESTS_PATH="$(pwd)/../data/crud/unified"
export GRIDFS_TESTS_PATH="$(pwd)/../data/gridfs"
export INITIAL_DNS_SEEDLIST_DISCOVERY_TESTS_PATH="$(pwd)/../data/initial_dns_seedlist_discovery"
export READ_WRITE_CONCERN_OPERATION_TESTS_PATH="$(pwd)/../data/read-write-concern/operation"
export RETRYABLE_READS_LEGACY_TESTS_PATH="$(pwd)/../data/retryable-reads/legacy"
export RETRYABLE_READS_UNIFIED_TESTS_PATH="$(pwd)/../data/retryable-reads/unified"
export RETRYABLE_WRITES_UNIFIED_TESTS_PATH="$(pwd)/../data/retryable-writes/unified"
export SESSION_UNIFIED_TESTS_PATH="$(pwd)/../data/sessions/unified"
export TRANSACTIONS_LEGACY_TESTS_PATH="$(pwd)/../data/transactions/legacy"
export TRANSACTIONS_UNIFIED_TESTS_PATH="$(pwd)/../data/transactions/unified"
export UNIFIED_FORMAT_TESTS_PATH=$(pwd)/../data/unified-format
export URI_OPTIONS_TESTS_PATH="$(pwd)/../data/uri-options"
export VERSIONED_API_TESTS_PATH=$(pwd)/../data/versioned-api
export WITH_TRANSACTION_TESTS_PATH="$(pwd)/../data/with_transaction"
export INDEX_MANAGEMENT_TESTS_PATH="$(pwd)/../data/index-management"

export MONGODB_API_VERSION="${MONGODB_API_VERSION}"

pushd ../../drivers-evergreen-tools
export DRIVERS_TOOLS=$(pwd)
if [[ "$OSTYPE" =~ cygwin ]]; then
  export DRIVERS_TOOLS=$(cygpath -m $DRIVERS_TOOLS)
fi
popd # ../../drivers-evergreen-tools

export MONGOCXX_TEST_TLS_CA_FILE="$DRIVERS_TOOLS/.evergreen/x509gen/ca.pem"

if [ "$(uname -m)" == "ppc64le" ]; then
  echo "Skipping CSFLE test setup (CDRIVER-4246/CXX-2423)"
else
  # export environment variables for encryption tests
  set +o errexit

  # Avoid printing credentials in logs.
  set +o xtrace

  echo "Setting temporary credentials..."
  pushd "$DRIVERS_TOOLS/.evergreen/csfle"
  export AWS_SECRET_ACCESS_KEY="${cse_aws_secret_access_key}"
  export AWS_ACCESS_KEY_ID="${cse_aws_access_key_id}"
  export AWS_DEFAULT_REGION="us-east-1"
  echo "Running activate-kmstlsvenv.sh..."
  . ./activate-kmstlsvenv.sh
  echo "Running activate-kmstlsvenv.sh... done."
  echo "Running set-temp-creds.sh..."
  . ./set-temp-creds.sh
  echo "Running set-temp-creds.sh... done."
  deactivate
  popd # "$DRIVERS_TOOLS/.evergreen/csfle"
  echo "Setting temporary credentials... done."

  # Ensure temporary credentials were properly set.
  if [ -z "$CSFLE_AWS_TEMP_ACCESS_KEY_ID" ]; then
    echo "Failed to set temporary credentials!"
    exit 1
  fi

  export MONGOCXX_TEST_CSFLE_TLS_CA_FILE=$DRIVERS_TOOLS/.evergreen/x509gen/ca.pem
  export MONGOCXX_TEST_CSFLE_TLS_CERTIFICATE_KEY_FILE=$DRIVERS_TOOLS/.evergreen/x509gen/client.pem
  export MONGOCXX_TEST_AWS_TEMP_ACCESS_KEY_ID="$CSFLE_AWS_TEMP_ACCESS_KEY_ID"
  export MONGOCXX_TEST_AWS_TEMP_SECRET_ACCESS_KEY="$CSFLE_AWS_TEMP_SECRET_ACCESS_KEY"
  export MONGOCXX_TEST_AWS_TEMP_SESSION_TOKEN="$CSFLE_AWS_TEMP_SESSION_TOKEN"
  export MONGOCXX_TEST_AWS_SECRET_ACCESS_KEY="${cse_aws_secret_access_key}"
  export MONGOCXX_TEST_AWS_ACCESS_KEY_ID="${cse_aws_access_key_id}"
  export MONGOCXX_TEST_AZURE_TENANT_ID="${cse_azure_tenant_id}"
  export MONGOCXX_TEST_AZURE_CLIENT_ID="${cse_azure_client_id}"
  export MONGOCXX_TEST_AZURE_CLIENT_SECRET="${cse_azure_client_secret}"
  export MONGOCXX_TEST_GCP_EMAIL="${cse_gcp_email}"
  export MONGOCXX_TEST_GCP_PRIVATEKEY="${cse_gcp_privatekey}"

  set -o errexit
fi

if [ "$(uname -m)" == "ppc64le" ]; then
  echo "Skipping CSFLE test setup (CDRIVER-4246/CXX-2423)"
else
  # Register CA certificate required by KMS TLS connections.
  echo "Registering CA certificate for KMS TLS tests..."
  register_ca_cert() {
    local OS=$(uname -s | tr '[:upper:]' '[:lower:]')
    echo "register_ca_cert: OS: $OS"
    case "$OSTYPE" in
    cygwin*)
      certutil.exe -addstore "Root" "$DRIVERS_TOOLS\.evergreen\x509gen\ca.pem"
      ;;
    darwin*)
      sudo security add-trusted-cert -d -r trustRoot -k /Library/Keychains/System.keychain $DRIVERS_TOOLS/.evergreen/x509gen/ca.pem
      ;;
    *)
      if [ -f /etc/redhat-release ]; then
        # CSE KMS TLS tests are disabled on RHEL due to sudo permissions. See BUILD-14068.
        # sudo cp -v $DRIVERS_TOOLS/.evergreen/x509gen/ca.pem /usr/share/pki/ca-trust-source/anchors/cdriver.crt
        # sudo update-ca-trust extract --verbose
        export MONGOCXX_TEST_SKIP_KMS_TLS_TESTS=1
      else
        sudo cp -v $DRIVERS_TOOLS/.evergreen/x509gen/ca.pem /usr/local/share/ca-certificates/cdriver.crt
        sudo update-ca-certificates --verbose
      fi
      ;;
    esac
  } >/dev/null
  register_ca_cert
  echo "Registering CA certificate for KMS TLS tests... done."

  # Ensure mock KMS servers are running before starting tests.
  wait_for_kms_server() {
    for i in $(seq 60); do
      # Exit code 7: "Failed to connect to host".
      if
        curl -s "localhost:$1"
        (($? != 7))
      then
        return 0
      else
        sleep 1
      fi
    done
    echo "Could not detect mock KMS server on port $1"
    return 1
  }
  echo "Waiting for mock KMS servers to start..."
  wait_for_kms_server 8999
  wait_for_kms_server 9000
  wait_for_kms_server 9001
  wait_for_kms_server 9002
  wait_for_kms_server 5698
  echo "Waiting for mock KMS servers to start... done."
fi

if [[ "$OSTYPE" =~ cygwin ]]; then
  CTEST_OUTPUT_ON_FAILURE=1 MSBuild.exe /p:Configuration=${build_type} /verbosity:minimal RUN_TESTS.vcxproj
  # Only run examples if MONGODB_API_VERSION is unset. We do not append
  # API version to example clients, so examples will fail when requireApiVersion
  # is true.
  if [[ -z "$MONGODB_API_VERSION" ]]; then
    echo "Running examples..."
    if ! CTEST_OUTPUT_ON_FAILURE=1 MSBuild.exe /p:Configuration=${build_type} /verbosity:minimal examples/run-examples.vcxproj >|output.txt 2>&1; then
      # Only emit output on failure.
      cat output.txt 1>&2
      exit 1
    fi
    echo "Running examples... done."
  fi
else
  # ENABLE_SLOW_TESTS is required to run the slow tests that are disabled by default. The slow tests should not be run if explicitly disabled.
  if [ -z "${disable_slow_tests}" ]; then
    export MONGOCXX_ENABLE_SLOW_TESTS=1
  fi

  ulimit -c unlimited || true

  if [ "${use_mongocryptd}" = "true" ]; then
    echo "Will run tests using mongocryptd (instead of crypt_shared library)"
  else
    echo "Will run tests using crypt_shared library (instead of mongocryptd)"
    # Set by run-orchestration.sh in "start_mongod".
    export CRYPT_SHARED_LIB_PATH="${CRYPT_SHARED_LIB_PATH}"
    echo "CRYPT_SHARED_LIB_PATH=$CRYPT_SHARED_LIB_PATH"
  fi

  export ASAN_OPTIONS="detect_leaks=1"
  export UBSAN_OPTIONS="print_stacktrace=1"

  run_test() { $@; }

  if [[ "${TEST_WITH_ASAN}" == "ON" || "${TEST_WITH_UBSAN}" == "ON" ]]; then
    export PATH="/usr/lib/llvm-3.8/bin:$PATH"
  elif [[ "${TEST_WITH_VALGRIND}" == "ON" ]]; then
    run_test() {
      valgrind --leak-check=full --track-origins=yes --num-callers=50 --error-exitcode=1 --error-limit=no --read-var-info=yes --suppressions=../etc/memcheck.suppressions $@
    }
  fi

  # Run tests and examples 1-by-1 with "run_test" so we can run them with valgrind.
  run_test ./src/bsoncxx/test/test_bson
  run_test ./src/mongocxx/test/test_driver
  run_test ./src/mongocxx/test/test_client_side_encryption_specs
  run_test ./src/mongocxx/test/test_crud_specs
  run_test ./src/mongocxx/test/test_gridfs_specs
  run_test ./src/mongocxx/test/test_command_monitoring_specs
  run_test ./src/mongocxx/test/test_instance
  run_test ./src/mongocxx/test/test_transactions_specs
  run_test ./src/mongocxx/test/test_logging
  run_test ./src/mongocxx/test/test_retryable_reads_specs
  run_test ./src/mongocxx/test/test_read_write_concern_specs
  run_test ./src/mongocxx/test/test_unified_format_spec

  # Some platforms like OS X don't support the /mode syntax to the -perm option
  # of find(1), and some platforms like Ubuntu 16.04 don't support the +mode
  # syntax, so we use Perl to help us find executable files.
  EXAMPLES=$(find examples -type f | sort | perl -nlwe 'print if -x')

  # Only run examples if MONGODB_API_VERSION is unset. We do not append
  # API version to example clients, so examples will fail when requireApiVersion
  # is true.
  if [[ -z "$MONGODB_API_VERSION" ]]; then
    for test in $EXAMPLES; do
      echo "Running $test"
      case "$test" in
      *encryption*)
        echo " - Skipping client side encryption example"
        ;;
      *change_stream*)
        echo " - TODO CXX-1201, enable for servers that support change streams"
        ;;
      *client_session*)
        echo " - TODO CXX-1201, enable for servers that support change streams"
        ;;
      *with_transaction*)
        echo " - TODO CXX-1201, enable for servers that support transactions"
        ;;
      *causal_consistency*)
        echo " - TODO CXX-1201, enable for servers that support transactions"
        ;;
      *)
        if ! run_test $test >|output.txt 2>&1; then
          # Only emit output on failure.
          cat output.txt 1>&2
          exit 1
        fi
        ;;
      esac
    done
  fi
fi

popd # ./build

export CMAKE_PREFIX_PATH=$PREFIX:$(pwd)/build/install
if [ -n "${lib_dir}" ]; then
  export PKG_CONFIG_PATH=$PREFIX/${lib_dir}/pkgconfig:$(pwd)/build/install/${lib_dir}/pkgconfig
else
  export PKG_CONFIG_PATH=$PREFIX/lib/pkgconfig:$(pwd)/build/install/lib/pkgconfig
fi
export USE_STATIC_LIBS="${USE_STATIC_LIBS}"
export BUILD_TYPE="${build_type}"

export CXXFLAGS="${example_projects_cxxflags}"
export LDFLAGS="${example_projects_ldflags}"
export CC="${example_projects_cc}"
export CXX="${example_projects_cxx}"
export CXX_STANDARD="${example_projects_cxx_standard}"
if [[ "$OSTYPE" =~ cygwin ]]; then
  export MSVC=1
elif [ "$(uname -s | tr '[:upper:]' '[:lower:]')" == "darwin" ]; then
  export DYLD_LIBRARY_PATH="$(pwd)/build/install/lib:$DYLD_LIBRARY_PATH"
else
  if [ -n "${lib_dir}" ]; then # only needed on Linux
    export LD_LIBRARY_PATH="$(pwd)/build/install/${lib_dir}:$LD_LIBRARY_PATH"
  else
    export LD_LIBRARY_PATH="$(pwd)/build/install/lib:$LD_LIBRARY_PATH"
  fi
fi
# The example projects never run under valgrind, since we haven't added execution
# logic to handle `run_test()`.
#
# Only run example projects if MONGODB_API_VERSION is unset. We do not append
# API version to example clients, so example projects will fail when requireApiVersion
# is true.
if [[ -z "$MONGODB_API_VERSION" ]]; then
  echo "Building example projects..."
  . "$PREFIX/.evergreen/scripts/find-cmake-latest.sh"
  export cmake_binary
  cmake_binary="$(find_cmake_latest)"
  command -v "$cmake_binary"
  .evergreen/build_example_projects.sh
  echo "Building example projects... done."
fi
unset MONGODB_API_VERSION
